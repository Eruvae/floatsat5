/*
 * SenseEncoder.cpp
 *
 *  Created on: 29.11.2017
 *      Author: TobiZ
 */

#include "SenseEncoder.h"
#include "CommInterfaces.h"

SenseEncoder senseEncoder;

SenseEncoder::SenseEncoder()
{
}

void SenseEncoder::run()
{
	setPeriodicBeat(10*MILLISECONDS, 100*MILLISECONDS);
	int ret = enc.init();
	while(1)
	{
		enc.read_pos();
		int64_t degr = enc.get_rot_deg();
		int32_t rps = enc.get_rot_speed(100*MILLISECONDS);
		PRINTF("Encoder reading: %d, %d, RPS: %d\n", ret, degr, rps);
		suspendUntilNextBeat();
	}
}
