#include "AnalogDevice.h"

AnalogBarometer_TypeDef AnalogBarometer;

static void GenerateDataRepresentation() {
    sprintf(AnalogBarometer.DataRepr,
            "[%s] %.3f %.3f %.3f;",
            AnalogBarometer.DeviceName,
            AnalogBarometer.Data.Pressure,
            AnalogBarometer.Data.mmHg,
            AnalogBarometer.Data.Altitude);
}

void AnalogBarometer_Init(void) {
    strcpy(AnalogBarometer.DeviceName, "AnalogBarometer");
    AnalogBarometer.ADC_Channel = 0;
    AnalogBarometer_ReadData();
    AnalogBarometer.Data.base_mmHg = AnalogBarometer.Data.mmHg;
}

void AnalogBarometer_ReadData(){
    //1. read data using ADC
    //Note: 3.3V supply = 0.2V-2.7V range (0->100 kPa)
    float Voltage = 1.0f - 0.2f; //put calculation of voltage from ADC value
    float Range = 100000.0f / (2.7f - 0.2f);
    float Pressure = Voltage * Range;
    AnalogBarometer.Data.Pressure = Pressure;
    AnalogBarometer.Data.mmHg = AnalogBarometer.Data.Pressure * 0.0075006156130264;
    AnalogBarometer.Data.Altitude = (AnalogBarometer.Data.base_mmHg - AnalogBarometer.Data.mmHg) * 10.5;
    GenerateDataRepresentation();
}
