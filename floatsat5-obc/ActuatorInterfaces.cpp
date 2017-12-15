/*
 * ActuatorInterfaces.cpp
 *
 *  Created on: 23.11.2017
 *      Author: TobiZ
 */

#include "ActuatorInterfaces.h"
#include "Topics.h"

HAL_GPIO dc_enable(GPIO_066); // PE2
HAL_GPIO dc_test(GPIO_067); // PE3
HAL_GPIO hbridge_a_ina(GPIO_036); // PC4
HAL_GPIO hbridge_a_inb(GPIO_017); //PB1
HAL_PWM pwm_wheel(PWM_IDX12); // PB12

HAL_GPIO hbridge_b_ina(GPIO_016);
HAL_GPIO hbridge_b_inb(GPIO_071);

ActuatorInterfaces act;

#define LIMIT(x, min, max)	((x) < (min) ?(min) : (x) > (max) ? (max) : (x))
#define ABS(x)	((x) >= 0 ? (x) : -(x))
#define SIGN(x)	((x) >= 0 ? 1 : -1)

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

void ActuatorInterfaces::setWheelDirection(bool forward)
{
	if (forward)
	{
		hbridge_a_inb.setPins(0);
		hbridge_a_ina.setPins(1);
	}
	else
	{
		hbridge_a_ina.setPins(0);
		hbridge_a_inb.setPins(1);
	}
}

void ActuatorInterfaces::run()
{
	float period = 0.5;
	setPeriodicBeat(0, period * SECONDS);
	int i = 0;
	float err_int = 0;
	int16_t targetSpeed, wheelSpeed;
	int16_t oldTargetWheelSpeed = 0;
	while(1)
	{
		i++;
		int test = dc_test.readPins();
		//PRINTF("DC/DC Status: %d\n", test);

		/*if (i % 80 < 20 || i % 80 > 60)
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
		hbridge_b_inb.setPins(~hbridge_b_inb.readPins());*/


		const float p = 1.0f;
		const float i = 1.0f;


		reactionWheelTargetSpeed.get(targetSpeed);
		reactionWheelSpeedBuffer.get(wheelSpeed);

		PRINTF("Target Speed: %d, Current Speed: %d\n", targetSpeed, wheelSpeed);

		if (targetSpeed != oldTargetWheelSpeed)
			err_int = 0; // if target speed changed, reset i part

		oldTargetWheelSpeed = err_int;

		float baseDutyCycle = SIGN(targetSpeed) * (ABS(targetSpeed) + 375.07) / 8617.5306; // Formula from measurements

		float error = targetSpeed - wheelSpeed;
		err_int += error * period;

		float errorDutyCycle = p * error / 8617.5306;
		float errorIntDutyCycle = i * err_int / 8617.5306;

		float dutyCycle = baseDutyCycle + errorDutyCycle +  errorIntDutyCycle;

		PRINTF("Base Duty Cylcle: %f, Calculated duty cycle: %f\n", baseDutyCycle, dutyCycle);

		setWheelDirection(dutyCycle >= 0);

		dutyCycle = LIMIT(ABS(dutyCycle), 0, 1);

		pwm_wheel.write(dutyCycle*1000);

		suspendUntilNextBeat();
	}
}
