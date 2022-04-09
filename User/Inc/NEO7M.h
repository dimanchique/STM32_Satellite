#pragma once

#include "stm32h7xx_hal.h"
#include "string.h"
#include "stdlib.h"

#define GPS_DATA_SIZE           500
#define GPS_PAYLOAD_SIZE        100

/** Setup NEO7M UART Commands **/
#define SetupGPSRate        "181\n98\n6\n8\n6\n0\n100\n0\n1\n0\n1\n0\n122\n18\n181\n98\n6\n8\n0\n0\n14\n48\n"                      //Period 500ms (2Hz)
#define DisableGSAPacket    "181\n98\n6\n1\n8\n0\n240\n2\n0\n0\n0\n0\n0\n1\n2\n50\n181\n98\n6\n1\n2\n0\n240\n2\n251\n19\n"
#define DisableGSVPacket    "181\n98\n6\n1\n8\n0\n240\n3\n0\n0\n0\n0\n0\n1\n3\n57\n181\n98\n6\n1\n2\n0\n240\n3\n252\n20\n"
#define DisableVTGPacket    "181\n98\n6\n1\n8\n0\n240\n5\n0\n0\n0\n0\n0\n1\n5\n71\n181\n98\n6\n1\n2\n0\n240\n5\n254\n22\n"

/** Struct for Extra Data **/
typedef struct {
    float Altitude;
    char AltitudeUnits;
    float SpeedInKnots;
} ExtraData;

/** GPS Protocol Struct **/
typedef struct {
    char PacketName[5];
    int IsValid;
    float RawTime;
    char TimeRepr[8];
    float RawLatitude;
    char LatitudeDirection;
    float RawLongitude;
    char LongitudeDirection;
    float LongitudeDegrees;
    float LatitudeDegrees;
    ExtraData Extras;
} GPSProtocol;

/** NEO7M Device Struct **/
typedef struct {
    char Message[GPS_DATA_SIZE];
    char TempMessage[GPS_DATA_SIZE];
    char PayloadMessage[GPS_PAYLOAD_SIZE];
    GPSProtocol GPGGA;
    GPSProtocol GPGLL;
    GPSProtocol GPRMC;
    float SkipFloat;
    int SkipInt;
    char SkipChar;
} GPS_TypeDef;

/** Functions Prototypes **/
void NEO7M_Init(void);
void ProcessResponse();
