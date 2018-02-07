/*
 * Telemetry.h
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#ifndef TELEMETRYSENDER_H_
#define TELEMETRYSENDER_H_

#include "rodos.h"
#include "Topics.h"

class TelemetrySender : public Thread
{
private:
	CommBuffer<PowerData> powerDataBuffer;
	CommBuffer<Pose> filteredPoseBuffer;
	CommBuffer<IMUData> imuDataBuffer;
	CommBuffer<int16_t> reactionWheelSpeedBuffer;
	CommBuffer<IRData> infraredDataBuffer;
	CommBuffer<ActuatorData> actuatorDataBuffer;
	CommBuffer<Pose2D> starTrackerPoseBuffer;
	CommBuffer<OTData> objectTrackingPoseBuffer;
	CommBuffer<RadioPosition> radioPositionBuffer;
	CommBuffer<ThrusterControls> thrusterControlsBuffer;
	CommBuffer<PoseControllerMode> poseControllerModeBuffer;
	CommBuffer<RaspiStatus> raspiStatusBuffer;
	CommBuffer<MissionState> missionStateBuffer;

	Subscriber powerDataSub;
	Subscriber filteredPoseSub;
	Subscriber imuDataSub;
	Subscriber reactionWheelSpeedSub;
	Subscriber infraredDataSub;
	Subscriber actuatorDataSub;
	Subscriber starTrackerPoseSub;
	Subscriber objectTrackingPoseSub;
	Subscriber radioPositionSub;
	Subscriber thrusterControlsSub;
	Subscriber poseControllerModeSub;
	Subscriber raspiStatusSub;
	Subscriber missionStateSub;
public:
	TelemetrySender();
	void run();
};

#endif /* TELEMETRYSENDER_H_ */
