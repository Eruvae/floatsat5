/*
 * Topics.cpp
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#include "Topics.h"
#include <cstdio>

// define Topics and CommBuffers/Fifos here

Topic<Telecommand> telecommand(100,"Message2");

// Topics for internal communication
Topic<PowerData> itPowerData(3000, "Power Data IT");
Topic<Pose> itFilteredPose(3001, "Filtered Pose IT");
Topic<IMUData> itImuData(3002, "IMU raw data IT");
Topic<int16_t> itReactionWheelSpeed(3003, "Reaction Wheel Speed IT");
Topic<IRData> itInfraredData(3004, "Infrared Data IT");
Topic<ActuatorData> itActuatorData(3005, "Valve Status IT");

Topic<Pose2D> itStarTrackerPose(3006, "Star Tracker Pose IT");
Topic<OTData> itObjectTrackingPose(3007, "Object Tracking Pose IT");
Topic<Position2D> itRadioPosition(3008, "Radio Position IT");

Topic<ThrusterControls> itThrusterControls(3009, "Thruster Controls IT");
Topic<PoseControllerMode> itPoseControllerMode(3010, "Pose Controller Mode IT");

// TM topics, should only be published in TM Sender thread
Topic<PowerData> tmPowerData(5000, "Power Data TM");
Topic<Pose> tmFilteredPose(5001, "Filtered Pose TM");
Topic<IMUData> tmImuData(5002, "IMU raw data TM");
Topic<int16_t> tmReactionWheelSpeed(5003, "Reaction Wheel Speed TM");
Topic<IRData> tmInfraredData(5004, "Infrared Data TM");
Topic<ActuatorData> tmActuatorData(5005, "Valve Status TM");

Topic<Pose2D> tmStarTrackerPose(5006, "Star Tracker Pose TM");
Topic<OTData> tmObjectTrackingPose(5007, "Object Tracking Pose TM");
Topic<Position2D> tmRadioPosition(5008, "Radio Position TM");

Topic<ThrusterControls> tmThrusterControls(5009, "Thruster Controls TM");
Topic<PoseControllerMode> tmPoseControllerMode(5010, "Pose Controller Mode TM");

Fifo<DebugMessage, 50> debugMsgFifo;
Topic<DebugMessage> tmDebugMsg(6000, "Debug Message TM");

int print_debug_msg(const char *format, ...)
{
	DebugMessage msg;
	va_list arg;
	int done;
	va_start (arg, format);
	done = vsprintf (msg.str, format, arg);
	va_end (arg);
	debugMsgFifo.put(msg);
	return done;
}

// TC CommBuffers
CommBuffer<int16_t> tcReactionWheelTargetSpeed;
Fifo<IMUCommand, 10> tcImuCommand;
CommBuffer<Pose> tcTargetPose;
//CommBuffer<bool> tcActivateController;
CommBuffer<ControlParameters> tcControlParams;
CommBuffer<float> desiredRotationSpeed;
CommBuffer<bool> activateValvePWM;
