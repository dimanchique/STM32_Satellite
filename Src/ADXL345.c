#include "ADXL345.h"

ADXL_Init_TypeDef ADXL345 = {0};

//------------------------------------------------
static void ADXL_InitialCallibrate(void){
	double x, y, z;
	ADXL_Read(&x, &y, &z);
	x*=ADXL345_ACC_SCALE;
	y*=ADXL345_ACC_SCALE; 
	z*=ADXL345_ACC_SCALE;
	I2C_WriteByte(ADXL345.Communicator, ADXL345_OFSX, (uint8_t)((x-ADXL345_ACC_SCALE)/4));
	I2C_WriteByte(ADXL345.Communicator, ADXL345_OFSY, (uint8_t)((y-ADXL345_ACC_SCALE)/4));
	I2C_WriteByte(ADXL345.Communicator, ADXL345_OFSZ, (uint8_t)((z-ADXL345_ACC_SCALE)/4));
}
//------------------------------------------------
void ADXL_DefaultSettings(){
	//Communication Section
	ADXL345.Communicator.Name = "ADXL345";
	ADXL345.Communicator.CommAddress = ADXL345_ADDRESS;
	ADXL345.Communicator.FactAddress = ADXL345_ADDRESS>>1;
	ADXL345.Communicator.Device_ID = ADXL345_ID;
	ADXL345.Communicator.ID_Register = ADXL345_ID_REGISTER;
	ADXL345.Communicator.Status = HAL_OK;
	//Setup Section	
	ADXL345.Datarate = ADXL345_DATARATE;
	ADXL345.DataFormat = ADXL345_ACC_RESOLUTION;
	ADXL345.Power = 0x08;
}
//------------------------------------------------
void ADXL_Init(){
	CheckDeviceState(&ADXL345.Communicator);
	if (ADXL345.Communicator.Status != HAL_ERROR)
		Verify_Device(&ADXL345.Communicator);
	
	if (ADXL345.Communicator.Status != HAL_ERROR){
		I2C_WriteByte(ADXL345.Communicator, ADXL345_BW_RATE, ADXL345.Datarate);
		I2C_WriteByte(ADXL345.Communicator, ADXL345_DATA_FORMAT, ADXL345.DataFormat);
		I2C_WriteByte(ADXL345.Communicator, ADXL345_POWER_CTL, 0x00);
		I2C_WriteByte(ADXL345.Communicator, ADXL345_POWER_CTL, ADXL345.Power);
		HAL_Delay(100);
		ADXL_InitialCallibrate();
	}
}
//------------------------------------------------
void ADXL_Read(double *x, double *y, double *z){
	if (ADXL345.Communicator.Status != HAL_ERROR)
		CheckDeviceState(&ADXL345.Communicator);
	
	if (ADXL345.Communicator.Status != HAL_ERROR){
		uint8_t data[6] = {0};
		int16_t xx,yy,zz;
		I2C_ReadData48(ADXL345.Communicator, ADXL345_DATAX0, data);
		xx = (int16_t)((data[1]<<8)|data[0]);
		yy = (int16_t)((data[3]<<8)|data[2]);
		zz = (int16_t)((data[5]<<8)|data[4]);
		*x = (double)(xx/ADXL345_ACC_SCALE); 
		*y = (double)(yy/ADXL345_ACC_SCALE); 
		*z = (double)(zz/ADXL345_ACC_SCALE); 
	}
	else{
		*x = 0;
		*y = 0;
		*z = 0;
	}
}
//------------------------------------------------
