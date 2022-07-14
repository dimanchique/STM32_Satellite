#include "I2C_Interface.h"
#include "Logger.h"

static struct {
    I2C_HandleTypeDef I2C_Instance;
    HAL_StatusTypeDef OperationResult;
} I2C_Bus = {0};

extern I2C_HandleTypeDef hi2c1;

static void I2C_VerifyDevice(I2C_TypeDefStruct *Communicator);
static void I2C_CheckDeviceState(I2C_TypeDefStruct *Communicator);
static void ReportResult(I2C_TypeDefStruct *Communicator, OperationType Operation, uint8_t BlockSize);
static void I2C_ReadData16(uint8_t DeviceAddress, uint8_t RegisterAddress, uint16_t *Value);
static void I2C_ReadData24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value);

void I2C_Init(void) {
    I2C_Bus.I2C_Instance = hi2c1;
    I2C_Bus.OperationResult = HAL_OK;
}

void I2C_SetupCommunicator(I2C_TypeDefStruct *Communicator, char* DeviceName, uint8_t DeviceAddress, uint8_t DeviceID, uint8_t DeviceIDRegister){
    Communicator->Name = DeviceName;
    Communicator->State = NotInitialized;
    Communicator->CommAddress = DeviceAddress<<1;
    Communicator->Device_ID = DeviceID;
    Communicator->ID_Register = DeviceIDRegister;
}

uint8_t I2C_DeviceCheckedAndVerified(I2C_TypeDefStruct *Communicator){
    I2C_CheckDeviceState(Communicator);
    if (Communicator->ConnectionStatus == HAL_OK) {
        I2C_VerifyDevice(Communicator);
        if (Communicator->ConnectionStatus == HAL_OK)
            return 1;
    }
    return 0;
}

void I2C_WriteData8(I2C_TypeDefStruct *Communicator, uint8_t RegisterAddress, uint8_t Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Write(&I2C_Bus.I2C_Instance,
                                                Communicator->CommAddress,
                                                RegisterAddress,
                                                I2C_MEMADD_SIZE_8BIT,
                                                &Value,
                                                1,
                                                0x50);
    ReportResult(Communicator, Writing, 8);
}

void I2C_ReadData8(I2C_TypeDefStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               Communicator->CommAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               Value,
                                               1,
                                               0x50);
    ReportResult(Communicator, Reading, 8);
}

void I2C_ReadDataNx8(I2C_TypeDefStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value, uint8_t NumberOfBytes) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               Communicator->CommAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               (uint8_t *) Value,
                                               NumberOfBytes,
                                               0x50);
    ReportResult(Communicator, Reading, 8*NumberOfBytes);
}

void I2C_ReadDataU16(I2C_TypeDefStruct *Communicator, uint8_t RegisterAddress, uint16_t *Value) {
    I2C_ReadData16(Communicator->CommAddress, RegisterAddress, Value);
    ReportResult(Communicator, Reading, 16);
}

void I2C_ReadDataS16(I2C_TypeDefStruct *Communicator, uint8_t RegisterAddress, int16_t *Value) {
    I2C_ReadData16(Communicator->CommAddress, RegisterAddress, (uint16_t *) Value);
    ReportResult(Communicator, Reading, 16);
}


void I2C_ReadDataU24BE(I2C_TypeDefStruct *Communicator, uint8_t RegisterAddress, uint32_t *Value) {
    I2C_ReadData24(Communicator->CommAddress, RegisterAddress, Value);
    *(uint32_t *) Value = be24_to_word24(*(uint32_t *) Value) & 0x00FFFFFF;
    ReportResult(Communicator, Reading, 24);
}

static void I2C_VerifyDevice(I2C_TypeDefStruct *Communicator) {
    uint8_t ReceivedID;
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               Communicator->CommAddress,
                                               Communicator->ID_Register,
                                               1,
                                               &ReceivedID,
                                               1,
                                               0x10);
    if (I2C_Bus.OperationResult == HAL_OK) {
        Communicator->ConnectionStatus = ReceivedID == Communicator->Device_ID ? HAL_OK: HAL_ERROR;
        Communicator->State = ReceivedID == Communicator->Device_ID ? Working : ID_Check_Error;
    }
}

static void I2C_CheckDeviceState(I2C_TypeDefStruct *Communicator) {
    Communicator->ConnectionStatus = HAL_I2C_IsDeviceReady(&I2C_Bus.I2C_Instance,
                                                           Communicator->CommAddress,
                                                           2,
                                                           0x10);
    if (Communicator->State == NotInitialized) {
        Communicator->State = Communicator->ConnectionStatus == HAL_OK ? Initialized :InitializationError;
    } else if (Communicator->State == Initialized || Communicator->State == Working) {
        Communicator->State = Communicator->ConnectionStatus == HAL_OK ? Working :ConnectionLost;
    } else if (Communicator->ConnectionStatus == HAL_OK)
        Communicator->State = Working; //connection restored?
}

static void ReportResult(I2C_TypeDefStruct *Communicator, OperationType Operation, uint8_t BlockSize) {
    Communicator->ConnectionStatus = I2C_Bus.OperationResult == HAL_OK ? HAL_OK : HAL_ERROR;
    Communicator->State = I2C_Bus.OperationResult == HAL_OK ? Working : Error;
#ifdef ENABLE_DEBUG
    LogOperation(Communicator, Operation, BlockSize);
#endif
}

static void I2C_ReadData16(uint8_t DeviceAddress, uint8_t RegisterAddress, uint16_t *Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               DeviceAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               (uint8_t *) Value,
                                               2,
                                               0x50);
}

static void I2C_ReadData24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               DeviceAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               (uint8_t *) Value,
                                               3,
                                               0x50);
}
