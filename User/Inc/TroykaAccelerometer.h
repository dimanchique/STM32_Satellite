#include "I2C_Process.h"
#include "ModuleCoreTypes.h"

/** Device Info **/
#define TROYKA_ACCELEROMETER_ADDRESS                        0x18 //LIS331DLH
#define TROYKA_ACELEROMETER_ID_REGISTER                0x0F
#define TROYKA_ACELEROMETER_ID                                    0x32

/** Registers Addresses */
#define TROYKA_ACELEROMETER_CTRL_REG1                    0x20
#define TROYKA_ACELEROMETER_CTRL_REG2                    0x21
#define TROYKA_ACELEROMETER_CTRL_REG3                    0x22
#define TROYKA_ACELEROMETER_CTRL_REG4                    0x23
#define TROYKA_ACELEROMETER_CTRL_REG5                    0x24
#define TROYKA_ACELEROMETER_HP_FILTER_RESET        0x25
#define TROYKA_ACELEROMETER_REFERENCE                    0x26
#define TROYKA_ACELEROMETER_STATUS_REG                    0x27
#define TROYKA_ACELEROMETER_OUT_X_L                        0x28
#define TROYKA_ACELEROMETER_OUT_X_H                        0x29
#define TROYKA_ACELEROMETER_OUT_Y_L                        0x2A
#define TROYKA_ACELEROMETER_OUT_Y_H                        0x2B
#define TROYKA_ACELEROMETER_OUT_Z_L                        0x2C
#define TROYKA_ACELEROMETER_OUT_Z_H                        0x2D

/** LIS331DLH Device Struct **/
typedef struct {
    I2C_DeviceStruct Communicator;
    struct AccelerometerData Data;
    char DataRepr[50];
} TroykaAccelerometer_TypeDef;
