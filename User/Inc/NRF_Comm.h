#include "stm32h7xx_hal.h"
//------------------------------------------------
extern SPI_HandleTypeDef hspi2;
//------------------------------------------------
/* Registers Addresses */
#define NRF_CONFIG 								0x00
#define NRF_EN_AA       						0x01
#define NRF_EN_RXADDR   						0x02
#define NRF_SETUP_AW    						0x03
#define NRF_SETUP_RETR  						0x04
#define NRF_RF_CH       						0x05
#define NRF_RF_SETUP    						0x06
#define NRF_NRF_STATUS  						0x07
#define NRF_OBSERVE_TX  						0x08
#define NRF_CD          						0x09
#define NRF_RX_ADDR_P0  						0x0A
#define NRF_RX_ADDR_P1  						0x0B
#define NRF_RX_ADDR_P2  						0x0C
#define NRF_RX_ADDR_P3  						0x0D
#define NRF_RX_ADDR_P4  						0x0E
#define NRF_RX_ADDR_P5  						0x0F
#define NRF_TX_ADDR     						0x10
#define NRF_RX_PW_P0    						0x11
#define NRF_RX_PW_P1    						0x12
#define NRF_RX_PW_P2    						0x13
#define NRF_RX_PW_P3    						0x14
#define NRF_RX_PW_P4    						0x15
#define NRF_RX_PW_P5    						0x16
#define NRF_FIFO_STATUS 						0x17
#define NRF_DYNPD	    						0x1C
#define NRF_FEATURE	    						0x1D
//------------------------------------------------
/* Instruction Mnemonics */
#define NRF_R_REGISTER    					0x00
#define NRF_W_REGISTER    					0x20
#define NRF_REGISTER_MASK 					0x1F
#define NRF_ACTIVATE      					0x50
#define NRF_R_RX_PL_WID   					0x60
#define NRF_R_RX_PAYLOAD  					0x61
#define NRF_W_TX_PAYLOAD_NO_ACK  		0xB0
#define NRF_W_TX_PAYLOAD  					0xA0
#define NRF_W_ACK_PAYLOAD 					0xA8
#define NRF_FLUSH_TX      					0xE1
#define NRF_FLUSH_RX      					0xE2
#define NRF_REUSE_TX_PL   					0xE3
#define NRF_NOP           					0xFF
//------------------------------------------------
/* P model bit Mnemonics */
#define NRF_DR_LOW   						5
#define NRF_DR_HIGH  						3
#define NRF_PWR_LOW  						1
#define NRF_WR_HIGH 						2
//------------------------------------------------
/* Bit Mnemonics */
#define NRF_MASK_RX_DR  					6
#define NRF_MASK_TX_DS  					5
#define NRF_MASK_MAX_RT 					4
#define NRF_EN_CRC      					3
#define NRF_CRCO        					2
#define NRF_PWR_UP      					1
#define NRF_PRIM_RX     					0
#define NRF_ENAA_P5     					5
#define NRF_ENAA_P4     					4
#define NRF_ENAA_P3     					3
#define NRF_ENAA_P2     					2
#define NRF_ENAA_P1     					1
#define NRF_ENAA_P0     					0
#define NRF_ERX_P5      					5
#define NRF_ERX_P4      					4
#define NRF_ERX_P3      					3
#define NRF_ERX_P2      					2
#define NRF_ERX_P1      					1
#define NRF_ERX_P0      					0
#define NRF_AW          					0
#define NRF_ARD         					4
#define NRF_ARC         					0
#define NRF_PLL_LOCK    					4
#define NRF_RF_DR       					3
#define NRF_RF_PWR      					6
#define NRF_RX_DR       					6
#define NRF_TX_DS       					5
#define NRF_MAX_RT      					4
#define NRF_RX_P_NO     					1
#define NRF_TX_FULL     					0
#define NRF_PLOS_CNT    					4
#define NRF_ARC_CNT     					0
#define NRF_TX_REUSE    					6
#define NRF_FIFO_FULL   					5
#define NRF_TX_EMPTY    					4
#define NRF_RX_FULL     					1
#define NRF_RX_EMPTY    					0
#define NRF_DPL_P5	    					5
#define NRF_DPL_P4	    					4
#define NRF_DPL_P3	    					3
#define NRF_DPL_P2	    					2
#define NRF_DPL_P1	    					1
#define NRF_DPL_P0	    					0
#define NRF_EN_DPL	    					2
#define NRF_EN_ACK_PAY  					1
#define NRF_EN_DYN_ACK  					0
//------------------------------------------------
#define HIGH 								1
#define LOW  								0
#define CSN_Pin 							GPIO_PIN_4
#define CSN_GPIO_Port 						GPIOA
#define CE_Pin 								GPIO_PIN_3
#define CE_GPIO_Port 						GPIOA
//------------------------------------------------
#define rf24_max(a,b) (a>b?a:b)
#define rf24_min(a,b) (a<b?a:b)
//------------------------------------------------
typedef struct{
	uint8_t DataRate;
	uint8_t AcknowledgeEnable;
	uint8_t AcknowledgePayload;
	uint64_t Pipe;
	uint8_t Channel;
	uint8_t Retries;
	uint8_t RetryDelay;
	uint8_t PowerLevel;
	uint8_t AddressWidth;
	uint8_t DynamicPayloadEnable;
	uint8_t PayloadSize;
	uint8_t TX_Delay;
	uint8_t is_P_Variant;
} NRF_Init_TypeDef;
//------------------------------------------------
typedef enum{
	NRF_PA_MIN,
	NRF_PA_LOW,
	NRF_PA_HIGH,
	NRF_PA_MAX,
	NRF_PA_ERROR
} NRF_PowerLevel;
//------------------------------------------------
typedef enum{
	NRF_1MBPS,
	NRF_2MBPS,
	NRF_250KBPS
} NRF_DataRate;
//------------------------------------------------
typedef enum{
	NRF_CRC_DISABLED,
	NRF_CRC_8,
	NRF_CRC_16
} NRF_CRCLength;
//------------------------------------------------
void delay_us(uint32_t us);

void csn(uint8_t level);
void ce(uint8_t level);

uint8_t spiTrans(uint8_t cmd);
uint8_t get_status(void);

uint8_t flush_rx(void);
uint8_t flush_tx(void);

uint8_t read_register(uint8_t reg);
uint8_t write_buff_register(uint8_t reg, const uint8_t* buf, uint8_t len);
uint8_t write_register(uint8_t reg, uint8_t value);

uint8_t write_payload(const void* buf, uint8_t data_len, const uint8_t writeType, NRF_Init_TypeDef *NRF_Module);

uint8_t setDataRate(NRF_Init_TypeDef *NRF_Module);
void setRetries(uint8_t delay, uint8_t count);
void setPALevel(uint8_t level);
void toggle_features(void);
void setChannel(uint8_t channel);
void powerUp(void);
void setAutoAck(uint8_t enable);
void openWritingPipe(NRF_Init_TypeDef *NRF_Module);
