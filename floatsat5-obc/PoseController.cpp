/*
 * PoseController.cpp
 *
 *  Created on: 17.01.2018
 *      Author: TobiZ
 */

#include "PoseController.h"
#include <cmath>

PoseController poseController;

PoseController::PoseController() : filterePoseSub(itFilteredPose, filteredPoseBuffer),
								reactionWheelSpeedSub(itReactionWheelSpeed, reactionWheelSpeedBuffer),
								starTrackerPoseSub(itStarTrackerPose, starTrackerPoseBuffer),
								poseControllerModeSub(itPoseControllerMode, poseControllerModeBuffer)
{}


void PoseController::run()
{
	const float period = 0.1;
	setPeriodicBeat(20*MILLISECONDS, period*SECONDS);
	Pose filteredPose;
	Pose targetPose = {0};
	tcTargetPose.put(targetPose); // Start with 0
	int16_t currentRwSpeed;
	bool activated = false;
	PoseControllerMode mode = PoseControllerMode::STANDBY;
	tcActivateController.put(activated);
	itPoseControllerMode.publish(mode);
	float oldeX = 0, oldeY = 0;
	const float k = 100.f, td = 1.5f, gamma = sqrt(3)/2;
	while(1)
	{
		tcActivateController.get(activated);
		poseControllerModeBuffer.get(mode);
		filteredPoseBuffer.get(filteredPose);
		reactionWheelSpeedBuffer.get(currentRwSpeed);
		tcTargetPose.get(targetPose);
		if (mode == PoseControllerMode::CHANGE_ATTITUDE)
		{
			// yaw control
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
		else if (mode == PoseControllerMode::ROTATE)
		{
			float targetRotationSpeed = 20; // DPS, change to tc later
			float dyaw = filteredPose.dyaw;
		}
		else if (mode == PoseControllerMode::FOLLOW_TRAJECTORY)
		{
			float x = filteredPose.x;
			float y = filteredPose.y;
			float goalX = targetPose.x;
			float goalY = targetPose.y;

			float eX = goalX - x;
			float eY = goalY - y;
			rotateCoord(eX, eY, filteredPose.yaw*M_PI/180.0, eX, eY);

			float eX_dif = (eX - oldeX) / period;
			float eY_dif = (eY - oldeY) / period;

			oldeX = eX;
			oldeY = eY;

			float zX = k * (eX + td * eX_dif);
			float zY = k * (eY + td * eY_dif);

			float gX = SIGN(zX) * (1 - exp(-ABS(zX))) / sqrt(2);
			float gY = SIGN(zY) * (1 - exp(-ABS(zY))) / sqrt(2);

			float alpha = atan2(gY, gX) * 180.f/M_PI;
			MOD(alpha, 0, 360);

			print_debug_msg("Ex: %.2f, Ey: %.2f, Yaw: %.2f", eX, eY, filteredPose.yaw);

			ThrusterControls controls;
			if (alpha < 120)
			{
				controls.f1 = gamma*gX + gamma*gY/sqrt(3);
				controls.f2 = 2*gamma/sqrt(3)*gY;
				controls.f3 = 0;
			}
			else if (alpha < 240)
			{
				controls.f1 = 0;
				controls.f2 = gamma*gY/sqrt(3) - gamma*gX;
				controls.f3 = -gamma*gX - gamma*gY/sqrt(3);
			}
			else
			{
				controls.f1 = gamma*gX - gamma*gY/sqrt(3);
				controls.f2 = 0;
				controls.f3 = -2*gamma*gY/sqrt(3);
			}
			itThrusterControls.publish(controls);
		}

		suspendUntilNextBeat();
	}
}
