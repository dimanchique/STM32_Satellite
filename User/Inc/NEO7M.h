#pragma once
#include "stm32h7xx_hal.h"
#include "string.h"
#include "stdlib.h"

#define GPS_DATA_SIZE							500
#define GPS_PAYLOAD_SIZE					100

typedef struct{
  float Altitude;
  char AltitudeUnits;
  float SpeedInKnots;
} ExtraData;

typedef struct{
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

typedef struct{
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

void NEO7M_ReadData(void);
