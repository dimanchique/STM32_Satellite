#pragma once
#include "CoreTypes.h"

#define VoltageToPressure(x)        x * (100000.0f / (2.7f - 0.2f)) * 10.0f
#define ADC_Resolution              (3.3f / 65535)

void AnalogBaro_Init(void);
void AnalogBaro_ReadData();
