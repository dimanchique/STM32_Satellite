#include "MPU6050.h"

MPU_Init_TypeDef MPU6050 = {0};

static double ACC_ERROR_X, ACC_ERROR_Y, ACC_ERROR_Z, GYRO_ERROR_X, GYRO_ERROR_Y, GYRO_ERROR_Z = 0;
static uint16_t RecAccX, RecAccY, RecAccZ, RecGyroX, RecGyroY, RecGyroZ, RecTemp;

static void Calibrate(void){
	uint16_t n = 0;
	double AccX, AccY, AccZ, GyroX, GyroY, GyroZ = 0;
	while (n < 500) {
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_X, &RecAccX);
		RecAccX = ((uint16_t)(RecAccX<<8))|((uint16_t)(RecAccX>>8));
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_Y, &RecAccY);
		RecAccY = ((uint16_t)(RecAccY<<8))|((uint16_t)(RecAccY>>8));
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_Z, &RecAccZ);
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
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_X, &RecGyroX);
		RecGyroX = ((uint16_t)(RecGyroX<<8))|((uint16_t)(RecGyroX>>8));
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_Y, &RecGyroY);
		RecGyroY = ((uint16_t)(RecGyroY<<8))|((uint16_t)(RecGyroY>>8));
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_Z, &RecGyroZ);
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

void MPU_DefaultSettings(){
	//Communication Section
	MPU6050.Communicator.Name = "MPU6050";
	MPU6050.Communicator.CommAddress = MPU6050_ADDRESS;
	MPU6050.Communicator.FactAddress = MPU6050_ADDRESS>>1;
	MPU6050.Communicator.Device_ID = MPU6050_ID;
	MPU6050.Communicator.ID_Register = MPU6050_ID_REGISTER;
	CheckDeviceState(&MPU6050.Communicator);
	//Setup Section	
	MPU6050.GyroRes = MPU6050_GYRO_RESOLUTION;
	MPU6050.GyroScale = MPU6050_GYRO_RESOLUTION;
	MPU6050.AccRes = MPU6050_ACC_RESOLUTION;
	MPU6050.AccScale = MPU6050_ACC_RESOLUTION;
	MPU6050.GyroTest = MPU6050_TEST_GYRO;
	MPU6050.AccTest = MPU6050_TEST_ACC;	
}

void MPU_Init(){
	CheckDeviceState(&MPU6050.Communicator);
	if (MPU6050.Communicator.Status != HAL_ERROR)
		Verify_Device(&MPU6050.Communicator);	
	
	if (MPU6050.Communicator.Status != HAL_ERROR){
		I2C_WriteByte(MPU6050.Communicator, MPU6050_PWR_MGMT_1, MPU6050_RESET);
		I2C_WriteByte(MPU6050.Communicator, MPU6050_ACCEL_CONFIG, MPU6050.AccRes|MPU6050.AccTest);
		I2C_WriteByte(MPU6050.Communicator, MPU6050_GYRO_CONFIG, MPU6050.GyroRes|MPU6050.GyroTest);
		Calibrate();
	}
}

void MPU_Read_ACC(void){
	CheckDeviceState(&MPU6050.Communicator);
	if (MPU6050.Communicator.Status != HAL_ERROR){
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_X, &RecAccX);
		RecAccX = ((uint16_t)(RecAccX<<8))|((uint16_t)(RecAccX>>8));
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_Y, &RecAccY);
		RecAccY = ((uint16_t)(RecAccY<<8))|((uint16_t)(RecAccY>>8));
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_Z, &RecAccZ);
		RecAccZ = ((uint16_t)(RecAccZ<<8))|((uint16_t)(RecAccZ>>8));
		MPU6050.Data.X_Acceleration = RecAccX/MPU6050_ACC_SCALE - GYRO_ERROR_X;
		MPU6050.Data.Y_Acceleration = RecAccY/MPU6050_ACC_SCALE - GYRO_ERROR_Y;
		MPU6050.Data.Z_Acceleration = RecAccZ/MPU6050_ACC_SCALE - GYRO_ERROR_Z;
	}
	else{		
		MPU6050.Data.X_Acceleration = 0;
		MPU6050.Data.Y_Acceleration = 0;
		MPU6050.Data.Z_Acceleration = 0;
	}
}

void MPU_Read_GYRO(void){
	CheckDeviceState(&MPU6050.Communicator);
	if (MPU6050.Communicator.Status != HAL_ERROR){
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_X, &RecGyroX);
		RecGyroX = ((uint16_t)(RecGyroX<<8))|((uint16_t)(RecGyroX>>8));
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_Y, &RecGyroY);
		RecGyroY = ((uint16_t)(RecGyroY<<8))|((uint16_t)(RecGyroY>>8));
		I2C_ReadDataU16(MPU6050.Communicator, MPU6050_GYRO_Z, &RecGyroZ);
		RecGyroZ = ((uint16_t)(RecGyroZ<<8))|((uint16_t)(RecGyroZ>>8));
		MPU6050.Data.X_Gyro = RecGyroX/MPU6050_GYRO_SCALE - GYRO_ERROR_X;
		MPU6050.Data.Y_Gyro = RecGyroY/MPU6050_GYRO_SCALE - GYRO_ERROR_Y;
		MPU6050.Data.Z_Gyro = RecGyroZ/MPU6050_GYRO_SCALE - GYRO_ERROR_Z;
	}
	else{
		MPU6050.Data.X_Gyro = 0;
		MPU6050.Data.Y_Gyro = 0;
		MPU6050.Data.Z_Gyro = 0;
	}
}

void MPU_Read_TEMP(void){
	CheckDeviceState(&MPU6050.Communicator);
	if (MPU6050.Communicator.Status != HAL_ERROR){
		I2C_ReadDataS16(MPU6050.Communicator, MPU6050_TEMP, &RecTemp);
		RecTemp = ((int16_t)(RecTemp<<8))|((int16_t)(RecTemp>>8));
		MPU6050.Data.Temperature = RecTemp/340 + 36.53;
	}
	else
		MPU6050.Data.Temperature = 0;
}
