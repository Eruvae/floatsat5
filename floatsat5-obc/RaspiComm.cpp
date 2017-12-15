/*
 * RaspiComm.cpp
 *
 *  Created on: 14.12.2017
 *      Author: TobiZ
 */

#include "RaspiComm.h"

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
		raspiUART.read(buf, 256);

		PRINTF("Received from Raspi: %s\n", buf);
	}

}
