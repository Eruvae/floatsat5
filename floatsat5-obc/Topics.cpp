/*
 * Topics.cpp
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#include "Topics.h"

// define Topics, CommBuffers/Fifos and Subscribers here

Topic<Telemetry1> telemetry1(5661,"Message1");
Topic<Telemetry2> telemetry2(5771,"Message2");
Topic<Telecommand> telecommand(300,"Message2");

Topic<IMUData> imuTopic(-1, "IMU raw data"); // might need to set fixed ID later
CommBuffer<IMUData> imuBuffer;
Subscriber imuSubscriber(imuTopic, imuBuffer);
