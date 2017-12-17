/*
 * PoseFilter.cpp
 *
 *  Created on: 04.12.2017
 *      Author: TobiZ
 */

#include "PoseFilter.h"

#include <cmath>

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

PoseFilter poseFilter;

PoseFilter::PoseFilter() : imuDataSub(itImuData, imuDataBuffer)
{
}

void PoseFilter::run()
{
	double delt = 100;
	double alpha = 0.9;
	double filteredRoll = 0, filteredPitch = 0, filteredHeading = 0;
	setPeriodicBeat(15*MILLISECONDS, delt*MILLISECONDS);
	while(1)
	{
		/*double rota[3][3] = { {0, 1, 0}, {1, 0, 0}, {0, 0, 1} };
		Matrix<3> rot(rota);
		double veca[3] = { 1, 2, 3 };
		Vector<3> vec(veca);

		Vector<3> res = rot * vec;

		PRINTF("Test: %f, %f, %f", res[0][0], res[1][0], res[2][0]);*/

		IMUData imuData;
		imuDataBuffer.get(imuData);

		double gyro[3], acc[3], mag[3];
		for (int i = 0; i < 3; i++)
		{
			gyro[i] = *(&imuData.gyro_x + i)*(GYRO_FACTOR_2000DPS*M_PI/180.0);
			acc[i] = *(&imuData.acc_x + i)*ACC_FACTOR_2G;
			mag[i] = *(&imuData.mag_x + i)*MAG_FACTOR_2GA;
		}

		double pitchAccMag = atan2(acc[0], sqrt(acc[1]*acc[1]+acc[2]*acc[2]));
		double rollAccMag = atan2(acc[1], sqrt(acc[0]*acc[0]+acc[2]*acc[2]));

		double mxh = mag[0]*cos(filteredPitch) + mag[2]*sin(filteredPitch);
		double myh = mag[0]*sin(filteredRoll)*sin(filteredPitch) + mag[1]*cos(filteredRoll) - mag[2]*sin(filteredRoll)*cos(filteredPitch);

		double headingAccMag = atan2(myh, mxh);

		double dp = (cos(filteredRoll)*cos(filteredPitch)*gyro[1] - sin(filteredRoll)*cos(filteredPitch)*gyro[2]) / cos(filteredPitch);
		double dr = (cos(filteredPitch)*gyro[0] + sin(filteredRoll)*sin(filteredPitch)*gyro[1] + cos(filteredRoll)*sin(filteredPitch)*gyro[2]) / cos(filteredPitch);
		double dh = (sin(filteredRoll)*gyro[1] + cos(filteredRoll)*gyro[2]) / cos(filteredPitch);

		double pitchGyro = fmod(filteredPitch +  dp*delt/1000, 2*M_PI);
		double rollGyro = fmod(filteredRoll + dr*delt/1000, 2*M_PI);
		double headingGyro = fmod(filteredHeading + dh*delt/1000, 2*M_PI);

		filteredPitch = alpha * pitchGyro + (1 - alpha) * pitchAccMag;
		filteredRoll = alpha * rollGyro + (1 - alpha) * rollAccMag;
		filteredHeading = alpha * headingGyro + (1 - alpha) * headingAccMag;

		Pose pose;
		pose.pitch=filteredPitch*180.0/M_PI;
		pose.roll=filteredRoll*180.0/M_PI;
		pose.yaw=filteredHeading*180.0/M_PI;

		itFilteredPose.publish(pose);

		suspendUntilNextBeat();
	}
}
