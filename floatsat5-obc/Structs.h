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

#endif /* STRUCTS_H_ */
