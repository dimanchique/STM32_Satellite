#include "SIM900.h"

GSM_TypeDefStruct SIM900 = {0};
extern UART_HandleTypeDef huart2;

static void SendCommand(char *Command);
static uint8_t ResponseIs(char *WantedResponse);
static uint16_t ReceivedDataSize;

void SIM900_Init(void) {
    SendCommand("AT\n");
    SIM900_SendMessage("SIM900 Module Connected");
}

void SIM900_SendMessage(char *Message) {
    SendCommand("AT+CMGF=1\n");
    SendCommand("AT+CMGS=\"+79879922773\"\n");
    SendCommand(Message);
    SendCommand("\n");
    SendCommand((char *) 26);
}

static void SendCommand(char *Command) {
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
