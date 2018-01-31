/*
 * ValvePWM.h
 *
 *  Created on: 31.01.2018
 *      Author: TobiZ
 */

#ifndef VALVEPWM_H_
#define VALVEPWM_H_

#include <rodos.h>
#include "Topics.h"

class ValvePWM : public Thread
{
	CommBuffer<ThrusterControls> thrusterControlsBuffer;
	Subscriber thrusterControlsSubscriber;

	uint64_t period;

	void sendThrusters(int n1, int n2, float f1, float f2);
public:
	ValvePWM();

	void run();
};

#endif /* VALVEPWM_H_ */
