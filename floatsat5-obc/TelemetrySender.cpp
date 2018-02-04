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
		poseControllerModeSub(itPoseControllerMode, poseControllerModeBuffer)
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
		Position2D radioPosition;
		ThrusterControls thrusterControls;
		PoseControllerMode poseControllerMode;

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


		//PRINTF("TM rw speed: %d\n", reactionWheelSpeed);

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
		tmPoseControllerMode.publish(poseControllerMode);
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

