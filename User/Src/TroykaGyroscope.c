#include "TroykaGyroscope.h"
#include "Logger.h"

TroykaGyroscope_TypeDef Gyroscope = {0};

static void GenerateDataRepresentation(uint8_t ConnectionValid) {
    if (ConnectionValid)
        sprintf(Gyroscope.DataRepr,
                "[%s] %.3f %.3f %.3f;",
                Gyroscope.Communicator.Name,
                Gyroscope.GyroData.GyroX,
                Gyroscope.GyroData.GyroY,
                Gyroscope.GyroData.GyroZ);
    else
        sprintf(Gyroscope.DataRepr, "[%s] NULL;", Gyroscope.Communicator.Name);
}

void TroykaGyroscope_Init() {
    /** Communication Section **/
    I2C_SetupCommunicator(&Gyroscope.Communicator,
                          "I3G4250D",
                          TROYKA_GYROSCOPE_ADDRESS,
                          TROYKA_GYROSCOPE_ID,
                          TROYKA_GYROSCOPE_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&Gyroscope.Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&Gyroscope.Communicator)){
        I2C_WriteData8(&Gyroscope.Communicator,
                       TROYKA_GYROSCOPE_CTRL_REG1,
                       TROYKA_GYROSCOPE_CTRL_REG1_XYZ_EN);
        uint8_t data;
        I2C_ReadData8(&Gyroscope.Communicator,
                      TROYKA_GYROSCOPE_CTRL_REG4,
                      &data);
        data &= ~(TROYKA_GYROSCOPE_RESET_RANGE);
        data |= TROYKA_GYROSCOPE_RANGE;
        I2C_WriteData8(&Gyroscope.Communicator,
                       TROYKA_GYROSCOPE_CTRL_REG4,
                       data);
        if (Gyroscope.Communicator.ConnectionStatus == HAL_OK)
            Gyroscope.Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&Gyroscope.Communicator);
#endif
}

void TroykaGyroscope_ReadData() {
    if (Gyroscope.Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadData6x8(&Gyroscope.Communicator, TROYKA_GYROSCOPE_OUT_X_L, data);
        Gyroscope.GyroData.GyroX = ((int16_t) ((data[1] << 8) | data[0]) * TROYKA_GYROSCOPE_SCALE);
        Gyroscope.GyroData.GyroY = ((int16_t) ((data[3] << 8) | data[2]) * TROYKA_GYROSCOPE_SCALE);
        Gyroscope.GyroData.GyroZ = ((int16_t) ((data[5] << 8) | data[4]) * TROYKA_GYROSCOPE_SCALE);
    } else {
        Gyroscope.GyroData.GyroX = 0;
        Gyroscope.GyroData.GyroY = 0;
        Gyroscope.GyroData.GyroZ = 0;
    }
    GenerateDataRepresentation(Gyroscope.Communicator.ConnectionStatus == HAL_OK);
}
