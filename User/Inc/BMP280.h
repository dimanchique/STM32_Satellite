#pragma once
#include "I2C_Process.h"
#include "ModuleCoreTypes.h"

/** Device Info **/
#define BMP280_ADDRESS 									0x76
#define BMP280_ID 										0x58

/** Registers Addresses **/
#define BMP280_TEMP_REGISTER 							0xFA 	//Temperature 0xFA-0xFC (MSB-LSB-XLSB)
#define BMP280_PRESSURE_REGISTER						0xF7 	//Temperature 0xF7-0xF9 (MSB-LSB-XLSB)
#define BMP280_CONFIG_REGISTER 							0xF5	//Configuration Register
#define BMP280_CTRL_REGISTER							0xF4 	//Control Register (D0-D1 - Power Control)
#define BMP280_STATUS_REGISTER							0xF3 	//Status Register (D0 and D3 describes data status)
#define BMP280_RESET_REGISTER							0xE0 	//Put 'BMP_SoftReset' value here to soft reset chip
#define BMP280_ID_REGISTER								0xD0 	//Device ID Register
#define BMP280_REGISTER_DIG_T1 							0x88
#define BMP280_REGISTER_DIG_T2 							0x8A
#define BMP280_REGISTER_DIG_T3 							0x8C
#define BMP280_REGISTER_DIG_P1 							0x8E
#define BMP280_REGISTER_DIG_P2 							0x90
#define BMP280_REGISTER_DIG_P3 							0x92
#define BMP280_REGISTER_DIG_P4 							0x94
#define BMP280_REGISTER_DIG_P5 							0x96
#define BMP280_REGISTER_DIG_P6 							0x98
#define BMP280_REGISTER_DIG_P7 							0x9A
#define BMP280_REGISTER_DIG_P8 							0x9C
#define BMP280_REGISTER_DIG_P9 							0x9E

/** Oversampling Settings **/
#define BMP280_SKIPMEASURING 							0x00	//Dont Measure
#define BMP280_OVERSAMPLINGx1							0x01	//Ultra-Low Power           16 bit 	//if Pressure x1	-> Temperature x1 (Recommended)
#define BMP280_OVERSAMPLINGx2 							0x02	//Low Power 				17 bit 	//if Pressure x2	-> Temperature x1 (Recommended)
#define BMP280_OVERSAMPLINGx4 							0x03	//Standart Resolution 		18 bit 	//if Pressure x4 	-> Temperature x1 (Recommended)
#define BMP280_OVERSAMPLINGx8 							0x04	//High Resolution 			19 bit 	//if Pressure x8 	-> Temperature x1 (Recommended)
#define BMP280_OVERSAMPLINGx16							0x05	//Ultra-High Resolution 	20 bit 	//if Pressure x16   -> Temperature x2 (Recommended)

/** Power Settings **/
#define BMP280_NORMALMODE								0x03	//Continuous measuring with period T (Standby Time)
#define BMP280_FORCEDMODE								0x02	//Make one measure then go to Sleep Mode
#define BMP280_SLEEPMODE								0x00	//DEFAULT! No measurements, low power consumption

/** Standby Time Settings **/
#define BMP280_STANDBYTIME1								0x00	//Standby Time = 0.5ms
#define BMP280_STANDBYTIME2								0x01	//Standby Time = 62.5ms
#define BMP280_STANDBYTIME3								0x02	//Standby Time = 125ms
#define BMP280_STANDBYTIME4								0x03	//Standby Time = 250ms
#define BMP280_STANDBYTIME5								0x04	//Standby Time = 500ms
#define BMP280_STANDBYTIME6								0x05	//Standby Time = 1000ms
#define BMP280_STANDBYTIME7								0x06	//Standby Time = 2000ms
#define BMP280_STANDBYTIME8								0x07	//Standby Time = 4000ms

/** Filter Coefficients Settings **/
#define BMP280_NOFILTERING								0x00    //Measure without filtering
#define BMP280_FILTERCOEFF2								0x01    //IIR Coefficient = 2  (Bandwidth = 0.223*ODR)
#define BMP280_FILTERCOEFF4								0x02    //IIR Coefficient = 4  (Bandwidth = 0.092*ODR)
#define BMP280_FILTERCOEFF8								0x03    //IIR Coefficient = 8  (Bandwidth = 0.042*ODR)
#define BMP280_FILTERCOEFF16							0x04    //IIR Coefficient = 16 (Bandwidth = 0.021*ODR)

/** Value to reset BMP 280 and value to determine updating status **/
#define BMP280_SOFTRESET 								0xB6    //Put IC to SoftReset Mode
#define BMP280_IS_UPDATING								0x01    //BMP280 updating

/** Calibration Data Struct **/
struct BMP280_CalibrationData {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
};

/** Configuration Data Struct **/
struct BMP280_ConfigurationData {
    uint8_t FilterCoefficient;
    uint8_t Temperature_Oversampling;
    uint8_t Pressure_Oversampling;
    uint8_t StandbyTime;
    uint8_t Power;
};

/** BMP280 Device Struct **/
typedef struct {
    I2C_DeviceStruct Communicator;

    struct BMP280_ConfigurationData Configuration;
    struct BMP280_CalibrationData CalibrationCoefficients;
    struct BarometerData Data;
    struct ModuleExtraData ExtraData;
    char DataRepr[50];
} BMP280_TypeDef;

/** Functions Prototypes **/
void BMP_Init(void);
void BMP_ReadData(void);
