#include "stm32h7xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

#define PI 3.14159265358979323846

#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#define be24toword(a) ((((a)>>16)&0x000000ff)|((a)&0x0000ff00)|(((a)<<16)&0x00ff0000))

/* Function Prototypes */
void I2C_Scan(void);

uint8_t DeviceFound(uint8_t Address);
void Verify_Device(uint8_t DeviceAddress, uint8_t ID_Address, uint8_t DeviceID, HAL_StatusTypeDef *status);
void CheckDeviceState(uint8_t DeviceAddress, uint8_t *status);

void I2C_WriteByte(uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t Value);

void I2C_ReadData8(uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t *Value);
void I2C_ReadDataU16(uint8_t DeviceAddress, uint8_t RegisterAddress, uint16_t *Value);
void I2C_ReadDataS16(uint8_t DeviceAddress, uint8_t RegisterAddress, int16_t *Value);
void I2C_ReadReg_U24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value);
void I2C_ReadDataBE_U24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value);
void I2C_ReadData48(uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t* Value);
void Error(void);
