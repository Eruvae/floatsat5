/*
 * SenseInfrared.cpp
 *
 *  Created on: 19.11.2017
 *      Author: TobiZ
 */

#include "SenseInfrared.h"
#include "Topics.h"
#include "CommInterfaces.h"

SenseInfrared senseInfrared(i2c2_bus);

SenseInfrared::SenseInfrared(HAL_I2C &i2c, GPIO_PIN enable_pin1, GPIO_PIN enable_pin2, float sensorDistance)
	: sensors(i2c, enable_pin1, enable_pin2, sensorDistance)
{}

void SenseInfrared::init()
{
	sensors.init();
}

void SenseInfrared::run()
{
	setPeriodicBeat(4*MILLISECONDS, 100*MILLISECONDS);

	sensors.initializeSensors();

	while(1)
	{
		//uint8_t test_regw[] = {0x00, 0x00};
		//uint8_t data;
		//int suc = i2c_bus.writeRead(INFRARED_I2C_ADDR, test_regw, 2, &data, 1);

		IRData data;

		/*uint8_t rangeStatus;
		rangeStatus = sensors.readRangeStatus(INFRARED1_I2C_ADDR);
		print_debug_msg("Range 1 Status %x\n", rangeStatus);
		rangeStatus = sensors.readRangeStatus(INFRARED2_I2C_ADDR);
		if (rangeStatus != 0) print_debug_msg("Range 2 Status %x\n", rangeStatus);*/

		//print_debug_msg("IR Status: %d, %d\n", infrared1_status, infrared2_status);

		data.range1 = sensors.readRange(INFRARED1_I2C_ADDR);
		data.range2 = sensors.readRange(INFRARED2_I2C_ADDR);

		float distance, angle;
		sensors.getRangeAndAngle(data.range1, data.range2, distance, angle);

		data.distance = distance;
		data.angle = angle;

		itInfraredData.publish(data);

		suspendUntilNextBeat();
	}
}
