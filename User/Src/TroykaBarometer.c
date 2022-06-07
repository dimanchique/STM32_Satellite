#include "TroykaBarometer.h"
#include "Logger.h"

TroykaBarometer_TypeDef TroykaBarometer = {0};

static void TroykaBarometer_Calibrate() {
    TroykaBarometer_ReadData();
    TroykaBarometer.Data.base_mmHg = TroykaBarometer.Data.mmHg;
}

static void GenerateDataRepresentation() {
    if (TroykaBarometer.Communicator.ConnectionStatus == HAL_OK)
        sprintf(TroykaBarometer.DataRepr,
                "[%s] %.2f; %.3f; %.3f; %.3f;",
                TroykaBarometer.Communicator.Name,
                TroykaBarometer.Data.Temperature,
                TroykaBarometer.Data.Pressure,
                TroykaBarometer.Data.mmHg,
                TroykaBarometer.Data.Altitude);
    else
        sprintf(TroykaBarometer.DataRepr, "[%s] %s;", TroykaBarometer.Communicator.Name, UNREACHABLE);
}

static void TroykaBarometer_ReadPressure() {
    uint8_t data[3] = {0};
    I2C_ReadData3x8(&TroykaBarometer.Communicator,
                    0x80 | TROYKA_BAROMETER_PRESS_POUT_XL,
                    data);
    double millibars = (data[2] << 16 | (uint16_t) data[1] << 8 | data[0]) / 4096.0;
    TroykaBarometer.Data.Pressure = millibars * MILLIBARS_TO_PASCALS;
    TroykaBarometer.Data.mmHg = millibars * MILLIBARS_TO_MMHG;
    TroykaBarometer.Data.Altitude = (TroykaBarometer.Data.base_mmHg - TroykaBarometer.Data.mmHg) * 10.5f;
}

static void TroykaBarometer_ReadTemperature() {
    uint8_t data[2] = {0};
    I2C_ReadData2x8(&TroykaBarometer.Communicator,
                    0x80 | TROYKA_BAROMETER_TEMP_OUT_L,
                    data);
    TroykaBarometer.Data.Temperature = 42.5f + (float) (int16_t) ((data[1] << 8) | data[0]) / 480.0f;
}

void TroykaBarometer_Init() {
    /** Communication Section **/
    I2C_SetupCommunicator(&TroykaBarometer.Communicator,
                          "TroykaBarometer",
                          TROYKA_BAROMETER_ADDRESS,
                          TROYKA_BAROMETER_ID,
                          TROYKA_BAROMETER_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&TroykaBarometer.Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&TroykaBarometer.Communicator)){
        I2C_WriteData8(&TroykaBarometer.Communicator,
                       TROYKA_BAROMETER_CTRL_REG1,
                       TROYKA_BAROMETER_REG1_ODR1 |
                       TROYKA_BAROMETER_REG1_ODR2 |
                       TROYKA_BAROMETER_REG1_PD);
        HAL_Delay(50);
        TroykaBarometer_Calibrate();
        if (TroykaBarometer.Communicator.ConnectionStatus == HAL_OK)
            TroykaBarometer.Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&TroykaBarometer.Communicator);
#endif
}

void TroykaBarometer_ReadData() {
    if (TroykaBarometer.Communicator.ConnectionStatus == HAL_OK) {
        TroykaBarometer_ReadPressure();
        TroykaBarometer_ReadTemperature();
    }
    GenerateDataRepresentation();
}
