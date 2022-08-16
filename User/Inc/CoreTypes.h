#pragma once

#include "stm32h7xx_hal.h"
#include "string.h"
#include "stdio.h"

#define UNREACHABLE "NO CONNECTION"
#define be24_to_word24(a)               ((((a)>>16)&0x000000ff)|((a)&0x0000ff00)|(((a)<<16)&0x00ff0000)) //flip MSB and LSB
#define Pa_to_mmHg(x)                   ((x) * 0.0075006156130264)
#define mB_to_Pa(x)                     ((x)*100)
#define mB_to_mmHg(x)                   ((x)*0.75f)
#define mmHg_to_Altitude(ref, mmHg)     (((ref)-(mmHg)) * 10.5f)
#define GSM_MESSAGE_SIZE                100
#define GSM_RESPONSE_SIZE               40
#define GPS_DATA_SIZE                   300
#define GPS_PAYLOAD_SIZE                100
#define SetDeviceStateOK(PORT, PIN)     HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_SET)
#define SetDeviceStateError(PORT, PIN)  HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET)

#define LED_PORT                        GPIOD
#define Logger_PIN                      GPIO_PIN_4
#define BMP_PIN                         GPIO_PIN_6
#define ADXL_PIN                        GPIO_PIN_11
#define MPU_PIN                         GPIO_PIN_13
#define TroykaAcc_PIN                   GPIO_PIN_15
//#define GPS_PIN                         GPIO_PIN_2
//#define TroykaBaro_PIN                  GPIO_PIN_6
//#define TroykaGyro_PIN                  GPIO_PIN_3

/** Operation Type Enum **/
typedef enum {
    Reading, Writing
} OperationType;

/** Connection Status Enum **/
typedef enum {
    NotInitialized, Initialized, InitializationError, Working, Error, ConnectionLost, ID_Check_Error
} ConnectionStatusTypeDef;

/** Device Struct **/
typedef struct {
    void *Communicator;
    char *DeviceName;
    char DataRepr[60];
} Device_TypeDefStruct;

/**              **/
/** SENSORS DATA **/
/**              **/
/** Barometer Data Struct **/
struct BarometerData {
    double Pressure;
    double Altitude;
    double mmHg;
    double mmHg_ref;
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
/** I2C Communication Bus Struct **/
typedef struct {
    ConnectionStatusTypeDef State;
    char *Name;
    uint8_t CommAddress;
    uint8_t Device_ID;
    uint8_t ID_Register;
    HAL_StatusTypeDef ConnectionStatus;
} I2C_TypeDefStruct;

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
    uint8_t MinimalDataReceived;
    float SkipFloat;
    int SkipInt;
    char SkipChar;
} GPS_TypeDefStruct;

/**     **/
/** GSM **/
/**     **/
/** SIM900 Device Struct **/
typedef struct {
    HAL_StatusTypeDef DeviceStatus;
    HAL_StatusTypeDef CommandStatus;
    char Message[GSM_MESSAGE_SIZE];
    char Response[GSM_RESPONSE_SIZE];
} GSM_TypeDefStruct;
