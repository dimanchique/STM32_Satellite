#include "I2C_Process.h"

I2C_BusStruct I2C_Bus;
uint8_t AddressList[10] = {0};

//------------------------------------------------
void I2C_Init(void)
{
	I2C_Bus.I2C_Instance = hi2c1;
	I2C_Bus.Status = HAL_OK;
	I2C_Bus.Devices = 0;
}
//------------------------------------------------
void I2C_Scan(){
	uint8_t n = 0;
	for(uint8_t address = 8; address<128; address++){
		if(HAL_I2C_IsDeviceReady(&I2C_Bus.I2C_Instance, (uint16_t)(address<<1), 4, 50) == HAL_OK){
			AddressList[n] = address;
			I2C_Bus.Devices++;
			n++;
		}
	}	
}
//------------------------------------------------
uint8_t DeviceFound(I2C_Communicator Communicator){
	for (uint8_t i = 0; i<sizeof(AddressList); i++){
		if (AddressList[i]==Communicator.FactAddress)
			return 1;
	}
	return 0;
}
//------------------------------------------------
void CheckDeviceState(I2C_Communicator *Communicator){
	Communicator->Status = HAL_I2C_IsDeviceReady(&I2C_Bus.I2C_Instance, Communicator->CommAddress, 4, 50);
}
//------------------------------------------------
void Verify_Device(I2C_Communicator *Communicator){
	uint8_t RecievedID;
	I2C_Bus.Status = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance, Communicator->CommAddress, Communicator->ID_Register, 1, &RecievedID, 1, 0x10000);
	if (I2C_Bus.Status== HAL_OK){
		if (RecievedID == Communicator->Device_ID)
			Communicator->Status = HAL_OK;
		else
			Communicator->Status = HAL_ERROR;
	}
	if (I2C_Bus.Status == HAL_ERROR || Communicator->Status == HAL_ERROR)
		Error();
}
//------------------------------------------------
void I2C_WriteByte(I2C_Communicator Communicator, uint8_t RegisterAddress, uint8_t Value){
	I2C_Bus.Status = HAL_I2C_Mem_Write(&I2C_Bus.I2C_Instance, Communicator.CommAddress, (uint16_t)RegisterAddress, I2C_MEMADD_SIZE_8BIT, &Value, 1, 0x10000);
  	if(I2C_Bus.Status != HAL_OK) Error();
}
//------------------------------------------------
void I2C_ReadData8(I2C_Communicator Communicator, uint8_t RegisterAddress, uint8_t *Value){
  	I2C_Bus.Status = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance, Communicator.CommAddress, RegisterAddress, I2C_MEMADD_SIZE_8BIT, Value, 1, 0x10000);
  	if(I2C_Bus.Status != HAL_OK) Error();
}
//------------------------------------------------
static void I2C_ReadData16(uint8_t DeviceAddress, uint8_t RegisterAddress, uint16_t *Value){
  	I2C_Bus.Status = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance, DeviceAddress, RegisterAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Value, 2, 0x10000);
  	if(I2C_Bus.Status != HAL_OK) Error();
}
//------------------------------------------------
void I2C_ReadDataU16(I2C_Communicator Communicator, uint8_t RegisterAddress, uint16_t *Value){
  	I2C_ReadData16(Communicator.CommAddress, RegisterAddress, Value);
}
//------------------------------------------------
void I2C_ReadDataS16(I2C_Communicator Communicator, uint8_t RegisterAddress, int16_t *Value){
  	I2C_ReadData16(Communicator.CommAddress, RegisterAddress, (uint16_t*)Value);
}
//------------------------------------------------
static void I2C_ReadData24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value){
  	I2C_Bus.Status = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance, DeviceAddress, RegisterAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Value, 3, 0x10000);
  	if(I2C_Bus.Status != HAL_OK) Error();
}
//------------------------------------------------
void I2C_ReadDataBE_U24(I2C_Communicator Communicator, uint8_t RegisterAddress, uint32_t *Value){
  	I2C_ReadData24(Communicator.CommAddress, RegisterAddress, Value);
  	*(uint32_t *) Value = be24toword(*(uint32_t *) Value) & 0x00FFFFFF;
}
//------------------------------------------------
void I2C_ReadReg_U24(I2C_Communicator Communicator, uint8_t RegisterAddress, uint32_t *Value){
  	I2C_ReadData24(Communicator.CommAddress, RegisterAddress,Value);
  	*(uint32_t *) Value &= 0x00FFFFFF;
}
//------------------------------------------------
void I2C_ReadData48(I2C_Communicator Communicator, uint8_t RegisterAddress, uint8_t *Value){
  	I2C_Bus.Status = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance, Communicator.CommAddress, RegisterAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Value, 2, 0x10000);
  	if(I2C_Bus.Status != HAL_OK) Error();
}
//------------------------------------------------
void Error(){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}
//------------------------------------------------
