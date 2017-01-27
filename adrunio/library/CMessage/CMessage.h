/*
 * CMessage.h
 *
 *  Created on: Jan 27, 2017
 *      Author: sknable
 */

#ifndef COMMON_H_RPHY_CMESSAGE_H_
#define COMMON_H_RPHY_CMESSAGE_H_

class CMessage
{

	//Message Type and Lengths
	private:

		typedef enum
		{
			T_Enviroment = 1, L_Enviroment = 2,

		} Cluck_Types;

	//Message Definitions
	private:

		typedef struct _Cluck
		{

				byte messageType;
				byte message[31];

		} Cluck;


		typedef struct _Cluck_Environment
		{
				byte temp;
				byte humidity;

		} Msg_Environment;

	public:
		CMessage ();
		~CMessage ();

		void
		initEnvironment (float temp, float humidity);

		byte
		getEnvironment (float * temp, float * humidity);

		void
		serialize (byte * msg);

		byte
		deserialize (byte *msg);
};

#endif /* COMMON_H_RPHY_CMESSAGE_H_ */
