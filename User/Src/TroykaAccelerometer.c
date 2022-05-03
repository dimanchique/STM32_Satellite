#include "TroykaAccelerometer.h"
#include "Logger.h"

TroykaAccelerometer_TypeDef Accelerometer = {0};

static void GenerateDataRepresentation(uint8_t ConnectionValid) {
    if (ConnectionValid)
        sprintf(Accelerometer.DataRepr,
                "[%s] %.4f %.4f %.4f;",
                Accelerometer.Communicator.Name,
                Accelerometer.Data.AccX,
                Accelerometer.Data.AccY,
                Accelerometer.Data.AccZ);
    else
        sprintf(Accelerometer.DataRepr, "[%s] NULL;", Accelerometer.Communicator.Name);
}

void TroykaAccelerometer_Init(void) {
    /** Communication Section **/
    I2C_SetupCommunicator(&Accelerometer.Communicator,
                          "LIS331DLH",
                          TROYKA_ACCELEROMETER_ADDRESS,
                          TROYKA_ACCELEROMETER_ID,
                          TROYKA_ACCELEROMETER_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&Accelerometer.Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&Accelerometer.Communicator)){
        I2C_WriteData8(&Accelerometer.Communicator,
                       TROYKA_ACCELEROMETER_CTRL_REG1,
                       TROYKA_ACCELEROMETER_CTRL_REG1_XYZ_EN);
        uint8_t data;
        I2C_ReadData8(&Accelerometer.Communicator,
                      TROYKA_ACCELEROMETER_CTRL_REG4,
                      &data);
        data &= ~(TROYKA_ACCELEROMETER_RESET_RANGE);
        data |= TROYKA_ACCELEROMETER_RANGE;
        I2C_WriteData8(&Accelerometer.Communicator,
                       TROYKA_ACCELEROMETER_CTRL_REG4,
                       data);
        if (Accelerometer.Communicator.ConnectionStatus == HAL_OK)
            Accelerometer.Communicator.State = Initialized;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&Accelerometer.Communicator);
#endif
}

void TroykaAccelerometer_ReadData(void) {
    if (Accelerometer.Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadData6x8(&Accelerometer.Communicator, TROYKA_ACCELEROMETER_OUT_X_L, data);
        Accelerometer.Data.AccX = ((int16_t) ((data[1] << 8) | data[0]) * TROYKA_ACCELEROMETER_SCALE);
        Accelerometer.Data.AccY = ((int16_t) ((data[3] << 8) | data[2]) * TROYKA_ACCELEROMETER_SCALE);
        Accelerometer.Data.AccZ = ((int16_t) ((data[5] << 8) | data[4]) * TROYKA_ACCELEROMETER_SCALE);
    } else {
        Accelerometer.Data.AccX = 0;
        Accelerometer.Data.AccY = 0;
        Accelerometer.Data.AccZ = 0;
    }
    GenerateDataRepresentation(Accelerometer.Communicator.ConnectionStatus == HAL_OK);
}
