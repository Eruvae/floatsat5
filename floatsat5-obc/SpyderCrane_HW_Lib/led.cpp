/*
 * led.cpp
 *
 *  Created on: 1 Jun 2017
 *      Author: Minh
 */


#include "includes/led.h"



LED::LED(LED_SELECT color):
hal_led(
		color == GREEN ?	LED_GREEN :
		color == ORANGE ? 	LED_ORANGE:
		color == RED ? 		LED_ORANGE:
	  /*color == BLUE ?*/ 	LED_BLUE)
{}


int32_t LED::init(uint32_t initVal, bool isOutput, uint32_t numOfPins)
{
	return hal_led.init(isOutput, numOfPins, initVal);
}


void LED::toggle()
{
	hal_led.setPins(~hal_led.readPins());
}

void LED::turnOn(bool state)
{
	hal_led.setPins(state);
}

bool LED::isOn()
{
	return hal_led.readPins();
}
