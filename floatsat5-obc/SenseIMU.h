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
private:
	int16_t gyro_offset[3];
public:
	SenseIMU();

	void initIMU();
	int initGyro();
	void readGyro(int16_t data[], bool use_offset = true);
	void calibrateGyro();
	void run();
};

#endif /* SENSEIMU_H_ */
