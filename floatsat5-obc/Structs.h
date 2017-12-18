/*
 * Structs.h
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

// IMU Sensitivities
#define GYRO_FACTOR_245DPS		(245.0/INT16_MAX) //0.00875		// dps/digit
#define GYRO_FACTOR_500DPS		(500.0/INT16_MAX) //0.0175
#define GYRO_FACTOR_2000DPS		(2000.0/INT16_MAX) //0.07
#define ACC_FACTOR_2G			(2.0/INT16_MAX) //0.000081	// g/LSB
#define ACC_FACTOR_4G			(4.0/INT16_MAX) //0.000122
#define ACC_FACTOR_6G			(6.0/INT16_MAX) //0.000183
#define ACC_FACTOR_8G			(8.0/INT16_MAX) //0.000244
#define ACC_FACTOR_16G			(16.0/INT16_MAX) //0.000732
#define MAG_FACTOR_2GA			(2.0/INT16_MAX) //0.00008		// gauss/LSB
#define MAG_FACTOR_4GA			(4.0/INT16_MAX) //0.00016
#define MAG_FACTOR_8GA			(8.0/INT16_MAX) //0.00032
#define MAG_FACTOR_12GA			(12.0/INT16_MAX) //0.00048
#define TEMP_FACTOR				0.125

// Put all structs here, especially structs for Topics

struct __attribute__((packed)) IMUData
{
	int16_t gyro_x, gyro_y, gyro_z;
	int16_t acc_x, acc_y, acc_z;
	int16_t mag_x, mag_y, mag_z;
	int16_t temp;
};

struct __attribute__((packed)) IRData
{
	uint8_t range1;
	uint8_t range2;
	float distance;
	float angle;
};

struct __attribute__((packed)) PowerData
{
	int16_t batt_voltage;
	int16_t batt_current;
};

// Structs for Telecommunication; always use __attribute__((packed))

enum IMUCommand
{
	CALIB_GYRO = 0, CALIB_ACC, CALIB_MAG
};

struct __attribute__((packed)) Pose
{
	float x, y, z;
	float yaw, pitch, roll;
	float dyaw, dpitch, droll;
};

union __attribute__((packed)) TCdata
{
	IMUCommand imu_com;
	Pose pose;
	int16_t wheel_target_speed;
};

struct __attribute__((packed)) Telecommand
{
	uint8_t id;
	TCdata data;
	//float data;
	//char id;
};

#endif /* STRUCTS_H_ */
