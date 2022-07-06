#pragma once

#include "stm32h7xx_hal.h"
#include "string.h"
#include "stdio.h"

#define UNREACHABLE "UNREACHABLE"
#define be24_to_word24(a)           ((((a)>>16)&0x000000ff)|((a)&0x0000ff00)|(((a)<<16)&0x00ff0000)) //flip MSB and LSB
#define Pa_to_mmHg(x)               ((x) * 0.0075006156130264)
#define mB_to_Pa                    100
#define mB_to_mmHg                  0.75f
#define GSM_MESSAGE_SIZE            100
#define GSM_RESPONSE_SIZE           40
#define GPS_DATA_SIZE               300
#define GPS_PAYLOAD_SIZE            100

/** Operation Type Enum **/
typedef enum {
    Reading,
    Writing
} OperationType;

/** Connection Status Enum **/
typedef enum {
    NotInitialized,
    Initialized,
    InitializationError,
    Working,
    Error,
    ConnectionLost,
    ID_Check_Error
} ConnectionStatusType;

/** Device Struct **/
typedef struct {
    void* Communicator;
    char* DeviceName;
    char DataRepr[60];
} DeviceTypeDef;

/**         **/
/** SENSORS **/
/**         **/
/** Barometer Data Struct **/
struct BarometerData {
    double Pressure;
    double Altitude;
    double mmHg;
    double base_mmHg;
    float Temperature;
};

/** Accelerometer Data Struct **/
struct AccelerometerData {
    double AccX;
    double AccY;
    double AccZ;
};

/** Gyroscope Data Struct **/
struct GyroscopeData {
    double GyroX;
    double GyroY;
    double GyroZ;
};

/**     **/
/** I2C **/
/**     **/
/** I2C Bus Struct **/
typedef struct {
    I2C_HandleTypeDef I2C_Instance;
    HAL_StatusTypeDef OperationResult;
} I2C_BusStruct;

/** I2C Communication Bus Struct **/
typedef struct {
    ConnectionStatusType State;
    char* Name;
    uint8_t CommAddress;
    uint8_t Device_ID;
    uint8_t ID_Register;
    HAL_StatusTypeDef ConnectionStatus;
} I2C_CommunicatorStruct;

/** ADC Communication Bus Struct **/
typedef struct {
    ADC_HandleTypeDef *Instance;
    HAL_StatusTypeDef State;
    uint8_t Channel;
} ADC_CommunicatorStruct;

/**         **/
/** DEVICES **/
/**         **/
/** BMP280 Calibration Data Struct **/
struct CalibrationData {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
};

/**     **/
/** GPS **/
/**     **/
/** Struct for Extra Data **/
typedef struct {
    float Altitude;
    char AltitudeUnits;
    float SpeedInKnots;
} GPSExtraData;

/** GPS Protocol Struct **/
typedef struct {
    char PacketName[5];
    int IsValid;
    float RawTime;
    char TimeRepr[11];
    float RawLatitude;
    char LatitudeDirection;
    float RawLongitude;
    char LongitudeDirection;
    float LongitudeDegrees;
    float LatitudeDegrees;
    GPSExtraData Extras;
} GPSProtocol;

/** NEO7M Device Struct **/
typedef struct {
    uint8_t ReceivingFinished;
    char Message[GPS_DATA_SIZE];
    char TempMessage[GPS_DATA_SIZE];
    char PayloadMessage[GPS_PAYLOAD_SIZE];
    GPSProtocol GPGGA;
    GPSProtocol GPGLL;
    GPSProtocol GPRMC;
    float SkipFloat;
    int SkipInt;
    char SkipChar;
} GPS_TypeDef;

/**     **/
/** GSM **/
/**     **/
/** SIM900 Device Struct **/
typedef struct {
    HAL_StatusTypeDef Status;
    char Message[GSM_MESSAGE_SIZE];
    char Response[GSM_RESPONSE_SIZE];
} GSM_TypeDef;
