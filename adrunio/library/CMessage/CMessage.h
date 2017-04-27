#ifndef IOC_MESSAGE_h
#define IOC_MESSAGE_h

#include <Arduino.h>


#define ADD_UINT16(p, data) { *p++ = ((data & 0xff00) >> 8); \
		*p++ = (data & 0xff); }

#define ADD_UINT32(p, data) { *p++ = ((data & 0xff000000) >> 24); \
		*p++ = ((data & 0x00ff0000) >> 16); \
		*p++ = ((data & 0x0000ff00) >> 8); \
		*p++ = (data & 0x000000ff);  }

#define GET_UINT32(p, data) { data  = (*p++ << 24); \
		data |= (*p++ << 16); \
		data |= (*p++ << 8); \
		data |= *p++; }

#define GET_UINT16(p, data) { data  = (*p++ << 8); \
		data |= *p++; }


class CMessage
{
public:
	CMessage();
	~CMessage();

	//Message Type and Lengths
private:

	typedef enum
	{
		L_Type = 1,
		L_Size = 1,
		T_Enviroment = 1,
		T_Temp = 1,
		L_Temp = 1,
		T_Humidity = 2,
		L_Humidity = 1,
	} Cluck_TLV;
	//One Byte for Type
	//One Byte for Length
	//Variable Size Value.

	typedef enum
	{
		Enviroment = 1,
	} Cluck_Types;

	typedef struct _Cluck
	{
		byte messageType;
		byte message[32];
		byte size;
	} Cluck;

public:

	typedef struct _Cluck_Environment
	{
		byte temp;
		byte humidity;
	} Msg_Environment;

private:
	Cluck myMsg;

public:

	void
		initEnvironment(const Msg_Environment & msg);

	void
		getEnvironment(Msg_Environment & msg);

	void
		serialize();

	void
		deserialize();
};
#endif
