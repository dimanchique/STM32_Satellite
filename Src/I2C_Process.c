#include "I2C_Process.h"

I2C_BusTypeDef I2C_Bus;
uint8_t AddressList[10] = {0};

//------------------------------------------------
void I2C_Init(void)
{
	I2C_Bus.I2C_Instance = hi2c1;
	I2C_Bus.Status = HAL_OK;
}
//------------------------------------------------
void I2C_Scan(){
	uint8_t n = 0;
	for(uint8_t address = 1; address<128; address++){
		I2C_Bus.Status = HAL_I2C_IsDeviceReady(&I2C_Bus.I2C_Instance, (uint16_t)(address<<1), 4, 50);
		if(I2C_Bus.Status == HAL_OK)
		{
			AddressList[n] = address;
			n++;
		}
	}	
}
//------------------------------------------------
uint8_t DeviceFound(uint8_t Address){
	for (uint8_t i = 0; i<sizeof(AddressList); i++)
	{
		if (AddressList[i]==Address>>1)
			return 1;
	}
	return 0;
}
//------------------------------------------------
void CheckDeviceState(uint8_t DeviceAddress, HAL_StatusTypeDef *status){
	*status = HAL_I2C_IsDeviceReady(&hi2c1, DeviceAddress, 4, 50);
}
//------------------------------------------------
void Verify_Device(uint8_t DeviceAddress, uint8_t ID_Address, uint8_t DeviceID, HAL_StatusTypeDef *status){
	uint8_t RecievedID;
	if (HAL_I2C_Mem_Read(&hi2c1, DeviceAddress, ID_Address, 1, &RecievedID, 1, 0x10000) != HAL_OK){
		I2CBus_Ready = HAL_ERROR;
	}
	else{
		if (RecievedID == DeviceID)
			*status = HAL_OK;
		else
			*status = HAL_ERROR;
	}
	if ((I2CBus_Ready || *status) == HAL_ERROR)
		Error();
}
//------------------------------------------------
void I2C_WriteByte(uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t Value){
	I2CBus_Ready = HAL_I2C_Mem_Write(&hi2c1, DeviceAddress, (uint16_t)RegisterAddress, I2C_MEMADD_SIZE_8BIT, &Value, 1, 0x10000);
  	if(I2CBus_Ready != HAL_OK) Error();
}
//------------------------------------------------
void I2C_ReadData8(uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t *Value){
  	I2CBus_Ready = HAL_I2C_Mem_Read(&hi2c1, DeviceAddress, RegisterAddress, I2C_MEMADD_SIZE_8BIT, Value, 1, 0x10000);
  	if(I2CBus_Ready != HAL_OK) Error();
}
//------------------------------------------------
static void I2C_ReadData16(uint8_t DeviceAddress, uint8_t RegisterAddress, uint16_t *Value){
  	I2CBus_Ready = HAL_I2C_Mem_Read(&hi2c1, DeviceAddress, RegisterAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Value, 2, 0x10000);
  	if(I2CBus_Ready != HAL_OK) Error();
}
//------------------------------------------------
void I2C_ReadDataU16(uint8_t DeviceAddress, uint8_t RegisterAddress, uint16_t *Value){
  	I2C_ReadData16(DeviceAddress, RegisterAddress, Value);
}
//------------------------------------------------
void I2C_ReadDataS16(uint8_t DeviceAddress, uint8_t RegisterAddress, int16_t *Value){
  	I2C_ReadData16(DeviceAddress, RegisterAddress, (uint16_t*)Value);
}
//------------------------------------------------
static void I2C_ReadData24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value){
  	I2CBus_Ready = HAL_I2C_Mem_Read(&hi2c1, DeviceAddress, RegisterAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Value, 3, 0x10000);
  	if(I2CBus_Ready != HAL_OK) Error();
}
//------------------------------------------------
void I2C_ReadDataBE_U24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value){
  	I2C_ReadData24(DeviceAddress,RegisterAddress,Value);
  	*(uint32_t *) Value = be24toword(*(uint32_t *) Value) & 0x00FFFFFF;
}
//------------------------------------------------
void I2C_ReadReg_U24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value){
  	I2C_ReadData24(DeviceAddress, RegisterAddress,Value);
  	*(uint32_t *) Value &= 0x00FFFFFF;
}
//------------------------------------------------
void I2C_ReadData48(uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t *Value){
  	I2CBus_Ready = HAL_I2C_Mem_Read(&hi2c1, DeviceAddress, RegisterAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Value, 2, 0x10000);
  	if(I2CBus_Ready != HAL_OK) Error();
}
//------------------------------------------------
void Error(){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}
//------------------------------------------------
