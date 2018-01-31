/*
 * Topics.h
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#ifndef TOPICS_H_
#define TOPICS_H_

#include "rodos.h"
#include "Structs.h"

// declare Topics and CommBuffers/Fifos as extern, define in Topics.cpp

extern Topic<Telecommand> telecommand;

// Topics for internal communication
extern Topic<PowerData> itPowerData;
extern Topic<Pose> itFilteredPose;
extern Topic<IMUData> itImuData;
extern Topic<int16_t> itReactionWheelSpeed;
extern Topic<IRData> itInfraredData;
extern Topic<ActuatorData> itActuatorData;

extern Topic<Pose2D> itStarTrackerPose;
extern Topic<OTData> itObjectTrackingPose;
extern Topic<Position2D> itRadioPosition;

extern Topic<ThrusterControls> itThrusterControls;
extern Topic<PoseControllerMode> itPoseControllerMode;

// TM topics, should only be published in TM Sender thread
extern Topic<PowerData> tmPowerData;
extern Topic<Pose> tmFilteredPose;
extern Topic<IMUData> tmImuData;
extern Topic<int16_t> tmReactionWheelSpeed;
extern Topic<IRData> tmInfraredData;
extern Topic<ActuatorData> tmActuatorData;

extern Fifo<DebugMessage, 50> debugMsgFifo;
extern Topic<DebugMessage> tmDebugMsg;

int print_debug_msg(const char *format, ...);

// TC CommBuffers
extern CommBuffer<int16_t> tcReactionWheelTargetSpeed;
extern Fifo<IMUCommand, 10> tcImuCommand;
extern CommBuffer<Pose> tcTargetPose;
extern CommBuffer<bool> tcActivateController;

#endif /* TOPICS_H_ */
