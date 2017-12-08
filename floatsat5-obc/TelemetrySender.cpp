/*
 * Telemetry.cpp
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#include "TelemetrySender.h"
#include "Topics.h"

TelemetrySender telemetrySender;

TelemetrySender::TelemetrySender()
{
}

void TelemetrySender::run()
{
	int counter = 0;
	Telemetry1 sensor1;
	Telemetry2 sensor2;

	setPeriodicBeat(0, 10*MILLISECONDS);
	while(1)
	{
		sensor2.a=counter; sensor2.b=30;
		sensor2.data[0]=12.5;sensor2.data[1]=29.3;
		//telemetry2.publish(sensor2);

		if (counter % 100 == 0)
		{
			sensor1.ch[0]='K';
			sensor1.ch[1]='S';

			telemetry1.publish(sensor1);

			PRINTF("Sending telemetry\n");
		}

		counter++;
		suspendUntilNextBeat();
	}
}

