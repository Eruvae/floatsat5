/*
 * PoseController.h
 *
 *  Created on: 17.01.2018
 *      Author: TobiZ
 */

#include <rodos.h>
#include "Topics.h"

#ifndef POSECONTROLLER_H_
#define POSECONTROLLER_H_

class PoseController : public Thread
{
	const float period = 0.2;
	Pose filteredPose = {0};
	Pose2D targetPose = {0};
	Pose2D targetVel = {0};
	int16_t currentRwSpeed = 0;
	PoseControllerMode mode = PoseControllerMode::STANDBY;
	float oldX = 0, oldY = 0;
	float eX_int = 0, eY_int = 0;
	const static int ERR_ARR_SIZE = 10;
	float eX_arr[ERR_ARR_SIZE] = {0}, eY_arr[ERR_ARR_SIZE] = {0};
	int errIndex = 0;
	float oldYawErr = 0;
	float errYaw_int = 0;
	float errRot_int = 0, old_dYaw = 0;
	float attP = 50.f, attD = 100.f, attI = 2.f;
	float rotP = 100.f, rotD = 0.f, rotI = 10.f;
	float k = 5.f, td = 10.f, ti = 0.5f, gamma = sqrt(3)/2; // trajectory control params

	CommBuffer<Pose> filteredPoseBuffer;
	Subscriber filterePoseSub;

	CommBuffer<int16_t> reactionWheelSpeedBuffer;
	Subscriber reactionWheelSpeedSub;

	//CommBuffer<Pose2D> starTrackerPoseBuffer;
	//Subscriber starTrackerPoseSub;

	CommBuffer<PoseControllerMode> poseControllerModeBuffer;
	Subscriber poseControllerModeSub;

	//CommBuffer<OTData> otDataBuffer;
	//Subscriber otDataSub;

	Pose2D curveLinePos(float s, const Pose2D &start, const Pose2D &end)
	{
		float xDif = end.x - start.x;
		float yDif = end.y - start.y;
		float yawDif = end.yaw - start.yaw;
		MOD(yawDif, -180, 180);
		Pose2D pose = {start.x + s*xDif, start.y + s*yDif, start.yaw + s*yawDif};
		MOD(pose.yaw, -180, 180);
		return pose;
	}

	Pose2D curveLineVel(float s, const Pose2D &start, const Pose2D &end)
	{
		float xDif = end.x - start.x;
		float yDif = end.y - start.y;
		float yawDif = end.yaw - start.yaw;
		MOD(yawDif, -180, 180);
		Pose2D pose = {xDif, yDif, yawDif};
		return pose;
	}

	/*Pose2D curveLineAcc(float s, const Pose2D &start, const Pose2D &end)
	{
		Pose2D pose = {0, 0, 0};
		return pose;
	}*/

	Pose2D curveCirclePos(float s, const Pose2D &center, float r, float betaStart, float betaEnd)
	{
		float betaDif = betaEnd - betaStart;
		float beta = betaStart + betaDif * s;
		Pose2D pose = {center.x + r*cos(beta*M_PI/180), center.y + r*sin(beta*M_PI/180), 180 + beta};
		MOD(pose.yaw, -180, 180);
		return pose;
	}

	Pose2D curveCircleVel(float s, const Pose2D &center, float r, float betaStart, float betaEnd)
	{
		float betaDif = betaEnd - betaStart;
		float beta = betaStart + betaDif * s;
		Pose2D pose = {-r*betaDif*M_PI/180*sin(beta*M_PI/180), r*betaDif*M_PI/180*cos(beta*M_PI/180), betaDif};
		return pose;
	}

	float s(float t, float T)
	{
		if (t <= 0)
			return 0;
		else if (t <= T/2)
			return 2.f / (T*T) * (t*t);
		else if (t < T)
			return -2.f / (T*T) * ((t-T)*(t-T)) + 1;
		else
			return 1;
	}

	float ds(float t, float T)
	{
		if (t <= 0)
			return 0;
		else if (t <= T/2)
			return t / (T*T);
		else if (t < T)
			return -1 / (T*T) * (t-T);
		else
			return 0;
	}

	void controlYaw();
	void controlPosition();
public:
	PoseController();

	void run();
};

#endif /* POSECONTROLLER_H_ */
