#include "I2C_Interface.h"
#include "CoreTypes.h"

/** Device Info **/
#define TR_BARO_ADDR                        0x5C
#define TR_BARO_ID_REG                      0x0F
#define TR_BARO_ID                          0xBB

/** Registers Addresses **/
#define TR_BARO_CR1                         0x20
#define TR_BARO_POUT                        0x28 //Pressure XLSB-LSB-MSB 3 bytes
#define TR_BAR_TOUT                         0x2B //Temperature LSB-MSB 2 bytes

/** Device Values **/
#define TR_BARO_ODR1                        0x20
#define TR_BARO_ODR2                        0x40
#define TR_BARO_PD                          0x80

/** Functions Prototypes **/
void TrBaro_Init(void);
void TrBaro_ReadData(void);
