/*
 * SenseInfrared.h
 *
 *  Created on: 19.11.2017
 *      Author: TobiZ
 */

#ifndef SENSEINFRARED_H_
#define SENSEINFRARED_H_

#include "rodos.h"
#include "CommInterfaces.h"

#define INFRARED_PRINT_VERBOSITY 75

class SenseInfrared : public Thread
{
	void initializeSensors();
	int initInfrared(uint8_t id);
	int assignI2Caddress(uint8_t old_id, uint8_t new_id);
	uint8_t readRange(uint8_t id);
public:
	SenseInfrared();

	void init();
	void run();
};

#endif /* SENSEINFRARED_H_ */
