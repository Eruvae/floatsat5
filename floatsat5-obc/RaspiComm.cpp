/*
 * RaspiComm.cpp
 *
 *  Created on: 14.12.2017
 *      Author: TobiZ
 */

#include "RaspiComm.h"

RaspiComm raspiComm;

RaspiComm::RaspiComm()
{
	// TODO Auto-generated constructor stub

}

RaspiComm::~RaspiComm()
{
	// TODO Auto-generated destructor stub
}

void RaspiComm::run()
{
	char buf[256];
	while(1)
	{
		raspiUART.suspendUntilDataReady();
		int len = raspiUART.read(buf, 256);
		raspiUART.write(buf, len);
		//PRINTF("Received from Raspi: %s\n", buf);

		//const char *toSend = "Test Sending\n";


		suspendCallerUntil(NOW() + 500*MILLISECONDS);

	}

}
