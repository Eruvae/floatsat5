/*
 * Topics.cpp
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#include "Topics.h"

// define Topics, CommBuffers/Fifos and Subscribers here

Topic<IMUData> imuTopic(-1, "IMU raw data"); // might need to set fixed ID later
CommBuffer<IMUData> imuBuffer;
Subscriber imuSubscriber(imuTopic, imuBuffer);
