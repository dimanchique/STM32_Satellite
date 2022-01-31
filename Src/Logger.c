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

static void WriteLog(){
	if (Logger.current_slot<QUEUE_SLOTS){
		strcpy(Logger.Queue[Logger.current_slot++], Logger.Message);
		if (Logger.current_slot<QUEUE_SLOTS) return;
	}		
	strcpy(Logger.Message, "");
	for (uint8_t i = 0; i<QUEUE_SLOTS; i++){
		strcat(Logger.Message, Logger.Queue[i]);
		strcpy(Logger.Queue[i], "");
	}	
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
	if (Logger.FatFsStatus == FR_OK)
		Logger.Disk_Mounted = 1;
}

static void UnMountDisk(){
	Logger.FatFsStatus = f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
	Logger.Disk_Mounted = 0;
}

void LogI2C_Operation(I2C_Communicator Instance, uint8_t Operation, uint8_t BlockSize){
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

void LogState(I2C_Communicator Instance){
	
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
	
	strcpy(Logger.Message, log_level);
	strcat(Logger.Message, " Instance: ");
	strcat(Logger.Message, Logger.CurrentInstance);
	strcat(Logger.Message, ", Instance Message:");
	strcat(Logger.Message, log_status);
	strcat(Logger.Message, "\n");
	WriteLog();
}

static void CreateInitialLog(){
	Logger.FatFsStatus = f_open(&SDFile, DEFAULT_FILENAME, FA_CREATE_ALWAYS | FA_WRITE);
	if (Logger.FatFsStatus == FR_OK){
		Logger.File_Opened = 1;	
		sprintf(Logger.Message, "%s Initial log created. %s status: Initialized\n", LOG_LEVEL[LOG_MESSAGE], Logger.CurrentInstance);
		Logger.Write_Status = f_write(&SDFile, Logger.Message, strlen((char *)Logger.Message), (void *)&byteswritten);
		Logger.FatFsStatus = f_close(&SDFile);
		}
	if (Logger.FatFsStatus == FR_OK) Logger.File_Opened = 0;
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
