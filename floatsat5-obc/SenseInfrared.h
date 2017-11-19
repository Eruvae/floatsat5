/*
 * SenseInfrared.h
 *
 *  Created on: 19.11.2017
 *      Author: TobiZ
 */

#ifndef SENSEINFRARED_H_
#define SENSEINFRARED_H_

#include "rodos.h"
#include "CommInterfaces.h"

class SenseInfrared : public Thread
{
public:
	SenseInfrared();

	void init(){}
	void run();
};

#endif /* SENSEINFRARED_H_ */
