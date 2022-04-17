#pragma once

#include "stm32h7xx_hal.h"

#include "fatfs.h"
#include "stdio.h"
#include "string.h"

#define DEFAULT_FILENAME "DATA.txt"

#define QUEUE_SLOTS 2
#define MESSAGE_LENGTH 200

#define ENABLE_DEBUG

enum Level {
    LOG         = 0x00,
    DATA        = 0x01,
    WARNING     = 0x02
};

/** Logger Struct **/
typedef struct {
    char Message[(QUEUE_SLOTS) * MESSAGE_LENGTH];
    char Queue[QUEUE_SLOTS][MESSAGE_LENGTH];
    uint8_t current_slot;
    char CurrentInstance[15];
    FRESULT FatFsStatus;
    uint8_t Disk_Mounted;
    uint8_t File_Opened;
    FRESULT Write_Status;
} DiskWriter;

/** Functions Prototypes **/
void InitSDSystem(void);
void LogState(I2C_DeviceStruct Instance);
void LogI2C(I2C_BusStruct Instance);
void LogI2C_Operation(I2C_DeviceStruct Instance, OperationType Operation, uint8_t BlockSize);
void ForceDataLogging(void);
