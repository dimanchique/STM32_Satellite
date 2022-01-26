#include "stm32h7xx_hal.h"
#include "string.h"
#include "stdlib.h"

#define GPS_DATA_SIZE							600

#define GPS_TimePacket								1
#define GPS_LatitudePacket						2
#define GPS_LongitudePacket						4
#define GPS_NumOfSatsPacket						7
#define GPS_AltitudePacket						9
#define GPS_AltitudeUnitsPacket				10

static char GPGGA[] = {'$','G','P','G','G','A'};

extern UART_HandleTypeDef huart1;
extern uint8_t ReceptionEnd;

void RecieveData(char *RecievedData);
void ReadTime(char *RecievedData, char* Time);
void ReadGPSData(char *RecievedData, double* Latitude, double* Longitude, double* Altitude);
