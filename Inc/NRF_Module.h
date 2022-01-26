#include "NRF_Comm.h"

extern HAL_StatusTypeDef NRF_Ready;
//------------------------------------------------
uint8_t NRF_Connected(void);
void NRF_Init(NRF_Init_TypeDef *NRF_Module);
void NRF_DefaultInit(void);

void NRF_Transmit(const void* buf, uint8_t len);
void NRF_FastWrite(const void* buf, uint8_t len, const uint8_t multicast, uint8_t startTx);
