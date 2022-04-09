#pragma once

/** Basic Barometer Struct **/
struct BarometerData {
    double Pressure;
    double Altitude;
    double mmHg;
    double base_mmHg;
};

/** Basic Accelerometer Config Struct **/
struct AccelerometerConfig {
    uint8_t Datarate;
    uint8_t DataFormat;
    uint8_t Power;
};

/** Basic Accelerometer Data Struct **/
struct AccelerometerData {
    double AccX;
    double AccY;
    double AccZ;
};
/** Basic Gyroscope Struct **/
struct GyroscopeData {
    double GyroX;
    double GyroY;
    double GyroZ;
};

/** Struct for Extra Data (not every device has a temperature sensor) **/
struct ModuleExtraData {
    double Temperature;
};
