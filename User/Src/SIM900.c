#include "SIM900.h"

GSM_TypeDefStruct SIM900 = {0};

extern UART_HandleTypeDef huart2;

static void Send(char *Command);
static uint8_t ResponseIs(char *WantedResponse);
static uint16_t ReceivedDataSize;

void SIM900_Init(void){
    Send("AT");
    if (ResponseIs("AT"))
        Send("AT+CSQ");
    if (ResponseIs("OK"))
        Send("AT+CCID");
    if (ResponseIs("OK"))
        Send("AT+CREG?");
    if (ResponseIs("OK"))
        Send("AT+CMGF=1");
    if (ResponseIs("OK"))
        Send("AT+CMGS=\"+79879922773\"");
}

static void Send(char *Command) {
    HAL_UART_Transmit(&huart2, (uint8_t*)Command, strlen(Command), 200);
    SIM900.CommandStatus = HAL_UARTEx_ReceiveToIdle(&huart2, (uint8_t*)SIM900.Response, GSM_RESPONSE_SIZE, &ReceivedDataSize, 700);
}

static uint8_t ResponseIs(char *WantedResponse) {
    if (SIM900.CommandStatus == HAL_OK)
        if (strstr(SIM900.Response, WantedResponse)){
            SIM900.Status = HAL_OK;
            return 1;
        }
    SIM900.Status = HAL_ERROR;
    return 0;
}
