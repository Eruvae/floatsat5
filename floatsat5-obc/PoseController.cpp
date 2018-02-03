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
								poseControllerModeSub(itPoseControllerMode, poseControllerModeBuffer),
								otDataSub(itObjectTrackingPose, otDataBuffer)
{}


void PoseController::run()
{
	const float period = 0.2;
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
	float eX_int = 0, eY_int = 0;
	float oldYawErr = 0;
	float errYaw_int = 0;
	float attP = 10.f, attD = 4.f, attI = 0.f;
	float k = 1.5f, td = 5.f, ti = 0.05f, gamma = sqrt(3)/2; // trajectory control params
	ControlParameters params = {attP, attD, attI, k, td, ti};
	tcControlParams.put(params);
	while(1)
	{
		tcActivateController.get(activated);
		PoseControllerMode oldMode = mode;
		poseControllerModeBuffer.get(mode);
		filteredPoseBuffer.get(filteredPose);
		reactionWheelSpeedBuffer.get(currentRwSpeed);
		Pose oldTargetPose = targetPose;
		tcTargetPose.get(targetPose);
		tcControlParams.get(params);
		attP = params.attP; attD = params.attD; attI = params.attI;
		k = params.traP; td = params.traD; ti = params.traI;

		if (targetPose.x != oldTargetPose.x ||
			targetPose.y != oldTargetPose.y ||
			targetPose.yaw != oldTargetPose.yaw ||
			mode != oldMode) // reset int
		{
			eX_int = 0; eY_int = 0; errYaw_int = 0;
		}

		if (mode == PoseControllerMode::CHANGE_ATTITUDE)
		{
			// yaw control
			float yaw = filteredPose.yaw;
			float goalYaw = targetPose.yaw; // TODO: change via Topic

			//const float p = 10.0f;
			//const float i = 0.1f;
			//const float d = 0.5f;

			float error = goalYaw - yaw;
			MOD(error, -180, 180);

			errYaw_int += error * period;

			float rwSpeedDifDps = attP * error + attD * filteredPose.dyaw + attI * errYaw_int;

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
			// yaw control
			float yaw = filteredPose.yaw;
			float goalYaw = targetPose.yaw; // TODO: change via Topic

			//const float p = 15.0f;
			//const float i = 0.1f;
			//const float d = 0.2f;

			float error = goalYaw - yaw;
			MOD(error, -180, 180);

			errYaw_int += error * period;

			float rwSpeedDifDps = attP * error + attD * filteredPose.dyaw + attI * errYaw_int;

			int16_t rwSpeedDifRpm = (int16_t)(rwSpeedDifDps / 6);
			int16_t newRwSpeed = currentRwSpeed + rwSpeedDifRpm;

			tcReactionWheelTargetSpeed.put(newRwSpeed);

			// position control
			float x = filteredPose.x;
			float y = filteredPose.y;
			float goalX = targetPose.x;
			float goalY = targetPose.y;

			// USE OT YAW

			OTData otData;
			otDataBuffer.get(otData);
			float r = otData.g0 + 0.2;
			float otYaw = M_PI + otData.alpha + atan2(otData.G0, r);

			// END

			float eX = goalX - x;
			float eY = goalY - y;
			rotateCoord(eX, eY, filteredPose.yaw*M_PI/180.f , eX, eY);

			float eX_dif = (eX - oldeX) / period;
			float eY_dif = (eY - oldeY) / period;

			eX_int += eX * period;
			eY_int += eY * period;

			oldeX = eX;
			oldeY = eY;

			float zX = k * (eX + td * eX_dif + ti*eX_int);
			float zY = k * (eY + td * eY_dif + ti*eY_int);

			float gX = SIGN(zX) * (1.f - exp(-ABS(zX))) / sqrt(2);
			float gY = SIGN(zY) * (1.f - exp(-ABS(zY))) / sqrt(2);

			float alpha = atan2(gY, gX) * 180.f/M_PI;
			MOD(alpha, 0.f, 360.f);

			//print_debug_msg("Gx: %.2f, Gy: %.2f, Yaw: %.2f, a: %.2f", gX, gY, filteredPose.yaw, alpha);

			ThrusterControls controls;
			if (alpha > 300 || alpha < 60)
			{
				controls.f1 = 0; // gamma*gX + gamma*gY/sqrt(3);
				controls.f2 = gamma*(gX - gY/sqrt(3));//2*gamma/sqrt(3)*gY;
				controls.f3 = gamma*(gX + gY/sqrt(3));//0;
			}
			else if (alpha < 180)
			{
				controls.f1 = gamma*(-gX + gY/sqrt(3));//0;
				controls.f2 = 0; //gamma*gY/sqrt(3) - gamma*gX;
				controls.f3 = 2.f/sqrt(3)*gamma*gY; //-gamma*gX - gamma*gY/sqrt(3);
			}
			else
			{
				controls.f1 = gamma*(-gX - gY/sqrt(3));//gamma*gX - gamma*gY/sqrt(3);
				controls.f2 = -2.f/sqrt(3)*gamma*gY;
				controls.f3 = 0; //-2*gamma*gY/sqrt(3);
			}

			//float dummyX = 1, dummyY = 0;
			//rotateCoord(dummyX, dummyY, M_PI/2.f, dummyX, dummyY);
			//print_debug_msg("Dx: %.2f, Dy: %.2f\n", dummyX, dummyY);

			print_debug_msg("a: %.2f, f1: %.2f, f2: %.2f, f3: %.2f\n", alpha, controls.f1, controls.f2, controls.f3);
			itThrusterControls.publish(controls);
		}
		else
		{
			ThrusterControls controls;
			controls.f1 = 0;
			controls.f2 = 0;
			controls.f3 = 0;
			itThrusterControls.publish(controls);
		}

		suspendUntilNextBeat();
	}
}
