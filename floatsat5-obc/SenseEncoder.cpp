/*
 * SenseEncoder.cpp
 *
 *  Created on: 29.11.2017
 *      Author: TobiZ
 */

#include "SenseEncoder.h"
#include "CommInterfaces.h"
#include "Topics.h"

SenseEncoder senseEncoder;

SenseEncoder::SenseEncoder()
{
}

void SenseEncoder::run()
{

	int frequency = 10;
	setPeriodicBeat(10*MILLISECONDS, SECONDS/frequency);
	int ret = enc.init(ENC_POL_NORMAL, ENC_MODE_2A);
	while(1)
	{
		// read and reset counter (only speed is needed)
		int32_t reg = enc.readCounter();
		enc.resetCounter();


		int16_t rpm = reg * 60 * frequency / 32; // convert counter to RPM

		//PRINTF("Encoder reading: %d, %d, RPM: %d\n", ret, reg, rpm);

		itReactionWheelSpeed.publish(rpm);

		suspendUntilNextBeat();
	}
}
