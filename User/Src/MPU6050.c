#include "MPU6050.h"
#include "Logger.h"

MPU6050_TypeDef MPU6050 = {0};

static double ACC_ERROR_X, ACC_ERROR_Y, ACC_ERROR_Z, GYRO_ERROR_X, GYRO_ERROR_Y, GYRO_ERROR_Z = 0;
static uint16_t CalibrationCycles = 500;

static void GenerateDataRepresentation() {
    if (MPU6050.Communicator.ConnectionStatus == HAL_OK)
        sprintf(MPU6050.DataRepr,
                "[%s] %.3f %.3f %.3f %.3f %.3f %.3f %.3f;",
                MPU6050.Communicator.Name,
                MPU6050.AccData.AccX,
                MPU6050.AccData.AccY,
                MPU6050.AccData.AccZ,
                MPU6050.GyroData.GyroX,
                MPU6050.GyroData.GyroY,
                MPU6050.GyroData.GyroZ,
                MPU6050.Temperature);
    else
        sprintf(MPU6050.DataRepr, "[%s] %s;", MPU6050.Communicator.Name, UNREACHABLE);
}

static void MPU_Calibrate(void) {
    double AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
    uint8_t data[6] = {0};
    int16_t xx, yy, zz;

    for (uint16_t n = 0; n < CalibrationCycles; n++) {
        I2C_ReadData6x8(&MPU6050.Communicator, MPU6050_ACC, data);
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
        I2C_ReadData6x8(&MPU6050.Communicator, MPU6050_GYRO, data);
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

void MPU_Init() {
    /** Communication Section **/
    I2C_SetupCommunicator(&MPU6050.Communicator,
                          "MPU6050",
                          MPU6050_ADDRESS,
                          MPU6050_ID,
                          MPU6050_ID_REGISTER);
#ifdef ENABLE_DEBUG
    LogDeviceState(&MPU6050.Communicator);
#endif
    /** Setup Section **/
    if (I2C_DeviceCheckedAndVerified(&MPU6050.Communicator)){
        I2C_WriteData8(&MPU6050.Communicator,
                       MPU6050_PWR_MGMT_1,
                       MPU6050_RESET);
        I2C_WriteData8(&MPU6050.Communicator,
                       MPU6050_ACCEL_CONFIG,
                       (uint8_t)(MPU6050_ACC_RESOLUTION | MPU6050_TEST_ACC));
        I2C_WriteData8(&MPU6050.Communicator,
                       MPU6050_GYRO_CONFIG,
                       (uint8_t)(MPU6050_GYRO_RESOLUTION | MPU6050_TEST_GYRO));
        HAL_Delay(50);
        MPU_Calibrate();
        if (MPU6050.Communicator.ConnectionStatus == HAL_OK)
            MPU6050.Communicator.State = Working;
    }
#ifdef ENABLE_DEBUG
    LogDeviceState(&MPU6050.Communicator);
#endif
}

static void MPU_Read_Acceleration(void) {
    uint8_t data[6] = {0};
    int16_t xx, yy, zz;
    I2C_ReadData6x8(&MPU6050.Communicator, MPU6050_ACC, data);
    xx = (int16_t) ((data[1] << 8) | data[0]);
    yy = (int16_t) ((data[3] << 8) | data[2]);
    zz = (int16_t) ((data[5] << 8) | data[4]);
    MPU6050.AccData.AccX = (xx / MPU6050_ACC_SCALE) - ACC_ERROR_X;
    MPU6050.AccData.AccY = (yy / MPU6050_ACC_SCALE) - ACC_ERROR_Y;
    MPU6050.AccData.AccZ = (zz / MPU6050_ACC_SCALE) - ACC_ERROR_Z;
}

static void MPU_Read_Gyroscope(void) {
    uint8_t data[6] = {0};
    int16_t xx, yy, zz;
    I2C_ReadData6x8(&MPU6050.Communicator, MPU6050_GYRO, data);
    xx = (int16_t) ((data[1] << 8) | data[0]);
    yy = (int16_t) ((data[3] << 8) | data[2]);
    zz = (int16_t) ((data[5] << 8) | data[4]);
    MPU6050.GyroData.GyroX = xx / MPU6050_GYRO_SCALE - GYRO_ERROR_X;
    MPU6050.GyroData.GyroY = yy / MPU6050_GYRO_SCALE - GYRO_ERROR_Y;
    MPU6050.GyroData.GyroZ = zz / MPU6050_GYRO_SCALE - GYRO_ERROR_Z;
}

static void MPU_Read_Temperature(void) {
    uint8_t data[2] = {0};
    I2C_ReadData2x8(&MPU6050.Communicator,
                  MPU6050_TEMP,
                  data);
    MPU6050.Temperature = 36.53f + (float) (int16_t) ((data[0] << 8) | data[1]) / 340.0f;
}

void MPU_ReadData() {
    if (MPU6050.Communicator.ConnectionStatus == HAL_OK) {
        MPU_Read_Acceleration();
        MPU_Read_Gyroscope();
        MPU_Read_Temperature();
    }
    GenerateDataRepresentation();
}
