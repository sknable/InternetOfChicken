#include "DoorControl.h"

DoorControl::DoorControl(int In1pin, int In2pin, int PWMpin, STBYpin) : In1(In1pin), In2(In2pin), PWM(PWMpin), Standby(STBYpin)
{
}

void 
DoorControl::stop()
{
	digitalWrite(Standby, HIGH);
	digitalWrite(In1, LOW);
	digitalWrite(In2, LOW);
	analogWrite(PWM,0);
}

void 
DoorControl::close()
{
   digitalWrite(In1, LOW);
   digitalWrite(In2, HIGH);
   analogWrite(PWM, DEFAULTSPEED);
}

void 
DoorControl::open()
{
   digitalWrite(In1, HIGH);
   digitalWrite(In2, LOW);
   analogWrite(PWM, DEFAULTSPEED);
}