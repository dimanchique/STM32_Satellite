#include "TroykaBarometer.h"
#include "Logger.h"

DeviceTypeDef TroykaBarometer = {0};
static struct BarometerData TrBaro_Data = {0};
static I2C_CommunicatorStruct TrBaro_Communicator = {0};

static void TroykaBarometer_Calibrate();
static void GenerateDataRepresentation();
static void TroykaBarometer_ReadPressure();
static void TroykaBarometer_ReadTemperature();

void TroykaBarometer_Init() {
    TroykaBarometer.Communicator = &TrBaro_Communicator;
    TroykaBarometer.DeviceName = "TrBaro";
    /** Communication Section **/
    I2C_SetupCommunicator(&TrBaro_Communicator,
                          TroykaBarometer.DeviceName,
                          TROYKA_BAROMETER_ADDRESS,
                          TROYKA_BAROMETER_ID,
                          TROYKA_BAROMETER_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrBaro_Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&TrBaro_Communicator)){
        I2C_WriteData8(&TrBaro_Communicator,
                       TROYKA_BAROMETER_CTRL_REG1,
                       TROYKA_BAROMETER_REG1_ODR1 |
                       TROYKA_BAROMETER_REG1_ODR2 |
                       TROYKA_BAROMETER_REG1_PD);
        HAL_Delay(50);
        TroykaBarometer_Calibrate();
        if (TrBaro_Communicator.ConnectionStatus == HAL_OK)
            TrBaro_Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&TrBaro_Communicator);
#endif
}

void TroykaBarometer_ReadData() {
    if (TrBaro_Communicator.ConnectionStatus == HAL_OK) {
        TroykaBarometer_ReadPressure();
        TroykaBarometer_ReadTemperature();
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
