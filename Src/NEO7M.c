#include "NEO7M.h"
HAL_StatusTypeDef NEO7M_Ready = HAL_ERROR;

static uint16_t FindMatch(char *RecievedData, char* Word){
	uint16_t Start = 0, Pointer = 0, NumOfMatches = 0;

	while (Start<strlen(RecievedData)){
		if (RecievedData[Start]==Word[NumOfMatches]){
			if (NumOfMatches==0)	Pointer = Start;
			NumOfMatches++;
		}
		else NumOfMatches = 0;
		if (NumOfMatches==strlen(Word)) break;
		Start++;
	}
	return Pointer;
}

static uint16_t FindPacket(char *RecievedData, uint16_t Start, uint8_t PacketNum, uint16_t* PacketLength){
	uint8_t Entrances = 0;
	for (uint16_t position = Start; position<strlen(RecievedData); position++)
	{
		if (RecievedData[position]==',') Entrances++;
		if (Entrances == PacketNum){
			for (uint16_t length = position + 1; length<strlen(RecievedData); length++){
				if (RecievedData[length]==','){
					*PacketLength = length - position - 1;
					if (length - position - 1==0) NEO7M_Ready = HAL_ERROR;
					else NEO7M_Ready = HAL_OK;
					break;
				}			
			}
			return position + 1;	
		}			
	}
	return 0;
}

void RecieveData(char *RecievedData){
	ReceptionEnd = 0;
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t*)RecievedData, GPS_DATA_SIZE);
	while (ReceptionEnd==0);
}

void ReadTime(char *RecievedData, char* Time){	
	uint16_t Packet = FindMatch(RecievedData, GPGGA);
	uint16_t PacketLength = 0;
	uint8_t temp = 0;
	Packet = FindPacket(RecievedData, Packet, GPS_TimePacket, &PacketLength);
	if (((uint8_t)RecievedData[Packet])>=48){
		for(uint8_t i = 0; i<3; i++){
			temp = ((uint8_t)RecievedData[Packet + i*2]-48)*10 + ((uint8_t)RecievedData[Packet + i*2 + 1]-48);
			if (i==0){
				temp+=3;
				if (temp>=24) temp = temp - 24;
			}
			Time[i*2] = (temp/10)+48;
			Time[i*2+1] = (temp%10)+48;
		}
	}
}

void ReadGPSData(char *RecievedData, double* Latitude, double* Longitude, double* Altitude){
	uint32_t Position[2];
	uint16_t Packet = 0, PacketLength = 0;
	char *stop, TempAltitude[15] = {0};
	
	uint16_t Pointer = FindMatch(RecievedData, GPGGA);
	Packet = FindPacket(RecievedData, Pointer, GPS_LatitudePacket, &PacketLength);	
	if (NEO7M_Ready==HAL_OK){
		Position[0] = ((uint8_t)RecievedData[Packet]-48)*1000 + ((uint8_t)RecievedData[Packet+1]-48)*100 + ((uint8_t)RecievedData[Packet+2]-48)*10 + ((uint8_t)RecievedData[Packet+3]-48);
		Packet += 5;
		Position[1] = ((uint8_t)RecievedData[Packet]-48)*10000 + ((uint8_t)RecievedData[Packet+1]-48)*1000 + ((uint8_t)RecievedData[Packet+2]-48)*100 + ((uint8_t)RecievedData[Packet+3]-48)*10 + ((uint8_t)RecievedData[Packet+4]-48);
		*Latitude = (uint8_t)(Position[0]/100) + (Position[0]%100 + Position[1]/100000.0)/60.0;
		Packet = FindPacket(RecievedData, Pointer, GPS_LongitudePacket, &PacketLength);	
		Position[0] = ((uint8_t)RecievedData[Packet]-48)*10000 + ((uint8_t)RecievedData[Packet+1]-48)*1000 + ((uint8_t)RecievedData[Packet+2]-48)*100 + ((uint8_t)RecievedData[Packet+3]-48)*10 + ((uint8_t)RecievedData[Packet+4]-48);
		Packet += 6;
		Position[1] = ((uint8_t)RecievedData[Packet]-48)*10000 + ((uint8_t)RecievedData[Packet+1]-48)*1000 + ((uint8_t)RecievedData[Packet+2]-48)*100 + ((uint8_t)RecievedData[Packet+3]-48)*10 + ((uint8_t)RecievedData[Packet+4]-48);
		*Longitude = (uint8_t)(Position[0]/100) + (Position[0]%100 + Position[1]/100000.0)/60.0;
		Packet = FindPacket(RecievedData, Pointer, GPS_AltitudePacket, &PacketLength);	
		
		for (uint16_t index = Packet; index-Packet < PacketLength; index++){
			TempAltitude[index-Packet] = RecievedData[index];
		}
		*Altitude = strtod(TempAltitude, &stop);
	}
}
