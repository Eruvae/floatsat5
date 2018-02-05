/*
 * Telecommand.cpp
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#include "TelecommandReceiver.h"
#include "Topics.h"
#include "ActuatorInterfaces.h"
#include "RaspiComm.h"

// TC IDs
#define CALIB_IMU 0x00
#define SEND_POSE 0x01
#define SEND_RW_SPEED 0x02
#define THRUSTER_CONTROL 0x03
#define DEACTIVATE_CONTROLLER 0x04
#define CHANGE_PC_MODE	0x05
#define RPI_COMMAND	0x06
#define SEND_CONTROL_PARAMS 0x07
#define SEND_ROTATION_SPEED 0x08
#define SEND_POSE_TO_LIST 0x09

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

		//tcTargetPose.put(data.data.pose);
		tcTargetPose.put(data.data.pose);
		//itPoseControllerMode.publishConst(PoseControllerMode::FOLLOW_TRAJECTORY);
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
	else if (data.id == DEACTIVATE_CONTROLLER)
	{
		itPoseControllerMode.publishConst(PoseControllerMode::STANDBY);
		int16_t speed = 0;
		tcReactionWheelTargetSpeed.put(speed);
		//tcActivateController.put(data.data.boolData); // legacy

	}
	else if (data.id == CHANGE_PC_MODE)
	{
		PoseControllerMode mode = static_cast<PoseControllerMode>(data.data.pcMode);
		itPoseControllerMode.publish(mode);
	}
	else if (data.id == RPI_COMMAND)
	{
		//uint8_t *tmp = (uint8_t*)(&(data.data.rpiComData.command));
		//print_debug_msg("Bytes: %x, %x, %x, %x, %x, %x, %x, %x",tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7]);
		print_debug_msg("En: %d", data.data.rpiComData.enable);
		//print_debug_msg("Sizes: %d, %d", sizeof(RaspiCommand), sizeof(bool));
		raspiComm.sendCommand(static_cast<RaspiCommand>(data.data.rpiComData.command), data.data.rpiComData.enable);
	}
	else if (data.id == SEND_CONTROL_PARAMS)
	{
		tcControlParams.put(data.data.controlParams);
	}
	else if (data.id == SEND_ROTATION_SPEED)
	{
		float rotationSpeed = data.data.rotationSpeed;
		desiredRotationSpeed.put(rotationSpeed);
		itPoseControllerMode.publishConst(PoseControllerMode::ROTATE);
	}
	else if (data.id == SEND_POSE_TO_LIST)
	{
		static int i = 0;
		print_debug_msg("Pose rec: %d", ++i);
		tcNextTargetPoseList.put(data.data.pose);
	}
}
