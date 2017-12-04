/*
 * DebugTC.cpp
 *
 *  Created on: 04.12.2017
 *      Author: TobiZ
 */

#include "DebugTC.h"
#include "CommInterfaces.h"

DebugTC debugTC;

DebugTC::DebugTC()
{
	// TODO Auto-generated constructor stub

}

void DebugTC::run()
{
	while(1)
	{
		uart_stdout.suspendUntilDataReady();
		while(uart_stdout.isDataReady())
			PRINTF("%c", uart_stdout.getcharNoWait());
	}
}
