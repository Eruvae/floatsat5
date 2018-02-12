/*
 * Telemetry.cpp
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#include "TelemetrySender.h"
#include "RaspiComm.h"

TelemetrySender telemetrySender;

TelemetrySender::TelemetrySender() : powerDataSub(itPowerData, powerDataBuffer),
		filteredPoseSub(itFilteredPose, filteredPoseBuffer),
		imuDataSub(itImuData, imuDataBuffer),
		reactionWheelSpeedSub(itReactionWheelSpeed, reactionWheelSpeedBuffer),
		infraredDataSub(itInfraredData, infraredDataBuffer),
		actuatorDataSub(itActuatorData, actuatorDataBuffer),
		starTrackerPoseSub(itStarTrackerPose, starTrackerPoseBuffer),
		objectTrackingPoseSub(itObjectTrackingPose, objectTrackingPoseBuffer),
		radioPositionSub(itRadioPosition, radioPositionBuffer),
		thrusterControlsSub(itThrusterControls, thrusterControlsBuffer),
		poseControllerModeSub(itPoseControllerMode, poseControllerModeBuffer),
		raspiStatusSub(itRaspiStatus, raspiStatusBuffer),
		missionStateSub(itMissionState, missionStateBuffer)
{
}

void TelemetrySender::run()
{
	//int counter = 0;

	const uint32_t tm_pause_period = 10;
	setPeriodicBeat(20*MILLISECONDS, 200*MILLISECONDS);
	while(1)
	{
		PowerData powerData;
		Pose filteredPose;
		IMUData imuData;
		int16_t reactionWheelSpeed;
		IRData infraredData;
		ActuatorData actuatorStatus;
		Pose2D starTrackerPose;
		OTData objectTrackingPose;
		RadioPosition radioPosition;
		ThrusterControls thrusterControls;
		PoseControllerMode poseControllerMode;
		RaspiStatus raspiStatus;
		MissionState missionState;

		// get all data from internal topics
		powerDataBuffer.get(powerData);
		filteredPoseBuffer.get(filteredPose);
		imuDataBuffer.get(imuData);
		reactionWheelSpeedBuffer.get(reactionWheelSpeed);
		infraredDataBuffer.get(infraredData);
		actuatorDataBuffer.get(actuatorStatus);
		starTrackerPoseBuffer.get(starTrackerPose);
		objectTrackingPoseBuffer.get(objectTrackingPose);
		radioPositionBuffer.get(radioPosition);
		thrusterControlsBuffer.get(thrusterControls);
		poseControllerModeBuffer.get(poseControllerMode);
		raspiStatusBuffer.get(raspiStatus);
		missionStateBuffer.get(missionState);


		//PRINTF("TM rw speed: %d\n", reactionWheelSpeed);

		// publish them with break (necessary to avoid data loss) to TM topics; gateway takes care of forwarding
		tmPowerData.publish(powerData);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmFilteredPose.publish(filteredPose);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmImuData.publish(imuData);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmReactionWheelSpeed.publish(reactionWheelSpeed);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmInfraredData.publish(infraredData);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmActuatorData.publish(actuatorStatus);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmStarTrackerPose.publish(starTrackerPose);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmObjectTrackingPose.publish(objectTrackingPose);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmRadioPosition.publish(radioPosition);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmThrusterControls.publish(thrusterControls);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmPoseControllerMode.publishConst((int)poseControllerMode);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmRaspiStatus.publish(raspiStatus);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		tmMissionState.publishConst((int)missionState);
		suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);

		while(!debugMsgFifo.isEmpty())
		{
			DebugMessage msg;
			debugMsgFifo.get(msg);
			tmDebugMsg.publish(msg);
			suspendCallerUntil(NOW() + tm_pause_period*MILLISECONDS);
		}
		//counter++;
		suspendUntilNextBeat();
	}
}

