/*
 * PoseController.cpp
 *
 *  Created on: 17.01.2018
 *      Author: TobiZ
 */

#include "PoseController.h"

PoseController poseController;

PoseController::PoseController() : filterePoseSub(itFilteredPose, filteredPoseBuffer),
								reactionWheelSpeedSub(itReactionWheelSpeed, reactionWheelSpeedBuffer)
{
}


void PoseController::run()
{
	setPeriodicBeat(20*MILLISECONDS, 50*MILLISECONDS);
	Pose filteredPose;
	Pose targetPose = {0};
	tcTargetPose.put(targetPose); // Start with 0
	int16_t currentRwSpeed;
	bool activated = false;
	tcActivateController.put(activated);
	while(1)
	{
		tcActivateController.get(activated);
		if (activated)
		{
			filteredPoseBuffer.get(filteredPose);
			reactionWheelSpeedBuffer.get(currentRwSpeed);
			tcTargetPose.get(targetPose);
			float yaw = filteredPose.yaw;

			float goalYaw = targetPose.yaw; // TODO: change via Topic

			const float p = 10.0f;
			//const float i = 0.1f;
			//const float d = 0.5f;


			float error = goalYaw - yaw;
			MOD(error, -180, 180);

			float rwSpeedDifDps = -p * error;

			int16_t rwSpeedDifRpm = (int16_t)(rwSpeedDifDps / 6);

			int16_t newRwSpeed = currentRwSpeed + rwSpeedDifRpm;

			tcReactionWheelTargetSpeed.put(newRwSpeed);
		}

		suspendUntilNextBeat();
	}
}
