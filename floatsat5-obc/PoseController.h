/*
 * PoseController.h
 *
 *  Created on: 17.01.2018
 *      Author: TobiZ
 */

#include <rodos.h>
#include "Topics.h"

#ifndef POSECONTROLLER_H_
#define POSECONTROLLER_H_

class PoseController : public Thread
{
	CommBuffer<Pose> filteredPoseBuffer;
	Subscriber filterePoseSub;

	CommBuffer<int16_t> reactionWheelSpeedBuffer;
	Subscriber reactionWheelSpeedSub;
public:
	PoseController();

	void run();
};

#endif /* POSECONTROLLER_H_ */
