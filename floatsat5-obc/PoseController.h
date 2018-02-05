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
	const float period = 0.2;
	Pose filteredPose = {0};
	Pose2D targetPose = {0};
	int16_t currentRwSpeed = 0;
	PoseControllerMode mode = PoseControllerMode::STANDBY;
	float oldeX = 0, oldeY = 0;
	float eX_int = 0, eY_int = 0;
	float oldYawErr = 0;
	float errYaw_int = 0;
	float errRot_int = 0, old_dYaw = 0;
	float attP = 10.f, attD = -4.f, attI = 0.f;
	float rotP = 100.f, rotD = 0.f, rotI = 10.f;
	float k = 1.5f, td = 5.f, ti = 0.05f, gamma = sqrt(3)/2; // trajectory control params

	CommBuffer<Pose> filteredPoseBuffer;
	Subscriber filterePoseSub;

	CommBuffer<int16_t> reactionWheelSpeedBuffer;
	Subscriber reactionWheelSpeedSub;

	//CommBuffer<Pose2D> starTrackerPoseBuffer;
	//Subscriber starTrackerPoseSub;

	CommBuffer<PoseControllerMode> poseControllerModeBuffer;
	Subscriber poseControllerModeSub;

	//CommBuffer<OTData> otDataBuffer;
	//Subscriber otDataSub;

	void controlYaw();
	void controlPosition();
public:
	PoseController();

	void run();
};

#endif /* POSECONTROLLER_H_ */
