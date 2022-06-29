#include "Logger.h"
#include "CoreTypes.h"

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

extern DeviceTypeDef BMP280;
extern DeviceTypeDef ADXL345;
extern DeviceTypeDef MPU6050;
extern GPS_TypeDef NEO7M;
extern DeviceTypeDef TroykaAccelerometer;
extern DeviceTypeDef TroykaBarometer;
extern DeviceTypeDef TroykaGyroscope;
extern DeviceTypeDef AnalogBarometer;

static void SetFileName(uint8_t file_number);
static FRESULT OpenFile(uint8_t mode);
static void WriteLog();
static void MountDisk();

void InitSDSystem() {
    strcpy(Logger.Message, "");
    Logger.DiskMounted = 0;
    Logger.FileOpened = 0;
    Logger.FileCount = 0;

    SetFileName(Logger.FileCount);

    MountDisk();
    if (Logger.DiskMounted){
        strcpy(Logger.Message, "[LOG] Initial log created. FatFS status: Initialized\n");
        WriteLog();
    }
}

static void MountDisk() {
    Logger.FatFsStatus = f_mount(&SDFatFS, (TCHAR const *) SDPath, 0);
    if (Logger.FatFsStatus == FR_OK) {
        Logger.FatFsStatus = OpenFile(FA_CREATE_ALWAYS | FA_WRITE);
        if (Logger.FatFsStatus == FR_OK)
        {
            Logger.FatFsStatus = f_close(&SDFile);
            Logger.DiskMounted = 1;
        }
    }
}

static void SetFileName(uint8_t file_number) {
    sprintf(Logger.FileName, "DATA%d.txt", file_number);
    Logger.LinesCount = 0;
}

static FRESULT OpenFile(uint8_t mode) {
    return f_open(&SDFile, Logger.FileName, mode);
}

static void WriteLog() {
    if (Logger.FatFsStatus != FR_OK || !Logger.DiskMounted)
        return;

    NVIC_EnableIRQ(TIM17_IRQn);
    Logger.FatFsStatus = OpenFile(FA_OPEN_APPEND | FA_WRITE);
    if (Logger.FatFsStatus == FR_OK) {
        Logger.FileOpened = 1;
        Logger.FatFsStatus = f_write(&SDFile, Logger.Message, strlen((char*)Logger.Message), NULL);
        Logger.FatFsStatus = f_close(&SDFile);
    }
    if (Logger.FatFsStatus == FR_OK)
        Logger.FileOpened = 0;

    strcpy(Logger.Message, "");
    Logger.LinesCount++;

    if (Logger.LinesCount >= LINES_COUNT)
        SetFileName(++Logger.FileCount);
}

void LogOperation(I2C_CommunicatorStruct *Instance, OperationType Operation, uint8_t BlockSize) {
    if (Logger.FatFsStatus != FR_OK || !Logger.DiskMounted)
        return;

    char log_level[10];
    char result[20];

    if (Instance->ConnectionStatus == HAL_OK) {
        if (Operation==Reading)
            strcpy(result, "Success reading");
        else
            strcpy(result, "Success writing");
        strcpy(log_level, "[LOG]");
    }
    else {
        if (Operation==Reading)
            strcpy(result, "Error reading");
        else
            strcpy(result, "Error writing");
        strcpy(log_level, "[WARNING]");
    }
    sprintf(Logger.Message, "%s Instance: %s, Instance Message: %s %d bits\n",
            log_level,
            Instance->Name,
            result,
            BlockSize);
    WriteLog();
}

void LogDeviceState(I2C_CommunicatorStruct *Instance) {
    if (Logger.FatFsStatus != FR_OK || !Logger.DiskMounted)
        return;

    char log_level[10];
    switch (Instance->State) {
        case NotInitialized:
        case InitializationError:
        case ConnectionLost:
        case ID_Check_Error: {
            strcpy(log_level, "[WARNING]");
            break;
        }
        case Initialized:
        default:
        {
            strcpy(log_level, "[LOG]");
            break;
        }
    }
    sprintf(Logger.Message,
            "%s Instance: %s, Message: %s\n",
            log_level,
            Instance->Name,
            LogStatus[Instance->State]);
    WriteLog();
}

void ForceDataLogging() {
    if (Logger.FatFsStatus != FR_OK || !Logger.DiskMounted)
        return;

    NVIC_DisableIRQ(TIM6_DAC_IRQn);
    NVIC_DisableIRQ(TIM17_IRQn);
    sprintf(Logger.Message, "%lu [DATA] %s %s %s %s %s %s %s %s\n",
            HAL_GetTick(),
            NEO7M.PayloadMessage,
            BMP280.DataRepr,
            TroykaBarometer.DataRepr,
            AnalogBarometer.DataRepr,
            ADXL345.DataRepr,
            TroykaAccelerometer.DataRepr,
            MPU6050.DataRepr,
            TroykaGyroscope.DataRepr);
    NVIC_EnableIRQ(TIM17_IRQn);
    WriteLog();
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
