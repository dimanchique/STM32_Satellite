#include "I2C_Interface.h"
#include "CoreTypes.h"

/** Device Info **/
#define TROYKA_ACCELEROMETER_ADDRESS                    0x18
#define TROYKA_ACCELEROMETER_ID_REGISTER                0x0F
#define TROYKA_ACCELEROMETER_ID                         0x32

/** Registers Addresses */
#define TROYKA_ACCELEROMETER_CTRL_REG1                  0x20
#define TROYKA_ACCELEROMETER_CTRL_REG4                  0x23
#define TROYKA_ACCELEROMETER_OUT_X_L                    0x28

/** Device Values **/
#define TROYKA_ACCELEROMETER_CTRL_REG1_X_EN             0x01
#define TROYKA_ACCELEROMETER_CTRL_REG1_Y_EN             0x02
#define TROYKA_ACCELEROMETER_CTRL_REG1_Z_EN             0x04
#define TROYKA_ACCELEROMETER_CTRL_REG1_XYZ_EN           0x07

/** Resolution Settings **/
#define TROYKA_ACCELEROMETER_RESET_RANGE                0x30
//#define TROYKA_ACCELEROMETER_RANGE                    0                   //2G
//#define TROYKA_ACCELEROMETER_SCALE                    0.00006103515625    //2G
#define TROYKA_ACCELEROMETER_RANGE                      0x10                //4G
#define TROYKA_ACCELEROMETER_SCALE                      0.0001220703125     //4G
//#define TROYKA_ACCELEROMETER_RANGE                    0x30                //8G
//#define TROYKA_ACCELEROMETER_SCALE                    0.000238037109375   //8G

/** Functions Prototypes **/
void TroykaAccelerometer_Init(void);
void TroykaAccelerometer_ReadData(void);