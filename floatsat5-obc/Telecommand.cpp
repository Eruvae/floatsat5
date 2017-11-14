/*
 * Telecommand.cpp
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#include "Telecommand.h"

Telecommand::Telecommand()
{
}

void Telecommand::run()
{
	while(1)
	{
		bt_uart.suspendUntilDataReady();
	}
}
