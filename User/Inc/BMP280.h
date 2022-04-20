#pragma once
#include "I2C_Interface.h"
#include "ModuleCoreTypes.h"

/** Device Info **/
#define BMP280_ADDRESS 									0x76
#define BMP280_ID 										0x58

/** Registers Addresses **/
#define BMP280_ID_REGISTER								0xD0 	//Device ID Register
#define BMP280_TEMP_REGISTER 							0xFA 	//Temperature 0xFA-0xFC (MSB-LSB-XLSB)
#define BMP280_PRESSURE_REGISTER						0xF7 	//Temperature 0xF7-0xF9 (MSB-LSB-XLSB)
#define BMP280_CONFIG_REGISTER 							0xF5	//Configuration Register
#define BMP280_CTRL_REGISTER							0xF4 	//Control Register (D0-D1 - Power Control)
#define BMP280_STATUS_REGISTER							0xF3 	//Status Register (D0 and D3 describes data status)
#define BMP280_RESET_REGISTER							0xE0 	//Put 'BMP_SoftReset' value here to soft reset chip
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

/** Pressure Oversampling Settings **/
//#define BMP280_PRESSURE_OVERSAMPLING 					0x00	//Don't Measure
//#define BMP280_PRESSURE_OVERSAMPLING					0x01	//Ultra-Low Power           16 bit 	// -> Temperature Ultra-Low Power (Recommended)
//#define BMP280_PRESSURE_OVERSAMPLING 					0x02	//Low Power 				17 bit 	// -> Temperature Ultra-Low Power (Recommended)
//#define BMP280_PRESSURE_OVERSAMPLING 					0x03	//Standard Resolution 		18 bit 	// -> Temperature Ultra-Low Power (Recommended)
#define BMP280_PRESSURE_OVERSAMPLING 					0x04	//High Resolution 			19 bit 	// -> Temperature Ultra-Low Power (Recommended)
//#define BMP280_PRESSURE_OVERSAMPLING					0x05	//Ultra-High Resolution 	20 bit 	// -> Temperature Low Power (Recommended)

/** Temperature Oversampling Settings **/
//#define BMP280_TEMPERATURE_OVERSAMPLING 				0x00	//Dont Measure
#define BMP280_TEMPERATURE_OVERSAMPLING					0x01	//Ultra-Low Power           16 bit
//#define BMP280_TEMPERATURE_OVERSAMPLING 				0x02	//Low Power 			    17 bit

/** Power Settings **/
#define BMP280_POWER_MODE								0x03	//NormalMode; Continuous measuring with period T (Standby Time)
//#define BMP280_POWER_MODE								0x02	//ForceMode; Make one measure then go to Sleep Mode
//#define BMP280_POWER_MODE								0x00	//SleepMode; No measurements, low power consumption (Default State)

/** Standby Time Settings **/
//#define BMP280_STANDBYTIME							0x00	//Standby Time = 0.5ms
#define BMP280_STANDBYTIME								0x01	//Standby Time = 62.5ms
//#define BMP280_STANDBYTIME							0x02	//Standby Time = 125ms
//#define BMP280_STANDBYTIME							0x03	//Standby Time = 250ms
//#define BMP280_STANDBYTIME							0x04	//Standby Time = 500ms
//#define BMP280_STANDBYTIME							0x05	//Standby Time = 1000ms
//#define BMP280_STANDBYTIME							0x06	//Standby Time = 2000ms
//#define BMP280_STANDBYTIME							0x07	//Standby Time = 4000ms

/** Filter Coefficients Settings **/
//#define BMP280_FILTERING_TYPE							0x00    //Measure without filtering
//#define BMP280_FILTERING_TYPE							0x01    //IIR Coefficient = 2  (Bandwidth = 0.223*ODR)
//#define BMP280_FILTERING_TYPE							0x02    //IIR Coefficient = 4  (Bandwidth = 0.092*ODR)
#define BMP280_FILTERING_TYPE							0x03    //IIR Coefficient = 8  (Bandwidth = 0.042*ODR)
//#define BMP280_FILTERING_TYPE							0x04    //IIR Coefficient = 16 (Bandwidth = 0.021*ODR)

/** Value to reset BMP280 and value to detect updating status **/
#define BMP280_SOFTRESET 								0xB6
#define BMP280_IS_UPDATING								0x01    //BMP280 updating flag

/** Functions Prototypes **/
void BMP_Init(void);
void BMP_ReadData(void);
