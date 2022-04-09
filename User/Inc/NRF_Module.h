#include "NRF_Comm.h"

extern HAL_StatusTypeDef NRF_Ready;

/** Pipes Addresses **/
#define PIPE0 0x787878787878
#define PIPE1 0xE8E8F0F0E2LL
#define PIPE2 0xE8E8F0F0A2LL
#define PIPE3 0xE8E8F0F0D1LL
#define PIPE4 0xE8E8F0F0C3LL
#define PIPE5 0xE8E8F0F0E7LL

/** Functions Prototypes **/
uint8_t NRF_Connected(void);
void NRF_Init(void);
void NRF_DefaultInit(void);
void NRF_Transmit(const void *buf, uint8_t len);
void NRF_FastWrite(const void *buf, uint8_t len, const uint8_t multicast, uint8_t startTx);
