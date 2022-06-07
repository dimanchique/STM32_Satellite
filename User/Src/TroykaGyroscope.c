#include "TroykaGyroscope.h"
#include "Logger.h"

TroykaGyroscope_TypeDef TroykaGyroscope = {0};

static void GenerateDataRepresentation() {
    if (TroykaGyroscope.Communicator.ConnectionStatus == HAL_OK)
        sprintf(TroykaGyroscope.DataRepr,
                "[%s] %.3f %.3f %.3f;",
                TroykaGyroscope.Communicator.Name,
                TroykaGyroscope.GyroData.GyroX,
                TroykaGyroscope.GyroData.GyroY,
                TroykaGyroscope.GyroData.GyroZ);
    else
        sprintf(TroykaGyroscope.DataRepr, "[%s] %s;", TroykaGyroscope.Communicator.Name, UNREACHABLE);
}

void TroykaGyroscope_Init() {
    /** Communication Section **/
    I2C_SetupCommunicator(&TroykaGyroscope.Communicator,
                          "TroykaGyroscope",
                          TROYKA_GYROSCOPE_ADDRESS,
                          TROYKA_GYROSCOPE_ID,
                          TROYKA_GYROSCOPE_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&TroykaGyroscope.Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&TroykaGyroscope.Communicator)){
        I2C_WriteData8(&TroykaGyroscope.Communicator,
                       TROYKA_GYROSCOPE_CTRL_REG1,
                       TROYKA_GYROSCOPE_CTRL_REG1_XYZ_EN);
        uint8_t data;
        I2C_ReadData8(&TroykaGyroscope.Communicator,
                      TROYKA_GYROSCOPE_CTRL_REG4,
                      &data);
        data &= ~(TROYKA_GYROSCOPE_RESET_RANGE);
        data |= TROYKA_GYROSCOPE_RANGE;
        I2C_WriteData8(&TroykaGyroscope.Communicator,
                       TROYKA_GYROSCOPE_CTRL_REG4,
                       data);
        if (TroykaGyroscope.Communicator.ConnectionStatus == HAL_OK)
            TroykaGyroscope.Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&TroykaGyroscope.Communicator);
#endif
}

void TroykaGyroscope_ReadData() {
    if (TroykaGyroscope.Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadData6x8(&TroykaGyroscope.Communicator, TROYKA_GYROSCOPE_OUT_X_L, data);
        TroykaGyroscope.GyroData.GyroX = ((int16_t) ((data[1] << 8) | data[0]) * TROYKA_GYROSCOPE_SCALE);
        TroykaGyroscope.GyroData.GyroY = ((int16_t) ((data[3] << 8) | data[2]) * TROYKA_GYROSCOPE_SCALE);
        TroykaGyroscope.GyroData.GyroZ = ((int16_t) ((data[5] << 8) | data[4]) * TROYKA_GYROSCOPE_SCALE);
    }
    GenerateDataRepresentation();
}
