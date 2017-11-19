/*
 * Template.cpp
 *
 * Created on: 25.06.2014
 * Author: Atheel Redah
 *
 */

// Will be deleted

/*
#include "rodos.h"
#include <stdio.h>
#include "hal.h"
#include "math.h"

static Application module01("Template", 2001);

#define LED_GREEN GPIO_060
#define LED_ORANGE GPIO_061
#define LED_RED GPIO_062
#define LED_BLUE GPIO_063

#define BT_UART UART_IDX2
#define USB_UART UART_IDX3

#define IMU_I2C I2C_IDX2

HAL_GPIO GreenLED(LED_GREEN);
HAL_UART bt_uart(BT_UART);

struct SensorData{

};

Topic<SensorData> SensorDataTopic(-1, "Sensor Data");
CommBuffer<SensorData> SensorDataBuffer;
Subscriber SensorDataSubscriber(SensorDataTopic, SensorDataBuffer);


class Telemetry: public Thread {

public:

	Telemetry(const char* name) : Thread(name) {
	}

	void init() {
		GreenLED.init(true, 1, 0);
		bt_uart.init(921600);
	}

	void run() {

		while (1) {
			GreenLED.setPins(~GreenLED.readPins());
			PRINTF("Hello Rodos, the time now is %f \r\n",SECONDS_NOW());

			char data[] = "Test Bluetooth.\n";

			bt_uart.write(data, strlen(data));

			suspendCallerUntil(NOW()+1000*MILLISECONDS);
		}
	}
};*/
//Telemetry Telemetry("Telemetry");

/***********************************************************************/
