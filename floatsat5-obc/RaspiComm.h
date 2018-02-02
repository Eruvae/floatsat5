/*
 * RaspiComm.h
 *
 *  Created on: 14.12.2017
 *      Author: TobiZ
 */

#ifndef RASPICOMM_H_
#define RASPICOMM_H_

#include <rodos.h>
#include "CommInterfaces.h"
#include "Topics.h"

#define RASPI_UART_BUF_SIZE	64
#define MAX_RTM_SIZE 64
#define ID_MAX_SIZE 2
#define MAX_DATA_NUM 4

class RaspiComm : public Thread
{
	struct RTM
	{
		char id[ID_MAX_SIZE + 1];
		union
		{
			float data[MAX_DATA_NUM];
			Pose2D starTrackerPose;
			Position2D radioPose;
			OTData otData;
		};
	};

	enum {
		AWAITING_COMMAND, RECEIVING_COMMAND
	} state;

	int decodeRTM(const char *buf, int len, RTM &receivedData/*, char **tail*/);
	void publishData(RTM &receivedData);
public:
	RaspiComm();
	virtual ~RaspiComm();

	void run();
	void sendCommand(RaspiCommand com, bool enable);
};

extern RaspiComm raspiComm;

#endif /* RASPICOMM_H_ */
