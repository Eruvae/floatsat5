/*
 * PoseFilter.cpp
 *
 *  Created on: 04.12.2017
 *      Author: TobiZ
 */

#include "PoseFilter.h"

#include <cmath>



PoseFilter poseFilter;

PoseFilter::PoseFilter() : imuDataSub(itImuData, imuDataBuffer),
		starTrackerPoseSub(itStarTrackerPose, starTrackerPoseBuffer),
		radioPositionSub(itRadioPosition, radioPositionBuffer),
		otDataSub(itObjectTrackingPose, otDataBuffer),
		raspiStatusSub(itRaspiStatus, raspiStatusBuffer)
{
}

void PoseFilter::run()
{
	double delt = 0.05;
	double alpha = 0.9;
	double oldFilteredRoll = 0, oldFilteredPitch = 0, oldFilteredHeading = 0;
	double filteredRoll = 0, filteredPitch = 0, filteredHeading = 0;
	float otX = 0, otY = 0, otYaw = 0, otOldYaw = 0, dotYaw = 0;
	float stX = 0, stY = 0, stYaw = 0;
	float oldstYaw = 0;
	float magnetometerYawOffset = 60.f;
	float oldX = 0, oldY = 0;
	setPeriodicBeat(15*MILLISECONDS, delt*SECONDS);
	while(1)
	{
		/*double rota[3][3] = { {0, 1, 0}, {1, 0, 0}, {0, 0, 1} };
		Matrix<3> rot(rota);
		double veca[3] = { 1, 2, 3 };
		Vector<3> vec(veca);

		Vector<3> res = rot * vec;

		PRINTF("Test: %f, %f, %f", res[0][0], res[1][0], res[2][0]);*/

		// ---------------------- Attitude Determination ----------------------------

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

		double headingAccMag = -atan2(myh, mxh);

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
		MAXDIF_PI(rollGyro, rollAccMag);
		filteredRoll = alpha * rollGyro + (1 - alpha) * rollAccMag;
		MAXDIF_PI(headingGyro, headingAccMag);
		filteredHeading = alpha * headingGyro + (1 - alpha) * headingAccMag;

		double dpitch = (filteredPitch - oldFilteredPitch);
		MOD(dpitch, -M_PI, M_PI);
		dpitch /= delt;
		double droll = (filteredRoll - oldFilteredRoll);
		MOD(droll, -M_PI, M_PI);
		droll /= delt;
		double dyaw = (filteredHeading - oldFilteredHeading);
		MOD(dyaw, -M_PI, M_PI);
		dyaw /= delt;

		MOD(filteredPitch, -M_PI, M_PI);
		MOD(filteredRoll, -M_PI, M_PI);
		MOD(filteredHeading, -M_PI, M_PI);

		oldFilteredPitch = filteredPitch;
		oldFilteredRoll = filteredRoll;
		oldFilteredHeading = filteredHeading;

		// -------------------- Position Determination -------------------------

		RaspiStatus rpiStatus;
		raspiStatusBuffer.get(rpiStatus);

		Pose2D stPose;
		starTrackerPoseBuffer.get(stPose);

		RadioPosition rdPose;
		radioPositionBuffer.get(rdPose);

		float stdYaw = (stPose.yaw - oldstYaw) / delt;
		MOD(stdYaw, -180, 180);
		oldstYaw = stPose.yaw;

		//print_debug_msg("ObjYaw: %f; MagYaw: %f", otYaw*180.0/M_PI, filteredHeading*180.0/M_PI);

		Pose pose;
		if (rpiStatus.stEnabled)
		{
			pose.x = stPose.x; //otX;
			pose.y = -stPose.y; //otY;
		}
		else if (rpiStatus.rdEnabled)
		{
			pose.x = rdPose.x1;
			pose.y = rdPose.y1;
		}
		else
		{
			pose.x = oldX;
			pose.y = oldY;
		}
		oldX = pose.x;
		oldY = pose.y;
		pose.z = 0;
		pose.pitch = filteredPitch*180.0/M_PI;
		pose.roll = filteredRoll*180.0/M_PI;

		if (rpiStatus.stEnabled)
		{
			pose.yaw = stPose.yaw - 90.f;/*otYaw*180.0/M_PI;*//*filteredHeading*180.0/M_PI;*/
			magnetometerYawOffset = pose.yaw - filteredHeading*180.0/M_PI;
		}
		else
		{
			pose.yaw = filteredHeading*180.0/M_PI + magnetometerYawOffset;
		}
		MOD(pose.yaw, -180, 180);
		pose.dpitch = dpitch*180/M_PI;
		pose.droll = droll*180/M_PI;
		pose.dyaw = gyro[2]*180/M_PI;/*stdYaw;*///dyaw*180/M_PI;

		itFilteredPose.publish(pose);

		// DEBUG:
		/*OTData otData;
		otDataBuffer.get(otData);

		if (otData.found)
		{
			float r = otData.g0 + 0.2;
			otYaw = M_PI + otData.alpha + atan2(otData.G0, r);
			float angle = otData.alpha + M_PI - otYaw + pose.yaw*M_PI/180;
			otX = r * cos(angle);
			otY = r * sin(angle);
			MOD(otYaw, -M_PI, M_PI);

			print_debug_msg("OTdata: %.2f, %.2f, %.2f, %.2f, %.2f, %d", r, otX, otY, otYaw, angle, otData.found);
		}*/


		suspendUntilNextBeat();
	}
}
