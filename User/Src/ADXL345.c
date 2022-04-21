#include "ADXL345.h"
#include "Logger.h"

ADXL345_TypeDef ADXL345 = {0};

static void GenerateDataRepresentation(uint8_t ConnectionValid) {
    if (ConnectionValid)
        sprintf(ADXL345.DataRepr,
                "[%s] %.4f %.4f %.4f;",
                ADXL345.Communicator.Name,
                ADXL345.Data.AccX,
                ADXL345.Data.AccY,
                ADXL345.Data.AccZ);
    else
        sprintf(ADXL345.DataRepr, "[%s] NULL;", ADXL345.Communicator.Name);
}

static void ADXL_Calibrate(void) {
    ADXL_ReadData();
    ADXL345.Data.AccX /= ADXL345_ACC_SCALE;
    ADXL345.Data.AccY /= ADXL345_ACC_SCALE;
    ADXL345.Data.AccZ /= ADXL345_ACC_SCALE;
    I2C_WriteData8(&ADXL345.Communicator, ADXL345_OFSX, (uint8_t) ((ADXL345.Data.AccX - ADXL345_ACC_SCALE) / 4));
    I2C_WriteData8(&ADXL345.Communicator, ADXL345_OFSY, (uint8_t) ((ADXL345.Data.AccY - ADXL345_ACC_SCALE) / 4));
    I2C_WriteData8(&ADXL345.Communicator, ADXL345_OFSZ, (uint8_t) ((ADXL345.Data.AccZ - ADXL345_ACC_SCALE) / 4));
}

void ADXL_Init(void) {
    /** Communication Section **/
    ADXL345.Communicator.Name = "ADXL345";
    ADXL345.Communicator.State = NotInitialized;
    ADXL345.Communicator.CommAddress = ADXL345_ADDRESS<<1;
    ADXL345.Communicator.FactAddress = ADXL345_ADDRESS;
    ADXL345.Communicator.Device_ID = ADXL345_ID;
    ADXL345.Communicator.ID_Register = ADXL345_ID_REGISTER;
#ifdef ENABLE_DEBUG
    LogDeviceState(&ADXL345.Communicator);
#endif
    /** Setup Section **/
    CheckDeviceState(&ADXL345.Communicator);
    if (ADXL345.Communicator.ConnectionStatus == HAL_OK) {
        Verify_Device(&ADXL345.Communicator);
        if (ADXL345.Communicator.ConnectionStatus == HAL_OK) {
            I2C_WriteData8(&ADXL345.Communicator, ADXL345_BW_RATE, ADXL345_DATARATE);
            I2C_WriteData8(&ADXL345.Communicator, ADXL345_DATA_FORMAT, ADXL345_ACC_RESOLUTION);
            I2C_WriteData8(&ADXL345.Communicator, ADXL345_POWER_CTL, 0x00);
            I2C_WriteData8(&ADXL345.Communicator, ADXL345_POWER_CTL, 0x08);
            HAL_Delay(50);
            ADXL_Calibrate();
        }
        if (ADXL345.Communicator.ConnectionStatus == HAL_OK)
            ADXL345.Communicator.State = Initialized;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&ADXL345.Communicator);
#endif
}

void ADXL_ReadData(void) {
    if (ADXL345.Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadData6x8(&ADXL345.Communicator, ADXL345_DATAX0, data);
        ADXL345.Data.AccX = (double) ((int16_t) ((data[1] << 8) | data[0]) * ADXL345_ACC_SCALE);
        ADXL345.Data.AccY = (double) ((int16_t) ((data[3] << 8) | data[2]) * ADXL345_ACC_SCALE);
        ADXL345.Data.AccZ = (double) ((int16_t) ((data[5] << 8) | data[4]) * ADXL345_ACC_SCALE);
    } else {
        ADXL345.Data.AccX = 0;
        ADXL345.Data.AccY = 0;
        ADXL345.Data.AccZ = 0;
    }
    GenerateDataRepresentation(ADXL345.Communicator.ConnectionStatus == HAL_OK);
}
