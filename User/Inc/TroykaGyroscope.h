#include "I2C_Interface.h"
#include "CoreTypes.h"

/** Device Info **/
#define TRO_GYRO_ADDR					0x69 //I3G4250D
#define TR_GYRO_ID_REG 				    0x0F
#define TR_GYRO_ID 						0xD3

/** Registers Addresses **/
#define TR_GYRO_CR1 					0x20
#define TR_GYRO_CR4 					0x23
#define TR_GYRO_OUT                     0x28

/** Device Values **/
#define TR_GYR_CR1_XYZ_EN               0x07

/** Resolution Settings **/
#define TR_GYRO_RESET_RANGE             0x30
//#define TR_GYRO_RESET_RANGE           0                   //250DPS
//#define TR_GYRO_RESET_RANGE           0.00875f            //250DPS
#define TR_GYRO_RANGE                   0x10                //500DPS
#define TR_GYRO_SCALE                   0.0175              //500DPS
//#define TR_GYRO_RANGE                 0x30                //2000DPS
//#define TR_GYRO_SCALE                 0.07                //2000DPS

/** Functions Prototypes **/
void TrGyro_Init(void);
void TrGyro_ReadData(void);