#include "I2C_Interface.h"
#include "CoreTypes.h"

/** Device Info **/
#define TROYKA_BAROMETER_ADDRESS                        0x5C
#define TROYKA_BAROMETER_ID_REGISTER                    0x0F
#define TROYKA_BAROMETER_ID                             0xBB

/** Registers Addresses **/
#define TROYKA_BAROMETER_CTRL_REG1                      0x20
#define TROYKA_BAROMETER_PRESS_OUT                      0x28 //Pressure XLSB-LSB-MSB 3 bytes
#define TROYKA_BAROMETER_TEMP_OUT                       0x2B //Temperature LSB-MSB 2 bytes

/** Device Values **/
#define TROYKA_BAROMETER_REG1_ODR1                      0x20
#define TROYKA_BAROMETER_REG1_ODR2                      0x40
#define TROYKA_BAROMETER_REG1_PD                        0x80

/** Functions Prototypes **/
void TroykaBarometer_Init(void);
void TroykaBarometer_ReadData(void);
