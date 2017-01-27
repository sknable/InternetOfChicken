#include "CMessage.h";
#include <Wire.h>
/*
 * Cmessage.cpp
 *
 *  Created on: Jan 27, 2017
 *      Author: sknable
 */
CMessage::CMessage ()
{
}

CMessage::~CMessage ()
{
}

void
CMessage::initEnvironment (float temp, float humidity)
{
}

byte
CMessage::getEnvironment (float * temp, float * humidity)
{
}

void
CMessage::serialize (byte * msg)
{
	switch ()
		{
		case :
		Wire.write(msg,5);
		break;
	}

}

byte
CMessage::deserialize (byte *msg)
{
}

