/*
 * Topics.cpp
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#include "Topics.h"

// define Topics and CommBuffers/Fifos here

Topic<Telecommand> telecommand(100,"Message2");

// Topics for internal communication
Topic<PowerData> itPowerData(3000, "Power Data IT");
Topic<Pose> itFilteredPose(3001, "Filtered Pose IT");
Topic<IMUData> itImuData(3002, "IMU raw data IT");
Topic<int16_t> itReactionWheelSpeed(3003, "Reaction Wheel Speed IT");
Topic<IRData> itInfraredData(3004, "Infrared Data IT");

// TM topics, should only be published in TM Sender thread
Topic<PowerData> tmPowerData(5000, "Power Data TM");
Topic<Pose> tmFilteredPose(5001, "Filtered Pose TM");
Topic<IMUData> tmImuData(5002, "IMU raw data TM");
Topic<int16_t> tmReactionWheelSpeed(5003, "Reaction Wheel Speed TM");
Topic<IRData> tmInfraredData(5004, "Infrared Data TM");

// TC CommBuffers
CommBuffer<int16_t> tcReactionWheelTargetSpeed;
CommBuffer<IMUCommand> tcImuCommand;
