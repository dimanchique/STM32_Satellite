#include "MPU6050.h"
#include "Logger.h"

DeviceTypeDef MPU6050 = {0};
static I2C_CommunicatorStruct MPU_Communicator = {0};
static struct AccelerometerData MPU_AccData = {0};
static struct GyroscopeData MPU_GyroData = {0};
static float MPU_Temperature = 0;

static double ACC_ERROR_X, ACC_ERROR_Y, ACC_ERROR_Z, GYRO_ERROR_X, GYRO_ERROR_Y, GYRO_ERROR_Z;
static uint16_t CalibrationCycles = 500;

static void GenerateDataRepresentation();
static void MPU_Calibrate();
static void MPU_Read_Acceleration();
static void MPU_Read_Gyroscope();
static void MPU_Read_Temperature();

void MPU_Init() {
    MPU6050.Communicator = &MPU_Communicator;
    MPU6050.DeviceName = "MPU6050";
    /** Communication Section **/
    I2C_SetupCommunicator(&MPU_Communicator,
                          MPU6050.DeviceName,
                          MPU6050_ADDRESS,
                          MPU6050_ID,
                          MPU6050_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&MPU_Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&MPU_Communicator)){
        I2C_WriteData8(&MPU_Communicator,
                       MPU6050_PWR_MGMT_1,
                       MPU6050_RESET);
        I2C_WriteData8(&MPU_Communicator,
                       MPU6050_ACCEL_CONFIG,
                       (uint8_t)(MPU6050_ACC_RESOLUTION | MPU6050_TEST_ACC));
        I2C_WriteData8(&MPU_Communicator,
                       MPU6050_GYRO_CONFIG,
                       (uint8_t)(MPU6050_GYRO_RESOLUTION | MPU6050_TEST_GYRO));
        HAL_Delay(50);
        MPU_Calibrate();
        if (MPU_Communicator.ConnectionStatus == HAL_OK)
            MPU_Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&MPU_Communicator);
#endif
}

void MPU_ReadData() {
    if (MPU_Communicator.ConnectionStatus == HAL_OK) {
        MPU_Read_Acceleration();
        MPU_Read_Gyroscope();
        MPU_Read_Temperature();
    }
    GenerateDataRepresentation();
}

static void GenerateDataRepresentation() {
    if (MPU_Communicator.ConnectionStatus == HAL_OK)
        sprintf(MPU6050.DataRepr,
                "[%s] %.3f %.3f %.3f %.3f %.3f %.3f %.3f;",
                MPU6050.DeviceName,
                MPU_AccData.AccX,
                MPU_AccData.AccY,
                MPU_AccData.AccZ,
                MPU_GyroData.GyroX,
                MPU_GyroData.GyroY,
                MPU_GyroData.GyroZ,
                MPU_Temperature);
    else
        sprintf(MPU6050.DataRepr, "[%s] %s;", MPU6050.DeviceName, UNREACHABLE);
}

static void MPU_Calibrate() {
    uint8_t data[6] = {0};
    int16_t xx, yy, zz;
    ACC_ERROR_X = 0;
    ACC_ERROR_Y = 0;
    ACC_ERROR_Z = 0;
    GYRO_ERROR_X = 0;
    GYRO_ERROR_Y = 0;
    GYRO_ERROR_Z = 0;

    for (uint16_t n = 0; n < CalibrationCycles; n++) {
        I2C_ReadDataNx8(&MPU_Communicator, MPU6050_ACC, data, 6);
        xx = (int16_t) ((data[1] << 8) | data[0]);
        yy = (int16_t) ((data[3] << 8) | data[2]);
        zz = (int16_t) ((data[5] << 8) | data[4]);
        ACC_ERROR_X += xx / MPU6050_ACC_SCALE;
        ACC_ERROR_Y += yy / MPU6050_ACC_SCALE;
        ACC_ERROR_Z += zz / MPU6050_ACC_SCALE;
    }
    ACC_ERROR_X /= CalibrationCycles;
    ACC_ERROR_Y /= CalibrationCycles;
    ACC_ERROR_Z /= CalibrationCycles;

    for (uint16_t n = 0; n < CalibrationCycles; n++) {
        I2C_ReadDataNx8(&MPU_Communicator, MPU6050_GYRO, data, 6);
        xx = (int16_t) ((data[1] << 8) | data[0]);
        yy = (int16_t) ((data[3] << 8) | data[2]);
        zz = (int16_t) ((data[5] << 8) | data[4]);
        GYRO_ERROR_X += xx / MPU6050_GYRO_SCALE;
        GYRO_ERROR_Y += yy / MPU6050_GYRO_SCALE;
        GYRO_ERROR_Z += zz / MPU6050_GYRO_SCALE;
    }
    GYRO_ERROR_X /= CalibrationCycles;
    GYRO_ERROR_Y /= CalibrationCycles;
    GYRO_ERROR_Z /= CalibrationCycles;
}

static void MPU_Read_Acceleration() {
    uint8_t data[6] = {0};
    int16_t xx, yy, zz;
    I2C_ReadDataNx8(&MPU_Communicator, MPU6050_ACC, data, 6);
    xx = (int16_t) ((data[1] << 8) | data[0]);
    yy = (int16_t) ((data[3] << 8) | data[2]);
    zz = (int16_t) ((data[5] << 8) | data[4]);
    MPU_AccData.AccX = (xx / MPU6050_ACC_SCALE) - ACC_ERROR_X;
    MPU_AccData.AccY = (yy / MPU6050_ACC_SCALE) - ACC_ERROR_Y;
    MPU_AccData.AccZ = (zz / MPU6050_ACC_SCALE) - ACC_ERROR_Z;
}

static void MPU_Read_Gyroscope() {
    uint8_t data[6] = {0};
    int16_t xx, yy, zz;
    I2C_ReadDataNx8(&MPU_Communicator, MPU6050_GYRO, data, 6);
    xx = (int16_t) ((data[1] << 8) | data[0]);
    yy = (int16_t) ((data[3] << 8) | data[2]);
    zz = (int16_t) ((data[5] << 8) | data[4]);
    MPU_GyroData.GyroX = xx / MPU6050_GYRO_SCALE - GYRO_ERROR_X;
    MPU_GyroData.GyroY = yy / MPU6050_GYRO_SCALE - GYRO_ERROR_Y;
    MPU_GyroData.GyroZ = zz / MPU6050_GYRO_SCALE - GYRO_ERROR_Z;
}

static void MPU_Read_Temperature() {
    uint8_t data[2] = {0};
    I2C_ReadDataNx8(&MPU_Communicator, MPU6050_TEMP, data, 2);
    MPU_Temperature = 36.53f + (float) (int16_t) ((data[0] << 8) | data[1]) / 340.0f;
}
