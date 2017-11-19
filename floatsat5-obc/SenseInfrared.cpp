/*
 * SenseInfrared.cpp
 *
 *  Created on: 19.11.2017
 *      Author: TobiZ
 */

#include "SenseInfrared.h"

#define INFRARED_I2C_ADDR 0x29
#define INFRARED_WRITE 0x52
#define INFRARED_READ 0x53

SenseInfrared senseInfrared;

SenseInfrared::SenseInfrared()
{
	// TODO Auto-generated constructor stub

}

void SenseInfrared::run()
{
	setPeriodicBeat(0, 100*MILLISECONDS);
	while(1)
	{
		uint8_t test_regw[] = {0x00, 0x00};
		//uint8_t readb[] = {INFRARED_READ};
		uint8_t data;
		int suc1 = i2c_bus.write(INFRARED_I2C_ADDR, test_regw, 2);
		int suc2 = i2c_bus.read(INFRARED_I2C_ADDR, &data, 1);

		PRINTF("Test read IR: %x, %d, %d\n", data, suc1, suc2);
		suspendUntilNextBeat();
	}
}
