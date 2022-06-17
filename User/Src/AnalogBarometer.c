#include "AnalogDevice.h"

DeviceTypeDef AnalogBarometer = {0};
ADC_CommunicatorStruct Communicator = {0};
struct BarometerData AnalogBaroData;
extern ADC_HandleTypeDef hadc1;

static void GenerateDataRepresentation() {
    sprintf(AnalogBarometer.DataRepr,
            "[%s] %.3f %.3f %.3f;",
            AnalogBarometer.DeviceName,
            AnalogBaroData.Pressure,
            AnalogBaroData.mmHg,
            AnalogBaroData.Altitude);
}

static void AnalogBarometer_Calibrate(void){
    AnalogBarometer_ReadData();
    AnalogBaroData.base_mmHg = AnalogBaroData.mmHg;
}

void AnalogBarometer_Init(void) {
    Communicator.Channel = 0;
    Communicator.Instance = &hadc1;
    AnalogBarometer.DeviceName = "AnalogBaro";
    AnalogBarometer_Calibrate();
}

void AnalogBarometer_ReadData(){
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint16_t ADC_Raw = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    //Note: 3.3V supply = 0.2V-2.7V range (0->100 kPa)
    float Voltage = (float) ADC_Raw * ADC_Resolution - 0.2f; //add 0.2v offset
    AnalogBaroData.Pressure = VoltageToPressure(Voltage);
    AnalogBaroData.mmHg = Pa_to_mmHg(AnalogBaroData.Pressure);
    AnalogBaroData.Altitude = AnalogBaroData.base_mmHg - AnalogBaroData.mmHg *10.5;
    GenerateDataRepresentation();
}
