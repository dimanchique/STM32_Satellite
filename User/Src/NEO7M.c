#include "NEO7M.h"

GPS_TypeDefStruct NEO7M = {0};

extern UART_HandleTypeDef huart1;
static char *Keys[3] = {"GPGGA", "GPRMC", "GPGLL"};

typedef void (*FunctionsArray)(char *);
static uint8_t IsValid(char *packet);
static void ConvertData(GPSProtocol *GPSProtocol);
static void GenerateDataRepresentation();
static void ParseGPGGA(char *packet);
static void ParseGPRMC(char *packet);
static void ParseGPGLL(char *packet);
static void ProcessResponse();

static FunctionsArray ParsingFunctions[3] = {&ParseGPGGA, &ParseGPRMC, &ParseGPGLL};

void GPS_Init() {
    NEO7M.ReceivingFinished = 0;
    static char Message1[] = {0xb5, 0x62, 0x6, 0x1, 0x2, 0x0, 0xf0, 0x2, 0xfb,
                              0x13, 0x0, 0xb5, 0x62, 0x6, 0x1, 0x8, 0x0,
                              0xf0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                              0x1, 0x31, 0xb5, 0x62, 0x6, 0x1, 0x2, 0x0,
                              0xf0, 0x2, 0xfb, 0x13, 0x0}; //Disable GPGSA
    static char Message2[] = {0xb5, 0x62, 0x6, 0x1, 0x2, 0x0, 0xf0, 0x5, 0xfe,
                              0x16, 0x0, 0xb5, 0x62, 0x6, 0x1, 0x8, 0x0,
                              0xf0, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                              0x4, 0x46, 0xb5, 0x62, 0x6, 0x1, 0x2, 0x0,
                              0xf0, 0x5, 0xfe, 0x16, 0x0}; //Disable GPVTG
    static char Message3[] = {0xb5, 0x62, 0x6, 0x1, 0x2, 0x0, 0xf0, 0x3, 0xfc,
                              0x14, 0x0, 0xb5, 0x62, 0x6, 0x1, 0x8, 0x0,
                              0xf0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                              0x2, 0x38, 0xb5, 0x62, 0x6, 0x1, 0x2, 0x0,
                              0xf0, 0x3, 0xfc, 0x14, 0x0}; //Disable GPGSV

    HAL_UART_Transmit(&huart1, (uint8_t *) Message1, 37, 100);
    HAL_UART_Transmit(&huart1, (uint8_t *) Message2, 37, 100);
    HAL_UART_Transmit(&huart1, (uint8_t *) Message3, 37, 100);
    HAL_UART_Receive_DMA(&huart1, (uint8_t *) NEO7M.TempMessage, GPS_DATA_SIZE);
    sprintf(NEO7M.PayloadMessage, "[GPS] Waiting");
}

void GPS_ReadData() {
    if (NEO7M.ReceivingFinished) {
        NEO7M.ReceivingFinished = 0;
        ProcessResponse();
    }
}

static void ProcessResponse() {
    char *token, *packet;
    char Temp[GPS_DATA_SIZE];
    NEO7M.MinimalDataReceived = 0;
    for (uint8_t pack_num = 0; pack_num < 3; pack_num++) {
        strcpy(Temp, NEO7M.Message);
        token = strstr(Temp, Keys[pack_num]);
        if (token) {
            packet = strtok(token, "$");
            if (packet != NULL)
                ParsingFunctions[pack_num](packet);
            if (NEO7M.MinimalDataReceived)
                break;
        }
    }
    GenerateDataRepresentation();
}

static uint8_t IsValid(char *packet) {
    return strstr(packet, "V") == NULL;
}

static void ConvertData(GPSProtocol *GPSProtocol) {
    GPSProtocol->LatitudeDegrees = (float) ((int) (GPSProtocol->RawLatitude / 100) +
                                            ((GPSProtocol->RawLatitude / 100 - (int) (GPSProtocol->RawLatitude / 100)) *
                                             100 / 60));
    GPSProtocol->LongitudeDegrees = (float) ((int) (GPSProtocol->RawLongitude / 100) +
                                             ((GPSProtocol->RawLongitude / 100 -
                                               (int) (GPSProtocol->RawLongitude / 100)) * 100 / 60));

    uint32_t actualtime = ((int) GPSProtocol->RawTime + 30000) % 240000;
    uint8_t hh = actualtime / 10000;
    uint8_t mm = (actualtime / 100) % 100;
    uint8_t ss = actualtime % 100;
    uint8_t ms = (uint8_t) ((GPSProtocol->RawTime - (uint32_t) (GPSProtocol->RawTime)) * 100);
    sprintf(GPSProtocol->TimeRepr, "%02d:%02d:%02d.%02d", hh, mm, ss, ms);
}

static void GenerateDataRepresentation() {
    if (NEO7M.MinimalDataReceived) {
        if (NEO7M.GPGGA.IsValid) {
            sprintf(NEO7M.PayloadMessage,
                    "[GPS] %s %.5f%c %.5f%c %.3f%c GPGGA;",
                    NEO7M.GPGGA.TimeRepr,
                    NEO7M.GPGGA.LatitudeDegrees,
                    NEO7M.GPGGA.LatitudeDirection,
                    NEO7M.GPGGA.LongitudeDegrees,
                    NEO7M.GPGGA.LongitudeDirection,
                    NEO7M.GPGGA.Extras.Altitude,
                    NEO7M.GPGGA.Extras.AltitudeUnits);
        } else if (NEO7M.GPRMC.IsValid) {
            sprintf(NEO7M.PayloadMessage,
                    "[GPS] %s %f%c %f%c %f GPRMC;",
                    NEO7M.GPRMC.TimeRepr,
                    NEO7M.GPRMC.LatitudeDegrees,
                    NEO7M.GPRMC.LatitudeDirection,
                    NEO7M.GPRMC.LongitudeDegrees,
                    NEO7M.GPRMC.LongitudeDirection,
                    NEO7M.GPGGA.Extras.SpeedInKnots);
        } else if (NEO7M.GPGLL.IsValid) {
            sprintf(NEO7M.PayloadMessage,
                    "[GPS] %s %f%c %f%c GPGLL;",
                    NEO7M.GPGLL.TimeRepr,
                    NEO7M.GPGLL.LatitudeDegrees,
                    NEO7M.GPGLL.LatitudeDirection,
                    NEO7M.GPGLL.LongitudeDegrees,
                    NEO7M.GPGLL.LongitudeDirection);
        }
    }
    else
        sprintf(NEO7M.PayloadMessage, "[GPS] %s;", UNREACHABLE);
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
        NEO7M.MinimalDataReceived = 1;
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
        NEO7M.MinimalDataReceived = 1;
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
        NEO7M.MinimalDataReceived = 1;
    }
}
