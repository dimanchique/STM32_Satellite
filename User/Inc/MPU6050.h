#pragma once
#include "I2C_Interface.h"
#include "CoreTypes.h"

/** Device Info **/
#define MPU6050_ADDRESS 			0x68
#define MPU6050_ID 					0x68        //Device ID

/** Registers Addresses **/
#define MPU6050_ID_REGISTER			0x75        //Device ID Register
#define MPU6050_SELF_TEST_X 		0x0D
#define MPU6050_SELF_TEST_Y 		0x0E
#define MPU6050_SELF_TEST_Z 		0x0F
#define MPU6050_SELF_TEST_A 		0x10
#define MPU6050_CONFIG 				0x1A
#define MPU6050_GYRO_CONFIG 		0x1B
#define MPU6050_ACCEL_CONFIG 		0x1C
#define MPU6050_ACC_X 				0x3B
#define MPU6050_ACC_Y 				0x3D
#define MPU6050_ACC_Z 				0x3F
#define MPU6050_TEMP_H 				0x41
#define MPU6050_TEMP_L 				0x42
#define MPU6050_GYRO_X 				0x43
#define MPU6050_GYRO_Y 				0x45
#define MPU6050_GYRO_Z 				0x47
#define MPU6050_USER_CTRL 			0x6A
#define MPU6050_PWR_MGMT_1 			0x6B
#define MPU6050_PWR_MGMT_2 			0x6C

/** Accelerometer/Gyroscope Self-Test Settings **/
//#define MPU6050_TEST_ACC			0xE0
#define MPU6050_TEST_ACC			0x00
//#define MPU6050_TEST_GYRO			0xE0
#define MPU6050_TEST_GYRO			0x00

/** Gyroscope Resolution Settings **/
//#define MPU6050_GYRO_RESOLUTION	0x00        //250°/sec
//#define MPU6050_GYRO_SCALE		131.0       //250°/sec
#define MPU6050_GYRO_RESOLUTION		0x08        //500°/sec
#define MPU6050_GYRO_SCALE			65.5        //500°/sec
//#define MPU6050_GYRO_RESOLUTION	0x10        //1000°/sec
//#define MPU6050_GYRO_SCALE		32.8        //1000°/sec
//#define MPU6050_GYRO_RESOLUTION	0x18        //2000°/sec
//#define MPU6050_GYRO_SCALE		16.4        //2000°/sec

/** Accelerometer Resolution Settings **/
//#define MPU6050_ACC_RESOLUTION	0x00        //2g
//#define MPU6050_ACC_SCALE			16384.0     //2g
//#define MPU6050_ACC_RESOLUTION	0x08        //4g
//#define MPU6050_ACC_SCALE			8192.0      //4g
#define MPU6050_ACC_RESOLUTION		0x10        //8h
#define MPU6050_ACC_SCALE			4096.0      //8g
//#define MPU6050_ACC_RESOLUTION	0x18        //16g
//#define MPU6050_ACC_SCALE			2048.0      //16g

/** Value to reset MPU6050 and value to detect updating status **/
#define MPU6050_RESET				0x00

/** Functions Prototypes **/
void MPU_Init(void);
void MPU_ReadData(void);
