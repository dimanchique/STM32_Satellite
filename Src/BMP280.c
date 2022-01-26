#include "BMP280.h"

static BMP280_CalibData CalibData;
static float base_hhMg;
static int32_t temper_int;
HAL_StatusTypeDef BMP_Ready = HAL_ERROR;
//------------------------------------------------
static void Calibrate(void){
	float temp, press, alt, mmHg;
	BMP_GetAllData(&temp, &press, &alt, &mmHg);
	base_hhMg = press * 0.000750061683f;
}
//------------------------------------------------
static void BMP_GetTemperature(float* temperature){
	float temper_float = 0.0f;
	uint32_t temper_raw;
	int32_t val1, val2;
	while(BMP_Status() & BMP280_IS_UPDATING);
	I2C_ReadDataBE_U24(BMP280_ADDRESS, BMP280_TEMP_REGISTER,&temper_raw);
	temper_raw >>= 4;
	val1 = ((((temper_raw>>3) - ((int32_t)CalibData.dig_T1 << 1))) * ((int32_t)CalibData.dig_T2)) >> 11;
	val2 = (((((temper_raw>>4) - ((int32_t)CalibData.dig_T1)) * ((temper_raw>>4) - ((int32_t)CalibData.dig_T1))) >> 12) * ((int32_t)CalibData.dig_T3)) >> 14;
	temper_int = val1 + val2;
	temper_float = ((temper_int * 5 + 128) >> 8);
	temper_float /= 100.0f;
	*temperature = temper_float;
}
//------------------------------------------------
static float BMP_GetPressureAndTemperature(float* temperature){
  float press_float = 0.0f;
	uint32_t press_raw, pres_int;
	int64_t val1, val2, p;
	BMP_GetTemperature(temperature);
	while(BMP_Status() & BMP280_IS_UPDATING);
	I2C_ReadDataBE_U24(BMP280_ADDRESS, BMP280_PRESSURE_REGISTER, &press_raw);
	press_raw >>= 4;
	val1 = ((int64_t) temper_int) - 128000;
	val2 = val1 * val1 * (int64_t)CalibData.dig_P6;
	val2 = val2 + ((val1 * (int64_t)CalibData.dig_P5) << 17);
	val2 = val2 + ((int64_t)CalibData.dig_P4 << 35);
	val1 = ((val1 * val1 * (int64_t)CalibData.dig_P3) >> 8) + ((val1 * (int64_t)CalibData.dig_P2) << 12);
	val1 = (((((int64_t)1) << 47) + val1)) * ((int64_t)CalibData.dig_P1) >> 33;
	p = 1048576 - press_raw;
	p = (((p << 31) - val2) * 3125) / val1;
	val1 = (((int64_t)CalibData.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	val2 = (((int64_t)CalibData.dig_P8) * p) >> 19;
	p = ((p + val1 + val2) >> 8) + ((int64_t)CalibData.dig_P7 << 4);
	pres_int = ((p >> 8) * 1000) + (((p & 0xff) * 390625) / 100000);
	press_float = pres_int / 100.0f;
  return press_float;
}
//------------------------------------------------
static void BMP_ReadCoefficients(){
	if (BMP_Ready != HAL_ERROR)
		CheckDeviceState(BMP280_ADDRESS, &BMP_Ready);
	if (BMP_Ready != HAL_ERROR){
		I2C_ReadDataU16(BMP280_ADDRESS, BMP280_REGISTER_DIG_T1,&CalibData.dig_T1);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_T2,&CalibData.dig_T2);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_T3,&CalibData.dig_T3);
		I2C_ReadDataU16(BMP280_ADDRESS, BMP280_REGISTER_DIG_P1,&CalibData.dig_P1);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_P2,&CalibData.dig_P2);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_P3,&CalibData.dig_P3);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_P4,&CalibData.dig_P4);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_P5,&CalibData.dig_P5);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_P6,&CalibData.dig_P6);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_P7,&CalibData.dig_P7);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_P8,&CalibData.dig_P8);
		I2C_ReadDataS16(BMP280_ADDRESS, BMP280_REGISTER_DIG_P9,&CalibData.dig_P9);
	}
}
//------------------------------------------------
void BMP_DefaultInit(){
	BMP280_Init_TypeDef BMP280 = {0};
	BMP280.Power = BMP280_NORMALMODE;								//Measure Continuously
	BMP280.StandbyTime = BMP280_STANDBYTIME2;						//Measure every 125ms
	BMP280.PressureBMP280_OVERSAMPLING = BMP280_OVERSAMPLINGx4;		//Use Standart Resolution (18 bit)
	BMP280.TemperatureBMP280_OVERSAMPLING = BMP280_OVERSAMPLINGx1;	//Use Ultra-Low Power mode (16 bit)
	BMP280.FilterCoefficient = BMP280_FILTERCOEFF8;					//ODR = 7.3 Hz, RMS = 6.4 (see page 14, table 7 of datasheet)	
	BMP_Init(&BMP280);	
}
//------------------------------------------------
void BMP_Init(BMP280_Init_TypeDef *BMP280){
	CheckDeviceState(BMP280_ADDRESS, &BMP_Ready);
	if (BMP_Ready != HAL_ERROR)
		Verify_Device(BMP280_ADDRESS, BMP280_ID_REGISTER, BMP280_ID, &BMP_Ready);
	
	if (BMP_Ready != HAL_ERROR){
		BMP_SoftReset();
		while(BMP_Status() & BMP280_IS_UPDATING);
		BMP_ReadCoefficients();
		uint8_t ConfigRegister = (uint8_t)(BMP280->StandbyTime<<5 | BMP280->FilterCoefficient<<2);
		I2C_WriteByte(BMP280_ADDRESS, BMP280_CONFIG_REGISTER, ConfigRegister);	
		uint8_t CtrlRegister = (uint8_t)(BMP280->TemperatureBMP280_OVERSAMPLING<<5 | BMP280->PressureBMP280_OVERSAMPLING<<2 | BMP280->Power);
		I2C_WriteByte(BMP280_ADDRESS, BMP280_CTRL_REGISTER, CtrlRegister);	
		Calibrate();
	}
}
//------------------------------------------------
void BMP_GetAllData(float* temperature, float* pressure, float* altitude, float* mmHg){
	if (BMP_Ready != HAL_ERROR)
		CheckDeviceState(BMP280_ADDRESS, &BMP_Ready);
	if (BMP_Ready != HAL_ERROR){
		*pressure = BMP_GetPressureAndTemperature(temperature);
		*mmHg = *pressure * 0.000750061683f;
		*pressure/=10.0f;
		*altitude = (base_hhMg - *mmHg)*10.5;
	}
	else{
		*pressure = 0;
		*mmHg = 0;
		*altitude = 0;
	}
}
//------------------------------------------------
uint8_t BMP_Status(){
	uint8_t status;
	I2C_ReadData8(BMP280_ADDRESS, BMP280_STATUS_REGISTER, &status);
	return status&0x09;
}
//------------------------------------------------
void BMP_SoftReset(){
	I2C_WriteByte(BMP280_ADDRESS, BMP280_RESET_REGISTER, BMP280_SOFTRESET);
}
//------------------------------------------------
