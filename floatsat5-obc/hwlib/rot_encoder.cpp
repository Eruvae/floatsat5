/*
 * rot_encoder.cpp
 *
 *  Created on: 23 May 2017
 *      Author: Minh
 */

#include "rot_encoder.h"

#include "timemodel.h"
#include "stdlib.h"
#include "math.h"

#define MY_ABS(x)	((x) < 0 ? -(x) : (x))

RotaryEncoder::RotaryEncoder(ENC_TIMER_SELECT tim/*, uint16_t rot_resolution*/):
	tim(tim)//,
	//ROT_RESOLUTION(rot_resolution),
	//counter_max_val(get_timer_max() / ROT_RESOLUTION * ROT_RESOLUTION),
	//counter_1_4(counter_max_val / 4),
	//counter_3_4(counter_max_val - counter_1_4),
	//counter_old_val(0),
	//flow_count(0),
	//coding_factor(0),
	//counter_dif(0)
{
	switch(tim)
	{
	case ENC_TIM1:
		TIMER = TIM1;
		break;
	case ENC_TIM2:
		TIMER = TIM2;
		break;
	case ENC_TIM3:
		TIMER = TIM3;
		break;
	case ENC_TIM4:
		TIMER = TIM4;
		break;
	case ENC_TIM5:
		TIMER = TIM5;
		break;
	case ENC_TIM8:
		TIMER = TIM8;
		break;
	}
}


int RotaryEncoder::init(ENC_COUNTER_POLARITY polarity, ENC_COUNTER_MODE mode)
{
	ENC_TypeDef encoder;

	switch(polarity)
	{
	case ENC_POL_NORMAL:
		encoder.POLARITY = TIM_ICPolarity_Rising;
		break;
	case ENC_POL_REVERSE:
		encoder.POLARITY = TIM_ICPolarity_Falling;
		break;
	default:
		return -1;
	}


	switch (mode)
	{
	case ENC_MODE_2A:
		encoder.MODE = TIM_EncoderMode_TI1;
		//coding_factor = 2;
		break;
	case ENC_MODE_2B:
		encoder.MODE = TIM_EncoderMode_TI2;
		//coding_factor = 2;
		break;
	case ENC_MODE_4AB:
		//coding_factor = 4;
		encoder.MODE = TIM_EncoderMode_TI12;
		break;
	default:
		return -1;
	}


	switch(tim)
	{
	case ENC_TIM1:
		encoder.GPIO_AF = GPIO_AF_TIM1;
		encoder.RCC_Periph_TIMER = RCC_APB2Periph_TIM1;
		encoder.PIN_CH1 = ENC_TIM1_CH1_PIN;
		encoder.PIN_SOURCE_CH1 = ENC_TIM1_CH1_SOURCE;
		encoder.PORT_CH1 = ENC_TIM1_CH1_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH1 = ENC_TIM1_CH1_GPIO_CLK;
		encoder.PIN_CH2 = ENC_TIM1_CH2_PIN;
		encoder.PIN_SOURCE_CH2 = ENC_TIM1_CH2_SOURCE;
		encoder.PORT_CH2 = ENC_TIM1_CH2_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH2 = ENC_TIM1_CH2_GPIO_CLK;
		break;
	case ENC_TIM2:
		encoder.GPIO_AF = GPIO_AF_TIM2;
		encoder.RCC_Periph_TIMER = RCC_APB1Periph_TIM2;
		encoder.PIN_CH1 = ENC_TIM2_CH1_PIN;
		encoder.PIN_SOURCE_CH1 = ENC_TIM2_CH1_SOURCE;
		encoder.PORT_CH1 = ENC_TIM2_CH1_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH1 = ENC_TIM2_CH1_GPIO_CLK;
		encoder.PIN_CH2 = ENC_TIM2_CH2_PIN;
		encoder.PIN_SOURCE_CH2 = ENC_TIM2_CH2_SOURCE;
		encoder.PORT_CH2 = ENC_TIM2_CH2_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH2 = ENC_TIM2_CH2_GPIO_CLK;
		break;
	case ENC_TIM3:
		encoder.GPIO_AF = GPIO_AF_TIM3;
		encoder.RCC_Periph_TIMER = RCC_APB1Periph_TIM3;
		encoder.PIN_CH1 = ENC_TIM3_CH1_PIN;
		encoder.PIN_SOURCE_CH1 = ENC_TIM3_CH1_SOURCE;
		encoder.PORT_CH1 = ENC_TIM3_CH1_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH1 = ENC_TIM3_CH1_GPIO_CLK;
		encoder.PIN_CH2 = ENC_TIM3_CH2_PIN;
		encoder.PIN_SOURCE_CH2 = ENC_TIM3_CH2_SOURCE;
		encoder.PORT_CH2 = ENC_TIM3_CH2_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH2 = ENC_TIM3_CH2_GPIO_CLK;
		break;
	case ENC_TIM4:
		encoder.GPIO_AF = GPIO_AF_TIM4;
		encoder.RCC_Periph_TIMER = RCC_APB1Periph_TIM4;
		encoder.PIN_CH1 = ENC_TIM4_CH1_PIN;
		encoder.PIN_SOURCE_CH1 = ENC_TIM4_CH1_SOURCE;
		encoder.PORT_CH1 = ENC_TIM4_CH1_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH1 = ENC_TIM4_CH1_GPIO_CLK;
		encoder.PIN_CH2 = ENC_TIM4_CH2_PIN;
		encoder.PIN_SOURCE_CH2 = ENC_TIM4_CH2_SOURCE;
		encoder.PORT_CH2 = ENC_TIM4_CH2_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH2 = ENC_TIM4_CH2_GPIO_CLK;
		break;
	case ENC_TIM5:
		encoder.GPIO_AF = GPIO_AF_TIM5;
		encoder.RCC_Periph_TIMER = RCC_APB1Periph_TIM5;
		encoder.PIN_CH1 = ENC_TIM5_CH1_PIN;
		encoder.PIN_SOURCE_CH1 = ENC_TIM5_CH1_SOURCE;
		encoder.PORT_CH1 = ENC_TIM5_CH1_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH1 = ENC_TIM5_CH1_GPIO_CLK;
		encoder.PIN_CH2 = ENC_TIM5_CH2_PIN;
		encoder.PIN_SOURCE_CH2 = ENC_TIM5_CH2_SOURCE;
		encoder.PORT_CH2 = ENC_TIM5_CH2_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH2 = ENC_TIM5_CH2_GPIO_CLK;
		break;
	case ENC_TIM8:
		encoder.GPIO_AF = GPIO_AF_TIM8;
		encoder.RCC_Periph_TIMER = RCC_APB2Periph_TIM8;
		encoder.PIN_CH1 = ENC_TIM8_CH1_PIN;
		encoder.PIN_SOURCE_CH1 = ENC_TIM8_CH1_SOURCE;
		encoder.PORT_CH1 = ENC_TIM8_CH1_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH1 = ENC_TIM8_CH1_GPIO_CLK;
		encoder.PIN_CH2 = ENC_TIM8_CH2_PIN;
		encoder.PIN_SOURCE_CH2 = ENC_TIM8_CH2_SOURCE;
		encoder.PORT_CH2 = ENC_TIM8_CH2_GPIO_PORT;
		encoder.RCC_Periph_GPIO_CH2 = ENC_TIM8_CH2_GPIO_CLK;
		break;
	default:
		return -1;
	}


	enc_set_gpio(encoder);
	enc_set_timer(encoder);

	return 0;
}


void RotaryEncoder::enc_set_gpio(const ENC_TypeDef &encoder)
{
	GPIO_InitTypeDef gpio_init_structure;

	// Config Pins as AF-Input
	gpio_init_structure.GPIO_Mode = GPIO_Mode_AF;
	gpio_init_structure.GPIO_Speed = GPIO_Speed_100MHz;
	gpio_init_structure.GPIO_OType = GPIO_OType_PP;
	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_UP;

	// Clock Enable
	RCC_AHB1PeriphClockCmd(encoder.RCC_Periph_GPIO_CH1, ENABLE);
	RCC_AHB1PeriphClockCmd(encoder.RCC_Periph_GPIO_CH2, ENABLE);

	gpio_init_structure.GPIO_Pin = encoder.PIN_CH1;
	GPIO_Init(encoder.PORT_CH1, &gpio_init_structure);
	gpio_init_structure.GPIO_Pin = encoder.PIN_CH2;
	GPIO_Init(encoder.PORT_CH2, &gpio_init_structure);

	// Alternating Function from IO-Pin
	GPIO_PinAFConfig(encoder.PORT_CH1, encoder.PIN_SOURCE_CH1, encoder.GPIO_AF);
	GPIO_PinAFConfig(encoder.PORT_CH2, encoder.PIN_SOURCE_CH2, encoder.GPIO_AF);
}


void RotaryEncoder::enc_set_timer(const ENC_TypeDef &encoder)
{
	// Clock enable
	RCC_APB1PeriphClockCmd(encoder.RCC_Periph_TIMER, ENABLE);

	// Set
	TIM_EncoderInterfaceConfig(TIMER, encoder.MODE, encoder.POLARITY, TIM_ICPolarity_Rising);

	// Set max value of counter
	//TIM_SetAutoreload(TIMER, counter_max_val);

	// Timer enable
	TIM_Cmd(TIMER, ENABLE);

	resetCounter();
}

uint32_t RotaryEncoder::get_timer_max()
{
	switch(tim)
	{
	case ENC_TIM2: case ENC_TIM5:
		return UINT32_MAX;
	default:
		return UINT16_MAX;
	}
}


void RotaryEncoder::resetCounter()
{
	TIM_SetCounter(TIMER, 0);
}

void RotaryEncoder::setCounter(int32_t value)
{
	TIM_SetCounter(TIMER, value);
}

int32_t RotaryEncoder::readCounter()
{
	uint32_t val = TIM_GetCounter(TIMER);
	return *(int32_t*)(&val);
}
