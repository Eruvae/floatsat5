/*
 * ActuatorInterfaces.cpp
 *
 *  Created on: 23.11.2017
 *      Author: TobiZ
 */

#include "ActuatorInterfaces.h"

HAL_GPIO dc_enable(GPIO_066); // PE2
HAL_GPIO dc_test(GPIO_067); // PE3
HAL_GPIO hbridge_a_ina(GPIO_036); // PC4
HAL_GPIO hbridge_a_inb(GPIO_017); //PB1
HAL_PWM pwm_wheel(PWM_IDX12); // PB12

HAL_GPIO hbridge_b_ina(GPIO_016);
HAL_GPIO hbridge_b_inb(GPIO_071);

ActuatorInterfaces act;

ActuatorInterfaces::ActuatorInterfaces()
{
	// TODO Auto-generated constructor stub
}

void ActuatorInterfaces::init()
{
	dc_enable.init(true, 1, 1);
	dc_test.init(false, 1, 0);
	hbridge_a_ina.init(true, 1, 1);
	hbridge_a_inb.init(true, 1, 0);
	hbridge_b_ina.init(true, 1, 0);
	hbridge_b_inb.init(true, 1, 1);
	pwm_wheel.init(5000, 1000);
}

void ActuatorInterfaces::run()
{
	setPeriodicBeat(0, 500*MILLISECONDS);
	int i = 0;
	while(1)
	{
		i++;
		int test = dc_test.readPins();
		PRINTF("DC/DC Status: %d\n", test);

		if (i % 80 < 20 || i % 80 > 60)
		{
			hbridge_a_inb.setPins(0);
			hbridge_a_ina.setPins(1);
			pwm_wheel.write(500);
		}
		else
		{
			hbridge_a_ina.setPins(0);
			hbridge_a_inb.setPins(1);
			pwm_wheel.write(500);
		}
		hbridge_b_ina.setPins(~hbridge_b_ina.readPins());
		hbridge_b_inb.setPins(~hbridge_b_inb.readPins());
		suspendUntilNextBeat();
	}
}
