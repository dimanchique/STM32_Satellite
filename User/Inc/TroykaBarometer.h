#include "I2C_Process.h"
#include "ModuleCoreTypes.h"

/** Device Info **/
#define TROYKA_BAROMETER_ADDRESS                            0x5C
#define TROYKA_BAROMETER_ID_REGISTER                        0x0F
#define TROYKA_BAROMETER_LPS311_ID                            0xBB //LPS311AP
#define TROYKA_BAROMETER_LPS25_ID                            0xBD //LPS25HB
/** Registers Addresses **/
#define TROYKA_BAROMETER_REF_P_XL                            0x08
#define TROYKA_BAROMETER_REF_P_L                            0x09
#define TROYKA_BAROMETER_REF_P_H                            0x0A
#define TROYKA_BAROMETER_RES_CONF                            0x10
#define TROYKA_BAROMETER_CTRL_REG1                        0x20
#define TROYKA_BAROMETER_CTRL_REG2                        0x21
#define TROYKA_BAROMETER_CTRL_REG3                        0x22
#define TROYKA_BAROMETER_INT_CFG_REG                    0x23
#define TROYKA_BAROMETER_INT_SOURCE_REG                0x24
#define TROYKA_BAROMETER_THS_P_LOW_REG                0x25
#define TROYKA_BAROMETER_THS_P_HIGH_REG                0x26
#define TROYKA_BAROMETER_STATUS_REG                        0x27
#define TROYKA_BAROMETER_PRESS_POUT_XL_REH        0x28
#define TROYKA_BAROMETER_PRESS_OUT_L                    0x29
#define TROYKA_BAROMETER_PRESS_OUT_H                    0x2A
#define TROYKA_BAROMETER_TEMP_OUT_L                        0x2B
#define TROYKA_BAROMETER_TEMP_OUT_H                        0x2C
#define TROYKA_BAROMETER_AMP_CTRL                            0x30

/** Device Values **/
#define LPS_CTRL_REG1_ODR0                                        0x10
#define LPS_CTRL_REG1_ODR1                                        0x20
#define LPS_CTRL_REG1_ODR2                                        0x40
#define LPS_CTRL_REG1_PD                                            0x80
#define CELSIUS_TO_KELVIN                                        273.15
#define MILLIBARS_TO_PASCALS                                    100
#define MILLIBARS_TO_MILLIMETERSHG                        0.75

/** LPS311AP/LPS25HB Device Struct **/
typedef struct {
    I2C_DeviceStruct Communicator;
    uint8_t ControlData;
    struct BarometerData Data;
    struct ModuleExtraData ExtraData;
    char DataRepr[50];
} TroykaBarometer_TypeDef;

/** Functions Prototypes **/
void TroykaBarometer_Init(void);
void TroykaBarometer_ReadData(void);
