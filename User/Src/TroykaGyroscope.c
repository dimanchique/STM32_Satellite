#include "TroykaGyroscope.h"
#include "Logger.h"

DeviceTypeDef TroykaGyroscope = {0};
static struct GyroscopeData TrGyro_Data = {0};
static I2C_CommunicatorStruct TrGyro_Communicator = {0};

static void GenerateDataRepresentation();

void TroykaGyroscope_Init() {
    TroykaGyroscope.Communicator = &TrGyro_Communicator;
    TroykaGyroscope.DeviceName = "TrGyro";
    /** Communication Section **/
    I2C_SetupCommunicator(&TrGyro_Communicator,
                          TroykaGyroscope.DeviceName,
                          TROYKA_GYROSCOPE_ADDRESS,
                          TROYKA_GYROSCOPE_ID,
                          TROYKA_GYROSCOPE_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrGyro_Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&TrGyro_Communicator)){
        I2C_WriteData8(&TrGyro_Communicator,
                       TROYKA_GYROSCOPE_CTRL_REG1,
                       TROYKA_GYROSCOPE_CTRL_REG1_XYZ_EN);
        uint8_t data;
        I2C_ReadData8(&TrGyro_Communicator,
                      TROYKA_GYROSCOPE_CTRL_REG4,
                      &data);
        data &= ~(TROYKA_GYROSCOPE_RESET_RANGE);
        data |= TROYKA_GYROSCOPE_RANGE;
        I2C_WriteData8(&TrGyro_Communicator,
                       TROYKA_GYROSCOPE_CTRL_REG4,
                       data);
        if (TrGyro_Communicator.ConnectionStatus == HAL_OK)
            TrGyro_Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrGyro_Communicator);
#endif
}

void TroykaGyroscope_ReadData() {
    if (TrGyro_Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadData6x8(&TrGyro_Communicator, 0x80 | TROYKA_GYROSCOPE_OUT, data);
        TrGyro_Data.GyroX = ((int16_t) ((data[1] << 8) | data[0]) * TROYKA_GYROSCOPE_SCALE);
        TrGyro_Data.GyroY = ((int16_t) ((data[3] << 8) | data[2]) * TROYKA_GYROSCOPE_SCALE);
        TrGyro_Data.GyroZ = ((int16_t) ((data[5] << 8) | data[4]) * TROYKA_GYROSCOPE_SCALE);
    }
    GenerateDataRepresentation();
}

static void GenerateDataRepresentation() {
    if (TrGyro_Communicator.ConnectionStatus == HAL_OK)
        sprintf(TroykaGyroscope.DataRepr,
                "[%s] %.3f %.3f %.3f;",
                TroykaGyroscope.DeviceName,
                TrGyro_Data.GyroX,
                TrGyro_Data.GyroY,
                TrGyro_Data.GyroZ);
    else
        sprintf(TroykaGyroscope.DataRepr, "[%s] %s;", TroykaGyroscope.DeviceName, UNREACHABLE);
}
