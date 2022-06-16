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
} DiskWriter;

/** Functions Prototypes **/
void InitSDSystem(void);
void LogDeviceState(I2C_CommunicatorStruct *Instance);
void LogOperation(I2C_CommunicatorStruct *Instance, OperationType Operation, uint8_t BlockSize);
void ForceDataLogging(void);
