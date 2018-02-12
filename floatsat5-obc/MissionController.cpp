/*
 * MissionController.cpp
 *
 *  Created on: 30.01.2018
 *      Author: TobiZ
 */

#include "MissionController.h"
#include "RaspiComm.h"

MissionController missionController;

MissionController::MissionController() : filterePoseSub(itFilteredPose, filteredPoseBuffer),
		otDataSub(itObjectTrackingPose, otDataBuffer), missionStateSubscriber(itMissionState, missionStateBuffer)
{
}

void MissionController::run()
{
	setPeriodicBeat(18*MILLISECONDS, 200*MILLISECONDS);
	MissionState state = MissionState::STANDBY;
	itMissionState.publish(state);
	Pose currentPose;
	bool searchingStarted = false;
	Pose2D searchingPose;
	Pose2D targetPose;
	Pose2D preTargetPose;
	float preTargetDistance = 0;
	float otYaw = 0;
	float angle = 0;
	while(1)
	{
		missionStateBuffer.get(state);
		if (state == MissionState::START_SEARCHING) // prepare for searching target
		{
			// activate star tracker and wait for pose data
			bool stReceived = false;
			starTrackerReceived.put(stReceived);
			raspiComm.sendCommand(ST, true);
			while(stReceived == false)
			{
				suspendUntilNextBeat();
				starTrackerReceived.get(stReceived);
			}
			suspendCallerUntil(NOW() + 50*MILLISECONDS); // wait for filter
			if(!searchingStarted) // if first time looking: save current pose as searching pose
			{
				filteredPoseBuffer.get(currentPose);
				searchingPose = {currentPose.x, currentPose.y, currentPose.yaw};
				searchingStarted = true;
			}
			else // otherwise: add 20 degrees to previous pose to look to other direction
			{
				searchingPose.yaw += 20;
				MOD(searchingPose.yaw, -180, 180);
			}
			// order vehicle to stabilize at this pose for 5 seconds
			targetPoseSemaphore.enter();
			tcTargetPose.put(searchingPose);
			targetPoseSemaphore.leave();
			itPoseControllerMode.publishConst(PoseControllerMode::GOTO_POSE);
			suspendCallerUntil(NOW() + 5*SECONDS);
			// if not commanded to go to standby: go to target searching mode
			missionStateBuffer.get(state);
			if (state == MissionState::STANDBY) continue;
			state = MissionState::SEARCHING_TARGET;
			itMissionState.publish(state);
		}
		else if (state == MissionState::SEARCHING_TARGET)
		{
			// activate ocject detection; since no position without star tracker, change pose control mode to attitude control
			bool otReceived = false;
			objectTrackerReceived.put(otReceived);
			itPoseControllerMode.publishConst(PoseControllerMode::CHANGE_ATTITUDE); // may be removed when radio works
			raspiComm.sendCommand(OT, true);
			while(otReceived == false)
			{
				suspendUntilNextBeat();
				objectTrackerReceived.get(otReceived);
			}

			OTData otData;
			otDataBuffer.get(otData);
			if (otData.found) // if data received and object was found, calculate relative position of object
			{
				filteredPoseBuffer.get(currentPose);
				float rx = otData.g0 + 0.2;
				float r = sqrt(rx*rx + otData.G0*otData.G0);
				otYaw = M_PI + otData.alpha + atan2(otData.G0, rx);
				angle = otData.alpha + M_PI - otYaw + currentPose.yaw*M_PI/180;
				float otX = r * cos(angle);
				float otY = r * sin(angle);
				MOD(otYaw, -M_PI, M_PI);
				// save coordinates of target, and a point 40 centimeters before target
				targetPose = {currentPose.x + otX, currentPose.y + otY, (float)(angle*180.0/M_PI)};
				preTargetDistance = r - 0.4;
				preTargetPose = {currentPose.x + (r-0.4) * cos(angle), currentPose.y + (r-0.4) * sin(angle), (float)(angle*180.0/M_PI)};
				MOD(targetPose.yaw, -180, 180);

				// TEMPORARY: GO TO STANDBY, then manually dock
				/*bool stReceived = false;
				starTrackerReceived.put(stReceived);
				raspiComm.sendCommand(ST, true);
				while(stReceived == false)
				{
					suspendUntilNextBeat();
					starTrackerReceived.get(stReceived);
				}
				suspendCallerUntil(NOW() + 50*MILLISECONDS);

				targetPoseSemaphore.enter();
				tcTargetPose.put(searchingPose);
				targetPoseSemaphore.leave();
				itPoseControllerMode.publishConst(PoseControllerMode::GOTO_POSE);
				state = MissionState::STANDBY;
				itMissionState.publish(state);
				continue;*/

				// if not commanded to go to standby: move towards target
				missionStateBuffer.get(state);
				if (state == MissionState::STANDBY) continue;
				state = MissionState::MOVING_TO_TARGET;
				itMissionState.publish(state);
			}
			else // otherwise: acquire new attitude to search again
			{
				missionStateBuffer.get(state);
				if (state == MissionState::STANDBY) continue;
				state = MissionState::START_SEARCHING;
				itMissionState.publish(state);
			}
		}
		else if (state == MissionState::MOVING_TO_TARGET)
		{
			// activate star tracker for position control
			bool stReceived = false;
			starTrackerReceived.put(stReceived);
			raspiComm.sendCommand(ST, true);
			while(stReceived == false)
			{
				suspendUntilNextBeat();
				starTrackerReceived.get(stReceived);
			}
			suspendCallerUntil(NOW() + 50*MILLISECONDS);
			filteredPoseBuffer.get(currentPose);
			// move to pre-target position on line trajectory
			TrajectoryPlanData plan;
			plan.type = LINE;
			plan.lineData.startPose = {currentPose.x, currentPose.y, currentPose.yaw};
			plan.lineData.endPose = preTargetPose;
			plan.startTime = NOW() + 5*SECONDS;
			int64_t totalTime = 5*SECONDS + preTargetDistance*20*SECONDS;
			plan.endTime = NOW() + totalTime;
			trajPlanBuffer.put(plan);
			itPoseControllerMode.publishConst(PoseControllerMode::FOLLOW_TRAJECTORY_T);
			suspendCallerUntil(NOW() + totalTime + 5*SECONDS);
			missionStateBuffer.get(state);
			if (state == MissionState::STANDBY) continue;
			// state = MissionState::RENDEZVOUZ_TARGET; // next step: move towards docking port; wasn't testet
			state = MissionState::STANDBY; // hence: not going to other modes for now
			itMissionState.publish(state);
			/*targetPoseSemaphore.enter();
			tcTargetPose.put(searchingPose);
			targetPoseSemaphore.leave();
			itPoseControllerMode.publishConst(PoseControllerMode::GOTO_POSE);*/
			//print_debug_msg("T-Pose: %f, %f, %f", targetPose.x, targetPose.y, targetPose.yaw);
		}
		else if (state == MissionState::RENDEZVOUZ_TARGET)
		{
			// plan circle trajectory around target to get behind docking port of other vehicle
			TrajectoryPlanData plan;
			plan.type = CIRCLE;
			plan.circleData.centerPose = targetPose;
			plan.circleData.betaStart = 180 + angle;
			plan.circleData.betaEnd = 180 - otYaw*180/M_PI + currentPose.yaw;
			MOD(plan.circleData.betaStart, -180, 180);
			MOD(plan.circleData.betaEnd, -180, 180);
			float betaDif = plan.circleData.betaEnd - plan.circleData.betaStart;
			plan.circleData.r = 0.4;
			plan.startTime = NOW() + 1*SECONDS;
			int64_t totalTime = 1*SECONDS + ABS(betaDif)*M_PI/180*0.4*20*SECONDS;
			plan.endTime = NOW() + totalTime;
			trajPlanBuffer.put(plan);
			suspendCallerUntil(NOW() + totalTime + 5*SECONDS);
			// if not commanded to go to standby: dock
			missionStateBuffer.get(state);
			if (state == MissionState::STANDBY) continue;
			state = MissionState::DOCKING;
			itMissionState.publish(state);
		}
		else if (state == MissionState::DOCKING)
		{
			// move on line towards docking port
			filteredPoseBuffer.get(currentPose);
			TrajectoryPlanData plan;
			plan.type = LINE;
			plan.lineData.startPose = {currentPose.x, currentPose.y, currentPose.yaw};
			targetPose.yaw = currentPose.yaw;
			plan.lineData.endPose = targetPose;
			plan.startTime = NOW() + 1*SECONDS;
			plan.endTime = NOW() + 4*SECONDS;
			trajPlanBuffer.put(plan);
		}

		suspendUntilNextBeat();
	}
}
