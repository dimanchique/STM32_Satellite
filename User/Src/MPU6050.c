#include "MPU6050.h"
#include "Logger.h"

DeviceTypeDef MPU6050 = {0};
static I2C_CommunicatorStruct MPU_Communicator = {0};
static struct AccelerometerData MPU_AccData = {0};
static struct GyroscopeData MPU_GyroData = {0};
static float MPU_Temperature = 0;

static double ACC_ERROR_X, ACC_ERROR_Y, ACC_ERROR_Z, GYRO_ERROR_X, GYRO_ERROR_Y, GYRO_ERROR_Z = 0;
static uint16_t CalibrationCycles = 500;

static void GenerateDataRepresentation(void) {
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

static void MPU_Calibrate(void) {
    double AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
    uint8_t data[6] = {0};
    int16_t xx, yy, zz;

    for (uint16_t n = 0; n < CalibrationCycles; n++) {
        I2C_ReadData6x8(&MPU_Communicator, MPU6050_ACC, data);
        xx = (int16_t) ((data[1] << 8) | data[0]);
        yy = (int16_t) ((data[3] << 8) | data[2]);
        zz = (int16_t) ((data[5] << 8) | data[4]);
        AccX = xx / MPU6050_ACC_SCALE;
        AccY = yy / MPU6050_ACC_SCALE;
        AccZ = zz / MPU6050_ACC_SCALE;
        ACC_ERROR_X = ACC_ERROR_X + AccX;
        ACC_ERROR_Y = ACC_ERROR_Y + AccY;
        ACC_ERROR_Z = ACC_ERROR_Z + AccZ;
    }
    ACC_ERROR_X = ACC_ERROR_X / CalibrationCycles;
    ACC_ERROR_Y = ACC_ERROR_Y / CalibrationCycles;
    ACC_ERROR_Z = ACC_ERROR_Z / CalibrationCycles;

    for (uint16_t n = 0; n < CalibrationCycles; n++) {
        I2C_ReadData6x8(&MPU_Communicator, MPU6050_GYRO, data);
        xx = (int16_t) ((data[1] << 8) | data[0]);
        yy = (int16_t) ((data[3] << 8) | data[2]);
        zz = (int16_t) ((data[5] << 8) | data[4]);
        GyroX = xx / MPU6050_GYRO_SCALE;
        GyroY = yy / MPU6050_GYRO_SCALE;
        GyroZ = zz / MPU6050_GYRO_SCALE;
        GYRO_ERROR_X = GYRO_ERROR_X + GyroX;
        GYRO_ERROR_Y = GYRO_ERROR_Y + GyroY;
        GYRO_ERROR_Z = GYRO_ERROR_Z + GyroZ;
    }
    GYRO_ERROR_X = GYRO_ERROR_X / CalibrationCycles;
    GYRO_ERROR_Y = GYRO_ERROR_Y / CalibrationCycles;
    GYRO_ERROR_Z = GYRO_ERROR_Z / CalibrationCycles;
}

void MPU_Init(void) {
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

static void MPU_Read_Acceleration(void) {
    uint8_t data[6] = {0};
    int16_t xx, yy, zz;
    I2C_ReadData6x8(&MPU_Communicator, MPU6050_ACC, data);
    xx = (int16_t) ((data[1] << 8) | data[0]);
    yy = (int16_t) ((data[3] << 8) | data[2]);
    zz = (int16_t) ((data[5] << 8) | data[4]);
    MPU_AccData.AccX = (xx / MPU6050_ACC_SCALE) - ACC_ERROR_X;
    MPU_AccData.AccY = (yy / MPU6050_ACC_SCALE) - ACC_ERROR_Y;
    MPU_AccData.AccZ = (zz / MPU6050_ACC_SCALE) - ACC_ERROR_Z;
}

static void MPU_Read_Gyroscope(void) {
    uint8_t data[6] = {0};
    int16_t xx, yy, zz;
    I2C_ReadData6x8(&MPU_Communicator, MPU6050_GYRO, data);
    xx = (int16_t) ((data[1] << 8) | data[0]);
    yy = (int16_t) ((data[3] << 8) | data[2]);
    zz = (int16_t) ((data[5] << 8) | data[4]);
    MPU_GyroData.GyroX = xx / MPU6050_GYRO_SCALE - GYRO_ERROR_X;
    MPU_GyroData.GyroY = yy / MPU6050_GYRO_SCALE - GYRO_ERROR_Y;
    MPU_GyroData.GyroZ = zz / MPU6050_GYRO_SCALE - GYRO_ERROR_Z;
}

static void MPU_Read_Temperature(void) {
    uint8_t data[2] = {0};
    I2C_ReadData2x8(&MPU_Communicator,
                  MPU6050_TEMP,
                  data);
    MPU_Temperature = 36.53f + (float) (int16_t) ((data[0] << 8) | data[1]) / 340.0f;
}

void MPU_ReadData(void) {
    if (MPU_Communicator.ConnectionStatus == HAL_OK) {
        MPU_Read_Acceleration();
        MPU_Read_Gyroscope();
        MPU_Read_Temperature();
    }
    GenerateDataRepresentation();
}
