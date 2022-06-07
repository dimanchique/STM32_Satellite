#pragma once
#include "stm32h7xx_hal.h"
#include "string.h"
#include "stdlib.h"

/** Setup NEO7M UART Commands **/
#define SetupGPSRate        "181\n98\n6\n8\n6\n0\n100\n0\n1\n0\n1\n0\n122\n18\n181\n98\n6\n8\n0\n0\n14\n48\n"                      //Period 500ms (2Hz)
#define DisableGSAPacket    "181\n98\n6\n1\n8\n0\n240\n2\n0\n0\n0\n0\n0\n1\n2\n50\n181\n98\n6\n1\n2\n0\n240\n2\n251\n19\n"
#define DisableGSVPacket    "181\n98\n6\n1\n8\n0\n240\n3\n0\n0\n0\n0\n0\n1\n3\n57\n181\n98\n6\n1\n2\n0\n240\n3\n252\n20\n"
#define DisableVTGPacket    "181\n98\n6\n1\n8\n0\n240\n5\n0\n0\n0\n0\n0\n1\n5\n71\n181\n98\n6\n1\n2\n0\n240\n5\n254\n22\n"

/** Functions Prototypes **/
void ProcessResponse(void);
void GPS_ReadData(void);
