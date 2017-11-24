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
public:
	ActuatorInterfaces();

	void init();

	void run();
};

#endif /* ACTUATORINTERFACES_H_ */
