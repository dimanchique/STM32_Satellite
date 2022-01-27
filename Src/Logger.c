#include "Logger.h"

SD_HandleTypeDef hsd1;
DiskWriter Logger = {0};

static uint32_t byteswritten;
static const char LOG_LEVEL[][10] = {"LOG LEVEL", "[LOG]", "[DATA]", "[WARNING]"};

extern I2C_BusStruct I2C_Bus;

static void MX_SDMMC1_SD_Init(void)
{
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
}

static void MountSystem(){
	Logger.Status = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
	if (Logger.Status == FR_OK)
		Logger.Disk_Mounted = 1;
}

static void UnMountSystem(){
	Logger.Status = f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
	Logger.Disk_Mounted = 0;
}

static void CreateInitialLog(){
	Logger.Status = f_open(&SDFile, Logger.Filename, FA_CREATE_ALWAYS | FA_WRITE);
	Logger.File_Opened = 1;	
	strcpy(Logger.Message, "Initial log generated. FatFS working properly. "); 	
	char i2c_message[50];
	if (I2C_Bus.Status == HAL_OK)
		sprintf(i2c_message, "I2C Bus Status: HAL_OK; Connected devices: %d;", I2C_Bus.Devices);
	else
		strcpy(i2c_message, "I2C Bus Status: HAL_ERROR");
	strcat(Logger.Message, i2c_message);
	Logger.Write_Status = f_write(&SDFile, Logger.Message, strlen((char *)Logger.Message), (void *)&byteswritten);
	Logger.Status = f_close(&SDFile);
	Logger.File_Opened = 0;
}

void InitSDSystem(){	
	MX_SDMMC1_SD_Init();
	MX_FATFS_Init();
	InitSD();
}

void InitSD(){	
	Logger.Disk_Mounted = 0;
	Logger.File_Opened = 0;
	strcpy(Logger.Filename, DEFAULT_FILENAME);	
	MountSystem();
	if (Logger.Disk_Mounted){
		CreateInitialLog();
	}
}
