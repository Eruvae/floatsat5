/*
 * SenseInfrared.h
 *
 *  Created on: 19.11.2017
 *      Author: TobiZ
 */

#ifndef SENSEINFRARED_H_
#define SENSEINFRARED_H_

#include "rodos.h"

#define INFRARED_PRINT_VERBOSITY 75

class SenseInfrared : public Thread
{
	HAL_I2C &infrared_i2c;
	HAL_GPIO infrared_enable; // PA5, has to be changed if ADC is used
	HAL_GPIO infrared2_enable;

	int infrared1_status;
	int infrared2_status;

	void writeReg(uint8_t id, uint16_t reg, uint8_t val);
	uint8_t readReg(uint8_t id, uint16_t reg, uint8_t val);

	void reset_i2c();
	void initializeSensors();
	int initInfrared(uint8_t id);
	int assignI2Caddress(uint8_t old_id, uint8_t new_id);
	uint8_t readRange(uint8_t id);
	uint8_t readRangeStatus(uint8_t id);
	void recalibrate(uint8_t id);
public:
	SenseInfrared(HAL_I2C &i2c, GPIO_PIN enable_pin1 = GPIO_005, GPIO_PIN enable_pin2 = GPIO_003);

	void init();
	void run();
};

#endif /* SENSEINFRARED_H_ */
