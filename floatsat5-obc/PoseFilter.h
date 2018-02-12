/*
 * PoseFilter.h
 *
 *  Created on: 04.12.2017
 *      Author: TobiZ
 */

#ifndef POSEFILTER_H_
#define POSEFILTER_H_

#include <rodos.h>
#include "Topics.h"

class PoseFilter : public Thread
{
private:
	CommBuffer<IMUData> imuDataBuffer;
	Subscriber imuDataSub;

	CommBuffer<Pose2D> starTrackerPoseBuffer;
	Subscriber starTrackerPoseSub;

	CommBuffer<RadioPosition> radioPositionBuffer;
	Subscriber radioPositionSub;

	CommBuffer<OTData> otDataBuffer;
	Subscriber otDataSub;

	CommBuffer<RaspiStatus> raspiStatusBuffer;
	Subscriber raspiStatusSub;
public:
	PoseFilter();

	void run();
};

#endif /* POSEFILTER_H_ */
