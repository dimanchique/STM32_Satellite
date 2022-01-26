#include "MPU6050.h"

HAL_StatusTypeDef MPU_Ready = HAL_ERROR;
double ACC_ERROR_X, ACC_ERROR_Y, ACC_ERROR_Z, GYRO_ERROR_X, GYRO_ERROR_Y, GYRO_ERROR_Z = 0;
uint16_t RecAccX, RecAccY, RecAccZ, RecGyroX, RecGyroY, RecGyroZ, RecTemp;

static void Calibrate(void){
	uint16_t n = 0;
	double AccX, AccY, AccZ, GyroX, GyroY, GyroZ = 0;
	while (n < 500) {
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_X, &RecAccX);
		RecAccX = ((uint16_t)(RecAccX<<8))|((uint16_t)(RecAccX>>8));
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_Y, &RecAccY);
		RecAccY = ((uint16_t)(RecAccY<<8))|((uint16_t)(RecAccY>>8));
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_Z, &RecAccZ);
		RecAccZ = ((uint16_t)(RecAccZ<<8))|((uint16_t)(RecAccZ>>8));
		AccX = RecAccX/MPU6050_ACC_SCALE;
		AccY = RecAccY/MPU6050_ACC_SCALE;
		AccZ = RecAccZ/MPU6050_ACC_SCALE;
		ACC_ERROR_X = ACC_ERROR_X + AccX;
		ACC_ERROR_Y = ACC_ERROR_Y + AccY;
		ACC_ERROR_Z = ACC_ERROR_Z + AccZ;
    	n++;
  }
  ACC_ERROR_X = ACC_ERROR_X / 500;
  ACC_ERROR_Y = ACC_ERROR_Y / 500;
  n = 0;

  while (n < 500) {
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_X, &RecGyroX);
		RecGyroX = ((uint16_t)(RecGyroX<<8))|((uint16_t)(RecGyroX>>8));
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_Y, &RecGyroY);
		RecGyroY = ((uint16_t)(RecGyroY<<8))|((uint16_t)(RecGyroY>>8));
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_Z, &RecGyroZ);
		RecGyroZ = ((uint16_t)(RecGyroZ<<8))|((uint16_t)(RecGyroZ>>8));
    	GyroX = RecGyroX/MPU6050_GYRO_SCALE;
		GyroY = RecGyroY/MPU6050_GYRO_SCALE;
		GyroZ = RecGyroZ/MPU6050_GYRO_SCALE;
		GYRO_ERROR_X = GYRO_ERROR_X + GyroX;
		GYRO_ERROR_Y = GYRO_ERROR_Y + GyroY;
		GYRO_ERROR_Z = GYRO_ERROR_Z + GyroZ;
   		n++;
  }
  GYRO_ERROR_X = GYRO_ERROR_X / 500;
  GYRO_ERROR_Y = GYRO_ERROR_Y / 500;
  GYRO_ERROR_Z = GYRO_ERROR_Z / 500;
}

void MPU_Init(){
	CheckDeviceState(MPU6050_ADDRESS, &MPU_Ready);
	if (MPU_Ready != HAL_ERROR)	Verify_Device(MPU6050_ADDRESS, MPU6050_ID_REGISTER, MPU6050_ID, &MPU_Ready);	
	
	if (MPU_Ready != HAL_ERROR){
		I2C_WriteByte(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, MPU6050_RESET);
		I2C_WriteByte(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, MPU6050_ACC_RESOLUTION|MPU6050_TEST_ACC);
		I2C_WriteByte(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, MPU6050_GYRO_RESOLUTION|MPU6050_TEST_GYRO);
		//Calibrate();
	}
}

void MPU_Read_ACC(double* ACC_X, double* ACC_Y, double* ACC_Z){
	CheckDeviceState(MPU6050_ADDRESS, &MPU_Ready);
	if (MPU_Ready != HAL_ERROR){
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_X, &RecAccX);
		RecAccX = ((uint16_t)(RecAccX<<8))|((uint16_t)(RecAccX>>8));
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_Y, &RecAccY);
		RecAccY = ((uint16_t)(RecAccY<<8))|((uint16_t)(RecAccY>>8));
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_Z, &RecAccZ);
		RecAccZ = ((uint16_t)(RecAccZ<<8))|((uint16_t)(RecAccZ>>8));
		*ACC_X = RecAccX/MPU6050_ACC_SCALE - GYRO_ERROR_X;
		*ACC_Y = RecAccY/MPU6050_ACC_SCALE - GYRO_ERROR_Y;
		*ACC_Z = RecAccZ/MPU6050_ACC_SCALE - GYRO_ERROR_Z;
	}
	else{		
		*ACC_X = 0;
		*ACC_Y = 0;
		*ACC_Z = 0;
	}
}

void MPU_Read_GYRO(double* GYRO_X, double* GYRO_Y, double* GYRO_Z){
	CheckDeviceState(MPU6050_ADDRESS, &MPU_Ready);
	if (MPU_Ready != HAL_ERROR){
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_X, &RecGyroX);
		RecGyroX = ((uint16_t)(RecGyroX<<8))|((uint16_t)(RecGyroX>>8));
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_Y, &RecGyroY);
		RecGyroY = ((uint16_t)(RecGyroY<<8))|((uint16_t)(RecGyroY>>8));
		I2C_ReadDataU16(MPU6050_ADDRESS, MPU6050_GYRO_Z, &RecGyroZ);
		RecGyroZ = ((uint16_t)(RecGyroZ<<8))|((uint16_t)(RecGyroZ>>8));
		*GYRO_X = RecGyroX/MPU6050_GYRO_SCALE - GYRO_ERROR_X;
		*GYRO_Y = RecGyroY/MPU6050_GYRO_SCALE - GYRO_ERROR_Y;
		*GYRO_Z = RecGyroZ/MPU6050_GYRO_SCALE - GYRO_ERROR_Z;
	}
	else{
		*GYRO_X = 0;
		*GYRO_Y = 0;
		*GYRO_Z = 0;
	}
}

void MPU_Read_TEMP(double* temp){
	CheckDeviceState(MPU6050_ADDRESS, &MPU_Ready);
	if (MPU_Ready != HAL_ERROR){
		I2C_ReadDataS16(MPU6050_ADDRESS, MPU6050_TEMP, &RecTemp);
		RecTemp = ((int16_t)(RecTemp<<8))|((int16_t)(RecTemp>>8));
		*temp = RecTemp/340 + 36.53;
	}
	else
		*temp = 0;
}
