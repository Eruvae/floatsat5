/*
 * brush_motor.h
 *
 *  Created on: 26 May 2017
 *      Author: Minh
 */

#ifndef SPYDERCRANE_BRUSH_MOTOR_H_
#define SPYDERCRANE_BRUSH_MOTOR_H_

#include "rodos.h"
#include "hal/hal_pwm.h"
#include "hal/hal_gpio.h"


//--------------------------------------------------------------
// Rotate direction of Brush Motor
//--------------------------------------------------------------
typedef enum {
	MOT_POL_NORMAL,	// INA value 0 and INB value 1
	MOT_POL_REVERSE	// INA value 1 and INB value 0
}BR_MOTOR_POLARITY;


//--------------------------------------------------------------
// Brush Motor Select Options
//--------------------------------------------------------------
typedef enum {
	MOTOR_A,
	MOTOR_B,
	MOTOR_C,
	MOTOR_D
}BR_MOTOR_SELECT;


//--------------------------------------------------------------
// Definitions of PMW TIMER and GPIO
//--------------------------------------------------------------
#define BR_MOT_TIM_A	PWM_IDX12	// PWM A	Timer 4 CH 1
#define BR_MOT_IOA_A	GPIO_036	// INA A	STM32F4: PC4
#define BR_MOT_IOB_A	GPIO_017	// INB A	STM32F4: PB1

#define BR_MOT_TIM_B	PWM_IDX13	// PWM B	Timer 4 CH 2
#define BR_MOT_IOA_B	GPIO_016	// INA B	STM32F4: PB0
#define BR_MOT_IOB_B	GPIO_071	// INB B	STM32F4: PE7

#define BR_MOT_TIM_C	PWM_IDX14	// PWM C	Timer 4 CH 3
#define BR_MOT_IOA_C	GPIO_072	// INA C	STM32F4: PE8
#define BR_MOT_IOB_C	GPIO_074	// INB C	STM32F4: PE10

#define BR_MOT_TIM_D	PWM_IDX15	// PWM D	Timer 4 CH 4
#define BR_MOT_IOA_D	GPIO_076	// INA D	STM32F4: PE12
#define BR_MOT_IOB_D	GPIO_079	// INB D	STM32F4: PE15



class Brush_Motor
{
private:

	HAL_PWM pwm;
	HAL_GPIO ina;
	HAL_GPIO inb;

	uint16_t increments;

	uint16_t duty_cycle;

	uint8_t demo_dutyCycle;
	uint8_t demo_goUp;


public:

	static HAL_GPIO DCDCOn;		// GPIO_066		STM32F4: PE02
	static HAL_GPIO PowerOK;	// GPIO_067		STM32F4: PE03

	/*
	 * Motor select options are: BR_MOTOR_A, BR_MOTOR_B, BR_MOTOR_C, BR_MOTOR_D
	 */
	Brush_Motor(BR_MOTOR_SELECT motor);


	/*
	 * Initialize PWM without starting it -> start with "set_duty_cycle(N)"
	 * To get exact PWM frequency you must choose frequency and increments
	 * so that timerclock/frequency/increments = integer (no fraction)
	 * e.g. STM32F4 Timer 4:	84Mhz/1050/100 = 800Hz
	 * 							84Mhz/1200/100 = 700Hz
	 * 							84Mhz/1400/100 = 600Hz
	 *							84Mhz/1680/100 = 500Hz
	 * 							84Mhz/2100/100 = 400Hz
	 *							84Mhz/2800/100 = 300Hz
	 * 							84Mhz/4200/100 = 200Hz
	 * 							84Mhz/8400/100 = 100Hz
	 *
	 * STM32F4  : Tim4 (16 bit,84MHz)
	 *
	 * @param uint16_t frequency - frequency of PWM signal
	 * @param uint16_t increments - one PWM period is divided in N increments
	 */
	int init(uint16_t frequency = 8400, uint16_t increments = 100, BR_MOTOR_POLARITY pol = MOT_POL_NORMAL);
	int init(BR_MOTOR_POLARITY pol);


	/*
	 * Set PWM frequency directly to actual final desired frequency in Hz
	 */
	int set_frequency(uint16_t freq);


	uint16_t get_max_duty_cycle();


	/*
	 * Set PWM increment steps.
	 * This will change max possible duty cycle and the overall frequency of PWM (for information how to compute the frequency look in header file)
	 */
	int set_increments(uint16_t incr);

	BR_MOTOR_POLARITY get_dir();
	uint16_t get_duty_cycle();

	/*
	 * @param uint8_t value: Default range from 0 to 100 and is determined by increments
	 */
	void set_duty_cycle(uint8_t value);
	void set_dir(BR_MOTOR_POLARITY pol);

	/*
	 * @param bool state: true - to turn DCDC converter on
	 */
	static void turn_dcdcon(bool state);
	static bool is_power_ok();

	/*
	 * Demonstration of motor running, call this function inside a TIME_LOOP
	 * You may have to turn on DCDC Converter by calling turn_dcdcon(ON) before TIME_LOOP
	 */
	void demo();
};



#endif /* SPYDERCRANE_BRUSH_MOTOR_H_ */
