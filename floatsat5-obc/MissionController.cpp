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

void MissionController::run()
{
	setPeriodicBeat(30*MILLISECONDS, 200*MILLISECONDS);
	while(1)
	{

		suspendUntilNextBeat();
	}
}
