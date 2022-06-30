#include "AnalogBarometer.h"

DeviceTypeDef AnalogBarometer = {0};
ADC_CommunicatorStruct Communicator = {0};
struct BarometerData AnalogBaroData;
extern ADC_HandleTypeDef hadc2;

static void GenerateDataRepresentation();
static void AnalogBaro_Calibrate();

void AnalogBaro_Init() {
    Communicator.Channel = 0;
    Communicator.Instance = &hadc2;
    AnalogBarometer.DeviceName = "AnalogBaro";
    HAL_ADCEx_Calibration_Start(Communicator.Instance, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    AnalogBaro_Calibrate();
}

void AnalogBaro_ReadData(void){
    HAL_ADC_Start(Communicator.Instance);
    HAL_ADC_PollForConversion(Communicator.Instance, 10);
    uint16_t ADC_Raw = HAL_ADC_GetValue(Communicator.Instance);
    HAL_ADC_Stop(Communicator.Instance);

    //Note: 3.3V supply = 0.2V-2.7V range (0->100 kPa)
    float Voltage = (float) ADC_Raw * ADC_Resolution - 0.2f; //add 0.2v offset
    AnalogBaroData.Pressure = VoltageToPressure(Voltage);
    AnalogBaroData.mmHg = Pa_to_mmHg(AnalogBaroData.Pressure);
    AnalogBaroData.Altitude = AnalogBaroData.base_mmHg - AnalogBaroData.mmHg *10.5;
    GenerateDataRepresentation();
}

static void GenerateDataRepresentation() {
    sprintf(AnalogBarometer.DataRepr,
            "[%s] %.3f %.3f %.3f;",
            AnalogBarometer.DeviceName,
            AnalogBaroData.Pressure,
            AnalogBaroData.mmHg,
            AnalogBaroData.Altitude);
}

static void AnalogBaro_Calibrate() {
    AnalogBaro_ReadData();
    AnalogBaroData.base_mmHg = AnalogBaroData.mmHg;
}
