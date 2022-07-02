#pragma once
#include "I2C_Interface.h"
#include "CoreTypes.h"

/** Device Info **/
#define ADXL345_ADDRESS                             0x53
#define ADXL345_ID                                  0xE5        //Device ID

/** Registers Addresses **/
#define ADXL345_ID_REGISTER                         0x00        //Device ID Register
#define ADXL345_OFSX                                0x1E        //X-axis offset
#define ADXL345_OFSY                                0x1F        //Y-axis offset
#define ADXL345_OFSZ                                0x20        //Z-axis offset
#define ADXL345_BW_RATE                             0x2C        //Data rate and power mode control
#define ADXL345_POWER_CTL                           0x2D        //Power-saving features control
#define ADXL345_DATA_FORMAT                         0x31        //Data format control
#define ADXL345_DATA                                0x32        //Accelerometer XYZ LSB-MSB 6 bytes

/** Data rate Settings **/
//#define ADXL345_DATARATE                          0x09        //5Hz
#define ADXL345_DATARATE                            0x0A        //100Hz
//#define ADXL345_DATARATE                          0x0B        //200Hz

/** Resolution Settings **/
//#define ADXL345_ACC_SCALE                         0.0039      //2g
//#define ADXL345_ACC_RESOLUTION                    0x00        //2g
//#define ADXL345_ACC_SCALE                         0.0078      //4g
//#define ADXL345_ACC_RESOLUTION                    0x01        //4g
#define ADXL345_ACC_SCALE                           0.0156      //8g
#define ADXL345_ACC_RESOLUTION                      0x02        //8g
//#define ADXL345_ACC_SCALE                         0.0312      //16g
//#define ADXL345_ACC_RESOLUTION                    0x03        //16g

/** Functions Prototypes **/
void ADXL_Init(void);
void ADXL_ReadData(void);
