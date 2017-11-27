/*
 * Telecommand.cpp
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#include "TelecommandReceiver.h"

TelecommandReceiver telecommandReceiver;

void TelecommandReceiver::put(Telecommand &data)
{
	Telecommand* command = (Telecommand*)&data;
	receivedCommandNumber = command->data;
	receivedCommand= command->id;

	PRINTF("\n The Recieve command =%c  and recievedCommandNumber =  is %f", receivedCommand, receivedCommandNumber );
}
