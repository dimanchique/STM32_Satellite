#pragma once
#include "I2C_Process.h"
#include "ModuleCoreTypes.h"

/** Device Info **/
#define MPU6050_ADDRESS 			0x68
#define MPU6050_ID 					0x68 //Device ID

/** Registers Addresses **/
#define MPU6050_SELF_TEST_X 		0x0D
#define MPU6050_SELF_TEST_Y 		0x0E
#define MPU6050_SELF_TEST_Z 		0x0F
#define MPU6050_SELF_TEST_A 		0x10
#define MPU6050_SMPLRT_DIV 			0x19
#define MPU6050_CONFIG 				0x1A
#define MPU6050_GYRO_CONFIG 		0x1B
#define MPU6050_ACCEL_CONFIG 		0x1C
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
#define MPU6050_USER_CTRL 			0x6A
#define MPU6050_PWR_MGMT_1 			0x6B
#define MPU6050_PWR_MGMT_2 			0x6C
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
#define MPU6050_ACC_SCALE			16384.0
#endif

#ifdef MPU6050_4g
#define MPU6050_ACC_RESOLUTION		0x08
#define MPU6050_ACC_SCALE			8192.0
#endif

#ifdef MPU6050_8g
#define MPU6050_ACC_RESOLUTION		0x10
#define MPU6050_ACC_SCALE			4096.0
#endif

#ifdef MPU6050_16g
#define MPU6050_ACC_RESOLUTION		0x18
#define MPU6050_ACC_SCALE			2048.0
#endif

/** Functions Prototypes **/
void MPU_Init(void);
void MPU_ReadData(void);
