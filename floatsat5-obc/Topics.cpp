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
Topic<Telecommand> telecommand(100,"Message2");
Topic<IMUCommand> imuCommand(500, "IMU command");
Topic<Pose> filteredPose (1, "Filtered Pose");
Topic<PowerData> powerTelemetry(0, "Power Telemetry");
Topic<int16_t> reactionWheelSpeed(3, "Reaction Wheel Speed");
Topic<IRData> infraredData(4, "Infrared Data");
//Topic<int16_t> reactionWheelTargetSpeeed(6002, "Reaction Wheel Target Speed");
CommBuffer<int16_t> reactionWheelTargetSpeed;

Topic<IMUData> imuTopic(2, "IMU raw data"); // might need to set fixed ID later
CommBuffer<IMUData> imuBuffer;
Subscriber imuSubscriber(imuTopic, imuBuffer);
