#include "I2C_Process.h"
#include "Logger.h"

static I2C_BusStruct I2C_Bus = {0};

/**
  * @brief I2C struct initialization function
  * @param None
  * @retval None
  */
void I2C_Init(void)
{
	I2C_Bus.I2C_Instance = hi2c1;
	I2C_Bus.OperationResult = HAL_OK;
	I2C_Bus.Scanned = 0;
	I2C_Bus.Devices = 0;
	LogI2C(I2C_Bus);
}

/**
  * @brief Scan addresses 8 – 127 and filling AddressList with found devices
  * @note Capacity of AddressList is defined in I2C_Process.h as MAX_I2C_DEVICES value (default is 10)
  * @param None
  * @retval None
  */
void I2C_Scan(void)
{
	uint8_t n = 0;
	for(uint8_t address = 8; address<128; address++)
    {
		if(HAL_I2C_IsDeviceReady(&I2C_Bus.I2C_Instance, 
                                 (uint16_t)(address<<1), 
                                 4, 
                                 50) == HAL_OK)
        {
			I2C_Bus.AddressList[n] = address;
			I2C_Bus.Devices++;
			n++;
		}
	}
	I2C_Bus.Scanned = 1;
	LogI2C(I2C_Bus);
}

/**
  * @brief Check status of I2C device
  * @note
  * Connection: HAL_STATUS \n
  * State: Connection Status Enum (see I2C_Process.h)
  * @param Communicator Pointer to Device (I2C_DeviceStruct)
  * @retval None
  */
void CheckDeviceState(I2C_DeviceStruct *Communicator)
{
	Communicator->ConnectionStatus = HAL_I2C_IsDeviceReady(&I2C_Bus.I2C_Instance, 
                                                           Communicator->CommAddress, 
                                                           4, 
                                                           50);
	if (Communicator->State == NotInitialized)
	{
		if (Communicator->ConnectionStatus == HAL_OK)
            Communicator->State = Initialized;
		else
            Communicator->State = InitializationError;
	}
	else if (Communicator->State == Initialized || Communicator->State == Working)
	{
		if (Communicator->ConnectionStatus == HAL_OK)
            Communicator->State = Working;
		else
            Communicator->State = ConnectionLost;
	}
	else if (Communicator->ConnectionStatus == HAL_OK)
        Communicator->State = Working; //connection restored?
}

/**
  * @brief I2C device verification using built-in ID register
  * @note
  * Connection: HAL_OK -> State: Working; \n
  * Connection: HAL_ERROR -> State: ID_Check_Error;
  * @param Communicator Pointer to device (I2C_DeviceStruct)
  * @retval None
  */
void Verify_Device(I2C_DeviceStruct *Communicator)
{
	uint8_t ReceivedID;
	I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance, 
                                               Communicator->CommAddress, 
                                               Communicator->ID_Register, 
                                               1, 
                                               &ReceivedID,
                                               1, 
                                               0x10000);
	if (I2C_Bus.OperationResult== HAL_OK)
    {
		if (ReceivedID == Communicator->Device_ID)
        {
			Communicator->ConnectionStatus = HAL_OK;
			Communicator->State = Working;
		}
		else
        {
			Communicator->ConnectionStatus = HAL_ERROR;
			Communicator->State = ID_Check_Error;
		}
	}
}

/**
  * @brief Static function to pull read/write operation result to Communicator
  * @note
  * OperationResult: HAL_OK -> WritingSuccess/ReadingSuccess \n
  * OperationResult: HAL_ERROR -> WritingError/ReadingError \n
  * If defined DEBUG: Logging result to SD Card
  * @param Communicator Pointer to device (I2C_DeviceStruct)
  * @param Operation Operation Enum (see I2C_Process.h)
  * @param BlockSize Size of wrote/read data block (uint8_t)
  * @retval None
  */
static void ReportResult(I2C_DeviceStruct *Communicator, uint8_t Operation, uint8_t BlockSize)
{
	if(I2C_Bus.OperationResult != HAL_OK)
    {
		if (Operation==Writing)
            Communicator->State = WritingError;
		else
            Communicator->State = ReadingError;
		Communicator->ConnectionStatus = HAL_ERROR;
	}		
	else
    {
		if (Operation==Writing)
            Communicator->State = WritingSuccess;
		else
            Communicator->State = ReadingSuccess;
		Communicator->ConnectionStatus = HAL_OK;
	}		
	#ifdef DEBUG	
	LogI2C_Operation(*Communicator, Operation, BlockSize);
	#endif
}

/**
  * @brief Writing 8bit data block
  * @note Reports result to Communicator struct
  * @param Communicator Pointer to device (I2C_DeviceStruct)
  * @param RegisterAddress Register address to read (uint8_t)
  * @param Value Data (uint8_t)
  * @retval None
  */
void I2C_WriteData8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t Value)
{
	I2C_Bus.OperationResult = HAL_I2C_Mem_Write(&I2C_Bus.I2C_Instance,
                                                Communicator->CommAddress,
                                                (uint16_t)RegisterAddress,
                                                I2C_MEMADD_SIZE_8BIT,
                                                &Value,
                                                1,
                                                0x10000);
	ReportResult(Communicator, Writing, 8);
}

/**
  * @brief Reading 8bit data block
  * @note Reports result to Communicator struct
  * @param Communicator Pointer to device (I2C_DeviceStruct)
  * @param RegisterAddress Register address to read (uint8_t)
  * @param Value Pointer to returning data (uint8_t)
  * @retval None
  */
void I2C_ReadData8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value)
{
  I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                             Communicator->CommAddress,
                                             RegisterAddress,
                                             I2C_MEMADD_SIZE_8BIT,
                                             Value,
                                             1,
                                             0x10000);
  ReportResult(Communicator, Reading, 8);
}

/**
  * @brief Reading 3x8bit data block
  * @note Reports result to Communicator struct
  * @param Communicator Pointer to device (I2C_DeviceStruct)
  * @param RegisterAddress Register address to read (uint8_t)
  * @param Value Pointer to returning data (uint8_t array)
  * @retval None
  */
void I2C_ReadData3x8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value)
{
  I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance, 
                                             Communicator->CommAddress, 
                                             RegisterAddress, 
                                             I2C_MEMADD_SIZE_8BIT, 
                                             (uint8_t*)Value, 
                                             3, 
                                             0x10000);
  ReportResult(Communicator, Reading, 24);
}

/**
  * @brief Reading 6x8bit data block
  * @note Reports result to Communicator struct
  * @param Communicator Pointer to device (I2C_DeviceStruct)
  * @param RegisterAddress Register address to read (uint8_t)
  * @param Value Pointer to returning data (uint8_t array)
  * @retval None
  */
void I2C_ReadData6x8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value)
{
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               Communicator->CommAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               (uint8_t*)Value,
                                               6,
                                               0x10000);
    ReportResult(Communicator, Reading, 48);
}

/**
  * @brief Reading 16bit data block (signed/unsigned)
  * @note Doesn't reports result to Communicator struct
  * @param DeviceAddress I2C address of device
  * @param RegisterAddress Register address to read (uint8_t)
  * @param Value Pointer to returning data (uint16_t)
  * @retval None
  */
static void I2C_ReadData16(uint8_t DeviceAddress, uint8_t RegisterAddress, uint16_t *Value)
{
  I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                             DeviceAddress,
                                             RegisterAddress,
                                             I2C_MEMADD_SIZE_8BIT,
                                             (uint8_t*)Value,
                                             2,
                                             0x10000);
}

/**
  * @brief Reading 16bit data block (unsigned)
  * @note Reports result to Communicator struct
  * @param Communicator Pointer to device (I2C_DeviceStruct)
  * @param RegisterAddress Register address to read (uint8_t)
  * @param Value Pointer to returning data (uint16_t)
  * @retval None
  */
void I2C_ReadDataU16(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint16_t *Value)
{
  I2C_ReadData16(Communicator->CommAddress, RegisterAddress, Value);
  ReportResult(Communicator, Reading, 16);
}

/**
  * @brief Reading 16bit data block (signed)
  * @note Reports result to Communicator struct
  * @param Communicator Pointer to device (I2C_DeviceStruct)
  * @param RegisterAddress Register address to read (uint8_t)
  * @param Value Pointer to returning data (int16_t)
  * @retval None
  */
void I2C_ReadDataS16(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, int16_t *Value)
{
  I2C_ReadData16(Communicator->CommAddress, RegisterAddress, (uint16_t*)Value);
  ReportResult(Communicator, Reading, 16);
}

/**
  * @brief Reading 24bit data block
  * @param DeviceAddress I2C device address
  * @param RegisterAddress Register address to read (uint8_t)
  * @param Value Pointer to returning data (uint32_t)
  * @retval None
  */
static void I2C_ReadData24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value)
{
  I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                             DeviceAddress,
                                             RegisterAddress,
                                             I2C_MEMADD_SIZE_8BIT,
                                             (uint8_t*)Value,
                                             3,
                                             0x10000);
}

/**
  * @brief Reading 24bit data block (unsigned Big Endian)
  * @note Reports result to Communicator struct
  * @param Communicator Pointer to device (I2C_DeviceStruct)
  * @param RegisterAddress Register address to read (uint8_t)
  * @param Value Pointer to returning data (uint32_t)
  * @retval None
  */
void I2C_ReadDataU24BE(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint32_t *Value)
{
  I2C_ReadData24(Communicator->CommAddress, RegisterAddress, Value);
  *(uint32_t *) Value = be24toword(*(uint32_t *) Value) & 0x00FFFFFF;
  ReportResult(Communicator, Reading, 24);
}
