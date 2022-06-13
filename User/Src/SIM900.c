#include "SIM900.h"

GSM_TypeDef SIM900 = {0};
extern UART_HandleTypeDef huart2;

static void SendCommand(char *Command, char *Response) {
    HAL_UART_Transmit(&huart2, (uint8_t*)Command, strlen(Command), 500);
    HAL_Delay(500);
    HAL_UART_Receive(&huart2, (uint8_t*)Response, GSM_RESPONSE_SIZE, 500);
}

static uint8_t ResponseIs(char *WantedResponse){
    if (strstr(SIM900.Response, WantedResponse)){
        SIM900.Status = HAL_OK;
        return 1;
    }
    SIM900.Status = HAL_ERROR;
    return 0;
}

void SIM900_Init(void){
    SendCommand("AT", SIM900.Response);
    if (ResponseIs("OK"))
        SendCommand("AT+CSQ", SIM900.Response);
    if (ResponseIs("OK"))
        SendCommand("AT+CCID", SIM900.Response);
    if (ResponseIs("OK"))
        SendCommand("AT+CREG?", SIM900.Response);
    if (ResponseIs("OK"))
        SendCommand("AT+CMGF=1", SIM900.Response);
    if (ResponseIs("OK"))
        SendCommand("AT+CMGS=\"+79879922773\"", SIM900.Response);
}