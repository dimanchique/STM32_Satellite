#include "TroykaGyroscope.h"
#include "Logger.h"

DeviceTypeDef TrGyro = {0};
static struct GyroscopeData TrGyro_Data = {0};
static I2C_CommunicatorStruct TrGyro_Communicator = {0};

static void GenerateDataRepresentation(void) {
    if (TrGyro_Communicator.ConnectionStatus == HAL_OK)
        sprintf(TrGyro.DataRepr,
                "[%s] %.3f %.3f %.3f;",
                TrGyro.DeviceName,
                TrGyro_Data.GyroX,
                TrGyro_Data.GyroY,
                TrGyro_Data.GyroZ);
    else
        sprintf(TrGyro.DataRepr, "[%s] %s;", TrGyro.DeviceName, UNREACHABLE);
}

void TrGyro_Init(void) {
    TrGyro.Communicator = &TrGyro_Communicator;
    TrGyro.DeviceName = "TrGyro";
    /** Communication Section **/
    I2C_SetupCommunicator(&TrGyro_Communicator,
                          TrGyro.DeviceName,
                          TRO_GYRO_ADDR,
                          TR_GYRO_ID,
                          TR_GYRO_ID_REG);
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrGyro_Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&TrGyro_Communicator)){
        I2C_WriteData8(&TrGyro_Communicator, TR_GYRO_CR1, TR_GYR_CR1_XYZ_EN);
        uint8_t data;
        I2C_ReadData8(&TrGyro_Communicator, TR_GYRO_CR4, &data);
        data &= ~(TR_GYRO_RESET_RANGE);
        data |= TR_GYRO_RANGE;
        I2C_WriteData8(&TrGyro_Communicator, TR_GYRO_CR4, data);
        if (TrGyro_Communicator.ConnectionStatus == HAL_OK)
            TrGyro_Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrGyro_Communicator);
#endif
}

void TrGyro_ReadData(void) {
    if (TrGyro_Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadData6x8(&TrGyro_Communicator, 0x80 | TR_GYRO_OUT, data);
        TrGyro_Data.GyroX = ((int16_t) ((data[1] << 8) | data[0]) * TR_GYRO_SCALE);
        TrGyro_Data.GyroY = ((int16_t) ((data[3] << 8) | data[2]) * TR_GYRO_SCALE);
        TrGyro_Data.GyroZ = ((int16_t) ((data[5] << 8) | data[4]) * TR_GYRO_SCALE);
    }
    GenerateDataRepresentation();
}
