#include "SIM900.h"

GSM_TypeDefStruct SIM900 = {0};
extern UART_HandleTypeDef huart2;

static void Send(char *Command);
static uint8_t ResponseIs(char *WantedResponse);
static uint16_t ReceivedDataSize;

void SIM900_Init(void) {
    Send("AT\n");
    SendMessageUsingSIM("SIM900 Module Connected");
}

void SendMessageUsingSIM(char *Message) {
    Send("AT+CMGF=1\n");
    Send("AT+CMGS=\"+79879922773\"\n");
    Send(Message);
    Send("\n");
    Send((char *) 26);
}

static void Send(char *Command) {
    HAL_UART_Transmit(&huart2, (uint8_t *) Command, strlen(Command), 300);
    SIM900.CommandStatus = HAL_UARTEx_ReceiveToIdle(&huart2,
                                                    (uint8_t *) SIM900.Response,
                                                    GSM_RESPONSE_SIZE,
                                                    &ReceivedDataSize,
                                                    700);
}

static uint8_t ResponseIs(char *WantedResponse) {
    if (SIM900.CommandStatus == HAL_OK)
        if (strstr(SIM900.Response, WantedResponse))
            return 1;
    return 0;
}
