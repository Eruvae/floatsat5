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

extern Topic<IMUData> imuTopic;
extern CommBuffer<IMUData> imuBuffer;

//extern Topic<bool> wifiStatus;

// Thread commands
extern Topic<IMUCommand> imuCommand;

// Telemetry test topics
extern Topic<Telemetry1> telemetry1;
extern Topic<Telemetry2> telemetry2;
extern Topic<Telecommand> telecommand;

#endif /* TOPICS_H_ */
