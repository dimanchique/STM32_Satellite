#include "BMP280.h"
#include "Logger.h"

Device_TypeDefStruct BMP280 = {0};

static uint16_t DeviceLED = BMP_PIN;
static GPIO_TypeDef *DeviceLED_Port = BMP_PORT;
static I2C_TypeDefStruct BMP_Communicator = {0};
static struct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
} BMP_CallData = {0};
static struct BarometerData BMP_BaroData = {0};

static uint8_t BMP_IsUpdating();
static void BMP_SoftReset();
static void GenerateDataRepresentation();
static void BMP_Calibrate();
static void BMP_ReadCoefficients();
static void WaitForDataReady();

void BMP_Init(void) {
    BMP280.Communicator = &BMP_Communicator;
    BMP280.DeviceName = "BMP280";
    /** Communication Section **/
    I2C_SetupCommunicator(&BMP_Communicator,
                          BMP280.DeviceName,
                          BMP280_ADDRESS,
                          BMP280_ID,
                          BMP280_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&BMP_Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&BMP_Communicator)) {
        BMP_SoftReset();
        while (BMP_IsUpdating()) {}
        BMP_ReadCoefficients();
        I2C_WriteData8(&BMP_Communicator,
                       BMP280_CONFIG_REGISTER,
                       BMP280_STANDBYTIME << 5 | BMP280_FILTERING_TYPE << 2);
        I2C_WriteData8(&BMP_Communicator,
                       BMP280_CTRL_REGISTER,
                       BMP280_TEMPERATURE_OVERSAMPLING << 5 | BMP280_PRESSURE_OVERSAMPLING << 2 | BMP280_POWER_MODE);
        HAL_Delay(50);
        BMP_Calibrate();
        if (BMP_Communicator.ConnectionStatus == HAL_OK)
            BMP_Communicator.State = Initialized;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&BMP_Communicator);
#endif
}


void BMP_ReadData(void) {
    if (BMP_Communicator.ConnectionStatus == HAL_OK) {
        uint32_t temper_raw;
        int32_t temper_int;
        int32_t val11, val12;

        float press_float;
        uint32_t press_raw, pres_int;
        int64_t val1, val2, p;

        /** Reading Temperature First **/
        WaitForDataReady();
        I2C_ReadDataU24BE(&BMP_Communicator, BMP280_TEMP_REGISTER, &temper_raw);
        temper_raw >>= 4;
        val11 = (int32_t) ((((temper_raw >> 3) - ((int32_t) BMP_CallData.dig_T1 << 1))) *
                           ((int32_t) BMP_CallData.dig_T2)) >> 11;
        val12 = (int32_t) (((((temper_raw >> 4) - ((int32_t) BMP_CallData.dig_T1)) *
                             ((temper_raw >> 4) - ((int32_t) BMP_CallData.dig_T1))) >> 12) *
                           ((int32_t) BMP_CallData.dig_T3)) >> 14;
        temper_int = val11 + val12;
        BMP_BaroData.Temperature = (float) ((temper_int * 5 + 128) >> 8) / 100.0f;

        /** Reading Pressure **/
        WaitForDataReady();
        I2C_ReadDataU24BE(&BMP_Communicator, BMP280_PRESSURE_REGISTER, &press_raw);
        press_raw >>= 4;
        val1 = ((int64_t) temper_int) - 128000;
        val2 = val1 * val1 * (int64_t) BMP_CallData.dig_P6;
        val2 = val2 + ((val1 * (int64_t) BMP_CallData.dig_P5) << 17);
        val2 = val2 + ((int64_t) BMP_CallData.dig_P4 << 35);
        val1 = ((val1 * val1 * (int64_t) BMP_CallData.dig_P3) >> 8) +
               ((val1 * (int64_t) BMP_CallData.dig_P2) << 12);
        val1 = (((((int64_t) 1) << 47) + val1)) * ((int64_t) BMP_CallData.dig_P1) >> 33;
        p = 1048576 - press_raw;
        p = (((p << 31) - val2) * 3125) / val1;
        val1 = (((int64_t) BMP_CallData.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        val2 = (((int64_t) BMP_CallData.dig_P8) * p) >> 19;
        p = ((p + val1 + val2) >> 8) + ((int64_t) BMP_CallData.dig_P7 << 4);
        pres_int = ((p >> 8) * 1000) + (((p & 0xff) * 390625) / 100000);
        press_float = (float) pres_int / 1000.0f;
        BMP_BaroData.Pressure = (double) press_float;
        BMP_BaroData.mmHg = Pa_to_mmHg(BMP_BaroData.Pressure);
        BMP_BaroData.Altitude = mmHg_to_Altitude(BMP_BaroData.mmHg_ref, BMP_BaroData.mmHg);
    }

    GenerateDataRepresentation();
}


static uint8_t BMP_IsUpdating() {
    uint8_t status;
    I2C_ReadData8(&BMP_Communicator, BMP280_STATUS_REGISTER, &status);
    return (status & 0x09) & BMP280_IS_UPDATING;
}

static void BMP_SoftReset() {
    I2C_WriteData8(&BMP_Communicator, BMP280_RESET_REGISTER, BMP280_SOFTRESET);
}

static void GenerateDataRepresentation() {
    if (BMP_Communicator.ConnectionStatus == HAL_OK) {
        sprintf(BMP280.DataRepr,
                "[%s] %.2f %.3f %.3f %.3f;",
                BMP280.DeviceName,
                BMP_BaroData.Temperature,
                BMP_BaroData.Pressure,
                BMP_BaroData.mmHg,
                BMP_BaroData.Altitude);
        SetDeviceStateOK(DeviceLED_Port, DeviceLED);
    } else {
        sprintf(BMP280.DataRepr, "[%s] %s;", BMP280.DeviceName, UNREACHABLE);
        SetDeviceStateError(DeviceLED_Port, DeviceLED);
    }
}

static void BMP_Calibrate() {
    BMP_ReadData();
    BMP_BaroData.mmHg_ref = BMP_BaroData.mmHg;
}

static void BMP_ReadCoefficients() {
    if (BMP_Communicator.ConnectionStatus == HAL_OK) {
        I2C_ReadDataU16(&BMP_Communicator, BMP280_REGISTER_DIG_T1, &BMP_CallData.dig_T1);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_T2, &BMP_CallData.dig_T2);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_T3, &BMP_CallData.dig_T3);
        I2C_ReadDataU16(&BMP_Communicator, BMP280_REGISTER_DIG_P1, &BMP_CallData.dig_P1);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_P2, &BMP_CallData.dig_P2);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_P3, &BMP_CallData.dig_P3);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_P4, &BMP_CallData.dig_P4);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_P5, &BMP_CallData.dig_P5);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_P6, &BMP_CallData.dig_P6);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_P7, &BMP_CallData.dig_P7);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_P8, &BMP_CallData.dig_P8);
        I2C_ReadDataS16(&BMP_Communicator, BMP280_REGISTER_DIG_P9, &BMP_CallData.dig_P9);
    }
}

static void WaitForDataReady() {
    while (BMP_IsUpdating()) {}
}
