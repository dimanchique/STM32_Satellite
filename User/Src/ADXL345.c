#include "ADXL345.h"
#include "Logger.h"
#include "string.h"
//------------------------------------------------
ADXL_TypeDef ADXL345 = {0};
extern volatile DiskWriter Logger;
//------------------------------------------------
static void GenerateDataRepresentation(uint8_t ConnectionValid){
	if(ConnectionValid)
		sprintf(ADXL345.DataRepr,
						"[%s] %.4f; %.4f; %.4f;",
						ADXL345.Communicator.Name,
						ADXL345.Data.x,
						ADXL345.Data.y,
						ADXL345.Data.z);
	else
		sprintf(ADXL345.DataRepr, "[%s] NULL;", ADXL345.Communicator.Name);
}
//------------------------------------------------
static void ADXL_Calibrate(void){
	// Need to work around calibration
	ADXL_ReadData();
	ADXL345.Data.x/=ADXL345_ACC_SCALE;
	ADXL345.Data.y/=ADXL345_ACC_SCALE; 
	ADXL345.Data.z/=ADXL345_ACC_SCALE;
    I2C_WriteData8(&ADXL345.Communicator, ADXL345_OFSX, (uint8_t) ((ADXL345.Data.x - ADXL345_ACC_SCALE) / 4));
    I2C_WriteData8(&ADXL345.Communicator, ADXL345_OFSY, (uint8_t) ((ADXL345.Data.y - ADXL345_ACC_SCALE) / 4));
    I2C_WriteData8(&ADXL345.Communicator, ADXL345_OFSZ, (uint8_t) ((ADXL345.Data.z - ADXL345_ACC_SCALE) / 4));
}
//------------------------------------------------
void ADXL_Init(){
	//Communication Section
	ADXL345.Communicator.Name = "ADXL345";
	ADXL345.Communicator.State = NotInitialized;
	ADXL345.Communicator.CommAddress = CommunicationAddress(ADXL345_ADDRESS);
	ADXL345.Communicator.FactAddress = ADXL345_ADDRESS;
	ADXL345.Communicator.Device_ID = ADXL345_ID;
	ADXL345.Communicator.ID_Register = ADXL345_ID_REGISTER;
	//Setup Section	
	ADXL345.Datarate = ADXL345_DATARATE;
	ADXL345.DataFormat = ADXL345_ACC_RESOLUTION;
	ADXL345.Power = 0x08;	
	LogState(ADXL345.Communicator);
	
	CheckDeviceState(&ADXL345.Communicator);
	if (ADXL345.Communicator.ConnectionStatus == HAL_OK)
	{
		Verify_Device(&ADXL345.Communicator);	
		if (ADXL345.Communicator.ConnectionStatus == HAL_OK){
            I2C_WriteData8(&ADXL345.Communicator, ADXL345_BW_RATE, ADXL345.Datarate);
            I2C_WriteData8(&ADXL345.Communicator, ADXL345_DATA_FORMAT, ADXL345.DataFormat);
            I2C_WriteData8(&ADXL345.Communicator, ADXL345_POWER_CTL, 0x00);
            I2C_WriteData8(&ADXL345.Communicator, ADXL345_POWER_CTL, ADXL345.Power);
			HAL_Delay(100);
			ADXL_Calibrate();
		}
		if (ADXL345.Communicator.ConnectionStatus == HAL_OK) ADXL345.Communicator.State = Initialized;		
	}
	LogState(ADXL345.Communicator);
}
//------------------------------------------------
void ADXL_ReadData(){
	if (ADXL345.Communicator.ConnectionStatus == HAL_OK){
		uint8_t data[6] = {0};
        I2C_ReadData6x8(&ADXL345.Communicator, ADXL345_DATAX0, data);
		ADXL345.Data.x = (double)((int16_t)((data[1]<<8)|data[0])*ADXL345_ACC_SCALE); 
		ADXL345.Data.y = (double)((int16_t)((data[3]<<8)|data[2])*ADXL345_ACC_SCALE); 
		ADXL345.Data.z = (double)((int16_t)((data[5]<<8)|data[4])*ADXL345_ACC_SCALE); 
	}
	else{
		ADXL345.Data.x = 0;
		ADXL345.Data.y = 0;
		ADXL345.Data.z = 0;
	}	
	GenerateDataRepresentation(ADXL345.Communicator.ConnectionStatus == HAL_OK);
}
