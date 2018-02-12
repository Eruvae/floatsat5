/*
 * IRSensorArray.cpp
 *
 *  Created on: 30.01.2018
 *      Author: TobiZ
 */

#include "IRSensorArray.h"
#include <cmath>

#define SYSRANGE_START 0x18

#define INFRARED_PRINT_VERBOSITY 75

IRSensorArray::IRSensorArray(HAL_I2C &i2c, GPIO_PIN enable_pin1, GPIO_PIN enable_pin2, float sensorDistance) :
		infrared_i2c(i2c),
		infrared_enable(enable_pin1),
		infrared2_enable(enable_pin2),
		infrared1_status(-1),
		infrared2_status(-1),
		sensorDistance(sensorDistance)
{}

void IRSensorArray::writeReg(uint8_t id, uint16_t reg, uint8_t val)
{
	uint8_t toWrite[] = {(uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF), val};
	infrared_i2c.write(id, toWrite, sizeof(toWrite));
}

uint8_t IRSensorArray::readReg(uint8_t id, uint16_t reg, uint8_t val)
{
	uint8_t toWrite[] = {(uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF)};
	uint8_t retVal;
	infrared_i2c.writeRead(id, toWrite, sizeof(toWrite), &retVal, 1);
	return retVal;
}

void IRSensorArray::init()
{
	infrared_enable.init(true, 1, 0);
	infrared2_enable.init(true, 1, 0);
}

void IRSensorArray::reset_i2c()
{
	infrared_i2c.reset();
	AT(NOW() + 0.5*MILLISECONDS);
	infrared_i2c.init(400000);
}

int IRSensorArray::initInfrared(uint8_t id)
{
	// required initializations
	writeReg(id, 0x0207, 0x01);
	writeReg(id, 0x0208, 0x01);
	writeReg(id, 0x0096, 0x00);
	writeReg(id, 0x0097, 0xfd);
	writeReg(id, 0x00e3, 0x00);
	writeReg(id, 0x00e4, 0x04);
	writeReg(id, 0x00e5, 0x02);
	writeReg(id, 0x00e6, 0x01);
	writeReg(id, 0x00e7, 0x03);
	writeReg(id, 0x00f5, 0x02);
	writeReg(id, 0x00d9, 0x05);
	writeReg(id, 0x00db, 0xce);
	writeReg(id, 0x00dc, 0x03);
	writeReg(id, 0x00dd, 0xf8);
	writeReg(id, 0x009f, 0x00);
	writeReg(id, 0x00a3, 0x3c);
	writeReg(id, 0x00b7, 0x00);
	writeReg(id, 0x00bb, 0x3c);
	writeReg(id, 0x00b2, 0x09);
	writeReg(id, 0x00ca, 0x09);
	writeReg(id, 0x0198, 0x01);
	writeReg(id, 0x01b0, 0x17);
	writeReg(id, 0x01ad, 0x00);
	writeReg(id, 0x00ff, 0x05);
	writeReg(id, 0x0100, 0x05);
	writeReg(id, 0x0199, 0x05);
	writeReg(id, 0x01a6, 0x1b);
	writeReg(id, 0x01ac, 0x3e);
	writeReg(id, 0x01a7, 0x1f);
	writeReg(id, 0x0030, 0x00);

	uint8_t config_sysrange[] = {0x00, SYSRANGE_START, 0x03, 0xFF, 0x00, 0x09};
	// continuous mode, 255mm high threshold, 0mm low threshold, 100ms between measurements
	int res = infrared_i2c.write(id, config_sysrange, sizeof(config_sysrange));
	if(res < 0)
		reset_i2c();

	return res;
}

int IRSensorArray::assignI2Caddress(uint8_t old_id, uint8_t new_id)
{
	uint8_t write_reg[] = {0x02, 0x12, new_id};
	int res = infrared_i2c.write(old_id, write_reg, 3);
	if(res < 0)
		reset_i2c();

	return res;
}

uint8_t IRSensorArray::readRange(uint8_t id)
{
	uint8_t read_reg[] = {0x00, 0x62};
	uint8_t ret;
	int readRes = infrared_i2c.writeRead(id, read_reg, 2, &ret, 1);
	if (readRes < 0)
	{
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "I2C Sensor %d read failed!\n", readRes);
		reset_i2c();
		initializeSensors();
	}
	return ret;
}

uint8_t IRSensorArray::readRangeStatus(uint8_t id)
{
	uint8_t read_reg[] = {0x00, 0x4D};
	uint8_t ret;
	int readRes = infrared_i2c.writeRead(id, read_reg, 2, &ret, 1);
	/*if (readRes < 0)
	{
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "I2C Sensor %d read status failed!\n", readRes);
		reset_i2c();
		initializeSensors();
	}*/
	return ret;
}

void IRSensorArray::recalibrate(uint8_t id)
{
	uint8_t write_reg[] = {0x00, 0x2E, 0x01};
	infrared_i2c.write(id, write_reg, 3);
}

void IRSensorArray::getRangeAndAngle(uint8_t range1, uint8_t range2, float &range, float &angle)
{
	range = ((float)range1 + (float)range2) / 2.0f;
	angle = atan(((float)range1 - (float)range2) / sensorDistance);
}

void IRSensorArray::initializeSensors()
{
	// disable both sensors first
	infrared_enable.setPins(0);
	infrared2_enable.setPins(0);
	infrared1_status = -1;
	infrared2_status = -1;

	Thread::suspendCallerUntil(NOW() + 10*MILLISECONDS);

	// enable sensor 2
	infrared2_enable.setPins(1);
	Thread::suspendCallerUntil(NOW() + 10*MILLISECONDS);
	int res = -1;

	// change I2C address of sensor 2
	res = assignI2Caddress(INFRARED1_I2C_ADDR, INFRARED2_I2C_ADDR);
	Thread::suspendCallerUntil(NOW() + 10*MILLISECONDS);

	if (res < 0)
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Changing Infrared I2C address failed: %d\n", res);
	else
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Changing Infrared I2C address successful\n", res);

	// initialize sensor 2
	res = -1;
	for(int i = 0; res < 0 && i < 10; i++)
	{
		res = initInfrared(INFRARED2_I2C_ADDR);
		Thread::suspendCallerUntil(NOW() + 10*MILLISECONDS);
	}

	if (res < 0)
	{
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Initializing Infrared 2 failed: %d\n", res);
		return;
	}
	else
	{
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Initializing Infrared 2 successful\n", res);
		infrared2_status = 0;
	}

	// enable sensor 1
	infrared_enable.setPins(1);
	Thread::suspendCallerUntil(NOW() + 10*MILLISECONDS);

	// initialize sensor 1
	res = -1;
	for(int i = 0; res < 0 && i < 10; i++)
	{
		res = initInfrared(INFRARED1_I2C_ADDR);
		Thread::suspendCallerUntil(NOW() + 10*MILLISECONDS);
	}

	if (res < 0)
	{
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Initializing Infrared 1 failed: %d\n", res);
		return;
	}
	else
	{
		PRINTF_CONDITIONAL(INFRARED_PRINT_VERBOSITY, "Initializing Infrared 1 successful\n", res);
		infrared1_status = 0;
	}

}
