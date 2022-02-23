#pragma once
#include "I2C_Process.h"
//------------------------------------------------
/* Device Info */
#define MPU6050_ADDRESS 				0x68
#define MPU6050_ID 							0x68 //Device ID
//------------------------------------------------
/* Registers Addresses */
#define MPU6050_SELF_TEST_X 		0x0D
#define MPU6050_SELF_TEST_Y 		0x0E
#define MPU6050_SELF_TEST_Z 		0x0F
#define MPU6050_SELF_TEST_A 		0x10
#define MPU6050_SMPLRT_DIV 			0x19
#define MPU6050_CONFIG 				0x1A
#define MPU6050_GYRO_CONFIG 		0x1B
#define MPU6050_ACCEL_CONFIG 		0x1C
#define MPU6050_FIFO_EN 			0x23
#define MPU6050_I2C_MST_CTRL 		0x24
#define MPU6050_I2C_SLV0_ADDR 		0x25
#define MPU6050_I2C_SLV0_REG 		0x26
#define MPU6050_I2C_SLV0_CTRL 		0x27
#define MPU6050_I2C_SLV1_ADDR 		0x28
#define MPU6050_I2C_SLV1_REG 		0x29
#define MPU6050_I2C_SLV1_CTRL 		0x2A
#define MPU6050_I2C_SLV2_ADDR 		0x2B
#define MPU6050_I2C_SLV2_REG 		0x2C
#define MPU6050_I2C_SLV2_CTRL 		0x2D
#define MPU6050_I2C_SLV3_ADDR 		0x2E
#define MPU6050_I2C_SLV3_REG 		0x2F
#define MPU6050_I2C_SLV3_CTRL 		0x30
#define MPU6050_I2C_SLV4_ADDR 		0x31
#define MPU6050_I2C_SLV4_REG 		0x32
#define MPU6050_I2C_SLV4_DO 		0x33
#define MPU6050_I2C_SLV4_CTRL 		0x34
#define MPU6050_I2C_SLV4_DI 		0x35
#define MPU6050_I2C_MST_STATUS 		0x36
#define MPU6050_INT_PIN_CFG 		0x37
#define MPU6050_INT_ENABLE 			0x38
#define MPU6050_INT_STATUS 			0x3A
#define MPU6050_ACC_X 				0x3B
#define MPU6050_ACC_Y 				0x3D
#define MPU6050_ACC_Z 				0x3F
#define MPU6050_TEMP 				0x41
#define MPU6050_GYRO_X 				0x43
#define MPU6050_GYRO_Y 				0x45
#define MPU6050_GYRO_Z 				0x47
#define MPU6050_EXT_SENS_DATA_00    0x49
#define MPU6050_EXT_SENS_DATA_01    0x4A
#define MPU6050_EXT_SENS_DATA_02    0x4B
#define MPU6050_EXT_SENS_DATA_03    0x4C
#define MPU6050_EXT_SENS_DATA_04    0x4D
#define MPU6050_EXT_SENS_DATA_05    0x4E
#define MPU6050_EXT_SENS_DATA_06    0x4F
#define MPU6050_EXT_SENS_DATA_07    0x50
#define MPU6050_EXT_SENS_DATA_08    0x51
#define MPU6050_EXT_SENS_DATA_09    0x52
#define MPU6050_EXT_SENS_DATA_10    0x53
#define MPU6050_EXT_SENS_DATA_11    0x54
#define MPU6050_EXT_SENS_DATA_12    0x55
#define MPU6050_EXT_SENS_DATA_13    0x56
#define MPU6050_EXT_SENS_DATA_14    0x57
#define MPU6050_EXT_SENS_DATA_15    0x58
#define MPU6050_EXT_SENS_DATA_16    0x59
#define MPU6050_EXT_SENS_DATA_17    0x5A
#define MPU6050_EXT_SENS_DATA_18    0x5B
#define MPU6050_EXT_SENS_DATA_19    0x5C
#define MPU6050_EXT_SENS_DATA_20    0x5D
#define MPU6050_EXT_SENS_DATA_21    0x5E
#define MPU6050_EXT_SENS_DATA_22    0x5F
#define MPU6050_EXT_SENS_DATA_23    0x60
#define MPU6050_I2C_SLV0_DO 	    0x63
#define MPU6050_I2C_SLV1_DO 	    0x64
#define MPU6050_I2C_SLV2_DO 	    0x65
#define MPU6050_I2C_SLV3_DO 	    0x66
#define MPU6050_I2C_MST_DELAY_CTRL 	0x67
#define MPU6050_SIGNAL_PATH_RESET 	0x68
#define MPU6050_USER_CTRL 			0x6A
#define MPU6050_PWR_MGMT_1 			0x6B
#define MPU6050_PWR_MGMT_2 			0x6C
#define MPU6050_FIFO_COUNTH 		0x72
#define MPU6050_FIFO_COUNTL 		0x73
#define MPU6050_FIFO_R_W 			0x74
#define MPU6050_ID_REGISTER			0x75 //Device ID Register

#define MPU6050_RESET				0x00
//#define MPU6050_SELFTEST_ACC
//#define MPU6050_SELFTEST_GYRO

#ifdef MPU6050_SELFTEST_ACC
#define MPU6050_TEST_ACC			0xE0
#else
#define MPU6050_TEST_ACC			0x00
#endif

#ifdef MPU6050_SELFTEST_GYRO
#define MPU6050_TEST_GYRO			0xE0
#else
#define MPU6050_TEST_GYRO			0x00
#endif

//GYROSCOPE//
//#define MPU6050_250GYRO
#define MPU6050_500GYRO
//#define MPU6050_1000GYRO
//#define MPU6050_2000GYRO

#ifdef MPU6050_250GYRO
#define MPU6050_GYRO_RESOLUTION		0x00
#define MPU6050_GYRO_SCALE			131.0
#endif

#ifdef MPU6050_500GYRO
#define MPU6050_GYRO_RESOLUTION		0x08
#define MPU6050_GYRO_SCALE			65.5
#endif

#ifdef MPU6050_1000GYRO
#define MPU6050_GYRO_RESOLUTION		0x10
#define MPU6050_GYRO_SCALE			32.8
#endif

#ifdef MPU6050_2000GYRO
#define MPU6050_GYRO_RESOLUTION		0x18
#define MPU6050_GYRO_SCALE			16.4
#endif

//ACCELERATION//
//#define MPU6050_2g
//#define MPU6050_4g
#define MPU6050_8g
//#define MPU6050_16g

#ifdef MPU6050_2g
#define MPU6050_ACC_RESOLUTION		0x00
#define MPU6050_ACC_SCALE			16384
#endif

#ifdef MPU6050_4g
#define MPU6050_ACC_RESOLUTION		0x08
#define MPU6050_ACC_SCALE			8192
#endif

#ifdef MPU6050_8g
#define MPU6050_ACC_RESOLUTION		0x10
#define MPU6050_ACC_SCALE			4096
#endif

#ifdef MPU6050_16g
#define MPU6050_ACC_RESOLUTION		0x18
#define MPU6050_ACC_SCALE			2048
#endif

typedef struct{
	I2C_DeviceStruct Communicator;	
	
	uint8_t GyroRes;
	uint8_t GyroScale;
	uint8_t AccRes;
	uint8_t AccScale;
	uint8_t GyroTest;
	uint8_t AccTest;
	
	struct MPU_Data{
		double X_Acceleration;
		double Y_Acceleration;
		double Z_Acceleration;
		double X_Gyro;
		double Y_Gyro;
		double Z_Gyro;
		double Temperature;
	}Data;
	char DataRepr[50];
} MPU_TypeDef;

void MPU_Init(void);
void MPU_ReadData(void);
