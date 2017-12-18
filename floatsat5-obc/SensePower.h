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
	void initSensor(uint8_t sensorAddr, uint16_t calib);
	int16_t readReg(uint8_t sensorAddr, uint8_t reg);
public:
	SensePower();

	void run();
};

#endif /* SENSEPOWER_H_ */
