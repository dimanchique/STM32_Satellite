#pragma once
#include "stm32h7xx_hal.h"
#include "CoreTypes.h"

#define be24_to_word24(a) ((((a)>>16)&0x000000ff)|((a)&0x0000ff00)|(((a)<<16)&0x00ff0000)) //flip MSB and LSB

/** Base Functions Prototypes **/
void I2C_Init(void);
void I2C_VerifyDevice(I2C_DeviceStruct *Communicator);
void I2C_CheckDeviceState(I2C_DeviceStruct *Communicator);
void I2C_SetupCommunicator(I2C_DeviceStruct *Communicator, char* DeviceName, uint8_t DeviceAddress, uint8_t DeviceID, uint8_t DeviceIDRegister);
uint8_t I2C_DeviceCheckedAndVerified(I2C_DeviceStruct *Communicator);

/** Communication Functions Prototypes **/
void I2C_WriteData8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t Value);
void I2C_ReadData8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value);
void I2C_ReadData2x8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value);
void I2C_ReadData3x8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value);
void I2C_ReadData6x8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value);
void I2C_ReadDataU16(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint16_t *Value);
void I2C_ReadDataS16(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, int16_t *Value);
void I2C_ReadDataU24BE(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint32_t *Value);