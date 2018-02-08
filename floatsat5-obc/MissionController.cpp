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
		if (state == MissionState::START_SEARCHING)
		{
			bool stReceived = false;
			starTrackerReceived.put(stReceived);
			raspiComm.sendCommand(ST, true);
			while(stReceived == false)
			{
				suspendUntilNextBeat();
				starTrackerReceived.get(stReceived);
			}
			suspendCallerUntil(NOW() + 50*MILLISECONDS);
			if(!searchingStarted)
			{
				filteredPoseBuffer.get(currentPose);
				searchingPose = {currentPose.x, currentPose.y, currentPose.yaw};
				searchingStarted = true;
			}
			else
				searchingPose.yaw += 20;

			targetPoseSemaphore.enter();
			tcTargetPose.put(searchingPose);
			targetPoseSemaphore.leave();
			itPoseControllerMode.publishConst(PoseControllerMode::GOTO_POSE);
			suspendCallerUntil(NOW() + 5*SECONDS);
			missionStateBuffer.get(state);
			if (state == MissionState::STANDBY) continue;
			state = MissionState::SEARCHING_TARGET;
			itMissionState.publish(state);
		}
		else if (state == MissionState::SEARCHING_TARGET)
		{
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
			if (otData.found)
			{
				filteredPoseBuffer.get(currentPose);
				float rx = otData.g0 + 0.2;
				float r = sqrt(rx*rx + otData.G0*otData.G0);
				otYaw = M_PI + otData.alpha + atan2(otData.G0, rx);
				angle = otData.alpha + M_PI - otYaw + currentPose.yaw*M_PI/180;
				float otX = r * cos(angle);
				float otY = r * sin(angle);
				MOD(otYaw, -M_PI, M_PI);
				targetPose = {currentPose.x + otX, currentPose.y + otY, (float)(angle*180.0/M_PI)};
				preTargetDistance = r - 0.2;
				preTargetPose = {currentPose.x + (r-0.2) * cos(angle), currentPose.y + (r-0.2) * sin(angle), (float)(angle*180.0/M_PI)};
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

				missionStateBuffer.get(state);
				if (state == MissionState::STANDBY) continue;
				state = MissionState::MOVING_TO_TARGET;
				itMissionState.publish(state);
			}
			else
			{
				missionStateBuffer.get(state);
				if (state == MissionState::STANDBY) continue;
				state = MissionState::START_SEARCHING;
				itMissionState.publish(state);
			}
		}
		else if (state == MissionState::MOVING_TO_TARGET)
		{
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
			// state = MissionState::RENDEZVOUZ_TARGET;
			state = MissionState::STANDBY; // not going to other modes
			itMissionState.publish(state);
			/*targetPoseSemaphore.enter();
			tcTargetPose.put(searchingPose);
			targetPoseSemaphore.leave();
			itPoseControllerMode.publishConst(PoseControllerMode::GOTO_POSE);*/
			//print_debug_msg("T-Pose: %f, %f, %f", targetPose.x, targetPose.y, targetPose.yaw);
		}
		else if (state == MissionState::RENDEZVOUZ_TARGET)
		{
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
			missionStateBuffer.get(state);
			if (state == MissionState::STANDBY) continue;
			state = MissionState::RENDEZVOUZ_TARGET;
			itMissionState.publish(state);
		}
		else if (state == MissionState::DOCKING)
		{
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
