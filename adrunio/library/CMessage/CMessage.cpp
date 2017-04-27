#include "stdafx.h"
#include "Message.h"

CMessage::CMessage()
{
}

CMessage::~CMessage()
{
}
void
CMessage::initEnvironment(const Msg_Environment & msg)
{
	memset(&myMsg, 0, sizeof(myMsg));

	myMsg.messageType = Enviroment;

	byte * p = &myMsg.message[0];

	*p++ = T_Enviroment;
	*p++ = L_Temp + L_Type + L_Size + L_Humidity + L_Type + L_Size;

	*p++ = T_Temp;
	*p++ = L_Temp;
	*p++ = msg.humidity;

	*p++ = T_Humidity;
	*p++ = L_Humidity;
	*p++ = msg.temp;
}

void
CMessage::getEnvironment(Msg_Environment & msg)
{
	byte * p = &myMsg.message[0];

	for (int bytesRead = 0; bytesRead < myMsg.size;)
	{
		byte type = *p++;
		byte length = *p++;

		switch (type)
		{
		case T_Temp:
			msg.temp = *p++;
			break;

		case T_Humidity:
			msg.humidity = *p++;
			break;

		default:
			break;
		}

		bytesRead += length;
	}

	printf("Temp: %d Hum: %d \n", msg.temp, msg.humidity);
}

void
CMessage::serialize()
{
	printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n", myMsg.message[0], myMsg.message[1], myMsg.message[2], myMsg.message[3], myMsg.message[4], myMsg.message[5], myMsg.message[6], myMsg.message[7], myMsg.message[8], myMsg.message[9]);
	printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n", myMsg.message[10], myMsg.message[11], myMsg.message[12], myMsg.message[13], myMsg.message[14], myMsg.message[15], myMsg.message[16], myMsg.message[17], myMsg.message[18], myMsg.message[19]);
	printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n", myMsg.message[20], myMsg.message[21], myMsg.message[22], myMsg.message[23], myMsg.message[24], myMsg.message[25], myMsg.message[26], myMsg.message[27], myMsg.message[28], myMsg.message[29]);
	printf("%02x %02x \n", myMsg.message[30], myMsg.message[31]);
}

void
CMessage::deserialize()
{
	memset(&myMsg, 0, sizeof(myMsg));
	byte * p = &myMsg.message[0];
	myMsg.messageType = *p++;
	myMsg.size = *p++;
}