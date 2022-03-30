#pragma once
#include "stm32h7xx_hal.h"
extern I2C_HandleTypeDef hi2c1;

#define be24toword(a) ((((a)>>16)&0x000000ff)|((a)&0x0000ff00)|(((a)<<16)&0x00ff0000)) //flip MSB with LSB
#define CommunicationAddress(a) a<<1

#define MAX_I2C_DEVICES 10

/* I2C Bus Struct */
typedef struct I2C_BusStruct
{
	I2C_HandleTypeDef I2C_Instance;
	HAL_StatusTypeDef OperationResult;
	uint8_t AddressList[MAX_I2C_DEVICES];
	uint8_t Scanned;
	uint8_t Devices;
} I2C_BusStruct;

/* I2C Device Struct */
typedef struct I2C_DeviceStruct
{
	char* Name;	
	uint8_t State;
	char* WarningLevel;
	uint8_t CommAddress;
	uint8_t FactAddress;
	uint8_t Device_ID;
	uint8_t ID_Register;
	HAL_StatusTypeDef ConnectionStatus;
} I2C_DeviceStruct;

/* Connection Status Enum */
enum ConnectionStatus
{
	NotInitialized,
	Initialized,
	InitializationError,
	Working,
	ConnectionLost,
	WritingError,
	WritingSuccess,
	ReadingError,
	ReadingSuccess,
	ID_Check_Error
};

/* Operation Type Enum */
enum Operation
{
	Reading,
	Writing
};

/* Base Functions Prototypes */
void I2C_Init(void);
void I2C_Scan(void);
void Verify_Device(I2C_DeviceStruct *Communicator);
void CheckDeviceState(I2C_DeviceStruct *Communicator);

/* Communication Functions Prototypes */
void I2C_WriteData8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t Value);
void I2C_ReadData8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value);
void I2C_ReadData3x8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value);
void I2C_ReadDataU16(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint16_t *Value);
void I2C_ReadDataS16(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, int16_t *Value);
void I2C_ReadDataU24BE(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint32_t *Value);
void I2C_ReadData6x8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t* Value);
