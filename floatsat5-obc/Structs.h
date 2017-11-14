/*
 * Structs.h
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

// Put all structs here, especially structs for Topics

struct IMUData
{
	int16_t roll, pitch, yaw;
	int16_t accX, accY, accZ;
	int16_t magX, magY, magZ;
};

// Structs for Telecommunication; always use __attribute__((packed))

struct __attribute__((packed)) PoseData
{
	uint8_t sync;
	uint8_t id;
	uint32_t time;

	int16_t px;
	int16_t py;
	int16_t pz;
	int16_t yaw;
	int16_t pitch;
	int16_t roll;

	uint32_t check;
};

#endif /* STRUCTS_H_ */
