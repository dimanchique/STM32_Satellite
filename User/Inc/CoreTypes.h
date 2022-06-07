#pragma once

#define UNREACHABLE "UNREACHABLE"

/** Operation Type Enum **/
typedef enum { Reading, Writing } OperationType;

/** Connection Status Enum **/
typedef enum {
    NotInitialized,
    Initialized,
    InitializationError,
    Working,
    ConnectionLost,
    WritingError,
    WritingSuccess,
    ReadingError,
    ReadingSuccess,
    ID_Check_Error
} ConnectionStatusType;

/**              **/
/** SENSORS BASE **/
/**              **/
/** TroykaBarometer Data Struct **/
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

/** TroykaGyroscope Data Struct **/
struct GyroscopeData {
    double GyroX;
    double GyroY;
    double GyroZ;
};

/**     **/
/** I2C **/
/**     **/
/** I2C Bus Struct **/
typedef struct I2C_BusStruct {
    I2C_HandleTypeDef I2C_Instance;
    HAL_StatusTypeDef OperationResult;
} I2C_BusStruct;

/** Base I2C Device Struct **/
typedef struct I2C_DeviceStruct {
    char *Name;
    ConnectionStatusType State;
    uint8_t CommAddress;
    uint8_t Device_ID;
    uint8_t ID_Register;
    HAL_StatusTypeDef ConnectionStatus;
} I2C_DeviceStruct;

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

/** BMP280 Device Struct **/
typedef struct {
    I2C_DeviceStruct Communicator;
    struct CalibrationData CalibrationCoefficients;
    struct BarometerData Data;
    char DataRepr[50];
} BMP280_TypeDef;

/** ADXL345 Device Struct **/
typedef struct {
    I2C_DeviceStruct Communicator;
    struct AccelerometerData Data;
    char DataRepr[50];
} ADXL345_TypeDef;

/** MPU6050 Device Struct **/
typedef struct {
    I2C_DeviceStruct Communicator;
    struct GyroscopeData GyroData;
    struct AccelerometerData AccData;
    float Temperature;
    char DataRepr[50];
} MPU6050_TypeDef;

/** LPS311AP/LPS25HB Device Struct **/
typedef struct {
    I2C_DeviceStruct Communicator;
    struct BarometerData Data;
    char DataRepr[50];
} TroykaBarometer_TypeDef;

/** LIS331DLH Device Struct **/
typedef struct {
    I2C_DeviceStruct Communicator;
    struct AccelerometerData Data;
    char DataRepr[50];
} TroykaAccelerometer_TypeDef;

/** I3G4250D Device Struct **/
typedef struct {
    I2C_DeviceStruct Communicator;
    struct GyroscopeData GyroData;
    char DataRepr[50];
} TroykaGyroscope_TypeDef;

typedef struct {
    struct BarometerData Data;
    char *DeviceName;
    uint8_t ADC_Channel;
    char DataRepr[50];
} AnalogBarometer_TypeDef;

/**     **/
/** GPS **/
/**     **/
#define GPS_DATA_SIZE           500
#define GPS_PAYLOAD_SIZE        100

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
    char TimeRepr[8];
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
#define MESSAGE_SIZE            100
#define RESPONSE_SIZE           40

/** SIM900 Device Struct **/
typedef struct {
    HAL_StatusTypeDef Status;
    char Message[MESSAGE_SIZE];
    char Response[RESPONSE_SIZE];
} GSM_TypeDef;
