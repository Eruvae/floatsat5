/*
 * rot_encoder.h
 *
 *  Created on: 23 May 2017
 *      Author: Minh
 */

#ifndef ROT_ENCODER_H
#define ROT_ENCODER_H

#include "rodos.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"


//--------------------------------------------------------------
// Mode of Encoder
//--------------------------------------------------------------
typedef enum {
	ENC_MODE_2A,	// Counter counts at edges of input CH1 (A)
	ENC_MODE_2B,	// Counter counts at edges of input CH2 (B)
	ENC_MODE_4AB	// Counter counts at edges of input CH1 (A) AND input CH2 (B)
}ENC_COUNTER_MODE;


//--------------------------------------------------------------
// Direction of Encoder
//--------------------------------------------------------------
typedef enum {
	ENC_POL_NORMAL,	// Counter increments at counter-clockwise turning
					// for Rotary Encoder Model J733
					// when wired white cable to CH1 GPIO and green cable to CH2 GPIO
	ENC_POL_REVERSE	// Counter decrements at counter-clockwise turning
					// for Rotary Encoder Model J733
					// when wired white cable to CH1 GPIO and green cable to CH2 GPIO
}ENC_COUNTER_POLARITY;


//--------------------------------------------------------------
// Timer and GPIO distribution
//--------------------------------------------------------------
typedef enum {		// STM32F4
	ENC_TIM1,		// Timer 1, CH1 - PE9,  CH2 - PA9	(NOT WORKING)
	ENC_TIM2,		// Timer 2, CH1 - PA15, CH2 - PB3
	ENC_TIM3,		// Timer 3, CH1 - PC6,  CH2 - PC7
	ENC_TIM4,		// Timer 4, CH1 - PB6,  CH2 - PB7	(NOT WORKING if H-Bridges of STM32F4 Extension Board are in use since their PWM uses same TIMER)
	ENC_TIM5,		// Timer 5, CH1 - PA0,  CH2 - PA1
	ENC_TIM8,		// Timer 8, CH1 - PA7,  CH2 - PB14	(NOT WORKING)
}ENC_TIMER_SELECT;


//--------------------------------------------------------------
// Definitions of Quadrature Encoder input pins
//--------------------------------------------------------------
// Timer 1:
#define ENC_TIM1_CH1_GPIO_PORT         GPIOE
#define ENC_TIM1_CH1_GPIO_CLK          RCC_AHB1Periph_GPIOE
#define ENC_TIM1_CH1_PIN               GPIO_Pin_9
#define ENC_TIM1_CH1_SOURCE            GPIO_PinSource9

#define ENC_TIM1_CH2_GPIO_PORT         GPIOA
#define ENC_TIM1_CH2_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define ENC_TIM1_CH2_PIN               GPIO_Pin_9
#define ENC_TIM1_CH2_SOURCE            GPIO_PinSource9

// Timer 2:
#define ENC_TIM2_CH1_GPIO_PORT         GPIOA
#define ENC_TIM2_CH1_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define ENC_TIM2_CH1_PIN               GPIO_Pin_15
#define ENC_TIM2_CH1_SOURCE            GPIO_PinSource15

#define ENC_TIM2_CH2_GPIO_PORT         GPIOB
#define ENC_TIM2_CH2_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define ENC_TIM2_CH2_PIN               GPIO_Pin_3
#define ENC_TIM2_CH2_SOURCE            GPIO_PinSource3

// Timer 3:
#define ENC_TIM3_CH1_GPIO_PORT         GPIOC
#define ENC_TIM3_CH1_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ENC_TIM3_CH1_PIN               GPIO_Pin_6
#define ENC_TIM3_CH1_SOURCE            GPIO_PinSource6

#define ENC_TIM3_CH2_GPIO_PORT         GPIOC
#define ENC_TIM3_CH2_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ENC_TIM3_CH2_PIN               GPIO_Pin_7
#define ENC_TIM3_CH2_SOURCE            GPIO_PinSource7

// Timer 4:
#define ENC_TIM4_CH1_GPIO_PORT         GPIOB
#define ENC_TIM4_CH1_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define ENC_TIM4_CH1_PIN               GPIO_Pin_6
#define ENC_TIM4_CH1_SOURCE            GPIO_PinSource6

#define ENC_TIM4_CH2_GPIO_PORT         GPIOB
#define ENC_TIM4_CH2_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define ENC_TIM4_CH2_PIN               GPIO_Pin_7
#define ENC_TIM4_CH2_SOURCE            GPIO_PinSource7

// Timer 5:
#define ENC_TIM5_CH1_GPIO_PORT         GPIOA
#define ENC_TIM5_CH1_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define ENC_TIM5_CH1_PIN               GPIO_Pin_0
#define ENC_TIM5_CH1_SOURCE            GPIO_PinSource0

#define ENC_TIM5_CH2_GPIO_PORT         GPIOA
#define ENC_TIM5_CH2_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define ENC_TIM5_CH2_PIN               GPIO_Pin_1
#define ENC_TIM5_CH2_SOURCE            GPIO_PinSource1

// Timer 8:
#define ENC_TIM8_CH1_GPIO_PORT         GPIOA
#define ENC_TIM8_CH1_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define ENC_TIM8_CH1_PIN               GPIO_Pin_7
#define ENC_TIM8_CH1_SOURCE            GPIO_PinSource7

#define ENC_TIM8_CH2_GPIO_PORT         GPIOB
#define ENC_TIM8_CH2_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define ENC_TIM8_CH2_PIN               GPIO_Pin_14
#define ENC_TIM8_CH2_SOURCE            GPIO_PinSource14


//--------------------------------------------------------------
// Structure of Encoder-Parameters
//--------------------------------------------------------------
typedef struct {
	  uint8_t GPIO_AF;
	  GPIO_TypeDef* PORT_CH1;
	  GPIO_TypeDef* PORT_CH2;
	  uint16_t PIN_CH1;
	  uint16_t PIN_CH2;
	  uint8_t PIN_SOURCE_CH1;
	  uint8_t PIN_SOURCE_CH2;
	  uint32_t RCC_Periph_GPIO_CH1;
	  uint32_t RCC_Periph_GPIO_CH2;
	  uint32_t RCC_Periph_TIMER;
	  uint16_t MODE;	// Mode determines at which edges counter is counting
	  uint16_t POLARITY;
}ENC_TypeDef;

class Rotary_Encoder
{
private:
	  TIM_TypeDef* TIMER;
	  ENC_TIMER_SELECT tim;

	  uint16_t ROT_RESOLUTION;

	  uint8_t coding_factor;	// a value necessary to compute exact angular position and depends on ENC_COUNTER_MODE selected
	  uint16_t counter_max_val;
	  uint16_t counter_old_val;
	  uint16_t counter_1_4;		// 1/4 of counter_max_val, helps to determine underflow/overflow of counter
	  uint16_t counter_3_4;		// 3/4 of counter_max_val, helps to determine underflow/overflow of counter
	  int32_t flow_count;		// decremented by 1 if counter underflow, incremented by 1 if counter overflow
	  int32_t counter_dif;		// difference of counter value from previous iteration and current iteration

	  void enc_set_gpio(const ENC_TypeDef &encoder);
	  void enc_set_timer(const ENC_TypeDef &encoder);
	  void reset_counter();

public:

	  /*
	   * Select Timer between ENC_TIM2 to ENC_TIM5
	   * STM32F4:
	   *	ENC_TIM2	CH1 - PA15, CH2 - PB3
	   *	ENC_TIM3	CH1 - PC6,  CH2 - PC7
	   *	ENC_TIM4	CH1 - PB6,  CH2 - PB7	(NOT WORKING if H-Bridges of STM32F4 Extension Board are in use since their PWM uses same TIMER)
	   *	ENC_TIM5	CH1 - PA0,  CH2 - PA1
	   * NOTE:
	   * 	For correct polarity connect white cable to CH1 pin and green cable to CH2 pin (Rotary Encoder Model J733)
	   * 	Default Rotary Resolution is set to 600 (works with HW: Rotary Encoder Model J733 with 600 Pulses per Full Revolution)
	   */
	  Rotary_Encoder(ENC_TIMER_SELECT tim, uint16_t rot_resolution = 600);


	  /*
	   * Counter polarity options are:	ENC_POL_NORMAL (default)
	   * 								ENC_POL_REVERSE
	   *
	   * Counter mode options are:		ENC_MODE_2A (default)s
	   * 								ENC_MODE_2B
	   * 								ENC_MODE_4AB
	   */
	  int init(ENC_COUNTER_POLARITY polarity = ENC_POL_NORMAL, ENC_COUNTER_MODE mode = ENC_MODE_2A);


	  void set_pos(int64_t degree);

	  void reset();

	  /*
	   * This function shall be called at the beginning of loop iteration
	   * and is necessary for the getter functions of this class to return proper values
	   * Identifies underflow/overflow of counter and computes the difference of counter value
	   * from previous iteration call and current iteration call
	   */
	  void read_pos();


	  /*
	   * In order to get proper value call ONLY ONCE (for each iteration) read_pos() at the beginning of loop iteration
	   * Returns Counter value relative to start position
	   */
	  int64_t get_pos_rel_to_start();


	  /*
	   * In order to get proper value call ONLY ONCE (for each iteration) read_pos() at the beginning of loop iteration
	   * Returns Rotation degree relative to start position
	   */
	  int64_t get_rot_deg();


	  /*
	   * In order to get proper value call ONLY ONCE (for each iteration) read_pos() at the beginning of loop iteration
	   * @param long long beat: Specify period duration of loop iteration (Sample Time)
	   * Returns Rotation speed in degree per second
	   */
	  int32_t get_rot_speed(long long beat);
	  int32_t get_rot_speed_abs(long long beat);


	  /*
	   * In order to get proper value call ONLY ONCE (for each iteration) read_pos() at the beginning of loop iteration
	   * @param long long beat: Specify period duration of loop iteration (Sample Time)
	   * Returns Rotation speed in revolutions per second
	   */
	  double get_rps(long long beat);
	  double get_rps_abs(long long beat);

};

#endif  /* ROT_ENCODER_H_ */

