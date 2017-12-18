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

#define MOD(x, min, max) \
	do { \
		while((x) > (max)) x -= ((max) - (min)); \
		while((x) < (min)) x += ((max) - (min)); \
	} while(0)

#define MAXDIF_PI(x, y) \
	do { \
		if ((x) - (y) > M_PI) (y) += 2*M_PI; \
		else if ((y) - (x) > M_PI) (x) += 2*M_PI; \
	} while(0)

PoseFilter poseFilter;

PoseFilter::PoseFilter() : imuDataSub(itImuData, imuDataBuffer)
{
}

void PoseFilter::run()
{
	double delt = 0.05;
	double alpha = 0.9;
	double oldFilteredRoll = 0, oldFilteredPitch = 0, oldFilteredHeading = 0;
	double filteredRoll = 0, filteredPitch = 0, filteredHeading = 0;
	setPeriodicBeat(15*MILLISECONDS, delt*SECONDS);
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

		double pitchGyro = filteredPitch + dp*delt;
		MOD(pitchGyro, -M_PI, M_PI);
		double rollGyro = filteredRoll + dr*delt;
		MOD(rollGyro, -M_PI, M_PI);
		double headingGyro = filteredHeading + dh*delt;
		MOD(headingGyro, -M_PI, M_PI);


		MAXDIF_PI(pitchGyro, pitchAccMag);
		filteredPitch = alpha * pitchGyro + (1 - alpha) * pitchAccMag;
		MOD(filteredPitch, -M_PI, M_PI);
		MAXDIF_PI(rollGyro, rollGyro);
		filteredRoll = alpha * rollGyro + (1 - alpha) * rollGyro;
		MOD(filteredRoll, -M_PI, M_PI);
		MAXDIF_PI(headingGyro, headingAccMag);
		filteredHeading = alpha * headingGyro + (1 - alpha) * headingAccMag;
		MOD(filteredHeading, -M_PI, M_PI);

		double dpitch = (filteredPitch - oldFilteredPitch) / delt;
		double droll = (filteredRoll - oldFilteredRoll) / delt;
		double dyaw = (filteredHeading - oldFilteredHeading) / delt;

		Pose pose;
		pose.x = 0;
		pose.y = 0;
		pose.z = 0;
		pose.pitch = filteredPitch*180.0/M_PI;
		pose.roll = filteredRoll*180.0/M_PI;
		pose.yaw = filteredHeading*180.0/M_PI;
		pose.dpitch = dpitch*180/M_PI;
		pose.droll = droll*180/M_PI;
		pose.dyaw = dyaw*180/M_PI;

		itFilteredPose.publish(pose);

		suspendUntilNextBeat();
	}
}
