/*
 * Telemetry.h
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#ifndef TELEMETRYSENDER_H_
#define TELEMETRYSENDER_H_

#include "rodos.h"

class TelemetrySender : public Thread
{
public:
	TelemetrySender();
	void run();
};

#endif /* TELEMETRYSENDER_H_ */
