#include "stm32h7xx_hal.h"
#include "fatfs.h"
#include "stdio.h"
#include "string.h"

#define DEFAULT_FILENAME "DATA.txt"
#define LOG_HEADER 0
#define LOG_MESSAGE 1
#define DATA_MESSAGE 2
#define WARNING_MESSAGE 3

typedef struct{
	char Filename[20];
	char Message[100];
	FRESULT Status;
	uint8_t Disk_Mounted;
	uint8_t File_Opened;
	FRESULT Write_Status;
} DiskWriter;

void InitSDSystem(void);
void InitSD(void);
