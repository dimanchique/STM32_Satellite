#include "stm32h7xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

#define PI 3.14159265358979323846

#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#define be24toword(a) ((((a)>>16)&0x000000ff)|((a)&0x0000ff00)|(((a)<<16)&0x00ff0000))

#ifndef STRUCTS_DEFINED
////I2C Bus Struct////
typedef struct{
	I2C_HandleTypeDef I2C_Instance;
	HAL_StatusTypeDef Status;
} I2C_BusStruct;
////Devices List Struct////
typedef struct{
	char* Name;	
	uint8_t CommAddress;
	uint8_t FactAddress;
	uint8_t Device_ID;
	uint8_t ID_Register;
	HAL_StatusTypeDef Status;
} I2C_Communicator;
#define STRUCTS_DEFINED
#endif
/* Function Prototypes */
////I2C Tool////
void I2C_Init(void);
////Devices Tools////
void I2C_Scan(void);
uint8_t DeviceFound(I2C_Communicator Communicator);
void Verify_Device(I2C_Communicator *Communicator);
void CheckDeviceState(I2C_Communicator *Communicator);
////Write Functions////
void I2C_WriteByte(I2C_Communicator Communicator, uint8_t RegisterAddress, uint8_t Value);
////Read Functions////
void I2C_ReadData8(I2C_Communicator Communicator, uint8_t RegisterAddress, uint8_t *Value);
void I2C_ReadDataU16(I2C_Communicator Communicator, uint8_t RegisterAddress, uint16_t *Value);
void I2C_ReadDataS16(I2C_Communicator Communicator, uint8_t RegisterAddress, int16_t *Value);
void I2C_ReadReg_U24(I2C_Communicator Communicator, uint8_t RegisterAddress, uint32_t *Value);
void I2C_ReadDataBE_U24(I2C_Communicator Communicator, uint8_t RegisterAddress, uint32_t *Value);
void I2C_ReadData48(I2C_Communicator Communicator, uint8_t RegisterAddress, uint8_t* Value);
////Utils Functions////
void Error(void);
