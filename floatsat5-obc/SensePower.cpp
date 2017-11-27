/*
 * SensePower.cpp
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#include "SensePower.h"

#include "CommInterfaces.h"

#define CONFIG_REG	0x00
#define SHUNT_VOLT	0x01

#define READ_FLAG	0x80

SensePower sensePower;

SensePower::SensePower()
{
	// TODO Auto-generated constructor stub

}

void SensePower::run()
{
	uint8_t toWrite[] = {CONFIG_REG, 0x4F, 0x32};
	i2c_bus.write(CURR_BATT_I2C_ADDR, toWrite, 3);


	//...

	setPeriodicBeat(2*MILLISECONDS, 100*MILLISECONDS);
	while(1)
	{
		uint8_t toWriteR[] = {SHUNT_VOLT | READ_FLAG};
		int16_t data[1];
		i2c_bus.writeRead(CURR_BATT_I2C_ADDR, toWriteR, 1, (uint8_t*)data, 2);

		//PRINTF("Data: %d\n", data[0]);

		suspendUntilNextBeat();
	}
}

