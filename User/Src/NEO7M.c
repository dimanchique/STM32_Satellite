#include "NEO7M.h"

//------------------------------------------------
extern UART_HandleTypeDef huart1;
volatile uint8_t ReceivingEnd = 0;
static char *Keys[3] = {"GPGGA", "GPRMC", "GPGLL"};
static uint8_t hh, mm, ss;
GPS_TypeDef NEO7M = {0};
typedef void (*FunctionsArray)(char*);

static uint8_t IsValid(char *packet);
static void ConvertData(GPSProtocol *GPSProtocol);
static void GenerateDataRepresentation();
static void ParseGPGGA(char *packet);
static void ParseGPRMC(char *packet);
static void ParseGPGLL(char *packet);
static void ProcessResponse();

static FunctionsArray func_arr[3] = {&ParseGPGGA, &ParseGPRMC, &ParseGPGLL};

void GPS_Init(){
    if (HAL_UARTEx_ReceiveToIdle(&huart1, (uint8_t*)NEO7M.Message, GPS_DATA_SIZE, NULL, 1000) == HAL_TIMEOUT)
        sprintf(NEO7M.PayloadMessage, "[GPS] %s;", UNREACHABLE);
    else
        sprintf(NEO7M.PayloadMessage, "[GPS] Connectiong;");
}

void GPS_ReadData()
{
    ReceivingEnd = 1;
    if (HAL_UARTEx_ReceiveToIdle(&huart1, (uint8_t*)NEO7M.Message, GPS_DATA_SIZE, NULL, 1000) == HAL_TIMEOUT)
    {
        sprintf(NEO7M.PayloadMessage, "[GPS] %s;", UNREACHABLE);
        return;
    }
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t*)NEO7M.Message, GPS_DATA_SIZE);
    while (ReceivingEnd==1);
    if(IsValid(NEO7M.Message))
        ProcessResponse();
}

static void ProcessResponse() {
    char *token, *packet;
    int pack;
    for (pack = 0; pack < 3; pack++) {
        strcpy(NEO7M.TempMessage, NEO7M.Message);
        token = strstr(NEO7M.TempMessage, Keys[pack]);
        if (token) {
            packet = strtok(token, "$");
            if (packet != NULL)
                func_arr[pack](packet);
        }
    }
    GenerateDataRepresentation();
}

static uint8_t IsValid(char *packet) {
    char *gapV = strstr(packet, "V");
    char *gapN = strstr(packet, "N");
    return gapV == NULL && gapN == NULL;
}

static void ConvertData(GPSProtocol *GPSProtocol) {
    GPSProtocol->LatitudeDegrees = (float) ((int) (GPSProtocol->RawLatitude / 100) +
                                            ((GPSProtocol->RawLatitude / 100 -
                                            (int) (GPSProtocol->RawLatitude / 100)) * 100 / 60));
    GPSProtocol->LongitudeDegrees = (float) ((int) (GPSProtocol->RawLongitude / 100) +
                                            ((GPSProtocol->RawLongitude / 100 -
                                            (int) (GPSProtocol->RawLongitude / 100)) * 100 / 60));
    hh = (uint16_t) (GPSProtocol->RawTime) / 10000;
    if (hh >= 21) hh = hh - 21; //convert from UTC to UTC+3
    mm = (uint8_t) (GPSProtocol->RawTime) / 100 % 100;
    ss = (uint8_t) (GPSProtocol->RawTime) % 100;
    sprintf(GPSProtocol->TimeRepr, "%d:%d:%d", hh, mm, ss);
}

static void GenerateDataRepresentation() {
    if (NEO7M.GPGGA.IsValid) {
        sprintf(NEO7M.PayloadMessage,
                "[GPS] %s %f%c %f%c %f%c GPGGA;",
                NEO7M.GPGGA.TimeRepr,
                NEO7M.GPGGA.LatitudeDegrees,
                NEO7M.GPGGA.LatitudeDirection,
                NEO7M.GPGGA.LongitudeDegrees,
                NEO7M.GPGGA.LongitudeDirection,
                NEO7M.GPGGA.Extras.Altitude,
                NEO7M.GPGGA.Extras.AltitudeUnits);
    }
    else if (NEO7M.GPRMC.IsValid) {
        sprintf(NEO7M.PayloadMessage,
                "[GPS] %s %f%c %f%c %f GPRMC;",
                NEO7M.GPRMC.TimeRepr,
                NEO7M.GPRMC.LatitudeDegrees,
                NEO7M.GPRMC.LatitudeDirection,
                NEO7M.GPRMC.LongitudeDegrees,
                NEO7M.GPRMC.LongitudeDirection,
                NEO7M.GPGGA.Extras.SpeedInKnots);
    }
    else if (NEO7M.GPGLL.IsValid) {
        sprintf(NEO7M.PayloadMessage,
                "[GPS] %s %f%c %f%c GPGLL;",
                NEO7M.GPGLL.TimeRepr,
                NEO7M.GPGLL.LatitudeDegrees,
                NEO7M.GPGLL.LatitudeDirection,
                NEO7M.GPGLL.LongitudeDegrees,
                NEO7M.GPGLL.LongitudeDirection);
    } else sprintf(NEO7M.PayloadMessage, "[GPS] %s;", UNREACHABLE);
}

static void ParseGPGGA(char *packet) {
    NEO7M.GPGGA.IsValid = IsValid(packet);
    if (NEO7M.GPGGA.IsValid) {
        sscanf(packet, "%5s,%f,%f,%c,%f,%c,%d,%d,%f,%f,%c",
               NEO7M.GPGGA.PacketName,
               &NEO7M.GPGGA.RawTime,
               &NEO7M.GPGGA.RawLatitude,
               &NEO7M.GPGGA.LatitudeDirection,
               &NEO7M.GPGGA.RawLongitude,
               &NEO7M.GPGGA.LongitudeDirection,
               &NEO7M.SkipInt,
               &NEO7M.SkipInt,
               &NEO7M.SkipFloat,
               &NEO7M.GPGGA.Extras.Altitude,
               &NEO7M.GPGGA.Extras.AltitudeUnits);
        ConvertData(&NEO7M.GPGGA);
    }
}

static void ParseGPRMC(char *packet) {
    NEO7M.GPRMC.IsValid = IsValid(packet);
    if (NEO7M.GPRMC.IsValid) {
        sscanf(packet, "%5s,%f,%c,%f,%c,%f,%c,%f",
               NEO7M.GPRMC.PacketName,
               &NEO7M.GPRMC.RawTime,
               &NEO7M.SkipChar,
               &NEO7M.GPRMC.RawLatitude,
               &NEO7M.GPRMC.LatitudeDirection,
               &NEO7M.GPRMC.RawLongitude,
               &NEO7M.GPRMC.LongitudeDirection,
               &NEO7M.GPRMC.Extras.SpeedInKnots);
        ConvertData(&NEO7M.GPRMC);
    }
}

static void ParseGPGLL(char *packet) {
    NEO7M.GPGLL.IsValid = IsValid(packet);
    if (NEO7M.GPGLL.IsValid) {
        sscanf(packet, "%5s,%f,%c,%f,%c,%f",
               NEO7M.GPGLL.PacketName,
               &NEO7M.GPGLL.RawLatitude,
               &NEO7M.GPGLL.LatitudeDirection,
               &NEO7M.GPGLL.RawLongitude,
               &NEO7M.GPGLL.LongitudeDirection,
               &NEO7M.GPGLL.RawTime);
        ConvertData(&NEO7M.GPGLL);
    }
}
