/*
 * Telemetry.h
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "rodos.h"

class Telemetry : public Thread
{
public:
	Telemetry();
	void run();
};

#endif /* TELEMETRY_H_ */
