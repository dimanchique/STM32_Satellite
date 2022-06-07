#include "TroykaAccelerometer.h"
#include "Logger.h"

TroykaAccelerometer_TypeDef TroykaAccelerometer = {0};

static void GenerateDataRepresentation() {
    if (TroykaAccelerometer.Communicator.ConnectionStatus == HAL_OK)
        sprintf(TroykaAccelerometer.DataRepr,
                "[%s] %.4f %.4f %.4f;",
                TroykaAccelerometer.Communicator.Name,
                TroykaAccelerometer.Data.AccX,
                TroykaAccelerometer.Data.AccY,
                TroykaAccelerometer.Data.AccZ);
    else
        sprintf(TroykaAccelerometer.DataRepr, "[%s] %s;", TroykaAccelerometer.Communicator.Name, UNREACHABLE);
}

void TroykaAccelerometer_Init(void) {
    /** Communication Section **/
    I2C_SetupCommunicator(&TroykaAccelerometer.Communicator,
                          "TroykaAccelerometer",
                          TROYKA_ACCELEROMETER_ADDRESS,
                          TROYKA_ACCELEROMETER_ID,
                          TROYKA_ACCELEROMETER_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&TroykaAccelerometer.Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&TroykaAccelerometer.Communicator)){
        I2C_WriteData8(&TroykaAccelerometer.Communicator,
                       TROYKA_ACCELEROMETER_CTRL_REG1,
                       TROYKA_ACCELEROMETER_CTRL_REG1_XYZ_EN);
        uint8_t data;
        I2C_ReadData8(&TroykaAccelerometer.Communicator,
                      TROYKA_ACCELEROMETER_CTRL_REG4,
                      &data);
        data &= ~(TROYKA_ACCELEROMETER_RESET_RANGE);
        data |= TROYKA_ACCELEROMETER_RANGE;
        I2C_WriteData8(&TroykaAccelerometer.Communicator,
                       TROYKA_ACCELEROMETER_CTRL_REG4,
                       data);
        if (TroykaAccelerometer.Communicator.ConnectionStatus == HAL_OK)
            TroykaAccelerometer.Communicator.State = Initialized;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&TroykaAccelerometer.Communicator);
#endif
}

void TroykaAccelerometer_ReadData(void) {
    if (TroykaAccelerometer.Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadData6x8(&TroykaAccelerometer.Communicator, TROYKA_ACCELEROMETER_OUT_X_L, data);
        TroykaAccelerometer.Data.AccX = ((int16_t) ((data[1] << 8) | data[0]) * TROYKA_ACCELEROMETER_SCALE);
        TroykaAccelerometer.Data.AccY = ((int16_t) ((data[3] << 8) | data[2]) * TROYKA_ACCELEROMETER_SCALE);
        TroykaAccelerometer.Data.AccZ = ((int16_t) ((data[5] << 8) | data[4]) * TROYKA_ACCELEROMETER_SCALE);
    }
    GenerateDataRepresentation();
}
