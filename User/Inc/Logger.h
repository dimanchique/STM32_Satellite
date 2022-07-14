#pragma once
#include "fatfs.h"

//#define ENABLE_DEBUG
#define MESSAGE_LENGTH  400
#define LINES_COUNT     400

/** Logger Struct **/
typedef struct {
    char Message[MESSAGE_LENGTH];
    uint16_t LinesCount;
    uint8_t FileCount;
    char FileName[15];
    FRESULT FatFsStatus;
    uint8_t DiskMounted;
    uint8_t FileOpened;
} Logger_TypeDefStruct;

/** Functions Prototypes **/
void InitSDSystem(void);
#ifdef ENABLE_DEBUG
void LogDeviceState(I2C_CommunicatorStruct *Instance);
void LogOperation(I2C_TypeDefStruct *Instance, OperationType Operation, uint8_t BlockSize);
#endif
void ForceDataLogging(void);
