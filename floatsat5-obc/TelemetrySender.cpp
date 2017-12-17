/*
 * Telemetry.cpp
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#include "TelemetrySender.h"

TelemetrySender telemetrySender;

TelemetrySender::TelemetrySender() : powerDataSub(itPowerData, powerDataBuffer),
		filteredPoseSub(itFilteredPose, filteredPoseBuffer),
		imuDataSub(itImuData, imuDataBuffer),
		reactionWheelSpeedSub(itReactionWheelSpeed, reactionWheelSpeedBuffer),
		infraredDataSub(itInfraredData, infraredDataBuffer)
{
}

void TelemetrySender::run()
{
	//int counter = 0;

	setPeriodicBeat(20*MILLISECONDS, 200*MILLISECONDS);
	while(1)
	{
		PowerData powerData;
		Pose filteredPose;
		IMUData imuData;
		int16_t reactionWheelSpeed;
		IRData infraredData;

		powerDataBuffer.get(powerData);
		filteredPoseBuffer.get(filteredPose);
		imuDataBuffer.get(imuData);
		reactionWheelSpeedBuffer.get(reactionWheelSpeed);
		infraredDataBuffer.get(infraredData);

		//PRINTF("TM rw speed: %d\n", reactionWheelSpeed);

		tmPowerData.publish(powerData);
		suspendCallerUntil(NOW() + 10*MILLISECONDS);
		tmFilteredPose.publish(filteredPose);
		suspendCallerUntil(NOW() + 10*MILLISECONDS);
		tmImuData.publish(imuData);
		suspendCallerUntil(NOW() + 10*MILLISECONDS);
		tmReactionWheelSpeed.publish(reactionWheelSpeed);
		suspendCallerUntil(NOW() + 10*MILLISECONDS);
		tmInfraredData.publish(infraredData);

		//counter++;
		suspendUntilNextBeat();
	}
}

