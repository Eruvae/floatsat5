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

HAL_GPIO hbridge_valve1(GPIO_072); // hbridge_c_ina
HAL_GPIO hbridge_valve2(GPIO_071); // hbridge_b_inb
HAL_GPIO hbridge_valve3(GPIO_016); // hbridge_b_ina

ActuatorInterfaces actuatorInterfaces;

ActuatorInterfaces::ActuatorInterfaces() : reactionWheelSpeedBuffer(), reactionWheelSpeedSub(itReactionWheelSpeed, reactionWheelSpeedBuffer)
{
}

void ActuatorInterfaces::init()
{
	dc_enable.init(true, 1, 1);
	dc_test.init(false, 1, 0);

	hbridge_a_ina.init(true, 1, 1);
	hbridge_a_inb.init(true, 1, 0);
	pwm_wheel.init(5000, 1000);

	hbridge_valve1.init(true, 1, 0);
	hbridge_valve2.init(true, 1, 0);
	hbridge_valve3.init(true, 1, 0);
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

void ActuatorInterfaces::setThrusterStatus(int number, bool status)
{
	if (number == 1)
		hbridge_valve1.setPins(status);
	else if (number == 2)
		hbridge_valve2.setPins(status);
	else if (number == 3)
		hbridge_valve3.setPins(status);
}

void ActuatorInterfaces::run()
{
	float period = 0.1;
	setPeriodicBeat(25*MILLISECONDS, period * SECONDS);
	int i = 0;
	float err_int = 0;
	int16_t targetSpeed = 0, wheelSpeed = 0;
	int16_t oldTargetWheelSpeed = 0;
	int16_t oldWheelSpeed = 0;
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


		const float p = 10.0f;
		const float i = 0.1f;
		const float d = 0.5f;


		oldWheelSpeed = wheelSpeed;
		tcReactionWheelTargetSpeed.get(targetSpeed);
		reactionWheelSpeedBuffer.get(wheelSpeed);

		//PRINTF("Target Speed: %d, Current Speed: %d\n", targetSpeed, wheelSpeed);

		if (targetSpeed != oldTargetWheelSpeed)
			err_int = 0; // if target speed changed, reset i part

		oldTargetWheelSpeed = err_int;

		float baseDutyCycle = SIGN(targetSpeed) * (ABS(targetSpeed) + 375.07) / 8617.5306; // Formula from measurements

		float error = targetSpeed - wheelSpeed;
		err_int += error * period;
		float err_dif = (wheelSpeed - oldWheelSpeed) / period;

		float errorDutyCycle = p * error / 8617.5306;
		float errorIntDutyCycle = i * err_int / 8617.5306;
		float errorDifDutyCycle = d * err_dif / 8617.5306;

		float dutyCycle = baseDutyCycle + errorDutyCycle /*+ errorIntDutyCycle*/ - errorDifDutyCycle;

		//PRINTF("Base Duty Cylcle: %f, Calculated duty cycle: %f\n", baseDutyCycle, dutyCycle);

		int8_t wheelDirection = SIGN(dutyCycle);
		setWheelDirection(wheelDirection >= 0);

		dutyCycle = LIMIT(ABS(dutyCycle), 0, 1);

		pwm_wheel.write(dutyCycle*1000);

		ActuatorData data;
		uint8_t valve1 = hbridge_valve1.readPins();
		uint8_t valve2 = hbridge_valve2.readPins();
		uint8_t valve3 = hbridge_valve3.readPins();

		data.valveStatus = valve1 | (valve2 << 1) | (valve3 << 2);
		data.rwDirection = wheelDirection;
		data.dutyCycle = dutyCycle;
		itActuatorData.publish(data);

		suspendUntilNextBeat();
	}
}
