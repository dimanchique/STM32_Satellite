#pragma once
#include "stm32h7xx_hal.h"
#include "CoreTypes.h"

/** Base Functions Prototypes **/
void I2C_Init(void);

/** Device Functions Prototypes **/
uint8_t I2C_DeviceCheckedAndVerified(I2C_CommunicatorStruct *Communicator);
void I2C_SetupCommunicator(I2C_CommunicatorStruct *Communicator,
                           char* DeviceName,
                           uint8_t DeviceAddress,
                           uint8_t DeviceID,
                           uint8_t DeviceIDRegister);

/** Communication Functions Prototypes **/
void I2C_WriteData8(I2C_CommunicatorStruct *Communicator, uint8_t RegisterAddress, uint8_t Value);
void I2C_ReadData8(I2C_CommunicatorStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value);
void I2C_ReadDataNx8(I2C_CommunicatorStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value, uint8_t NumberOfBytes);
void I2C_ReadDataU16(I2C_CommunicatorStruct *Communicator, uint8_t RegisterAddress, uint16_t *Value);
void I2C_ReadDataS16(I2C_CommunicatorStruct *Communicator, uint8_t RegisterAddress, int16_t *Value);
void I2C_ReadDataU24BE(I2C_CommunicatorStruct *Communicator, uint8_t RegisterAddress, uint32_t *Value);