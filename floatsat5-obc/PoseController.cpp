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
								//starTrackerPoseSub(itStarTrackerPose, starTrackerPoseBuffer),
								poseControllerModeSub(itPoseControllerMode, poseControllerModeBuffer)
								//otDataSub(itObjectTrackingPose, otDataBuffer)
{}

void PoseController::controlYaw()
{
	// yaw control
	float yaw = filteredPose.yaw;
	float goalYaw = targetPose.yaw;

	float error = goalYaw - yaw;
	MOD(error, -180, 180);

	errYaw_int += error * period;

	float rwSpeedDifDps = attP * error - attD * filteredPose.dyaw + attI * errYaw_int;

	int16_t rwSpeedDifRpm = (int16_t)(rwSpeedDifDps / 6);
	int16_t newRwSpeed = currentRwSpeed + rwSpeedDifRpm; // control output is difference of RW-speed

	tcReactionWheelTargetSpeed.put(newRwSpeed);
}

void PoseController::controlPosition()
{
	// position control
	float x = filteredPose.x;
	float y = filteredPose.y;

	// USE OT YAW
	/*OTData otData;
	otDataBuffer.get(otData);
	float r = otData.g0 + 0.2;
	float otYaw = M_PI + otData.alpha + atan2(otData.G0, r);*/
	// END

	// x and y errors in global system
	float eX = targetPose.x - x;
	float eY = targetPose.y - y;
	//print_debug_msg("Errors: %f, %f", eX, eY);
	rotateCoord(eX, eY, filteredPose.yaw*M_PI/180.f , eX, eY); // convert to vehicle system

	// same for velocity
	float velX = (x - oldX) / period;
	float velY = (y - oldY) / period;

	float eX_dif = targetVel.x - velX;
	float eY_dif = targetVel.y - velY;
	rotateCoord(eX_dif, eY_dif, filteredPose.yaw*M_PI/180.f , eX_dif, eY_dif);

	// use only ERR_ARR_SIZE values for integral part
	eX_int -= eX_arr[errIndex];
	eY_int -= eY_arr[errIndex];
	eX_arr[errIndex] = eX * period;
	eY_arr[errIndex] = eY * period;
	eX_int += eX_arr[errIndex];
	eY_int += eY_arr[errIndex];
	errIndex = (errIndex + 1) % ERR_ARR_SIZE;

	//eX_int += eX * period;
	//eY_int += eY * period;

	oldX = x;
	oldY = y;

	// calculate control values
	float zX = k * (eX + td * eX_dif + ti*eX_int);
	float zY = k * (eY + td * eY_dif + ti*eY_int);

	float gX = SIGN(zX) * (1.f - exp(-ABS(zX))) / sqrt(2);
	float gY = SIGN(zY) * (1.f - exp(-ABS(zY))) / sqrt(2);

	float alpha = atan2(gY, gX) * 180.f/M_PI;
	MOD(alpha, 0.f, 360.f);

	//print_debug_msg("Gx: %.2f, Gy: %.2f, Yaw: %.2f, a: %.2f", gX, gY, filteredPose.yaw, alpha);

	// decide which thrusters to activate and how long
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

	//print_debug_msg("a: %.2f, f1: %.2f, f2: %.2f, f3: %.2f\n", alpha, controls.f1, controls.f2, controls.f3);
	itThrusterControls.publish(controls); // send controls to valvePWN thread
}

void PoseController::run()
{
	setPeriodicBeat(20*MILLISECONDS, period*SECONDS);
	itPoseControllerMode.publish(mode);
	ControlParameters params = {attP, attD, attI, k, td, ti, rotP, rotD, rotI};
	tcControlParams.put(params);
	bool valvePWMEnabled = false;
	activateValvePWM.put(valvePWMEnabled);
	bool activateRWSpeedControl = true;
	while(1)
	{
		PoseControllerMode oldMode = mode;
		poseControllerModeBuffer.get(mode);
		filteredPoseBuffer.get(filteredPose);
		reactionWheelSpeedBuffer.get(currentRwSpeed);
		Pose2D oldTargetPose = targetPose;
		if (mode != PoseControllerMode::FOLLOW_TRAJECTORY && !tcNextTargetPoseList.isEmpty())
		{
			// go to follow trajectory mode if pose list was published
			mode = PoseControllerMode::FOLLOW_TRAJECTORY;
			itPoseControllerMode.publish(mode);
			tcNextTargetPoseList.get(targetPose);
			targetPoseSemaphore.enter();
			tcTargetPose.put(targetPose);
			targetPoseSemaphore.leave();
		}

		tcTargetPose.get(targetPose);

		tcControlParams.get(params);
		attP = params.attP; attD = params.attD; attI = params.attI;
		k = params.traP; td = params.traD; ti = params.traI;
		rotP = params.rotP; rotD = params.rotD; rotI = params.rotI;

		// desaturate wheel; not used
		/*if (activateRWSpeedControl && ABS(currentRwSpeed) > 4500)
		{
			activateRWSpeedControl = false;
			activateRWSpeedController.put(activateRWSpeedControl);
		}
		else if (!activateRWSpeedControl && ABS(currentRwSpeed) < 2000)
		{
			activateRWSpeedControl = true;
			activateRWSpeedController.put(activateRWSpeedControl);
		}*/

		if (targetPose.x != oldTargetPose.x ||
			targetPose.y != oldTargetPose.y ||
			targetPose.yaw != oldTargetPose.yaw ||
			mode != oldMode) // reset int
		{
			// reset integral parts if target or mode changed
			eX_int = 0; eY_int = 0; errYaw_int = 0; errRot_int = 0;
		}

		if (mode == PoseControllerMode::CHANGE_ATTITUDE) // control only attitude, no position
		{
			valvePWMEnabled = false;
			activateValvePWM.put(valvePWMEnabled);

			//if (activateRWSpeedControl)
			controlYaw();

		}
		else if (mode == PoseControllerMode::ROTATE) // rotate at desired speed
		{
			valvePWMEnabled = false;
			activateValvePWM.put(valvePWMEnabled);

			float targetRotationSpeed;
			desiredRotationSpeed.get(targetRotationSpeed);
			float dyaw = filteredPose.dyaw;

			float error = targetRotationSpeed - dyaw;

			float errRot_dif = (dyaw - old_dYaw) / period;

			errRot_int += error * period;

			float rwSpeedDifDps = rotP * error + rotD * errRot_dif + rotI * errRot_int;

			int16_t rwSpeedDifRpm = (int16_t)(rwSpeedDifDps / 6);
			int16_t newRwSpeed = currentRwSpeed + rwSpeedDifRpm;

			tcReactionWheelTargetSpeed.put(newRwSpeed);
		}
		else if (mode == PoseControllerMode::GOTO_POSE) // stay or goto set position
		{
			// activate valve PWM
			valvePWMEnabled = true;
			activateValvePWM.put(valvePWMEnabled);

			// target velocity is 0 (stay at point)
			targetVel.x = 0;
			targetVel.y = 0;
			targetVel.yaw = 0;
			//if (activateRWSpeedControl)
			controlYaw();

			controlPosition();
		}
		else if (mode == PoseControllerMode::FOLLOW_TRAJECTORY) // follow trajectory by using pose list (legacy, not used)
		{
			valvePWMEnabled = true;
			activateValvePWM.put(valvePWMEnabled);

			if (ABS(targetPose.x - filteredPose.x) < 0.1 && ABS(targetPose.y - filteredPose.y) < 0.1)
			{
				// use next pose as target if pose list not empty, otherwise stay at current pose
				if (!tcNextTargetPoseList.isEmpty())
				{
					tcNextTargetPoseList.get(targetPose);
					targetPoseSemaphore.enter();
					tcTargetPose.put(targetPose);
					targetPoseSemaphore.leave();
				}
				else
				{
					mode = PoseControllerMode::GOTO_POSE;
				}
			}

			targetVel.x = 0;
			targetVel.y = 0;
			targetVel.yaw = 0;

			controlYaw();
			controlPosition();
		}
		else if (mode == PoseControllerMode::FOLLOW_TRAJECTORY_T) // time based trajectory control
		{
			valvePWMEnabled = true;
			activateValvePWM.put(valvePWMEnabled);

			// read plan
			TrajectoryPlanData plan;
			trajPlanBuffer.get(plan);
			float t = (float)(NOW() - plan.startTime) / SECONDS;
			float T = (float)(plan.endTime - plan.startTime) / SECONDS;
			// calculate pose and velocity at current time for desired trajectory
			if (plan.type == LINE)
			{
				targetPose = curveLinePos(s(t, T), plan.lineData.startPose, plan.lineData.endPose);
				targetVel = curveLineVel(s(t, T), plan.lineData.startPose, plan.lineData.endPose);
			}
			else if (plan.type == CIRCLE)
			{
				targetPose = curveCirclePos(s(t, T), plan.circleData.centerPose, plan.circleData.r, plan.circleData.betaStart, plan.circleData.betaEnd);
				targetVel = curveCircleVel(s(t, T), plan.circleData.centerPose, plan.circleData.r, plan.circleData.betaStart, plan.circleData.betaEnd);
			}
			targetVel.x *= ds(t, T);
			targetVel.y *= ds(t, T);
			targetVel.yaw *= ds(t, T);

			print_debug_msg("TPose: %f, %f, %f", targetPose.x, targetPose.y, targetPose.yaw);
			//print_debug_msg("TVel: %f, %f, %f", targetVel.x, targetVel.y, targetVel.yaw);

			// use calculated pose and velocity as control input
			targetPoseSemaphore.enter();
			tcTargetPose.put(targetPose);
			targetPoseSemaphore.leave();
			controlYaw();
			controlPosition();
		}
		else
		{
			// deactivate valve PWM if standby
			valvePWMEnabled = false;
			activateValvePWM.put(valvePWMEnabled);

			/*ThrusterControls controls;
			controls.f1 = 0;
			controls.f2 = 0;
			controls.f3 = 0;
			itThrusterControls.publish(controls);*/
		}

		suspendUntilNextBeat();
	}
}
