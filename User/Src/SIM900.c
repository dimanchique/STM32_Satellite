#include "SIM900.h"

GSM_TypeDefStruct SIM900 = {0};
extern UART_HandleTypeDef huart2;

static void SIM900_SendCommand(char *Command);
static uint16_t ReceivedDataSize;

void SIM900_Init(void) {
    SIM900_SendCommand("AT\n");
    SIM900_SendMessage("SIM900 Module Connected");
}

void SIM900_SendMessage(char *Message) {
    SIM900_SendCommand("AT+CMGF=1\n");
    SIM900_SendCommand("AT+CMGS=\"+79879922773\"\n");
    SIM900_SendCommand(Message);
    SIM900_SendCommand("\n");
    SIM900_SendCommand((char *) 26);
}

static void SIM900_SendCommand(char *Command) {
    HAL_UART_Transmit(&huart2, (uint8_t *) Command, strlen(Command), 300);
    SIM900.CommandStatus = HAL_UARTEx_ReceiveToIdle(&huart2,
                                                    (uint8_t *) SIM900.Response,
                                                    GSM_RESPONSE_SIZE,
                                                    &ReceivedDataSize,
                                                    700);
}
