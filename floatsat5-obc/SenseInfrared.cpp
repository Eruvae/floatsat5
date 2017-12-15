/*
 * SenseInfrared.cpp
 *
 *  Created on: 19.11.2017
 *      Author: TobiZ
 */

#include "SenseInfrared.h"
#include "Topics.h"
#include <cmath>

#define INFRARED1_I2C_ADDR 0x29
#define INFRARED2_I2C_ADDR 0x30

#define SYSRANGE_START 0x18

HAL_GPIO infrared_enable(GPIO_005); // PA5, has to be changed if ADC is used
HAL_GPIO infrared2_enable(GPIO_003);

SenseInfrared senseInfrared;

SenseInfrared::SenseInfrared()
{
	// TODO Auto-generated constructor stub

}

void SenseInfrared::init()
{
	infrared_enable.init(true, 1, 0);
	infrared2_enable.init(true, 1, 0);
}

int SenseInfrared::initInfrared(uint8_t id)
{
	uint8_t config_sysrange[] = {0x00, SYSRANGE_START, 0x03, 0xFF, 0x00, 0x09};
	// continuous mode, 255mm high threshold, 0mm low threshold, 100ms between measurements
	int res = i2c2_bus.write(id, config_sysrange, sizeof(config_sysrange));
	if(res < 0)
		comm.reset_i2c(i2c2_bus);

	return res;
}

int SenseInfrared::assignI2Caddress(uint8_t old_id, uint8_t new_id)
{
	uint8_t write_reg[] = {0x02, 0x12, new_id};
	int res = i2c2_bus.write(old_id, write_reg, 3);
	if(res < 0)
		comm.reset_i2c(i2c2_bus);

	return res;
}

uint8_t SenseInfrared::readRange(uint8_t id)
{
	uint8_t read_reg[] = {0x00, 0x62};
	uint8_t ret;
	int readRes = i2c2_bus.writeRead(id, read_reg, 2, &ret, 1);
	//PRINTF("I2C read result: %x, %d\n", id, readRes);
	if (readRes < 0)
	{
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "I2C Sensor %d read failed!\n", readRes);
		comm.reset_i2c(i2c2_bus);
		initializeSensors();
	}
	return ret;
}

void SenseInfrared::initializeSensors()
{
	infrared_enable.setPins(0);
	infrared2_enable.setPins(0);

	suspendCallerUntil(NOW() + 1*MILLISECONDS);

	infrared2_enable.setPins(1);
	suspendCallerUntil(NOW() + 1*MILLISECONDS);
	int res = -1;
	for(int i = 0; res < 0 && i < 10; i++)
	{
		res = assignI2Caddress(INFRARED1_I2C_ADDR, INFRARED2_I2C_ADDR);
		suspendCallerUntil(NOW() + 10*MILLISECONDS);
	}

	if (res < 0)
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Changing Infrared I2C address failed: %d\n", res);
	else
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Changing Infrared I2C address successful\n", res);

	res = -1;
	for(int i = 0; res < 0 && i < 10; i++)
	{
		res = initInfrared(INFRARED2_I2C_ADDR);
		suspendCallerUntil(NOW() + 10*MILLISECONDS);
	}

	if (res < 0)
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Initializing Infrared 2 failed: %d\n", res);
	else
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Initializing Infrared 2 successful\n", res);

	infrared_enable.setPins(1);
	suspendCallerUntil(NOW() + 1*MILLISECONDS);
	res = -1;
	for(int i = 0; res < 0 && i < 10; i++)
	{
		res = initInfrared(INFRARED1_I2C_ADDR);
		suspendCallerUntil(NOW() + 10*MILLISECONDS);
	}

	if (res < 0)
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Initializing Infrared 1 failed: %d\n", res);
	else
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Initializing Infrared 1 successful\n", res);

}

void SenseInfrared::run()
{
	setPeriodicBeat(0, 100*MILLISECONDS);

	initializeSensors();

	const float D = 100.f;

	while(1)
	{
		//uint8_t test_regw[] = {0x00, 0x00};
		//uint8_t data;
		//int suc = i2c_bus.writeRead(INFRARED_I2C_ADDR, test_regw, 2, &data, 1);

		IRData data;

		data.range1 = readRange(INFRARED1_I2C_ADDR);
		data.range2 = readRange(INFRARED2_I2C_ADDR);

		data.distance = ((double)data.range1 + data.range2) / 2.0f;
		data.angle = atan((double)data.range1 - data.range2 / D);

		infraredData.publish(data);

		//PRINTF("Test read IR: %d, %d\n", range1, range2);
		suspendUntilNextBeat();
	}
}
