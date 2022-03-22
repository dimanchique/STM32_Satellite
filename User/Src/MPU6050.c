#include "MPU6050.h"
#include "Logger.h"
#include "string.h"
//------------------------------------------------
MPU_TypeDef MPU6050 = {0};
//------------------------------------------------
static double ACC_ERROR_X, ACC_ERROR_Y, ACC_ERROR_Z, GYRO_ERROR_X, GYRO_ERROR_Y, GYRO_ERROR_Z = 0;
static uint16_t RecTemp;
static uint16_t CalibrationCycles = 500;
//------------------------------------------------
static void GenerateDataRepresentation(uint8_t ConnectionValid){
	if(ConnectionValid)
		sprintf(MPU6050.DataRepr,
						"[%s] %.3f; %.3f; %.3f; %.3f; %.3f; %.3f; %.3f;",
						MPU6050.Communicator.Name,
						MPU6050.Data.X_Acceleration,
						MPU6050.Data.Y_Acceleration,
						MPU6050.Data.Z_Acceleration,
						MPU6050.Data.X_Gyro,
						MPU6050.Data.Y_Gyro,
						MPU6050.Data.Z_Gyro,
						MPU6050.Data.Temperature);
	else
		sprintf(MPU6050.DataRepr, "[%s] NULL;", MPU6050.Communicator.Name);
}
//------------------------------------------------
static void MPU_Calibrate(void){
	uint16_t n = 0;
	double AccX, AccY, AccZ, GyroX, GyroY, GyroZ = 0;
	uint8_t data[6] = {0};
	int16_t xx,yy,zz;
	
	while (n < CalibrationCycles) {
		I2C_ReadData48(&MPU6050.Communicator, MPU6050_ACC_X, data);
		xx = (int16_t)((data[1]<<8)|data[0]);
		yy = (int16_t)((data[3]<<8)|data[2]);
		zz = (int16_t)((data[5]<<8)|data[4]);
		AccX = xx/MPU6050_ACC_SCALE;
		AccY = yy/MPU6050_ACC_SCALE;
		AccZ = zz/MPU6050_ACC_SCALE;
		ACC_ERROR_X = ACC_ERROR_X + AccX;
		ACC_ERROR_Y = ACC_ERROR_Y + AccY;
		ACC_ERROR_Z = ACC_ERROR_Z + AccZ;
    n++;
  }
  ACC_ERROR_X = ACC_ERROR_X / CalibrationCycles;
  ACC_ERROR_Y = ACC_ERROR_Y / CalibrationCycles;
  n = 0;

  while (n < CalibrationCycles) {
		I2C_ReadData48(&MPU6050.Communicator, MPU6050_GYRO_X, data);
		xx = (int16_t)((data[1]<<8)|data[0]);
		yy = (int16_t)((data[3]<<8)|data[2]);
		zz = (int16_t)((data[5]<<8)|data[4]);
    GyroX = xx/MPU6050_GYRO_SCALE;
		GyroY = yy/MPU6050_GYRO_SCALE;
		GyroZ = zz/MPU6050_GYRO_SCALE;
		GYRO_ERROR_X = GYRO_ERROR_X + GyroX;
		GYRO_ERROR_Y = GYRO_ERROR_Y + GyroY;
		GYRO_ERROR_Z = GYRO_ERROR_Z + GyroZ;
   		n++;
  }
  GYRO_ERROR_X = GYRO_ERROR_X / CalibrationCycles;
  GYRO_ERROR_Y = GYRO_ERROR_Y / CalibrationCycles;
  GYRO_ERROR_Z = GYRO_ERROR_Z / CalibrationCycles;
}
//------------------------------------------------
void MPU_Init(){
	//Communication Section
	MPU6050.Communicator.Name = "MPU6050";
	MPU6050.Communicator.State = NotInitialized;
	MPU6050.Communicator.CommAddress = CommunicationAddress(MPU6050_ADDRESS);
	MPU6050.Communicator.FactAddress = MPU6050_ADDRESS;
	MPU6050.Communicator.Device_ID = MPU6050_ID;
	MPU6050.Communicator.ID_Register = MPU6050_ID_REGISTER;	
	//Setup Section	
	MPU6050.GyroRes = MPU6050_GYRO_RESOLUTION;
	MPU6050.GyroScale = MPU6050_GYRO_RESOLUTION;
	MPU6050.AccRes = MPU6050_ACC_RESOLUTION;
	MPU6050.AccScale = MPU6050_ACC_RESOLUTION;
	MPU6050.GyroTest = MPU6050_TEST_GYRO;
	MPU6050.AccTest = MPU6050_TEST_ACC;		
	LogState(MPU6050.Communicator);
	
	CheckDeviceState(&MPU6050.Communicator);
	if (MPU6050.Communicator.ConnectionStatus == HAL_OK)
	{
		Verify_Device(&MPU6050.Communicator);
		if (MPU6050.Communicator.ConnectionStatus == HAL_OK){
			I2C_WriteByte(&MPU6050.Communicator, MPU6050_PWR_MGMT_1, MPU6050_RESET);
			I2C_WriteByte(&MPU6050.Communicator, MPU6050_ACCEL_CONFIG, MPU6050.AccRes|MPU6050.AccTest);
			I2C_WriteByte(&MPU6050.Communicator, MPU6050_GYRO_CONFIG, MPU6050.GyroRes|MPU6050.GyroTest);
			MPU_Calibrate();
		}
		if (MPU6050.Communicator.ConnectionStatus == HAL_OK) MPU6050.Communicator.State = Initialized;
	}	
	LogState(MPU6050.Communicator);
}
//------------------------------------------------
static void MPU_Read_ACC(void){
	if (MPU6050.Communicator.ConnectionStatus == HAL_OK){
		uint8_t data[6] = {0};
		int16_t xx,yy,zz;
		I2C_ReadData48(&MPU6050.Communicator, MPU6050_ACC_X, data);
		xx = (int16_t)((data[1]<<8)|data[0]);
		yy = (int16_t)((data[3]<<8)|data[2]);
		zz = (int16_t)((data[5]<<8)|data[4]);
		MPU6050.Data.X_Acceleration = (double)(xx/MPU6050_ACC_SCALE) - ACC_ERROR_X;
		MPU6050.Data.Y_Acceleration = (double)(yy/MPU6050_ACC_SCALE) - ACC_ERROR_Y;
		MPU6050.Data.Z_Acceleration = (double)(zz/MPU6050_ACC_SCALE) - ACC_ERROR_Z;
	}
	else{		
		MPU6050.Data.X_Acceleration = 0;
		MPU6050.Data.Y_Acceleration = 0;
		MPU6050.Data.Z_Acceleration = 0;
	}
}
//------------------------------------------------
static void MPU_Read_GYRO(void){
	if (MPU6050.Communicator.ConnectionStatus == HAL_OK){
		uint8_t data[6] = {0};
		int16_t xx,yy,zz;
		I2C_ReadData48(&MPU6050.Communicator, MPU6050_GYRO_X, data);
		xx = (int16_t)((data[1]<<8)|data[0]);
		yy = (int16_t)((data[3]<<8)|data[2]);
		zz = (int16_t)((data[5]<<8)|data[4]);
		MPU6050.Data.X_Gyro = xx/MPU6050_GYRO_SCALE - GYRO_ERROR_X;
		MPU6050.Data.Y_Gyro = yy/MPU6050_GYRO_SCALE - GYRO_ERROR_Y;
		MPU6050.Data.Z_Gyro = zz/MPU6050_GYRO_SCALE - GYRO_ERROR_Z;
	}
	else{
		MPU6050.Data.X_Gyro = 0;
		MPU6050.Data.Y_Gyro = 0;
		MPU6050.Data.Z_Gyro = 0;
	}
}
//------------------------------------------------
static void MPU_Read_TEMP(void){
	if (MPU6050.Communicator.ConnectionStatus == HAL_OK){
		I2C_ReadDataS16(&MPU6050.Communicator, MPU6050_TEMP, &RecTemp);
		RecTemp = ((int16_t)(RecTemp<<8))|((int16_t)(RecTemp>>8));
		MPU6050.Data.Temperature = RecTemp/340 + 36.53;
	}
	else
		MPU6050.Data.Temperature = 0;
}
//------------------------------------------------
void MPU_ReadData(){
	MPU_Read_ACC();
	MPU_Read_GYRO();
	MPU_Read_TEMP();
	GenerateDataRepresentation(MPU6050.Communicator.ConnectionStatus == HAL_OK);
}
