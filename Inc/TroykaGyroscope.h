#include "I2C_Process.h"
//------------------------------------------------
/* Device Info */
#define TROYKA_GYROSCOPE_ADDRESS								0x69 //I3G4250D/L3G4200D
#define TROYKA_GYROSCOPE_ID_REGISTER 						0x0F
#define TROYKA_GYROSCOPE_ID 										0xD3
//------------------------------------------------
/* Registers Addresses */
#define TROYKA_GYROSCOPE_CTRL_REG1 							0x20
#define TROYKA_GYROSCOPE_CTRL_REG2 							0x21
#define TROYKA_GYROSCOPE_CTRL_REG3 							0x22
#define TROYKA_GYROSCOPE_CTRL_REG4 							0x23
#define TROYKA_GYROSCOPE_CTRL_REG5 							0x24
#define TROYKA_GYROSCOPE_REFERENCE 							0x25
#define TROYKA_GYROSCOPE_OUT_TEMP 							0x26
#define TROYKA_GYROSCOPE_STATUS_REG 						0x27
#define TROYKA_GYROSCOPE_OUT_X_L  							0x28
#define TROYKA_GYROSCOPE_OUT_X_H  							0x29
#define TROYKA_GYROSCOPE_OUT_Y_L  							0x2A
#define TROYKA_GYROSCOPE_OUT_Y_H  							0x2B
#define TROYKA_GYROSCOPE_OUT_Z_L  							0x2C
#define TROYKA_GYROSCOPE_OUT_Z_H  							0x2D
#define TROYKA_GYROSCOPE_FIFO_CTRL_REG  				0x2E
#define TROYKA_GYROSCOPE_FIFO_SRC_REG  					0x2F
#define TROYKA_GYROSCOPE_INT1_CFG  							0x30
#define TROYKA_GYROSCOPE_INT1_SRC  							0x31
#define TROYKA_GYROSCOPE_INT1_TSH_XH  					0x32
#define TROYKA_GYROSCOPE_INT1_TSH_XL  					0x33
#define TROYKA_GYROSCOPE_INT1_TSH_YH  					0x34
#define TROYKA_GYROSCOPE_INT1_TSH_YL  					0x35
#define TROYKA_GYROSCOPE_INT1_TSH_ZH  					0x36
#define TROYKA_GYROSCOPE_INT1_TSH_ZL  					0x37
#define TROYKA_GYROSCOPE_INT1_DURATION  				0x38
