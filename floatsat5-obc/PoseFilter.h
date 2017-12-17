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
public:
	PoseFilter();

	void run();
};

#endif /* POSEFILTER_H_ */
