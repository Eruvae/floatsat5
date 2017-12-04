/*
 * DebugTC.h
 *
 *  Created on: 04.12.2017
 *      Author: TobiZ
 */

#ifndef DEBUGTC_H_
#define DEBUGTC_H_

#include <rodos.h>

class DebugTC : public Thread
{
public:
	DebugTC();

	void run();
};

#endif /* DEBUGTC_H_ */
