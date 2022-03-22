#include "NRF_Comm.h"

HAL_StatusTypeDef NRF_Ready = HAL_ERROR;
//------------------------------------------------
void delay_us(uint32_t us){
	uint32_t us_count_tic =  us * (SystemCoreClock / 1000000);
	DWT->CYCCNT = 0U;
	while(DWT->CYCCNT < us_count_tic);
}
//------------------------------------------------
void csn(uint8_t level){
	HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, level);
	delay_us(5);
}
//------------------------------------------------
void ce(uint8_t level){
	HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, level);
}
//------------------------------------------------
uint8_t spiTrans(uint8_t cmd){
	uint8_t status = 0;
	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi2, &cmd, &status, 1, 1000);
	csn(HIGH);
	return status;
}
//------------------------------------------------
uint8_t get_status(void)
{
	return spiTrans(NRF_NOP);
}
//------------------------------------------------
uint8_t flush_rx(void){
	return spiTrans(NRF_FLUSH_RX);
}
//------------------------------------------------
uint8_t flush_tx(void){
	return spiTrans(NRF_FLUSH_TX);
}
//------------------------------------------------
uint8_t read_register(uint8_t reg){
	uint8_t addr = NRF_R_REGISTER | (NRF_REGISTER_MASK & reg);
	uint8_t dt = 0;

	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi2, &addr, &dt, 1, 1000);
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)0xff, &dt, 1, 1000);
	csn(HIGH);
	return dt;
}
//------------------------------------------------
uint8_t write_register(uint8_t reg, uint8_t value){
	uint8_t status = 0;
	uint8_t addr = NRF_W_REGISTER | (NRF_REGISTER_MASK & reg);
	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi2, &addr, &status, 1, 100);
	HAL_SPI_Transmit(&hspi2, &value, 1, 1000);
	csn(HIGH);
	return status;
}
//------------------------------------------------
uint8_t write_buff_register(uint8_t reg, const uint8_t* buf, uint8_t len){
	uint8_t status = 0;
	uint8_t addr = NRF_W_REGISTER | (NRF_REGISTER_MASK & reg);
	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi2, &addr, &status, 1, 100);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)buf, len, 100);
	csn(HIGH);
	return status;
}
//------------------------------------------------
uint8_t setDataRate(NRF_Init_TypeDef *NRF_Module){
	uint8_t result = 0;
	uint8_t setup = read_register(NRF_RF_SETUP);
	setup &= ~((1 << NRF_DR_LOW) | (1 << NRF_DR_HIGH));
	NRF_Module->TX_Delay = 85;
	if(NRF_Module->DataRate == NRF_250KBPS){
		setup |= (1 << NRF_DR_LOW);
		NRF_Module->TX_Delay = 155;
	}
	else{
		if(NRF_Module->DataRate == NRF_2MBPS){
			setup |= (1 << NRF_DR_HIGH);
			NRF_Module->TX_Delay = 65;
		}
	}
	write_register(NRF_RF_SETUP, setup);
	uint8_t ggg = read_register(NRF_RF_SETUP);
	if(ggg == setup) result = 1;
	return result;
}
//------------------------------------------------
void setRetries(uint8_t delay, uint8_t count){
	write_register(NRF_SETUP_RETR, (delay&0xf)<<NRF_ARD | (count&0xf)<<NRF_ARC);
}
//------------------------------------------------
void setPALevel(uint8_t level){
  uint8_t setup = read_register(NRF_RF_SETUP) & 0xF8;
  if(level > 3)level = (NRF_PA_MAX << 1) + 1;
  else level = (level << 1) + 1;
  write_register(NRF_RF_SETUP, setup |= level);
}
//------------------------------------------------
void toggle_features(void){
	uint8_t addr = NRF_ACTIVATE;
	csn(LOW);
	HAL_SPI_Transmit(&hspi2, &addr, 1, 1000);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)0x73, 1, 1000);
	csn(HIGH);
}
//------------------------------------------------
void setChannel(uint8_t channel){
	write_register(NRF_RF_CH, channel);
}
//------------------------------------------------
void powerUp(void){
	uint8_t cfg = read_register(NRF_CONFIG);
	if(!(cfg & (1 << NRF_PWR_UP))){
		write_register(NRF_CONFIG, cfg | (1 << NRF_PWR_UP));
		HAL_Delay(5);
	}
}
//------------------------------------------------
void setAutoAck(uint8_t enable){
	if(enable) write_register(NRF_EN_AA, 0x3F);
	else write_register(NRF_EN_AA, 0);
}
//------------------------------------------------
void openWritingPipe(NRF_Init_TypeDef *NRF_Module){
	//uint64_t pipe = NRF_Module->Pipe;
	//uint8_t AddressWidth = NRF_Module->AddressWidth;
	//uint8_t PayloadSize = NRF_Module->PayloadSize;
	write_buff_register(NRF_RX_ADDR_P0, (uint8_t*)&NRF_Module->Pipe, NRF_Module->AddressWidth);
	write_buff_register(NRF_TX_ADDR, (uint8_t*)&NRF_Module->Pipe, NRF_Module->AddressWidth);
	write_register(NRF_RX_PW_P0, NRF_Module->PayloadSize);
}
//------------------------------------------------
uint8_t write_payload(const void* buf, uint8_t data_len, const uint8_t writeType, NRF_Init_TypeDef *NRF_Module)
{
	uint8_t status = 0;
	const uint8_t* current = (const uint8_t*)buf;
	uint8_t addr = writeType;

	data_len = rf24_min(data_len, NRF_Module->PayloadSize);
	uint8_t blank_len = NRF_Module->DynamicPayloadEnable ? 0 : NRF_Module->PayloadSize - data_len;

	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi2, &addr, &status, 1, 100);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)current, data_len, 100);

	while(blank_len--)
	{
		uint8_t empt = 0;
		HAL_SPI_Transmit(&hspi2, &empt, 1, 100);
	}

	csn(HIGH);
	return status;
}
//------------------------------------------------
