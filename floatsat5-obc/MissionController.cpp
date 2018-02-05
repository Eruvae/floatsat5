/*
 * MissionController.cpp
 *
 *  Created on: 30.01.2018
 *      Author: TobiZ
 */

#include "MissionController.h"
#include "Topics.h"
#include "RaspiComm.h"

MissionController missionController;

MissionController::MissionController()
{
	// TODO Auto-generated constructor stub

}

MissionController::~MissionController()
{
	// TODO Auto-generated destructor stub
}

enum class MissionState
{
	STANDBY, START_SEARCHING, SEARCHING_TARGET, START_MOVING, GOTO_TARGET, DOCKING
};

void MissionController::run()
{
	setPeriodicBeat(30*MILLISECONDS, 200*MILLISECONDS);
	while(1)
	{
		MissionState state = MissionState::STANDBY;
		if (state == MissionState::START_SEARCHING)
		{
			raspiComm.sendCommand(OT, true);

			itPoseControllerMode.publishConst(PoseControllerMode::CHANGE_ATTITUDE);
		}
		else if (state == MissionState::SEARCHING_TARGET)
		{
			OTData otData;

			//TODO: get OT pose

			if (otData.found)
			{
				// SAVE FOUND POSE
			}
			else
			{
				// CHANGE YAW
			}
		}
		else if (state == MissionState::START_MOVING)
		{
			raspiComm.sendCommand(ST, true);
		}

		suspendUntilNextBeat();
	}
}
