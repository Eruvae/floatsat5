/*
 * RaspiComm.cpp
 *
 *  Created on: 14.12.2017
 *      Author: TobiZ
 */

#include "RaspiComm.h"
#include <cstdlib>
#include <cstdio>

RaspiComm raspiComm;

RaspiComm::RaspiComm() : state(AWAITING_COMMAND)
{
	// TODO Auto-generated constructor stub

}

RaspiComm::~RaspiComm()
{
	// TODO Auto-generated destructor stub
}

int RaspiComm::decodeRTM(const char *buf, int len, RTM &receivedData/*, char **tail*/)
{
	int readStatus = 0, idPos = 0;
	//*tail = (char*)buf;
	for(char *readP = (char*)buf; readP < buf + len; readP++)
	{
		if (readStatus == 0)
		{
			if (*readP == ':')
			{
				receivedData.id[idPos] = '\0';
				readStatus++;
			}
			else if (*readP == '\n')
			{
				receivedData.id[idPos] = '\0';
				//*tail = readP;
				return readStatus;
			}
			else
			{
				if (idPos < ID_MAX_SIZE)
					receivedData.id[idPos++] = *readP;
				else
					return -1;
			}
		}
		else
		{
			if (readStatus > MAX_DATA_NUM)
				return -1;

			if (strcmp(receivedData.id, "OT") == 0 && readStatus == 4) // OT found data
			{
				if (*readP == '1')
					receivedData.otData.found = true;
				else if (*readP == '0')
					receivedData.otData.found = false;
				else // error
					return -1;

				readP++;
			}
			else if (strcmp(receivedData.id, "RS") == 0) // Raspberry Pi Status
			{
				if (*readP == '1')
					receivedData.boolData[readStatus - 1] = true;
				else if (*readP == '0')
					receivedData.boolData[readStatus - 1] = false;
				else // error
					return -1;

				readP++;
			}
			else // default: float
			{
				char *tailP;
				receivedData.data[readStatus - 1] = strtod(readP, &tailP);
				if (tailP == readP)
					return -1;

				readP = tailP;
			}
			if (*readP == ':') // next command
				readStatus++;
			else if (*readP == '\n') // end command
			{
				//*tail = readP;
				return readStatus;
			}
			else
				return -1;
		}
	}
	return -1;
}

void RaspiComm::publishData(RTM &receivedData)
{
	if (strcmp(receivedData.id, "ST") == 0) // Star Tracker
	{
		//print_debug_msg("ST: %f, %f, %f", receivedData.starTrackerPose.x, receivedData.starTrackerPose.y, receivedData.starTrackerPose.yaw);
		itStarTrackerPose.publish(receivedData.starTrackerPose);
		bool stReceived = true;
		starTrackerReceived.put(stReceived);
	}
	else if (strcmp(receivedData.id, "OT") == 0) // Object Tracking
	{
		//print_debug_msg("OTdata: %.2f, %.2f, %.2f, %.2f, %d", r, otX, otY, otYaw, receivedData.otData.found);
		itObjectTrackingPose.publish(receivedData.otData);
		bool otReceived = true;
		objectTrackerReceived.put(otReceived);
	}
	else if (strcmp(receivedData.id, "RD") == 0) // Radio
	{
		print_debug_msg("RD-Pose: %f, %f, %f, %f", receivedData.radioPose.x1, receivedData.radioPose.y1, receivedData.radioPose.x2, receivedData.radioPose.y2);
		itRadioPosition.publish(receivedData.radioPose);
	}
	else if (strcmp(receivedData.id, "RS") == 0)
	{
		//print_debug_msg("Status: %d, %d, %d", receivedData.status.stEnabled, receivedData.status.otEnabled, receivedData.status.rdEnabled);
		itRaspiStatus.publish(receivedData.status);
	}
	else if (strcmp(receivedData.id, "OK") == 0)
	{
		const char *toWrite = "OK received!";
		raspiUART.write(toWrite, strlen(toWrite));
	}
}

void RaspiComm::sendCommand(RaspiCommand com, bool enable)
{
	char tc[5];
	tc[0] = '$';
	switch (com)
	{
		case ST: tc[1] = 'S'; tc[2] = 'T'; break;
		case OT: tc[1] = 'O'; tc[2] = 'T'; break;
		case RD: tc[1] = 'R'; tc[2] = 'D'; break;
		default: return;
	}
	tc[3] = enable ? '1' : '0';
	tc[4] = '\n';

	raspiUART.write(tc, 5);
}

void RaspiComm::run()
{
	char buf[MAX_RTM_SIZE];
	int index = 0;
	while(1)
	{
		raspiUART.suspendUntilDataReady();

		/*int len = raspiUART.read(buf, MAX_RTM_SIZE - 1);
		buf[len] = 0;
		char echoBuf[MAX_RTM_SIZE + 10];
		sprintf(echoBuf, "ECHO: %s\n", buf);
		raspiUART.write(echoBuf, strlen(echoBuf)); // ECHO*/

		char c = raspiUART.getcharNoWait();
		if (state == AWAITING_COMMAND)
		{
			if (c == '$')
			{
				state = RECEIVING_COMMAND;
				index = 0;
			}
		}
		else if (state == RECEIVING_COMMAND)
		{
			if (index < MAX_RTM_SIZE)
			{
				buf[index++] = c;
				if (c == '\n')
				{
					//buf[index] = 0;
					//print_debug_msg("Rec: %s\n", buf);
					RTM receivedData;
					int ret = decodeRTM(buf, index + 1, receivedData);

					if (ret >= 0)
						publishData(receivedData);

					index = 0;
					state = AWAITING_COMMAND;
				}
			}
			else // command too long
			{
				//buf[index] = 0;
				//print_debug_msg("Rec Failed: %s\n", buf);
				index = 0;
				state = AWAITING_COMMAND;
			}
		}

		/* char buf[RASPI_UART_BUF_SIZE];
		int len = raspiUART.read(buf, RASPI_UART_BUF_SIZE - 1);
		buf[len] = 0;
		char echoBuf[RASPI_UART_BUF_SIZE + 10];
		sprintf(echoBuf, "ECHO: %s\n", buf);
		raspiUART.write(echoBuf, strlen(echoBuf)); // ECHO
		for (int i = 0; i < len; i++)
		{
			if (buf[i] == '$')
			{
				RTM receivedData;
				char *tall;
				int retPos = decodeRTM(buf + i + 1, len - i - 1, receivedData, &tall);
				if (retPos >= 0) // success
				{
					// TODO: publish data
					char tmpBuf[512];
					sprintf(tmpBuf, "Data received - Ret: %d, ID: %s, D1: %f, D2, %f, D3: %f\n",
							retPos, receivedData.id, receivedData.data[0], receivedData.data[1], receivedData.data[2]);
					raspiUART.write(tmpBuf, strlen(tmpBuf));
					raspiUART.suspendUntilWriteFinished();
					i += tall - buf + 1;
				}
			}
		}*/

	}

}
