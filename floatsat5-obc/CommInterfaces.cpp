/*
 * CommInterfaces.cpp
 *
 *  Created on: 29.10.2017
 *      Author: TobiZ
 */

#include "CommInterfaces.h"

HAL_UART bt_uart(UART_IDX2);
HAL_SPI spi_bus(SPI_IDX1, GPIO_019, GPIO_020, GPIO_021); // SCK: PB3, MISO: PB4, MOSI: PB5
HAL_I2C i2c_bus(I2C_IDX1, GPIO_024, GPIO_025); // SCL: PB8, SDA: PB9
HAL_GPIO gyro_cs(GPIO_018); // CS: PB2
HAL_GPIO xm_cs(GPIO_032); // CS: PC0
HAL_GPIO imu_enable(GPIO_055);

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

void CommInterfaces::init()
{
	gyro_cs.init(true, 1, 1);
	xm_cs.init(true, 1, 1);
	imu_enable.init(true, 1, 1);
	spi_bus.init(1000000); // Max frequency for IMU: 10 MHz
	spi_bus.config(SPI_PARAMETER_MODE, 0);
	i2c_bus.init(400000); // I2C fast mode (400 kHz)
	bt_uart.init(921600);
}
