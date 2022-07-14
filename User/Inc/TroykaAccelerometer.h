#pragma once
#include "I2C_Interface.h"
#include "CoreTypes.h"

/** Device Info **/
#define TR_ACC_ADDR                     0x18
#define TR_ACC_ID_REG                   0x0F
#define TR_ACC_ID                       0x32

/** Registers Addresses */
#define TR_ACC_CR1                      0x20
#define TR_ACC_CR4                      0x23
#define TR_ACC_OUT                      0x28 //Accelerometer XYZ LSB-MSB 6 bytes

/** Device Values **/
#define TR_ACC_PM0                      0x20
#define TR_ACC_CR1_XYZ_EN               0x07

/** Resolution Settings **/
#define TR_ACC_RESET_RANGE                0x30
//#define TR_ACC_RANGE                    0                   //2G
//#define TR_ACC_SCALE                    0.00006103515625    //2G
#define TR_ACC_RANGE                      0x10                //4G
#define TR_ACC_SCALE                      0.0001220703125     //4G
//#define TR_ACC_RANGE                    0x30                //8G
//#define TR_ACC_SCALE                    0.000238037109375   //8G

/** Functions Prototypes **/
void TrAcc_Init(void);
void TrAcc_ReadData(void);
