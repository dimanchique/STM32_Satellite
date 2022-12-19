#pragma once
#include "fatfs.h"

#define MESSAGE_LENGTH  400
#define LINES_COUNT     400

/** Logger Struct **/
typedef struct {
    char Message[MESSAGE_LENGTH];
    uint16_t LinesCount;
    uint16_t FileCount;
    char FileName[15];
    FRESULT FatFsStatus;
    uint8_t DiskMounted;
    uint8_t FileOpened;
} Logger_TypeDefStruct;

/** Functions Prototypes **/
void InitSDSystem(void);
void ForceDataLogging(void);
