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

#endif /* TOPICS_H_ */
