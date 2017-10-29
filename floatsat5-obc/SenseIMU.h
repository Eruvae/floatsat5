/*
 * SenseIMU.h
 *
 *  Created on: 29.10.2017
 *      Author: TobiZ
 */

#ifndef SENSEIMU_H_
#define SENSEIMU_H_

#include "rodos.h"
#include "CommInterfaces.h"

class SenseIMU : public Thread
{
public:
	SenseIMU();

	void initIMU();
	void run();
};

#endif /* SENSEIMU_H_ */
