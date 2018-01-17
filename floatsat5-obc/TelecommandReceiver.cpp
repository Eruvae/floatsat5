/*
 * Telecommand.cpp
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#include "TelecommandReceiver.h"
#include "Topics.h"
#include "ActuatorInterfaces.h"

// TC IDs
#define CALIB_IMU 0x00
#define SEND_POSE 0x01
#define SEND_RW_SPEED 0x02
#define THRUSTER_CONTROL 0x03
#define ACTIVATE_CONTROLLER 0x04

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
		/*print_debug_msg("Pose command received: %f, %f, %f, %f, %f, %f\n"
				, data.data.pose.x, data.data.pose.y, data.data.pose.z
				, data.data.pose.yaw, data.data.pose.pitch, data.data.pose.roll);*/

		tcTargetPose.put(data.data.pose);
	}
	else if (data.id == SEND_RW_SPEED)
	{
		int16_t speed = data.data.wheel_target_speed;
		tcReactionWheelTargetSpeed.put(speed);
	}
	else if (data.id == THRUSTER_CONTROL)
	{
		actuatorInterfaces.setThrusterStatus(1, (data.data.valveControl & 0b1) == 0b1);
		actuatorInterfaces.setThrusterStatus(2, (data.data.valveControl & 0b10) == 0b10);
		actuatorInterfaces.setThrusterStatus(3, (data.data.valveControl & 0b100) == 0b100);
	}
	else if (data.id == ACTIVATE_CONTROLLER)
	{
		tcActivateController.put(data.data.boolData);
	}
}
