/*
 * Telecommand.h
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#ifndef TELECOMMANDRECEIVER_H_
#define TELECOMMANDRECEIVER_H_

#include "rodos.h"
#include "CommInterfaces.h"
#include "Topics.h"

class TelecommandReceiver : public SubscriberReceiver<Telecommand>
{
public:
	TelecommandReceiver () : SubscriberReceiver<Telecommand>(telecommand, "TelecommandRecieved"),
					receivedCommand(0), receivedCommandNumber(0) {}

	char receivedCommand;
	float receivedCommandNumber;

	void init () {}

	void put(Telecommand &data);
};

#endif /* TELECOMMANDRECEIVER_H_ */
