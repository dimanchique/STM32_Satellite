#include "I2C_Process.h"
//------------------------------------------------
/* Device Info */
#define TROYKA_MAGNETOMETER_ADDRESS							0x1C //LIS3MDL
#define TROYKA_MAGNETOMETER_ID_REGISTER 				0x0F
#define TROYKA_MAGNETOMETER_ID 									0x3D
//------------------------------------------------
/* Registers Addresses */
#define TROYKA_MAGNETOMETER_CTRL_REG1  					0x20
#define TROYKA_MAGNETOMETER_CTRL_REG2  					0x21
#define TROYKA_MAGNETOMETER_CTRL_REG3  					0x22
#define TROYKA_MAGNETOMETER_CTRL_REG4  					0x23
#define TROYKA_MAGNETOMETER_CTRL_REG5  					0x24
#define TROYKA_MAGNETOMETER_STATUS_REG  				0x27
#define TROYKA_MAGNETOMETER_OUT_X_L  						0x28
#define TROYKA_MAGNETOMETER_OUT_X_H  						0x29
#define TROYKA_MAGNETOMETER_OUT_Y_L  						0x2A
#define TROYKA_MAGNETOMETER_OUT_Y_H  						0x2B
#define TROYKA_MAGNETOMETER_OUT_Z_L  						0x2C
#define TROYKA_MAGNETOMETER_OUT_Z_H  						0x2D
#define TROYKA_MAGNETOMETER_TEMP_OUT_L  				0x2E
#define TROYKA_MAGNETOMETER_TEMP_OUT_H  				0x2F
#define TROYKA_MAGNETOMETER_INT_CFG  						0x30
#define TROYKA_MAGNETOMETER_INT_SRC  						0x31
#define TROYKA_MAGNETOMETER_INT_THS_L  					0x32
#define TROYKA_MAGNETOMETER_INT_THS_H  					0x33
