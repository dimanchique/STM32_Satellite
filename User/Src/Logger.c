#include "Logger.h"

#include "ModuleCoreTypes.h"
#include "NEO7M.h"

static char* LogLevel[] = {"[LOG]",
                           "[DATA]",
                           "[WARNING]"};

static char* LogStatus[] = {"NotInitialized",
                            "Initialized",
                            "Initialization Error",
                            "Working",
                            "Connection Lost",
                            "Error Writing",
                            "Success Writing",
                            "Error Reading",
                            "Success Reading",
                            "ID Check Error"};

extern SD_HandleTypeDef hsd1;
DiskWriter Logger = {0};

extern I2C_BusStruct I2C_Bus;
extern BMP280_TypeDef BMP280;
extern ADXL345_TypeDef ADXL345;
extern MPU6050_TypeDef MPU6050;
extern GPS_TypeDef NEO7M;
extern TroykaBarometer_TypeDef Barometer;

static void WriteLog() {
    if (Logger.CurrentMessageSlot < QUEUE_SLOTS) {
        strcpy(Logger.Queue[Logger.CurrentMessageSlot++], Logger.Message);
        if (Logger.CurrentMessageSlot < QUEUE_SLOTS)
            return;
    }

    NVIC_DisableIRQ(TIM17_IRQn);
    strcpy(Logger.Message, "");
    for (uint8_t i = 0; i < QUEUE_SLOTS; i++) {
        strcat(Logger.Message, Logger.Queue[i]);
        strcpy(Logger.Queue[i], "");
    }
    Logger.CurrentMessageSlot = 0;
    NVIC_EnableIRQ(TIM17_IRQn);

    Logger.FatFsStatus = f_open(&SDFile, DEFAULT_FILENAME, FA_OPEN_APPEND | FA_WRITE);
    if (Logger.FatFsStatus == FR_OK) {
        Logger.FileOpened = 1;
        Logger.FatFsStatus = f_write(&SDFile, Logger.Message, strlen((char *) Logger.Message), NULL);
        Logger.FatFsStatus = f_close(&SDFile);
    }
    if (Logger.FatFsStatus == FR_OK)
        Logger.FileOpened = 0;

    strcpy(Logger.Message, "");
}

void LogI2C_Operation(I2C_DeviceStruct *Instance, OperationType Operation, uint8_t BlockSize) {
    char log_level[10];
    char result[21];

    strcpy(Logger.CurrentInstance, Instance->Name);
    if (Instance->ConnectionStatus == HAL_OK) {
        if (Operation==Reading)
            strcpy(result, LogStatus[ReadingSuccess]);
        else
            strcpy(result, LogStatus[WritingSuccess]);
        strcpy(log_level, LogLevel[LOG]);
    }
    else {
        if (Operation==Reading)
            strcpy(result, LogStatus[ReadingError]);
        else
            strcpy(result, LogStatus[WritingError]);
        strcpy(log_level, LogLevel[WARNING]);
    }
    sprintf(Logger.Message, "%s Instance: %s, Instance Message: %s %d bits\n",
            log_level,
            Logger.CurrentInstance,
            result,
            BlockSize);
    WriteLog();
}

void LogState(I2C_DeviceStruct *Instance) {
    char log_level[10];
    char log_status[21];

    strcpy(log_status, LogStatus[Instance->State]);

    strcpy(Logger.CurrentInstance, Instance->Name);
    switch (Instance->State) {
        case NotInitialized:
        case InitializationError:
        case ConnectionLost:
        case ID_Check_Error: {
            strcpy(log_level, LogLevel[WARNING]);
            break;
        }
        case Initialized:
        default:
        {
            strcpy(log_level, LogLevel[LOG]);
            break;
        }
    }
    sprintf(Logger.Message,
            "%s Instance: %s, Message: %s\n",
            log_level,
            Logger.CurrentInstance,
            log_status);
    WriteLog();
}

void ForceDataLogging() {
    NVIC_DisableIRQ(TIM6_DAC_IRQn);
    NVIC_DisableIRQ(TIM17_IRQn);
    sprintf(Logger.Message, "%s %s %s %s %s %s\n",
            LogLevel[DATA],
            NEO7M.PayloadMessage,
            BMP280.DataRepr,
            Barometer.DataRepr,
            ADXL345.DataRepr,
            MPU6050.DataRepr);
    NVIC_EnableIRQ(TIM17_IRQn);
    WriteLog();
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

static void MX_SDMMC1_SD_Init(void) {
    hsd1.Instance = SDMMC1;
    hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
    hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
    hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd1.Init.ClockDiv = 0;
}

static void MountDisk() {
    Logger.FatFsStatus = f_mount(&SDFatFS, (TCHAR const *) SDPath, 0);
    if (Logger.FatFsStatus == FR_OK) {
        Logger.FatFsStatus = f_open(&SDFile, DEFAULT_FILENAME, FA_CREATE_ALWAYS | FA_WRITE);
        Logger.FatFsStatus = f_close(&SDFile);
        Logger.DiskMounted = 1;
    }
}

static void UnMountDisk() {
    Logger.FatFsStatus = f_mount(&SDFatFS, (TCHAR const *) NULL, 0);
    Logger.DiskMounted = 0;
}

static void CreateInitialLog() {
    sprintf(Logger.Message, "%s Initial log created. FatFS status: %s\n",
            LogLevel[LOG],
            LogStatus[Initialized]);
    WriteLog();
}

void InitSDSystem() {
    MX_SDMMC1_SD_Init();
    MX_FATFS_Init();
    strcpy(Logger.CurrentInstance, "FatFS");
    strcpy(Logger.Message, "");
    Logger.CurrentMessageSlot = 0;
    Logger.DiskMounted = 0;
    Logger.FileOpened = 0;
    MountDisk();
    if (Logger.DiskMounted)
        CreateInitialLog();
}