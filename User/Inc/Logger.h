#pragma once
#include "stm32h7xx_hal.h"

#include "fatfs.h"
#include "stdio.h"
#include "string.h"

#define DEFAULT_FILENAME "DATA.txt"

#define LOG_HEADER 0
#define LOG_MESSAGE 1
#define DATA_MESSAGE 2
#define WARNING_MESSAGE 3

#define QUEUE_SLOTS 2
#define MESSAGE_LENGTH 200

typedef struct{
	char Message[(QUEUE_SLOTS)*MESSAGE_LENGTH];
	char Queue[QUEUE_SLOTS][MESSAGE_LENGTH];
	uint8_t current_slot;
	char CurrentInstance[15];
	FRESULT FatFsStatus;
	uint8_t Disk_Mounted;
	uint8_t File_Opened;
	FRESULT Write_Status;
} DiskWriter;

void InitSDSystem(void);
void LogState(I2C_DeviceStruct Instance);
void LogI2C(I2C_BusStruct Instance);
void LogI2C_Operation(I2C_DeviceStruct Instance, uint8_t Operation, uint8_t BlockSize);
void ForceDataLogging(void);