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
    Barometer.Communicator.CommAddress = CommunicationAddress(TROYKA_BAROMETER_ADDRESS);
    Barometer.Communicator.FactAddress = TROYKA_BAROMETER_ADDRESS;
    Barometer.Communicator.Device_ID = TROYKA_BAROMETER_LPS311_ID;
    Barometer.Communicator.ID_Register = TROYKA_BAROMETER_ID_REGISTER;
    Barometer.ControlData = 0;
    Barometer.ControlData |= LPS_CTRL_REG1_ODR1 | LPS_CTRL_REG1_ODR2;
#ifdef ENABLE_DEBUG
    LogState(Barometer.Communicator);
#endif
    /** Setup Section **/
    CheckDeviceState(&Barometer.Communicator);
    if (Barometer.Communicator.ConnectionStatus == HAL_OK) {
        Verify_Device(&Barometer.Communicator);
        if (Barometer.Communicator.State == ID_Check_Error) {
            Barometer.Communicator.Name = "LPS25";
            Barometer.Communicator.Device_ID = TROYKA_BAROMETER_LPS25_ID;
            Verify_Device(&Barometer.Communicator);
            Barometer.ControlData = LPS_CTRL_REG1_ODR2;
        }
        if (Barometer.Communicator.ConnectionStatus == HAL_OK) {
            Barometer.ControlData |= LPS_CTRL_REG1_PD;
            I2C_WriteData8(&Barometer.Communicator, TROYKA_BAROMETER_CTRL_REG1, Barometer.ControlData);
            HAL_Delay(100);
            TroykaBarometer_Calibrate();
        }
        if (Barometer.Communicator.ConnectionStatus == HAL_OK) Barometer.Communicator.State = Initialized;
    }
#ifdef ENABLE_DEBUG
    LogState(Barometer.Communicator);
#endif
}

static void TroykaBarometer_ReadPressure() {
    if (Barometer.Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[3] = {0};
        uint32_t rawPressure;
        I2C_ReadData3x8(&Barometer.Communicator,
                        0x80 | TROYKA_BAROMETER_PRESS_POUT_XL_REH,
                        data);
        rawPressure = (uint32_t) data[2] << 16 | (uint16_t) data[1] << 8 | data[0];
        double millibars = rawPressure / 4096.0;
        Barometer.Data.Pressure = millibars * MILLIBARS_TO_PASCALS;
        Barometer.Data.mmHg = millibars * MILLIBARS_TO_MILLIMETERSHG;
        Barometer.Data.Altitude = (Barometer.Data.base_mmHg - Barometer.Data.mmHg) * 10.5;
    } else {
        Barometer.Data.Pressure = 0;
        Barometer.Data.mmHg = 0;
        Barometer.Data.Altitude = 0;
    }
}

static void TroykaBarometer_ReadTemperature() {
    if (Barometer.Communicator.ConnectionStatus == HAL_OK) {
        uint8_t data[2] = {0};
        I2C_ReadData8(&Barometer.Communicator,
                      TROYKA_BAROMETER_TEMP_OUT_L,
                      &data[0]);
        I2C_ReadData8(&Barometer.Communicator,
                      TROYKA_BAROMETER_TEMP_OUT_H,
                      &data[1]);
        Barometer.ExtraData.Temperature = 42.5 + (float) (int16_t) ((data[1] << 8) | data[0]) / 480.0;
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
