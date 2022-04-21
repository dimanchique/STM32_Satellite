#include "TroykaBarometer.h"
#include "Logger.h"

TroykaBarometer_TypeDef Barometer = {0};

static void TroykaBarometer_Calibrate() {
    TroykaBarometer_ReadData();
    Barometer.Data.base_mmHg = Barometer.Data.mmHg;
}

static void GenerateDataRepresentation(uint8_t ConnectionValid) {
    if (ConnectionValid)
        sprintf(Barometer.DataRepr,
                "[%s] %.2f; %.3f; %.3f; %.3f;",
                Barometer.Communicator.Name,
                Barometer.ExtraData.Temperature,
                Barometer.Data.Pressure,
                Barometer.Data.mmHg,
                Barometer.Data.Altitude);
    else
        sprintf(Barometer.DataRepr, "[%s] NULL;", Barometer.Communicator.Name);
}

void TroykaBarometer_Init() {
    /** Communication Section **/
    Barometer.Communicator.Name = "LPS311";
    Barometer.Communicator.State = NotInitialized;
    Barometer.Communicator.CommAddress = TROYKA_BAROMETER_ADDRESS<<1;
    Barometer.Communicator.FactAddress = TROYKA_BAROMETER_ADDRESS;
    Barometer.Communicator.Device_ID = TROYKA_BAROMETER_ID;
    Barometer.Communicator.ID_Register = TROYKA_BAROMETER_ID_REGISTER;
#ifdef ENABLE_DEBUG
    LogDeviceState(&Barometer.Communicator);
#endif
    /** Setup Section **/
    CheckDeviceState(&Barometer.Communicator);
    if (Barometer.Communicator.ConnectionStatus == HAL_OK) {
        Verify_Device(&Barometer.Communicator);
        if (Barometer.Communicator.ConnectionStatus == HAL_OK) {
            I2C_WriteData8(&Barometer.Communicator, TROYKA_BAROMETER_CTRL_REG1, LPS_CTRL_REG1_ODR1 | LPS_CTRL_REG1_ODR2 | LPS_CTRL_REG1_PD);
            HAL_Delay(50);
            TroykaBarometer_Calibrate();
        }
        if (Barometer.Communicator.ConnectionStatus == HAL_OK)
            Barometer.Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&Barometer.Communicator);
#endif
}

static void TroykaBarometer_ReadPressure() {
    if (Barometer.Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[3] = {0};
        I2C_ReadData3x8(&Barometer.Communicator,
                        0x80 | TROYKA_BAROMETER_PRESS_POUT_XL_REH,
                        data);
        double millibars = (data[2] << 16 | (uint16_t) data[1] << 8 | data[0]) / 4096.0;
        Barometer.Data.Pressure = millibars * MILLIBARS_TO_PASCALS;
        Barometer.Data.mmHg = millibars * MILLIBARS_TO_MMHG;
        Barometer.Data.Altitude = (Barometer.Data.base_mmHg - Barometer.Data.mmHg) * 10.5f;
    } else {
        Barometer.Data.Pressure = 0;
        Barometer.Data.mmHg = 0;
        Barometer.Data.Altitude = 0;
    }
}

static void TroykaBarometer_ReadTemperature() {
    if (Barometer.Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[2] = {0};
        I2C_ReadData2x8(&Barometer.Communicator,
                        TROYKA_BAROMETER_TEMP_OUT_L,
                        data);
        Barometer.ExtraData.Temperature = 42.5f + (float) (int16_t) ((data[1] << 8) | data[0]) / 480.0f;
    } else {
        Barometer.ExtraData.Temperature = 0;
    }
}

void TroykaBarometer_ReadData() {
    if (Barometer.Communicator.ConnectionStatus == HAL_OK) {
        TroykaBarometer_ReadPressure();
        TroykaBarometer_ReadTemperature();
    } else {
        Barometer.Data.Pressure = 0;
        Barometer.Data.mmHg = 0;
        Barometer.Data.Altitude = 0;
    }
    GenerateDataRepresentation(Barometer.Communicator.ConnectionStatus == HAL_OK);
}
