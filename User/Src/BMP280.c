#include "BMP280.h"
#include "Logger.h"

BMP280_TypeDef BMP280 = {0};
static int32_t temper_int;
extern DiskWriter Logger;

static uint8_t BMP_IsUpdating(void) {
    uint8_t status;
    I2C_ReadData8(&BMP280.Communicator, BMP280_STATUS_REGISTER, &status);
    return (status & 0x09) & BMP280_IS_UPDATING;
}

static void BMP_SoftReset(void) {
    I2C_WriteData8(&BMP280.Communicator, BMP280_RESET_REGISTER, BMP280_SOFTRESET);
}

static void GenerateDataRepresentation(uint8_t ConnectionValid) {
    if (ConnectionValid)
        sprintf(BMP280.DataRepr,
                "[%s] %.2f; %.3f; %.3f; %.3f;",
                BMP280.Communicator.Name,
                BMP280.ExtraData.Temperature,
                BMP280.Data.Pressure,
                BMP280.Data.mmHg,
                BMP280.Data.Altitude);
    else
        sprintf(BMP280.DataRepr, "[%s] NULL;", BMP280.Communicator.Name);
}

static void BMP_Calibrate(void) {
    BMP_ReadData();
    BMP280.Data.base_mmHg = BMP280.Data.mmHg;
}

static void BMP_ReadCoefficients(void) {
    if (BMP280.Communicator.ConnectionStatus == HAL_OK)
        CheckDeviceState(&BMP280.Communicator);
    if (BMP280.Communicator.ConnectionStatus == HAL_OK) {
        I2C_ReadDataU16(&BMP280.Communicator, BMP280_REGISTER_DIG_T1, &BMP280.CalibrationCoefficients.dig_T1);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_T2, &BMP280.CalibrationCoefficients.dig_T2);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_T3, &BMP280.CalibrationCoefficients.dig_T3);
        I2C_ReadDataU16(&BMP280.Communicator, BMP280_REGISTER_DIG_P1, &BMP280.CalibrationCoefficients.dig_P1);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_P2, &BMP280.CalibrationCoefficients.dig_P2);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_P3, &BMP280.CalibrationCoefficients.dig_P3);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_P4, &BMP280.CalibrationCoefficients.dig_P4);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_P5, &BMP280.CalibrationCoefficients.dig_P5);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_P6, &BMP280.CalibrationCoefficients.dig_P6);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_P7, &BMP280.CalibrationCoefficients.dig_P7);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_P8, &BMP280.CalibrationCoefficients.dig_P8);
        I2C_ReadDataS16(&BMP280.Communicator, BMP280_REGISTER_DIG_P9, &BMP280.CalibrationCoefficients.dig_P9);
    }
}

static void BMP_GetTemperature(void) {
    uint32_t temper_raw;
    int32_t val1, val2;
    while (BMP_IsUpdating()) {};
    I2C_ReadDataU24BE(&BMP280.Communicator, BMP280_TEMP_REGISTER, &temper_raw);
    temper_raw >>= 4;
    val1 = (int32_t) ((((temper_raw >> 3) - ((int32_t) BMP280.CalibrationCoefficients.dig_T1 << 1))) *
                      ((int32_t) BMP280.CalibrationCoefficients.dig_T2)) >> 11;
    val2 = (int32_t) (((((temper_raw >> 4) - ((int32_t) BMP280.CalibrationCoefficients.dig_T1)) *
                        ((temper_raw >> 4) - ((int32_t) BMP280.CalibrationCoefficients.dig_T1))) >> 12) *
                      ((int32_t) BMP280.CalibrationCoefficients.dig_T3)) >> 14;
    temper_int = val1 + val2;
    BMP280.ExtraData.Temperature = (float) ((temper_int * 5 + 128) >> 8) / 100.0f;
}

static void BMP_GetPressureAndTemperature(void) {
    float press_float;
    uint32_t press_raw, pres_int;
    int64_t val1, val2, p;
    BMP_GetTemperature();
    while (BMP_IsUpdating()) {};
    I2C_ReadDataU24BE(&BMP280.Communicator, BMP280_PRESSURE_REGISTER, &press_raw);
    press_raw >>= 4;
    val1 = ((int64_t) temper_int) - 128000;
    val2 = val1 * val1 * (int64_t) BMP280.CalibrationCoefficients.dig_P6;
    val2 = val2 + ((val1 * (int64_t) BMP280.CalibrationCoefficients.dig_P5) << 17);
    val2 = val2 + ((int64_t) BMP280.CalibrationCoefficients.dig_P4 << 35);
    val1 = ((val1 * val1 * (int64_t) BMP280.CalibrationCoefficients.dig_P3) >> 8) +
           ((val1 * (int64_t) BMP280.CalibrationCoefficients.dig_P2) << 12);
    val1 = (((((int64_t) 1) << 47) + val1)) * ((int64_t) BMP280.CalibrationCoefficients.dig_P1) >> 33;
    p = 1048576 - press_raw;
    p = (((p << 31) - val2) * 3125) / val1;
    val1 = (((int64_t) BMP280.CalibrationCoefficients.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    val2 = (((int64_t) BMP280.CalibrationCoefficients.dig_P8) * p) >> 19;
    p = ((p + val1 + val2) >> 8) + ((int64_t) BMP280.CalibrationCoefficients.dig_P7 << 4);
    pres_int = ((p >> 8) * 1000) + (((p & 0xff) * 390625) / 100000);
    press_float = (float) pres_int / 1000.0f;
    BMP280.Data.Pressure = (double) press_float;
    BMP280.Data.mmHg = BMP280.Data.Pressure * 0.00750061683;
}

void BMP_Init(void) {
    /** Communication Section **/
    BMP280.Communicator.Name = "BMP280";
    BMP280.Communicator.State = NotInitialized;
    BMP280.Communicator.CommAddress = CommunicationAddress(BMP280_ADDRESS);
    BMP280.Communicator.FactAddress = BMP280_ADDRESS;
    BMP280.Communicator.Device_ID = BMP280_ID;
    BMP280.Communicator.ID_Register = BMP280_ID_REGISTER;

    /** Data Section **/
    BMP280.Configuration.Power = BMP280_NORMALMODE;                                //Measure Continuously
    BMP280.Configuration.StandbyTime = BMP280_STANDBYTIME2;                        //Measure every 62.5ms
    BMP280.Configuration.Pressure_Oversampling = BMP280_OVERSAMPLINGx4;            //Use Standart Resolution (18 bit)
    BMP280.Configuration.Temperature_Oversampling = BMP280_OVERSAMPLINGx1;        //Use Ultra-Low Power mode (16 bit)
    BMP280.Configuration.FilterCoefficient = BMP280_FILTERCOEFF8;                //ODR = 7.3 Hz, RMS = 6.4 (see page 14, table 7 of datasheet)
    LogState(BMP280.Communicator);

    /** Setup Section **/
    CheckDeviceState(&BMP280.Communicator);
    if (BMP280.Communicator.ConnectionStatus == HAL_OK) {
        Verify_Device(&BMP280.Communicator);
        if (BMP280.Communicator.ConnectionStatus == HAL_OK) {
            BMP_SoftReset();
            while (BMP_IsUpdating()) {};
            BMP_ReadCoefficients();
            uint8_t ConfigRegister = (uint8_t) (BMP280.Configuration.StandbyTime << 5 |
                                                BMP280.Configuration.FilterCoefficient << 2);
            I2C_WriteData8(&BMP280.Communicator, BMP280_CONFIG_REGISTER, ConfigRegister);
            uint8_t CtrlRegister = (uint8_t) (BMP280.Configuration.Temperature_Oversampling << 5 |
                                              BMP280.Configuration.Pressure_Oversampling << 2 |
                                              BMP280.Configuration.Power);
            I2C_WriteData8(&BMP280.Communicator, BMP280_CTRL_REGISTER, CtrlRegister);
            HAL_Delay(100);
            BMP_Calibrate();
        }
        if (BMP280.Communicator.ConnectionStatus == HAL_OK) BMP280.Communicator.State = Initialized;
    }
    LogState(BMP280.Communicator);
}

void BMP_ReadData(void) {
    if (BMP280.Communicator.ConnectionStatus == HAL_OK) {
        BMP_GetPressureAndTemperature();
        BMP280.Data.Altitude = (BMP280.Data.base_mmHg - BMP280.Data.mmHg) * 10.5;
    } else {
        BMP280.Data.Pressure = 0;
        BMP280.Data.mmHg = 0;
        BMP280.Data.Altitude = 0;
    }
    GenerateDataRepresentation(BMP280.Communicator.ConnectionStatus == HAL_OK);
}
