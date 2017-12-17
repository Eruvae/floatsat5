/*
 * Telecommand.cpp
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#include "TelecommandReceiver.h"
#include "Topics.h"

// TC IDs
#define CALIB_IMU 0x00
#define SEND_POSE 0x01
#define SEND_RW_SPEED 0x02

TelecommandReceiver telecommandReceiver;

void TelecommandReceiver::put(Telecommand &data)
{
	if (data.id == CALIB_IMU)
	{
		PRINTF("Calibrate command received (%d)\n", data.data.imu_com);
		tcImuCommand.put(data.data.imu_com);
	}
	else if (data.id == SEND_POSE)
	{
		PRINTF("Pose command received: %f, %f, %f, %f, %f, %f\n"
				, data.data.pose.x, data.data.pose.y, data.data.pose.z
				, data.data.pose.yaw, data.data.pose.pitch, data.data.pose.roll);
	}
	else if (data.id == SEND_RW_SPEED)
	{
		int16_t speed = data.data.wheel_target_speed;
		tcReactionWheelTargetSpeed.put(speed);
	}
}
