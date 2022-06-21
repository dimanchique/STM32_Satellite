#include "TroykaAccelerometer.h"
#include "Logger.h"

DeviceTypeDef TrAcc = {0};
static struct AccelerometerData TrAcc_Data = {0};
static I2C_CommunicatorStruct TrAcc_Communicator = {0};

static void GenerateDataRepresentation(void) {
    if (TrAcc_Communicator.ConnectionStatus == HAL_OK)
        sprintf(TrAcc.DataRepr,
                "[%s] %.4f %.4f %.4f;",
                TrAcc.DeviceName,
                TrAcc_Data.AccX,
                TrAcc_Data.AccY,
                TrAcc_Data.AccZ);
    else
        sprintf(TrAcc.DataRepr, "[%s] %s;", TrAcc.DeviceName, UNREACHABLE);
}

void TrAcc_Init(void) {
    /** Communication Section **/
    TrAcc.Communicator = &TrAcc_Communicator;
    TrAcc.DeviceName = "TrAcc";
    I2C_SetupCommunicator(&TrAcc_Communicator,
                          TrAcc.DeviceName,
                          TR_ACC_ADDR,
                          TR_ACC_ID,
                          TR_ACC_ID_REG);
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrAcc_Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&TrAcc_Communicator)){
        I2C_WriteData8(&TrAcc_Communicator, TR_ACC_CR1, TR_ACC_CR1_XYZ_EN | TR_ACC_PM0);
        uint8_t data;
        I2C_ReadData8(&TrAcc_Communicator, TR_ACC_CR4, &data);
        data &= ~(TR_ACC_RESET_RANGE);
        data |= TR_ACC_RANGE;
        I2C_WriteData8(&TrAcc_Communicator, TR_ACC_CR4, data);
        if (TrAcc_Communicator.ConnectionStatus == HAL_OK)
            TrAcc_Communicator.State = Initialized;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrAcc_Communicator);
#endif
}

void TrAcc_ReadData(void) {
    if (TrAcc_Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadData6x8(&TrAcc_Communicator, 0x80 | TR_ACC_OUT, data);
        TrAcc_Data.AccX = ((int16_t) ((data[1] << 8) | data[0]) * TR_ACC_SCALE);
        TrAcc_Data.AccY = ((int16_t) ((data[3] << 8) | data[2]) * TR_ACC_SCALE);
        TrAcc_Data.AccZ = ((int16_t) ((data[5] << 8) | data[4]) * TR_ACC_SCALE);
    }
    GenerateDataRepresentation();
}
