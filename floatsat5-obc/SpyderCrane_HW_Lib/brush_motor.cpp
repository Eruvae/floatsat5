/*
 * brush_motor.cpp
 *
 *  Created on: 26 May 2017
 *      Author: Minh
 */

#include "includes/brush_motor.h"
#include "math.h"


HAL_GPIO Brush_Motor::DCDCOn(GPIO_066);
HAL_GPIO Brush_Motor::PowerOK(GPIO_067);

Brush_Motor::Brush_Motor(BR_MOTOR_SELECT motor):
		pwm(
				motor == MOTOR_A ? BR_MOT_TIM_A :
				motor == MOTOR_B ? BR_MOT_TIM_B :
				motor == MOTOR_C ? BR_MOT_TIM_C :
			 /* motor == BR_MOTOR_D*/ BR_MOT_TIM_D
		),
		increments(0),
		duty_cycle(0),
		demo_dutyCycle(0),
		demo_goUp(1)
{
	switch(motor)
	{
	case MOTOR_A:
		ina = HAL_GPIO(BR_MOT_IOA_A);
		inb = HAL_GPIO(BR_MOT_IOB_A);
		break;
	case MOTOR_B:
		ina = HAL_GPIO(BR_MOT_IOA_B);
		inb = HAL_GPIO(BR_MOT_IOB_B);
		break;
	case MOTOR_C:
		ina = HAL_GPIO(BR_MOT_IOA_C);
		inb = HAL_GPIO(BR_MOT_IOB_C);
		break;
	case MOTOR_D:
		ina = HAL_GPIO(BR_MOT_IOA_D);
		inb = HAL_GPIO(BR_MOT_IOB_D);
		break;
	}
}


int Brush_Motor::init(uint16_t frequency, uint16_t increments, BR_MOTOR_POLARITY pol)
{
	// Defaults: frequency=8400, increments=100		For more detail look in header file.
	if (pwm.init(frequency, increments) < 0)
		return -1;

	switch(pol)
	{
	case MOT_POL_NORMAL:	// default
		if (ina.init(1, 1, 0) < 0)
			return -1;
		if (inb.init(1, 1, 1) < 0)
			return -1;
		break;
	case MOT_POL_REVERSE:
		if (ina.init(1, 1, 1) < 0)
			return -1;
		if (inb.init(1, 1, 0) < 0)
			return -1;
		break;
	}


	if (DCDCOn.init(1, 1, 0) < 0)
		return -1;
	if (PowerOK.init(0, 1, 0) < 0)
		return -1;

	this->increments = increments;

	return 0;
}


int Brush_Motor::init(BR_MOTOR_POLARITY pol)
{
	return this->init(8400, 100, pol);
}


int Brush_Motor::set_frequency(uint16_t freq)
{
	return pwm.config(PWM_PARAMETER_FREQUENCY, ceil(84000000 / increments / freq));
}


int Brush_Motor::set_increments(uint16_t incr)
{
	increments = incr;
	return pwm.config(PWM_PARAMETER_INCREMENTS, increments);
}


uint16_t Brush_Motor::get_max_duty_cycle(){
	return increments;
}


uint16_t Brush_Motor::get_duty_cycle()
{
	return duty_cycle;
}


void Brush_Motor::set_duty_cycle(uint8_t value)
{
	duty_cycle = value;
	pwm.write(value);
}


BR_MOTOR_POLARITY Brush_Motor::get_dir()
{
	return (ina.readPins() == 0 && inb.readPins() == 1) ? MOT_POL_NORMAL : MOT_POL_REVERSE;
}


void Brush_Motor::set_dir(BR_MOTOR_POLARITY pol)
{
	switch(pol)
	{
	case MOT_POL_NORMAL:
		ina.setPins(0);
		inb.setPins(1);
		break;
	case MOT_POL_REVERSE:
		ina.setPins(1);
		inb.setPins(0);
		break;
	}
}


void Brush_Motor::turn_dcdcon(bool state)
{
	DCDCOn.setPins(state);
}


bool Brush_Motor::is_power_ok()
{
	return PowerOK.readPins();
}


void Brush_Motor::demo()
{
	if (Brush_Motor::is_power_ok())
	{
		if (get_dir() == MOT_POL_NORMAL)
		{
			if (demo_goUp)
			{
				demo_dutyCycle++;
				if (demo_dutyCycle >= 55)
					demo_goUp = 0;
			}
			else
			{
				demo_dutyCycle--;
				if (demo_dutyCycle <= 2)
				{
					demo_goUp = 1;
					set_dir(MOT_POL_REVERSE);
				}
			}
		}
		else
		{
			if (demo_goUp)
			{
				demo_dutyCycle++;
				if (demo_dutyCycle >= 55)
					demo_goUp = 0;
			}
			else
			{
				demo_dutyCycle--;
				if (demo_dutyCycle <= 2)
				{
					demo_goUp = 1;
					set_dir(MOT_POL_NORMAL);
				}
			}
		}

		set_duty_cycle(demo_dutyCycle);
	}
}
