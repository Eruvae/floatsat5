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
	int16_t acc_offset[3];
	int16_t mag_offset[3];
public:
	SenseIMU();

	int initGyro();
	int initXM();
	void readGyro(int16_t data[], bool use_offset = true);
	void readAcc(int16_t data[], bool use_offset = true);
	void readMag(int16_t data[], bool use_offset = true);
	void calibrateGyro();
	void calibrateAcc();
	void calibrateMag();
	void run();
};

#endif /* SENSEIMU_H_ */
