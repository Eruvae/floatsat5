/*
 * MissionController.cpp
 *
 *  Created on: 30.01.2018
 *      Author: TobiZ
 */

#include "MissionController.h"

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
		switch(state)
		{
		//TODO
		}

		suspendUntilNextBeat();
	}
}
