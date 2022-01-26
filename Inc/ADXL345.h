#include "I2C_Process.h"
//------------------------------------------------
/* Device Info */
#define ADXL345_ADDRESS 						0x53<<1
#define ADXL345_ID 								0xE5 //Device ID
//------------------------------------------------
/* Registers Addresses */
#define ADXL345_ID_REGISTER 					0x00 //Device ID Register
#define ADXL345_OFSX 							0x1E //X-axis offset 
#define ADXL345_OFSY 							0x1F //Y-axis offset 
#define ADXL345_OFSZ							0x20 //Z-axis offset 
#define ADXL345_BW_RATE 						0x2C //Data rate and power mode control 
#define ADXL345_POWER_CTL 						0x2D //Power-saving features control 
#define ADXL345_DATA_FORMAT 					0x31 //Data format control 
#define ADXL345_DATAX0 							0x32 //X-Axis Data 0 
#define ADXL345_DATAX1 							0x33 //X-Axis Data 1 
#define ADXL345_DATAY0 							0x34 //Y-Axis Data 0 
#define ADXL345_DATAY1 							0x35 //Y-Axis Data 1 
#define ADXL345_DATAZ0 							0x36 //Z-Axis Data 0 
#define ADXL345_DATAZ1 							0x37 //Z-Axis Data 1 
//------------------------------------------------
/* Parameters */

//DATA RATE//
//#define ADXL345_Rate50Hz
#define ADXL345_Rate100Hz
//#define ADXL345_Rate200Hz

//ACCELERATION//
//#define ADXL345_2g
//#define ADXL345_4g
#define ADXL345_8g
//#define ADXL345_16g
//------------------------------------------------
#ifdef ADXL345_Rate50Hz
#define ADXL345_DATARATE						0x09
#endif

#ifdef ADXL345_Rate100Hz
#define ADXL345_DATARATE						0x0A
#endif

#ifdef ADXL345_Rate200Hz
#define ADXL345_DATARATE						0x0B
#endif

#ifdef ADXL345_2g
#define ADXL345_ACC_SCALE						256.0
#define ADXL345_ACC_RESOLUTION					0x00
#endif

#ifdef ADXL345_4g
#define ADXL345_ACC_SCALE						128.0
#define ADXL345_ACC_RESOLUTION					0x01
#endif

#ifdef ADXL345_8g
#define ADXL345_ACC_SCALE						64.0
#define ADXL345_ACC_RESOLUTION					0x02
#endif

#ifdef ADXL345_16g
#define ADXL345_ACC_SCALE						32.0
#define ADXL345_ACC_RESOLUTION					0x03
#endif

enum AdxlBitNum{
D0,D1,D2,D3,D4,D5,D6,D7
};

typedef struct{
	I2C_Communicator Communicator;	
	
	uint8_t Datarate;
	uint8_t DataFormat;
	uint8_t Power;
} ADXL_Init_TypeDef;
//------------------------------------------------
void ADXL_Init(void);
void ADXL_DefaultSettings(void);
void ADXL_Read(double* x, double* y, double* z);
