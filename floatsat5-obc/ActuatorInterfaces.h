/*
 * ActuatorInterfaces.h
 *
 *  Created on: 23.11.2017
 *      Author: TobiZ
 */

#ifndef ACTUATORINTERFACES_H_
#define ACTUATORINTERFACES_H_

#include "rodos.h"

class ActuatorInterfaces : public Thread
{
private:
	CommBuffer<int16_t> reactionWheelSpeedBuffer;
	Subscriber reactionWheelSpeedSub;
public:
	ActuatorInterfaces();

	void init();

	void setWheelDirection(bool forward);

	void run();
};

#endif /* ACTUATORINTERFACES_H_ */
