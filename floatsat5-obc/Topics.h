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
extern Topic<RadioPosition> itRadioPosition;
extern Topic<ThrusterControls> itThrusterControls;
extern Topic<PoseControllerMode> itPoseControllerMode;
extern Topic<RaspiStatus> itRaspiStatus;
extern Topic<MissionState> itMissionState;

// TM topics, should only be published in TM Sender thread
extern Topic<PowerData> tmPowerData;
extern Topic<Pose> tmFilteredPose;
extern Topic<IMUData> tmImuData;
extern Topic<int16_t> tmReactionWheelSpeed;
extern Topic<IRData> tmInfraredData;
extern Topic<ActuatorData> tmActuatorData;
extern Topic<Pose2D> tmStarTrackerPose;
extern Topic<OTData> tmObjectTrackingPose;
extern Topic<RadioPosition> tmRadioPosition;
extern Topic<ThrusterControls> tmThrusterControls;
extern Topic<PoseControllerMode> tmPoseControllerMode;
extern Topic<RaspiStatus> tmRaspiStatus;
extern Topic<MissionState> tmMissionState;

extern Fifo<DebugMessage, 50> debugMsgFifo;
extern Topic<DebugMessage> tmDebugMsg;

int print_debug_msg(const char *format, ...);

// TC CommBuffers
extern CommBuffer<int16_t> tcReactionWheelTargetSpeed;
extern Fifo<IMUCommand, 10> tcImuCommand;
extern CommBuffer<TrajectoryPlanData> trajPlanBuffer;
extern Semaphore targetPoseSemaphore;
extern CommBuffer<Pose2D> tcTargetPose;
extern Fifo<Pose2D, 20> tcNextTargetPoseList;
//extern CommBuffer<bool> tcActivateController;
extern CommBuffer<ControlParameters> tcControlParams;
extern CommBuffer<float> desiredRotationSpeed;
extern CommBuffer<bool> activateRWSpeedController;
extern CommBuffer<bool> activateValvePWM;
extern CommBuffer<bool> starTrackerReceived;
extern CommBuffer<bool> objectTrackerReceived;

#endif /* TOPICS_H_ */
