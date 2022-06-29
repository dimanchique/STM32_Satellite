#include "TroykaBarometer.h"
#include "Logger.h"

DeviceTypeDef TrBaro = {0};
static struct BarometerData TrBaro_Data = {0};
static I2C_CommunicatorStruct TrBaro_Communicator = {0};

static void TroykaBarometer_Calibrate();
static void GenerateDataRepresentation();
static void TroykaBarometer_ReadPressure();
static void TroykaBarometer_ReadTemperature();

void TrBaro_Init() {
    TrBaro.Communicator = &TrBaro_Communicator;
    TrBaro.DeviceName = "TrBaro";
    /** Communication Section **/
    I2C_SetupCommunicator(&TrBaro_Communicator,
                          TrBaro.DeviceName,
                          TR_BARO_ADDR,
                          TR_BARO_ID,
                          TR_BARO_ID_REG);
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrBaro_Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&TrBaro_Communicator)){
        I2C_WriteData8(&TrBaro_Communicator,
                       TR_BARO_CR1,
                       TR_BARO_ODR1 |
                       TR_BARO_ODR2 |
                       TR_BARO_PD);
        HAL_Delay(50);
        TrBaro_Calibrate();
        if (TrBaro_Communicator.ConnectionStatus == HAL_OK)
            TrBaro_Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrBaro_Communicator);
#endif
}

void TrBaro_ReadData() {
    if (TrBaro_Communicator.ConnectionStatus == HAL_OK) {
        TrBaro_ReadPressure();
        TrBaro_ReadTemperature();
    }
    GenerateDataRepresentation();
}

static void TroykaBarometer_Calibrate() {
    TroykaBarometer_ReadData();
    TrBaro_Data.base_mmHg = TrBaro_Data.mmHg;
}

static void GenerateDataRepresentation() {
    if (TrBaro_Communicator.ConnectionStatus == HAL_OK)
        sprintf(TroykaBarometer.DataRepr,
                "[%s] %.2f; %.3f; %.3f; %.3f;",
                TroykaBarometer.DeviceName,
                TrBaro_Data.Temperature,
                TrBaro_Data.Pressure,
                TrBaro_Data.mmHg,
                TrBaro_Data.Altitude);
    else
        sprintf(TroykaBarometer.DataRepr, "[%s] %s;", TroykaBarometer.DeviceName, UNREACHABLE);
}

static void TroykaBarometer_ReadPressure() {
    uint8_t data[3] = {0};
    I2C_ReadData3x8(&TrBaro_Communicator,
                    0x80 | TROYKA_BAROMETER_PRESS_OUT,
                    data);
    double millibars = (data[2] << 16 | (uint16_t) data[1] << 8 | data[0]) / 4096.0;
    TrBaro_Data.Pressure = millibars * MILLIBARS_TO_PASCALS;
    TrBaro_Data.mmHg = millibars * MILLIBARS_TO_MMHG;
    TrBaro_Data.Altitude = (TrBaro_Data.base_mmHg - TrBaro_Data.mmHg) * 10.5f;
}

static void TroykaBarometer_ReadTemperature() {
    uint8_t data[2] = {0};
    I2C_ReadData2x8(&TrBaro_Communicator,
                    0x80 | TROYKA_BAROMETER_TEMP_OUT,
                    data);
    TrBaro_Data.Temperature = 42.5f + (float) (int16_t) ((data[1] << 8) | data[0]) / 480.0f;
}
