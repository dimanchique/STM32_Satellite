#include "I2C_Interface.h"
#include "CoreTypes.h"

/** Device Info **/
#define TROYKA_BAROMETER_ADDRESS                        0x5C
#define TROYKA_BAROMETER_ID_REGISTER                    0x0F
#define TROYKA_BAROMETER_ID                             0xBB
/** Registers Addresses **/
#define TROYKA_BAROMETER_CTRL_REG1                      0x20
#define TROYKA_BAROMETER_STATUS_REG                     0x27
#define TROYKA_BAROMETER_PRESS_POUT_XL_REH              0x28
#define TROYKA_BAROMETER_PRESS_OUT_L                    0x29
#define TROYKA_BAROMETER_PRESS_OUT_H                    0x2A
#define TROYKA_BAROMETER_TEMP_OUT_L                     0x2B
#define TROYKA_BAROMETER_TEMP_OUT_H                     0x2C

/** Device Values **/
#define TROYKA_BAROMETER_REG1_ODR0                      0x10
#define TROYKA_BAROMETER_REG1_ODR1                      0x20
#define TROYKA_BAROMETER_REG1_ODR2                      0x40
#define TROYKA_BAROMETER_REG1_PD                        0x80
#define MILLIBARS_TO_PASCALS                            100
#define MILLIBARS_TO_MMHG                               0.75f

/** Functions Prototypes **/
void TroykaBarometer_Init(void);
void TroykaBarometer_ReadData(void);
