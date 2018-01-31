/*
 * PoseFilter.h
 *
 *  Created on: 04.12.2017
 *      Author: TobiZ
 */

#ifndef POSEFILTER_H_
#define POSEFILTER_H_

#include <rodos.h>
#include "Matrix.h"
#include "Topics.h"

class PoseFilter : public Thread
{
private:
	CommBuffer<IMUData> imuDataBuffer;
	Subscriber imuDataSub;

	CommBuffer<Pose2D> starTrackerPoseBuffer;
	Subscriber starTrackerPoseSub;

	CommBuffer<Position2D> radioPositionBuffer;
	Subscriber radioPositionSub;
public:
	PoseFilter();

	void run();
};

#endif /* POSEFILTER_H_ */
