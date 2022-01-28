#include "NRF_Module.h"

static NRF_Init_TypeDef NRF_Module = {0};
//------------------------------------------------
uint8_t NRF_Connected(){
	uint8_t setup = read_register(NRF_SETUP_AW);
	NRF_Ready = setup >= 1 && setup <= 3 ? HAL_OK : HAL_ERROR;
	return NRF_Ready;
}
//------------------------------------------------
void NRF_DefaultInit(){
		NRF_Module.Retries = 15;
		NRF_Module.RetryDelay = 5;
		NRF_Module.PowerLevel = NRF_PA_MAX;
		NRF_Module.DataRate = NRF_1MBPS;
		NRF_Module.AcknowledgeEnable = 0;
		NRF_Module.AcknowledgePayload = 0;
		NRF_Module.Pipe = PIPE1;
		NRF_Module.Channel = 19;
		NRF_Module.AddressWidth = 5;
		NRF_Module.PayloadSize = 32;
		NRF_Module.TX_Delay = 0;
		NRF_Module.is_P_Variant = 0;
		
		NRF_Init();

		if(NRF_Ready != HAL_ERROR){
			setAutoAck(NRF_Module.AcknowledgeEnable);
			setChannel(NRF_Module.Channel);
			openWritingPipe(&NRF_Module);
		}
}
//------------------------------------------------
void NRF_Init(){
	uint8_t setup = 0;

	ce(LOW);
	csn(HIGH);
	HAL_Delay(5);

	write_register(NRF_CONFIG, 0x0C);
	setRetries(NRF_Module.RetryDelay, NRF_Module.Retries);
	setPALevel(NRF_Module.PowerLevel);
	if(setDataRate(&NRF_Module))
		NRF_Module.is_P_Variant = 1;
	setup = read_register(NRF_RF_SETUP);
	setDataRate(&NRF_Module);
	toggle_features();
	write_register(NRF_FEATURE, 0);
	write_register(NRF_DYNPD, 0);
	write_register(NRF_NRF_STATUS, (1 << NRF_RX_DR) | (1 << NRF_TX_DS) | (1 << NRF_MAX_RT));
	setChannel(76);
	flush_rx();
	flush_tx();
	powerUp();
	write_register(NRF_CONFIG, (read_register(NRF_CONFIG)) & ~(1 << NRF_PRIM_RX));
	NRF_Ready = (setup != 0 && setup != 0xff);
}
//------------------------------------------------
void NRF_Transmit(const void* buf, uint8_t len)
{
	NRF_FastWrite(buf, len, 1, 1);

	while(!(get_status() & ((1 << NRF_TX_DS) | (1 << NRF_MAX_RT))));

	ce(LOW);
	uint8_t status = write_register(NRF_NRF_STATUS, (1 << NRF_RX_DR) | (1 << NRF_TX_DS) | (1 << NRF_MAX_RT));

	if(status & (1 << NRF_MAX_RT))
	{
		flush_tx();
		NRF_Ready = HAL_ERROR;
	}

	NRF_Ready = HAL_OK;
}
//------------------------------------------------
void NRF_FastWrite(const void* buf, uint8_t len, const uint8_t multicast, uint8_t startTx)
{
	write_payload(buf, len, multicast ? NRF_W_TX_PAYLOAD_NO_ACK : NRF_W_TX_PAYLOAD, &NRF_Module);

	if(startTx)
	{
		ce(HIGH);
	}
}
//------------------------------------------------
