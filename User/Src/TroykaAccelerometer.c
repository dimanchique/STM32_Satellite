#include "TroykaAccelerometer.h"
#include "Logger.h"

DeviceTypeDef TroykaAccelerometer = {0};
static struct AccelerometerData TrAcc_Data = {0};
static I2C_CommunicatorStruct TrAcc_Communicator = {0};

static void GenerateDataRepresentation() {
    if (TrAcc_Communicator.ConnectionStatus == HAL_OK)
        sprintf(TroykaAccelerometer.DataRepr,
                "[%s] %.4f %.4f %.4f;",
                TroykaAccelerometer.DeviceName,
                TrAcc_Data.AccX,
                TrAcc_Data.AccY,
                TrAcc_Data.AccZ);
    else
        sprintf(TroykaAccelerometer.DataRepr, "[%s] %s;", TroykaAccelerometer.DeviceName, UNREACHABLE);
}

void TroykaAccelerometer_Init(void) {
    /** Communication Section **/
    TroykaAccelerometer.Communicator = &TrAcc_Communicator;
    TroykaAccelerometer.DeviceName = "TrAcc";
    I2C_SetupCommunicator(&TrAcc_Communicator,
                          TroykaAccelerometer.DeviceName,
                          TROYKA_ACCELEROMETER_ADDRESS,
                          TROYKA_ACCELEROMETER_ID,
                          TROYKA_ACCELEROMETER_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrAcc_Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&TrAcc_Communicator)){
        I2C_WriteData8(&TrAcc_Communicator,
                       TROYKA_ACCELEROMETER_CTRL_REG1,
                       TROYKA_ACCELEROMETER_CTRL_REG1_XYZ_EN | TROYKA_ACCELEROMETER_CTRL_REG1_PM0);
        uint8_t data;
        I2C_ReadData8(&TrAcc_Communicator,
                      TROYKA_ACCELEROMETER_CTRL_REG4,
                      &data);
        data &= ~(TROYKA_ACCELEROMETER_RESET_RANGE);
        data |= TROYKA_ACCELEROMETER_RANGE;
        I2C_WriteData8(&TrAcc_Communicator,
                       TROYKA_ACCELEROMETER_CTRL_REG4,
                       data);
        if (TrAcc_Communicator.ConnectionStatus == HAL_OK)
            TrAcc_Communicator.State = Initialized;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrAcc_Communicator);
#endif
}

void TroykaAccelerometer_ReadData(void) {
    if (TrAcc_Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadData6x8(&TrAcc_Communicator, 0x80 | TROYKA_ACCELEROMETER_OUT, data);
        TrAcc_Data.AccX = ((int16_t) ((data[1] << 8) | data[0]) * TROYKA_ACCELEROMETER_SCALE);
        TrAcc_Data.AccY = ((int16_t) ((data[3] << 8) | data[2]) * TROYKA_ACCELEROMETER_SCALE);
        TrAcc_Data.AccZ = ((int16_t) ((data[5] << 8) | data[4]) * TROYKA_ACCELEROMETER_SCALE);
    }
    GenerateDataRepresentation();
}
