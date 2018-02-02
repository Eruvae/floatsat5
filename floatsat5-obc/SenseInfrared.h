/*
 * SenseInfrared.h
 *
 *  Created on: 19.11.2017
 *      Author: TobiZ
 */

#ifndef SENSEINFRARED_H_
#define SENSEINFRARED_H_

#include "rodos.h"
#include "hwlib/IRSensorArray.h"

class SenseInfrared : public Thread
{
	IRSensorArray sensors;

public:
	SenseInfrared(HAL_I2C &i2c, GPIO_PIN enable_pin1 = GPIO_005, GPIO_PIN enable_pin2 = GPIO_003, float sensorDistance = 100.0f);

	void init();
	void run();
};

#endif /* SENSEINFRARED_H_ */
