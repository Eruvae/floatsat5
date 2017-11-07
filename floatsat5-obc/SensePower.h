/*
 * SensePower.h
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#ifndef SENSEPOWER_H_
#define SENSEPOWER_H_

#include "rodos.h"

class SensePower : public Thread
{
public:
	SensePower();

	void run();
};

#endif /* SENSEPOWER_H_ */
