#include "I2C_Interface.h"
#include "CoreTypes.h"

/** Device Info **/
#define TROYKA_GYROSCOPE_ADDRESS					0x69 //I3G4250D
#define TROYKA_GYROSCOPE_ID_REGISTER 				0x0F
#define TROYKA_GYROSCOPE_ID 						0xD3

/** Registers Addresses **/
#define TROYKA_GYROSCOPE_CTRL_REG1 					0x20
#define TROYKA_GYROSCOPE_CTRL_REG4 					0x23
#define TROYKA_GYROSCOPE_OUT_X_L  					0x28

/** Device Values **/
#define TROYKA_GYROSCOPE_CTRL_REG1_X_EN             0x01
#define TROYKA_GYROSCOPE_CTRL_REG1_Y_EN             0x02
#define TROYKA_GYROSCOPE_CTRL_REG1_Z_EN             0x04
#define TROYKA_GYROSCOPE_CTRL_REG1_XYZ_EN           0x07

/** Resolution Settings **/
#define TROYKA_GYROSCOPE_RESET_RANGE                0x30
//#define TROYKA_GYROSCOPE_RANGE                    0                   //250DPS
//#define TROYKA_GYROSCOPE_SCALE                    0.00875f            //250DPS
#define TROYKA_GYROSCOPE_RANGE                      0x10                //500DPS
#define TROYKA_GYROSCOPE_SCALE                      0.0175              //500DPS
//#define TROYKA_GYROSCOPE_RANGE                    0x30                //2000DPS
//#define TROYKA_GYROSCOPE_SCALE                    0.07                //2000DPS

/** Functions Prototypes **/
void TroykaGyroscope_Init(void);
void TroykaGyroscope_ReadData(void);