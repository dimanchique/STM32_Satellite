#pragma once
#include "fatfs.h"

//#define ENABLE_DEBUG
#define QUEUE_SLOTS     4
#define MESSAGE_LENGTH  300
#define LINES_COUNT     400

/** Logger Struct **/
typedef struct {
    char Message[(QUEUE_SLOTS) * MESSAGE_LENGTH];
    char Queue[QUEUE_SLOTS][MESSAGE_LENGTH];
    uint8_t CurrentMessageSlot;
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
