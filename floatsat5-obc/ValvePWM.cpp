/*
 * ValvePWM.cpp
 *
 *  Created on: 31.01.2018
 *      Author: TobiZ
 */

#include "ValvePWM.h"
#include "ActuatorInterfaces.h"

ValvePWM valvePWM;

ValvePWM::ValvePWM() : thrusterControlsSubscriber(itThrusterControls, thrusterControlsBuffer), period(200*MILLISECONDS)
{}

void ValvePWM::sendThrusters(int n1, int n2, float f1, float f2)
{
	actuatorInterfaces.setThrusterStatus(n1, f1 > 0);
	suspendCallerUntil(NOW() + f1*period/2.f);
	actuatorInterfaces.setThrusterStatus(n1, false);
	suspendCallerUntil(NOW() + (1-f1)*period/2.f);
	actuatorInterfaces.setThrusterStatus(n2, f2 > 0);
	suspendCallerUntil(NOW() + f2*period/2.f);
	actuatorInterfaces.setThrusterStatus(n2, false);
	//suspendCallerUntil(NOW() + (1-f2)*period/2.f);
}

void ValvePWM::run()
{
	setPeriodicBeat(30*MILLISECONDS, period);
	while(1)
	{
		ThrusterControls controls;
		thrusterControlsBuffer.get(controls);
		float f1 = controls.f1;
		float f2 = controls.f2;
		float f3 = controls.f3;

		if (f1 == 0)
		{
			sendThrusters(2, 3, f2, f3);
		}
		else if (f2 == 0)
		{
			sendThrusters(1, 3, f1, f3);
		}
		else
		{
			sendThrusters(1, 2, f1, f2);
		}

		suspendUntilNextBeat();

		/*actuatorInterfaces.setThrusterStatus(3, f1 > 0);
		actuatorInterfaces.setThrusterStatus(2, f2 > 0);
		actuatorInterfaces.setThrusterStatus(1, f3 > 0);
		if (f1 == 0)
		{
			if (f2 < f3)
			{
				suspendCallerUntil(NOW() + f2 * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(2, false);
				suspendCallerUntil(NOW() + (f3 - f2) * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(1, false);
			}
			else
			{
				suspendCallerUntil(NOW() + f3 * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(1, false);
				suspendCallerUntil(NOW() + (f2 - f3) * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(2, false);
			}
		}
		else if (f2 == 0)
		{
			if (f1 < f3)
			{
				suspendCallerUntil(NOW() + f1 * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(3, false);
				suspendCallerUntil(NOW() + (f3 - f1) * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(1, false);
			}
			else
			{
				suspendCallerUntil(NOW() + f3 * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(1, false);
				suspendCallerUntil(NOW() + (f1 - f3) * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(3, false);
			}
		}
		else // f3 == 0
		{
			if (f1 < f2)
			{
				suspendCallerUntil(NOW() + f1 * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(3, false);
				suspendCallerUntil(NOW() + (f2 - f1) * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(2, false);
			}
			else
			{
				suspendCallerUntil(NOW() + f2 * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(2, false);
				suspendCallerUntil(NOW() + (f1 - f2) * period * MILLISECONDS);
				actuatorInterfaces.setThrusterStatus(3, false);
			}
		}
		suspendUntilNextBeat();*/


	}
}
