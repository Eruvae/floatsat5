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
				float otYaw = M_PI + otData.alpha + atan2(otData.G0, rx);
				float angle = otData.alpha + M_PI - otYaw + currentPose.yaw*M_PI/180;
				float otX = r * cos(angle);
				float otY = r * sin(angle);
				MOD(otYaw, -M_PI, M_PI);
				targetPose = {currentPose.x + otX, currentPose.y + otY, (float)(angle*180.0/M_PI)};
				MOD(targetPose.yaw, -180, 180);
				missionStateBuffer.get(state);
				if (state == MissionState::STANDBY) continue;
				state = MissionState::START_MOVING;
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
		else if (state == MissionState::START_MOVING)
		{
			targetPoseSemaphore.enter();
			tcTargetPose.put(searchingPose);
			targetPoseSemaphore.leave();
			itPoseControllerMode.publishConst(PoseControllerMode::GOTO_POSE);
			print_debug_msg("T-Pose: %f, %f, %f", targetPose.x, targetPose.y, targetPose.yaw);
			raspiComm.sendCommand(ST, true);
		}

		suspendUntilNextBeat();
	}
}
