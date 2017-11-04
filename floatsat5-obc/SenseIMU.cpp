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

// Sensitivities
#define GYRO_FACTOR_245DPS		0.00875	// dps/digit
#define GYRO_FACTOR_500DPS		0.0175	// dps/digit
#define GYRO_FACTOR_2000DPS		0.07	// dps/digit

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

int SenseIMU::initGyro()
{
	uint8_t write_ctrl[6] = { CTRL_REG1_G | MS_FLAG, 0x4F, 0x00, 0x00, 0x40, 0x00};
	/*
	 * CTRL_REG1_G: 0x4F == 0b01001111 - 190Hz / 12.5 cutoff, normal mode, all axes enabled
	 * sets output data rate, bandwidth, power-down and enables
	 * Bits[7:0]: DR1 DR0 BW1 BW0 PD Zen Xen Yen
	 * DR[1:0] - Output data rate selection
	 * 			00=95Hz, 01=190Hz, 10=380Hz, 11=760Hz
	 * BW[1:0] - Bandwidth selection (sets cutoff frequency)
	 * 			Value depends on ODR. See datasheet table 21.
	 * PD - Power down enable (0=power down mode, 1=normal or sleep mode)
	 * Zen, Xen, Yen - Axis enable (0=disabled, 1=enabled)
	 *
	 * CTRL_REG2_G: 0x00
	 * Bits[7:0]: 0 0 HPM1 HPM0 HPCF3 HPCF2 HPCF1 HPCF0
	 * HPM[1:0] - High pass filter mode selection
	 * 			00=normal (reset reading HP_RESET_FILTER, 01=ref signal for filtering,
	 * 			10=normal, 11=autoreset on interrupt
	 * HPCF[3:0] - High pass filter cutoff frequency
	 * 			Value depends on data rate. See datasheet table 26.
	 *
	 * 	CTRL_REG3_G: 0x00
	 * 	Bits[7:0]: I1_IINT1 I1_BOOT H_LACTIVE PP_OD I2_DRDY I2_WTM I2_ORUN I2_EMPTY
	 * 	I1_INT1 - Interrupt enable on INT_G pin (0=disable, 1=enable)
	 * 	I1_BOOT - Boot status available on INT_G (0=disable, 1=enable)
	 * 	H_LACTIVE - Interrupt active configuration on INT_G (0:high, 1:low)
	 * 	PP_OD - Push-pull/open-drain (0=push-pull, 1=open-drain)
	 * 	I2_DRDY - Data ready on DRDY_G (0=disable, 1=enable)
	 * 	I2_WTM - FIFO watermark interrupt on DRDY_G (0=disable 1=enable)
	 * 	I2_ORUN - FIFO overrun interrupt on DRDY_G (0=disable 1=enable)
	 * 	I2_EMPTY - FIFO empty interrupt on DRDY_G (0=disable 1=enable)
	 *
	 * 	CTRL_REG4_G: 0x40 == 0b01000000 - continuous, little endian, 245dps, no self-test, 4 wire
	 * 	Bits[7:0] - BDU BLE FS1 FS0 - ST1 ST0 SIM
	 * 	BDU - Block data update (0=continuous, 1=output not updated until read
	 * 	BLE - Big/little endian (0=data LSB @ lower address, 1=LSB @ higher add)
	 * 	FS[1:0] - Full-scale selection
	 * 			00=245dps, 01=500dps, 10=2000dps, 11=2000dps
	 * 	ST[1:0] - Self-test enable
	 * 			00=disabled, 01=st 0 (x+, y-, z-), 10=undefined, 11=st 1 (x-, y+, z+)
	 * 	SIM - SPI serial interface mode select
	 * 			0=4 wire, 1=3 wire
	 *
	 * 	CTRL_REG5_G: 0x00
	 * 	Bits[7:0] - BOOT FIFO_EN - HPen INT1_Sel1 INT1_Sel0 Out_Sel1 Out_Sel0
	 * 	BOOT - Reboot memory content (0=normal, 1=reboot)
	 * 	FIFO_EN - FIFO enable (0=disable, 1=enable)
	 * 	HPen - HPF enable (0=disable, 1=enable)
	 * 	INT1_Sel[1:0] - Int 1 selection configuration
	 * 	Out_Sel[1:0] - Out selection configuration
	 */
	comm.selectSPISlave(GYRO);
	if (spi_bus.write(write_ctrl, 6) < 0)
		return -1;
	comm.disableSPISlaves();
	return 0;
}

void SenseIMU::readGyro(int16_t data[], bool use_offset)
{
	comm.selectSPISlave(GYRO);
	uint8_t readCom = OUT_GYRO | READ_FLAG | MS_FLAG;
	spi_bus.writeRead(&readCom, 1, (uint8_t*)data, 6);
	comm.disableSPISlaves();

	if (use_offset)
		for (int i = 0; i < 3; i++) data[i] -= gyro_offset[i];
}

void SenseIMU::calibrateGyro()
{
	setPeriodicBeat(0, 1*MILLISECONDS);
	int32_t sum[3] = {0};
	int16_t tmp[3];
	const int CALIB_COUNT = 1000;
	for (int i = 0; i < CALIB_COUNT; i++)
	{
		readGyro(tmp, false);
		for (int j = 0; j < 3; j++) sum[j] += tmp[j];
		suspendUntilNextBeat();
	}

	for (int i = 0; i < 3; i++)
		gyro_offset[i] = sum[i] / CALIB_COUNT;
}

void SenseIMU::run()
{
	setPeriodicBeat(0, 100*MILLISECONDS);
	while (initGyro() < 0)
	{
		suspendUntilNextBeat();
	}
	calibrateGyro();
	setPeriodicBeat(0, 100*MILLISECONDS);
	while(1)
	{
		/*uint8_t read_wami = WHO_AM_I_G | READ_FLAG;
		comm.selectSPISlave(GYRO);
		uint8_t res;
		spi_bus.writeRead(&read_wami, 1, &res, 1);
		comm.disableSPISlaves();
		PRINTF("Gyro WHO_AM_I: %x\n", res);*/
		int16_t gyroData[3];
		readGyro(gyroData);
		PRINTF("Gyro Raw: %f, %f, %f\n", gyroData[0]*GYRO_FACTOR_245DPS, gyroData[1]*GYRO_FACTOR_245DPS, gyroData[2]*GYRO_FACTOR_245DPS);
		suspendUntilNextBeat();
	}
}
