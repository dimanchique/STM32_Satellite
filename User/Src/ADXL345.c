#include "ADXL345.h"
#include "Logger.h"

Device_TypeDefStruct ADXL345 = {0};

static uint16_t DeviceLED = ADXL_PIN;
static I2C_TypeDefStruct ADXL_Communicator = {0};
static struct AccelerometerData ADXL_Data = {0};

static void GenerateDataRepresentation();
static void ADXL_Calibrate(void);

void ADXL_Init(void) {
    ADXL345.Communicator = &ADXL_Communicator;
    ADXL345.DeviceName = "ADXL345";
    /** Communication Section **/
    I2C_SetupCommunicator(&ADXL_Communicator,
                          ADXL345.DeviceName,
                          ADXL345_ADDRESS,
                          ADXL345_ID,
                          ADXL345_ID_REGISTER);

#ifdef ENABLE_DEBUG
    LogDeviceState(&ADXL_Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&ADXL_Communicator)) {
        I2C_WriteData8(&ADXL_Communicator, ADXL345_BW_RATE, ADXL345_DATARATE);
        I2C_WriteData8(&ADXL_Communicator, ADXL345_DATA_FORMAT, ADXL345_ACC_RESOLUTION);
        I2C_WriteData8(&ADXL_Communicator, ADXL345_POWER_CTL, 0x00);
        I2C_WriteData8(&ADXL_Communicator, ADXL345_POWER_CTL, 0x08);
        HAL_Delay(50);
        ADXL_Calibrate();
        if (ADXL_Communicator.ConnectionStatus == HAL_OK)
            ADXL_Communicator.State = Initialized;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&ADXL_Communicator);
#endif
}

void ADXL_ReadData(void) {
    if (ADXL_Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[6] = {0};
        I2C_ReadDataNx8(&ADXL_Communicator, ADXL345_DATA, data, 6);
        ADXL_Data.AccX = ((int16_t) ((data[1] << 8) | data[0]) * ADXL345_ACC_SCALE);
        ADXL_Data.AccY = ((int16_t) ((data[3] << 8) | data[2]) * ADXL345_ACC_SCALE);
        ADXL_Data.AccZ = ((int16_t) ((data[5] << 8) | data[4]) * ADXL345_ACC_SCALE);
    }
    GenerateDataRepresentation();
}

static void GenerateDataRepresentation() {
    if (ADXL_Communicator.ConnectionStatus == HAL_OK) {
        sprintf(ADXL345.DataRepr,
                "[%s] %.4f %.4f %.4f;",
                ADXL345.DeviceName,
                ADXL_Data.AccX,
                ADXL_Data.AccY,
                ADXL_Data.AccZ);
        SetDeviceStateOK(LED_PORT, DeviceLED);
    } else {
        sprintf(ADXL345.DataRepr, "[%s] %s;", ADXL345.DeviceName, UNREACHABLE);
        SetDeviceStateError(LED_PORT, DeviceLED);
    }
}

static void ADXL_Calibrate(void) {
    ADXL_ReadData();
    ADXL_Data.AccX /= ADXL345_ACC_SCALE;
    ADXL_Data.AccY /= ADXL345_ACC_SCALE;
    ADXL_Data.AccZ /= ADXL345_ACC_SCALE;
    I2C_WriteData8(&ADXL_Communicator, ADXL345_OFSX, (uint8_t) ((ADXL_Data.AccX - ADXL345_ACC_SCALE) / 4));
    I2C_WriteData8(&ADXL_Communicator, ADXL345_OFSY, (uint8_t) ((ADXL_Data.AccY - ADXL345_ACC_SCALE) / 4));
    I2C_WriteData8(&ADXL_Communicator, ADXL345_OFSZ, (uint8_t) ((ADXL_Data.AccZ - ADXL345_ACC_SCALE) / 4));
}
