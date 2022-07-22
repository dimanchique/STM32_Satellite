#include "TroykaBarometer.h"
#include "Logger.h"

Device_TypeDefStruct TrBaro = {0};

static struct BarometerData TrBaro_Data = {0};
static I2C_TypeDefStruct TrBaro_Communicator = {0};

static void TrBaro_Calibrate();

static void GenerateDataRepresentation();

static void TrBaro_ReadPressure();

static void TrBaro_ReadTemperature();

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
    if (I2C_DeviceCheckedAndVerified(&TrBaro_Communicator)) {
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

static void TrBaro_Calibrate() {
    TrBaro_ReadData();
    TrBaro_Data.mmHg_ref = TrBaro_Data.mmHg;
}

static void GenerateDataRepresentation() {
    if (TrBaro_Communicator.ConnectionStatus == HAL_OK)
        sprintf(TrBaro.DataRepr,
                "[%s] %.2f %.3f %.3f %.3f;",
                TrBaro.DeviceName,
                TrBaro_Data.Temperature,
                TrBaro_Data.Pressure,
                TrBaro_Data.mmHg,
                TrBaro_Data.Altitude);
    else
        sprintf(TrBaro.DataRepr, "[%s] %s;", TrBaro.DeviceName, UNREACHABLE);
}

static void TrBaro_ReadPressure() {
    uint8_t data[3] = {0};
    I2C_ReadDataNx8(&TrBaro_Communicator,
                    0x80 | TR_BARO_POUT,
                    data, 3);
    double millibars = (data[2] << 16 | (uint16_t) data[1] << 8 | data[0]) / 4096.0;
    TrBaro_Data.Pressure = mB_to_Pa(millibars);
    TrBaro_Data.mmHg = mB_to_mmHg(millibars);
    TrBaro_Data.Altitude = mmHg_to_Altitude(TrBaro_Data.mmHg_ref, TrBaro_Data.mmHg);
}

static void TrBaro_ReadTemperature() {
    uint8_t data[2] = {0};
    I2C_ReadDataNx8(&TrBaro_Communicator,
                    0x80 | TR_BARO_TOUT,
                    data, 2);
    TrBaro_Data.Temperature = 42.5f + (float) (int16_t) ((data[1] << 8) | data[0]) / 480.0f;
}
