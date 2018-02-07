/*
 * MissionController.h
 *
 *  Created on: 30.01.2018
 *      Author: TobiZ
 */

#ifndef MISSIONCONTROLLER_H_
#define MISSIONCONTROLLER_H_

#include <rodos.h>
#include "Topics.h"

class MissionController : public Thread
{
	CommBuffer<Pose> filteredPoseBuffer;
	Subscriber filterePoseSub;

	CommBuffer<MissionState> missionStateBuffer;
	Subscriber missionStateSubscriber;

	CommBuffer<OTData> otDataBuffer;
	Subscriber otDataSub;
public:
	MissionController();

	void run();
};

#endif /* MISSIONCONTROLLER_H_ */
