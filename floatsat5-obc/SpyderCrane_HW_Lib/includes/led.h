/*
 * led.h
 *
 *  Created on: 1 Jun 2017
 *      Author: Minh
 */

#ifndef INCLUDES_LED_H_
#define INCLUDES_LED_H_

#include "rodos.h"

#define LED_GREEN 	GPIO_060
#define LED_ORANGE 	GPIO_061
#define LED_RED		GPIO_062
#define LED_BLUE	GPIO_063

typedef enum {
	GREEN,
	ORANGE,
	RED,
	BLUE
} LED_SELECT;


class LED
{
private:
	HAL_GPIO hal_led;

public:

	LED(LED_SELECT color);

	int32_t init(uint32_t initVal = 0x00, bool isOutput = true, uint32_t numOfPins = 1);

	void toggle();

	void turnOn(bool state);

	bool isOn();
};


#endif /* INCLUDES_LED_H_ */
