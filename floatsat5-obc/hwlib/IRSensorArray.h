/*
 * IRSensorArray.h
 *
 *  Created on: 30.01.2018
 *      Author: TobiZ
 */

#ifndef HWLIB_IRSENSORARRAY_H_
#define HWLIB_IRSENSORARRAY_H_

#include <rodos.h>

#define INFRARED1_I2C_ADDR 0x29
#define INFRARED2_I2C_ADDR 0x30

class IRSensorArray
{
	HAL_I2C &infrared_i2c;
	HAL_GPIO infrared_enable;
	HAL_GPIO infrared2_enable;

	const float sensorDistance;

	int infrared1_status;
	int infrared2_status;

	void writeReg(uint8_t id, uint16_t reg, uint8_t val);
	uint8_t readReg(uint8_t id, uint16_t reg, uint8_t val);

	int assignI2Caddress(uint8_t old_id, uint8_t new_id);
	void reset_i2c();
	int initInfrared(uint8_t id);

public:
	IRSensorArray(HAL_I2C &i2c, GPIO_PIN enable_pin1 = GPIO_005, GPIO_PIN enable_pin2 = GPIO_003, float sensorDistance = 100.0f);

	void initializeSensors();
	uint8_t readRange(uint8_t id);
	uint8_t readRangeStatus(uint8_t id);
	void getRangeAndAngle(uint8_t range1, uint8_t range2, float &range, float &angle);
	void recalibrate(uint8_t id);

	void init();
};

#endif /* HWLIB_IRSENSORARRAY_H_ */
