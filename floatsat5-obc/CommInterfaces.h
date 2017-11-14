/*
 * CommInterfaces.h
 *
 *  Created on: 29.10.2017
 *      Author: TobiZ
 */

#ifndef COMMINTERFACES_H_
#define COMMINTERFACES_H_

#include "rodos.h"

#define CURR_BATT_I2C_ADDR 0b1000000
#define CURR_MOTA_I2C_ADDR 0b1000001
#define CURR_MOTB_I2C_ADDR 0b1000010
#define CURR_MOTC_I2C_ADDR 0b1000011
#define CURR_MOTD_I2C_ADDR 0b1000100
#define CURR_SER1_I2C_ADDR 0b1000101
#define CURR_SER2_I2C_ADDR 0b1000110
#define CURR_SER3_I2C_ADDR 0b1000111
#define CURR_SER4_I2C_ADDR 0b1001000

enum SPI_SS {NONE, GYRO, XM};

class CommInterfaces : public Initiator
{
private:
	SPI_SS selectedSlave;
public:
	CommInterfaces() : selectedSlave(NONE) {};

	int selectSPISlave(SPI_SS select);
	void disableSPISlaves();

	void init();
};

extern CommInterfaces comm;
extern HAL_SPI spi_bus;
extern HAL_I2C i2c_bus;
extern HAL_UART bt_uart;

#endif /* COMMINTERFACES_H_ */
