/*
 * CommInterfaces.cpp
 *
 *  Created on: 29.10.2017
 *      Author: TobiZ
 */

#include "CommInterfaces.h"
#include "Topics.h"

//HAL_UART bt_uart(UART_IDX2);
HAL_UART raspiUART(UART_IDX7);

HAL_SPI spi_bus(SPI_IDX1, GPIO_019, GPIO_020, GPIO_021); // SCK: PB3, MISO: PB4, MOSI: PB5
HAL_I2C i2c_bus(I2C_IDX1, GPIO_024, GPIO_025); // SCL: PB8, SDA: PB9
HAL_I2C i2c2_bus(I2C_IDX2);
HAL_GPIO gyro_cs(GPIO_018); // CS: PB2
HAL_GPIO xm_cs(GPIO_032); // CS: PC0
HAL_GPIO imu_enable(GPIO_055);

RotaryEncoder enc(ENC_TIM5/*, 64*/);

HAL_UART gatewayWifi(UART_IDX3);

#ifdef Wifi_WF121
WF121 wf(&gatewayWifi);
#endif

#ifdef Wifi_ESP8266
HAL_GPIO gpio0(GPIO_028);
HAL_GPIO enable(GPIO_046);
ESP8266 wf(&gatewayWifi,&enable,&gpio0);
#endif

LinkinterfaceWifi linkifwf(&wf);
Gateway gw(&linkifwf);

CommInterfaces comm;

int CommInterfaces::selectSPISlave(SPI_SS select)
{
    //if (selectedSlave != NONE)
    //    return -1;

    selectedSlave = select;

    switch(select)
    {
    case GYRO:
    	xm_cs.setPins(true);
        gyro_cs.setPins(false);
        break;
    case XM:
        gyro_cs.setPins(true);
        xm_cs.setPins(false);
        break;
    default:
    	gyro_cs.setPins(true);
    	xm_cs.setPins(true);
    }

    return 0;
}

void CommInterfaces::disableSPISlaves()
{
	gyro_cs.setPins(true);
	xm_cs.setPins(true);

	unsigned char reset = 0xFF;
	spi_bus.write(&reset, 1);

    selectedSlave = NONE;
}

void CommInterfaces::reset_i2c(HAL_I2C &bus)
{
	bus.reset();
	AT(NOW() + 0.5*MILLISECONDS);
	bus.init();
}

void CommInterfaces::init()
{
	printfVerbosity = 50;

	raspiUART.init(115200);
	gyro_cs.init(true, 1, 1);
	xm_cs.init(true, 1, 1);
	imu_enable.init(true, 1, 1);
	spi_bus.init(1000000); // Max frequency for IMU: 10 MHz
	spi_bus.config(SPI_PARAMETER_MODE, 0);
	i2c_bus.init(400000); // I2C fast mode (400 kHz)
	i2c2_bus.init(400000);
	//bt_uart.init(921600);

	//gw.resetTopicsToForward();
	gw.addTopicsToForward(&telecommand);

	gw.addTopicsToForward(&tmPowerData);
	gw.addTopicsToForward(&tmFilteredPose);
	gw.addTopicsToForward(&tmImuData);
	gw.addTopicsToForward(&tmReactionWheelSpeed);
	gw.addTopicsToForward(&tmInfraredData);
	gw.addTopicsToForward(&tmActuatorData);

	//.... More Topics to come
}

void CommInterfaces::run()
{
	PRINTF("Wifi getting initialized\n");
	int i = wf.init("FloatSat","floatsat");
	//int i = wf.init("EruvaeFS","194a69^V");

	PRINTF("Init finished\n");
	wf.enableUDPConnection(0x6D00A8C0,5000); // 192.168.0.109
	//wf.enableUDPConnection(0x0189A8C0, 5000); // 192.168.137.1; for hotspot wifi
		// 0x6400A8C0 = 192.168.0.100
		// 0x6500A8C0 = 192.168.0.101

	// Target IP Address: 192.168.0.102 = 0xC0A80066 (reverse and hex)
	// Target Port: 2000
	/**************************/

	/*TIME_LOOP(5*MILLISECONDS, 500*MILLISECONDS)
	{
		PRINTF("Wifi Status: %d\n", i);
	}*/

}
