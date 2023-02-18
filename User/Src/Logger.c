#include "Logger.h"
#include "CoreTypes.h"

Logger_TypeDefStruct Logger = {0};

extern SD_HandleTypeDef hsd1;

static char *LogStatus[] = {"NotInitialized",
                            "Initialized",
                            "Initialization Error",
                            "Working",
                            "Connection Lost",
                            "Error Writing",
                            "Success Writing",
                            "Error Reading",
                            "Success Reading",
                            "ID Check Error"};

extern Device_TypeDefStruct BMP280;
extern Device_TypeDefStruct ADXL345;
extern Device_TypeDefStruct MPU6050;
extern GPS_TypeDefStruct NEO7M;
extern Device_TypeDefStruct TrAcc;
extern Device_TypeDefStruct TrBaro;
extern Device_TypeDefStruct TrGyro;
extern Device_TypeDefStruct AnalogBarometer;

static void SetFileName(uint16_t file_number);
static void WriteLog();
static void MountDisk();

void InitSDSystem() {
    strcpy(Logger.Message, "");
    Logger.DiskMounted = 0;
    Logger.FileOpened = 0;
    Logger.FileCount = 0;

    SetFileName(Logger.FileCount);

    MountDisk();
}

static void MountDisk() {
    Logger.FatFsStatus = f_mount(&SDFatFS, (TCHAR const *) SDPath, 0);
    if (Logger.FatFsStatus == FR_OK) {
        Logger.FatFsStatus = f_open(&SDFile, Logger.FileName, FA_CREATE_ALWAYS | FA_WRITE);
        if (Logger.FatFsStatus == FR_OK) {
            Logger.FatFsStatus = f_close(&SDFile);
            Logger.DiskMounted = 1;
        }
    }
}

static void SetFileName(uint16_t file_number) {
    sprintf(Logger.FileName, "DATA%d.txt", file_number);
    Logger.LinesCount = 0;
}

static HAL_StatusTypeDef CheckDiskAndTryReconnect() {
    if (Logger.FatFsStatus != FR_OK || !Logger.DiskMounted) {
        MountDisk();
        if (Logger.FatFsStatus != FR_OK || !Logger.DiskMounted)
            return HAL_ERROR;
    }
    return HAL_OK;
}

static void WriteLog() {
    Logger.FatFsStatus = f_open(&SDFile, Logger.FileName, FA_OPEN_APPEND | FA_WRITE);
    if (Logger.FatFsStatus == FR_OK) {
        Logger.FileOpened = 1;
        Logger.FatFsStatus = f_write(&SDFile, Logger.Message, strlen((char *) Logger.Message), NULL);
        Logger.FatFsStatus = f_close(&SDFile);
        if (Logger.FatFsStatus == FR_OK) {
            Logger.FileOpened = 0;
            strcpy(Logger.Message, "");
            Logger.LinesCount++;
            if (Logger.LinesCount >= LINES_COUNT)
                SetFileName(++Logger.FileCount);
        }
    }
}

void ForceDataLogging() {
    if (CheckDiskAndTryReconnect() != HAL_OK)
        return;

    NVIC_DisableIRQ(TIM17_IRQn);
    sprintf(Logger.Message, "%f [DATA] %s %s %s %s %s %s %s %s\n",
            (float)HAL_GetTick()/60000,
            NEO7M.PayloadMessage,
            BMP280.DataRepr,
            TrBaro.DataRepr,
            AnalogBarometer.DataRepr,
            ADXL345.DataRepr,
            TrAcc.DataRepr,
            MPU6050.DataRepr,
            TrGyro.DataRepr);
    NVIC_EnableIRQ(TIM17_IRQn);
    WriteLog();
}
