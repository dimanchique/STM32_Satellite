#include "I2C_Process.h"
#include "Logger.h"

I2C_BusStruct I2C_Bus = {0};
extern I2C_HandleTypeDef hi2c1;

void I2C_Init(void) {
    I2C_Bus.I2C_Instance = hi2c1;
    I2C_Bus.OperationResult = HAL_OK;
    I2C_Bus.Scanned = 0;
    I2C_Bus.Devices = 0;
#ifdef ENABLE_DEBUG
    LogI2C(I2C_Bus);
#endif
}

void I2C_Scan(void) {
    for (uint8_t address = 8; address < 128; address++) {
        if (HAL_I2C_IsDeviceReady(&I2C_Bus.I2C_Instance,
                                  (uint16_t) (address << 1),
                                  2,
                                  50) == HAL_OK) {
            I2C_Bus.Devices++;
        }
    }
    I2C_Bus.Scanned = 1;
#ifdef ENABLE_DEBUG
    LogI2C(I2C_Bus);
#endif
}

void CheckDeviceState(I2C_DeviceStruct *Communicator) {
    Communicator->ConnectionStatus = HAL_I2C_IsDeviceReady(&I2C_Bus.I2C_Instance,
                                                           Communicator->CommAddress,
                                                           2,
                                                           50);
    if (Communicator->State == NotInitialized) {
        if (Communicator->ConnectionStatus == HAL_OK)
            Communicator->State = Initialized;
        else
            Communicator->State = InitializationError;
    } else if (Communicator->State == Initialized || Communicator->State == Working) {
        if (Communicator->ConnectionStatus == HAL_OK)
            Communicator->State = Working;
        else
            Communicator->State = ConnectionLost;
    } else if (Communicator->ConnectionStatus == HAL_OK)
        Communicator->State = Working; //connection restored?
}

void Verify_Device(I2C_DeviceStruct *Communicator) {
    uint8_t ReceivedID;
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               Communicator->CommAddress,
                                               Communicator->ID_Register,
                                               1,
                                               &ReceivedID,
                                               1,
                                               0x10000);
    if (I2C_Bus.OperationResult == HAL_OK) {
        if (ReceivedID == Communicator->Device_ID) {
            Communicator->ConnectionStatus = HAL_OK;
            Communicator->State = Working;
        } else {
            Communicator->ConnectionStatus = HAL_ERROR;
            Communicator->State = ID_Check_Error;
        }
    }
}

static void ReportResult(I2C_DeviceStruct *Communicator, OperationTypeDef Operation, uint8_t BlockSize) {
    if (I2C_Bus.OperationResult != HAL_OK) {
        if (Operation == Writing)
            Communicator->State = WritingError;
        else
            Communicator->State = ReadingError;
        Communicator->ConnectionStatus = HAL_ERROR;
    } else {
        if (Operation == Writing)
            Communicator->State = WritingSuccess;
        else
            Communicator->State = ReadingSuccess;
        Communicator->ConnectionStatus = HAL_OK;
    }
#ifdef ENABLE_DEBUG
    LogI2C_Operation(*Communicator, Operation, BlockSize);
#endif
}

void I2C_WriteData8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Write(&I2C_Bus.I2C_Instance,
                                                Communicator->CommAddress,
                                                (uint16_t) RegisterAddress,
                                                I2C_MEMADD_SIZE_8BIT,
                                                &Value,
                                                1,
                                                0x10000);
    ReportResult(Communicator, Writing, 8);
}

void I2C_ReadData8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               Communicator->CommAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               Value,
                                               1,
                                               0x10000);
    ReportResult(Communicator, Reading, 8);
}

void I2C_ReadData3x8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               Communicator->CommAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               (uint8_t *) Value,
                                               3,
                                               0x10000);
    ReportResult(Communicator, Reading, 24);
}

void I2C_ReadData6x8(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint8_t *Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               Communicator->CommAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               (uint8_t *) Value,
                                               6,
                                               0x10000);
    ReportResult(Communicator, Reading, 48);
}

static void I2C_ReadData16(uint8_t DeviceAddress, uint8_t RegisterAddress, uint16_t *Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               DeviceAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               (uint8_t *) Value,
                                               2,
                                               0x10000);
}

void I2C_ReadDataU16(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint16_t *Value) {
    I2C_ReadData16(Communicator->CommAddress, RegisterAddress, Value);
    ReportResult(Communicator, Reading, 16);
}

void I2C_ReadDataS16(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, int16_t *Value) {
    I2C_ReadData16(Communicator->CommAddress, RegisterAddress, (uint16_t *) Value);
    ReportResult(Communicator, Reading, 16);
}

static void I2C_ReadData24(uint8_t DeviceAddress, uint8_t RegisterAddress, uint32_t *Value) {
    I2C_Bus.OperationResult = HAL_I2C_Mem_Read(&I2C_Bus.I2C_Instance,
                                               DeviceAddress,
                                               RegisterAddress,
                                               I2C_MEMADD_SIZE_8BIT,
                                               (uint8_t *) Value,
                                               3,
                                               0x10000);
}

void I2C_ReadDataU24BE(I2C_DeviceStruct *Communicator, uint8_t RegisterAddress, uint32_t *Value) {
    I2C_ReadData24(Communicator->CommAddress, RegisterAddress, Value);
    *(uint32_t *) Value = be24_to_word24(*(uint32_t *) Value) & 0x00FFFFFF;
    ReportResult(Communicator, Reading, 24);
}
