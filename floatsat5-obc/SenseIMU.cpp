/*
 * SenseIMU.cpp
 *
 *  Created on: 29.10.2017
 *      Author: TobiZ
 */

#include "SenseIMU.h"

#define READ_FLAG		0x80
#define MS_FLAG			0x40

// Gyro registers
#define WHO_AM_I_G		0x0F	// r
#define CTRL_REG1_G		0x20	// rw, reg. 2-5 follow
#define OUT_GYRO		0x28	// r, 6 bytes

// Acc/Mag registers
#define OUT_TEMP		0x05	// r, 2 bytes
#define OUT_MAG			0x08	// r, 6 bytes
#define CTRL_REG0_XM	0x1F	// rw, reg 1-7 follow
#define OUT_ACC			0x28	// r, 6 bytes

SenseIMU senseIMU;

SenseIMU::SenseIMU()
{
	// TODO Auto-generated constructor stub
}

void SenseIMU::initIMU()
{
	comm.selectSPISlave(GYRO);
	// TODO: config gyro
	comm.disableSPISlaves();
	comm.selectSPISlave(ACC);
	// TODO: config acc/mag
	comm.disableSPISlaves();
}

void SenseIMU::run()
{

}
