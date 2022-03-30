#include "Logger.h"

#include "TroykaBarometer.h"
#include "BMP280.h"
#include "ADXL345.h"
#include "MPU6050.h"
#include "NEO7M.h"

extern SD_HandleTypeDef hsd1;
DiskWriter Logger = {0};

static uint32_t byteswritten;
static const char LOG_LEVEL[][11] = {"LOG LEVEL", "[LOG]", "[DATA]", "[WARNING]"};

extern I2C_BusStruct I2C_Bus;
extern BMP280_TypeDef BMP280;
extern ADXL_TypeDef ADXL345;
extern MPU_TypeDef MPU6050;
extern GPS_TypeDef NEO7M;
extern TroykaBarometer_TypeDef Barometer;

static void MX_SDMMC1_SD_Init(void)
{
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
}

static void WriteLog(){
	if (Logger.current_slot<QUEUE_SLOTS){
		strcpy(Logger.Queue[Logger.current_slot++], Logger.Message);
		if (Logger.current_slot<QUEUE_SLOTS) return;
	}			
	
	NVIC_DisableIRQ(TIM17_IRQn);
	strcpy(Logger.Message, "");
	for (uint8_t i = 0; i<QUEUE_SLOTS; i++){
		strcat(Logger.Message, Logger.Queue[i]);
		strcpy(Logger.Queue[i], "");
	}	
	NVIC_EnableIRQ(TIM17_IRQn);
	Logger.current_slot = 0;
	
	Logger.FatFsStatus = f_open(&SDFile, DEFAULT_FILENAME, FA_OPEN_APPEND | FA_WRITE);
	if (Logger.FatFsStatus == FR_OK){
		Logger.File_Opened = 1;	
		Logger.Write_Status = f_write(&SDFile, Logger.Message, strlen((char *)Logger.Message), (void *)&byteswritten);
		Logger.FatFsStatus = f_close(&SDFile);
		}
	if (Logger.FatFsStatus == FR_OK) Logger.File_Opened = 0;
		
	strcpy(Logger.Message, "");
}

static void MountDisk(){
	Logger.FatFsStatus = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
	if (Logger.FatFsStatus == FR_OK){
		Logger.FatFsStatus = f_open(&SDFile, DEFAULT_FILENAME, FA_CREATE_ALWAYS | FA_WRITE);
		Logger.FatFsStatus = f_close(&SDFile);
		Logger.Disk_Mounted = 1;
	}
}

static void UnMountDisk(){
	Logger.FatFsStatus = f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
	Logger.Disk_Mounted = 0;
}

void LogI2C_Operation(I2C_DeviceStruct Instance, uint8_t Operation, uint8_t BlockSize){
	strcpy(Logger.CurrentInstance, Instance.Name);
	char operation_type[10];
	char log_type[10];
	char result[10];
	if (Operation == Writing) strcpy(operation_type, "writing");
	else strcpy(operation_type, "reading");
	if (Instance.ConnectionStatus == HAL_OK) { strcpy(log_type, LOG_LEVEL[LOG_MESSAGE]); strcpy(result, "Success"); }
	else { strcpy(log_type, LOG_LEVEL[WARNING_MESSAGE]); strcpy(result, "Error"); }
	sprintf(Logger.Message, "%s Instance: %s, Instance Message: %s %s %d bits (I2C)\n", log_type, Logger.CurrentInstance, result, operation_type, BlockSize);
	WriteLog();
}

void LogI2C(I2C_BusStruct Instance){
	strcpy(Logger.CurrentInstance, "I2C Bus");
	if (Instance.Scanned == 0) sprintf(Logger.Message, "%s Instance: %s, Instance Message: Initialized\n", LOG_LEVEL[LOG_MESSAGE], Logger.CurrentInstance);
	else sprintf(Logger.Message, "%s Instance: %s, Found devices: %d\n", LOG_LEVEL[LOG_MESSAGE], Logger.CurrentInstance, Instance.Devices);
	WriteLog();
}

void LogState(I2C_DeviceStruct Instance){
	char log_level[15];
	char log_status[30];
	
	strcpy(Logger.CurrentInstance, Instance.Name);
	switch (Instance.State){
		case NotInitialized:
		{
			strcpy(log_level, LOG_LEVEL[LOG_MESSAGE]);
			strcpy(log_status, "Initializing");
			break;
		}
		case Initialized:
		{
			strcpy(log_level, LOG_LEVEL[LOG_MESSAGE]);
			strcpy(log_status, "Initialized");
			break;
		}
		case InitializationError:
		{
			strcpy(log_level, LOG_LEVEL[WARNING_MESSAGE]);
			strcpy(log_status, "Initialization Error");
			break;
		}
		case ConnectionLost:
		{
			strcpy(log_level, LOG_LEVEL[WARNING_MESSAGE]);
			strcpy(log_status, "Connection Lost");
			break;
		}
		case ID_Check_Error:
		{
			strcpy(log_level, LOG_LEVEL[WARNING_MESSAGE]);
			strcpy(log_status, "ID Check Error");
			break;
		}
		case WritingError:
		{
			strcpy(log_level, LOG_LEVEL[WARNING_MESSAGE]);
			strcpy(log_status, "Writing Error");
			break;
		}
		case ReadingError:
		{
			strcpy(log_level, LOG_LEVEL[WARNING_MESSAGE]);
			strcpy(log_status, "Reading Error");
			break;
		}
		default:
		{
			strcpy(log_level, LOG_LEVEL[LOG_MESSAGE]);
			strcpy(log_status, "Working");
			break;
		}			
	}

    sprintf(Logger.Message, "%s Instance: %s, Instance Message: %s\n", log_level, Logger.CurrentInstance, log_status);
	WriteLog();
}

static void CreateInitialLog(){
	sprintf(Logger.Message, "%s Initial log created. %s status: Initialized\n", LOG_LEVEL[LOG_MESSAGE], Logger.CurrentInstance);
	WriteLog();
}

void InitSDSystem(){	
	MX_SDMMC1_SD_Init();
	MX_FATFS_Init();
	strcpy(Logger.CurrentInstance, "FatFS");	
	strcpy(Logger.Message, "");
	Logger.current_slot = 0;
	Logger.Disk_Mounted = 0;
	Logger.File_Opened = 0;
	MountDisk();
	if (Logger.Disk_Mounted)
		CreateInitialLog();
}

void ForceDataLogging(){
	NVIC_DisableIRQ(TIM6_DAC_IRQn);
	NVIC_DisableIRQ(TIM17_IRQn);
	sprintf(Logger.Message, "%s %s %s %s %s\n", NEO7M.PayloadMessage, BMP280.DataRepr, Barometer.DataRepr, ADXL345.DataRepr, MPU6050.DataRepr);
	NVIC_EnableIRQ(TIM17_IRQn);
	WriteLog();
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
