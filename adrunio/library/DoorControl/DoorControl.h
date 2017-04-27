#ifndef IOC_DOOR_h
#define IOC_DOOR_h

#include <Arduino.h>

#define DEFAULTSPEED 255  

class DoorControl
{

typedef enum _DOOR_STATE
{
	DOOR_IDLE, 
	DOOR_OPENING, 
	DOOR_CLOSEING, 
	DOOR_ERROR,
} DOOR_STATE;

public:

	DoorControl(int In1pin, int In2pin, int PWMpin, STBYpin);
	void open();
	void close();
	void stop();
	
private:

	int In1, In2, PWM, Standby;
	
}


#endif