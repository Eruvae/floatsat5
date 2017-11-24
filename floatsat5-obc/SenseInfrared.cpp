/*
 * SenseInfrared.cpp
 *
 *  Created on: 19.11.2017
 *      Author: TobiZ
 */

#include "SenseInfrared.h"

#define INFRARED_I2C_ADDR 0x29

#define SYSRANGE_START 0x18

HAL_GPIO infrared_enable(GPIO_005); // PA5, has to be changed if ADC is used

SenseInfrared senseInfrared;

SenseInfrared::SenseInfrared()
{
	// TODO Auto-generated constructor stub

}

void SenseInfrared::init()
{
	infrared_enable.init(true, 1, 1);
}

void SenseInfrared::initInfrared()
{
	uint8_t config_sysrange[] = {0x00, SYSRANGE_START, 0x03, 0xFF, 0x00, 0x09};
	// continuous mode, 255mm high threshold, 0mm low threshold, 100ms between measurements
	i2c2_bus.write(INFRARED_I2C_ADDR, config_sysrange, sizeof(config_sysrange));

}

uint8_t SenseInfrared::readRange()
{
	uint8_t read_reg[] = {0x00, 0x62};
	uint8_t ret;
	i2c2_bus.writeRead(INFRARED_I2C_ADDR, read_reg, 2, &ret, 1);
	return ret;
}


void SenseInfrared::run()
{
	setPeriodicBeat(0, 100*MILLISECONDS);
	initInfrared();
	int ret = enc.init();
	while(1)
	{
		//uint8_t test_regw[] = {0x00, 0x00};
		//uint8_t data;
		//int suc = i2c_bus.writeRead(INFRARED_I2C_ADDR, test_regw, 2, &data, 1);

		uint8_t range = readRange();

		enc.read_pos();
		int64_t degr = enc.get_rot_deg();
		int32_t rps = enc.get_rot_speed(100*MILLISECONDS);

		PRINTF("Test read IR: %d\n", range);
		PRINTF("Encoder reading: %d, %d, RPS: %d\n", ret, degr, rps);
		suspendUntilNextBeat();
	}
}
