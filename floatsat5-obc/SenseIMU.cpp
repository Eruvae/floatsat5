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
#define WHO_AM_I_XM		0x0F	// r
#define OUT_TEMP		0x05	// r, 2 bytes
#define OUT_MAG			0x08	// r, 6 bytes
#define CTRL_REG0_XM	0x1F	// rw, reg 1-7 follow
#define OUT_ACC			0x28	// r, 6 bytes

// Sensitivities
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

SenseIMU senseIMU;

SenseIMU::SenseIMU()
{
}

int SenseIMU::initGyro()
{
	//												 REG1  REG2  REG3  REG4  REG5
	uint8_t write_ctrl[6] = { CTRL_REG1_G | MS_FLAG, 0x4F, 0x00, 0x00, 0xA0, 0x00};
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
	 * 	CTRL_REG4_G: 0xA0 == 0b10100000 - continuous, little endian, 245dps, no self-test, 4 wire
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
	int ret = spi_bus.write(write_ctrl, 6);
	comm.disableSPISlaves();
	return ret;
}

int SenseIMU::initXM()
{
	//												  REG0  REG1  REG2  REG3  REG4  REG5  REG6  REG7
	uint8_t write_ctrl[9] = { CTRL_REG0_XM | MS_FLAG, 0x00, 0x7F, 0xC0, 0x00, 0x00, 0x94, 0x00, 0x80};
	/* CTRL_REG0_XM (Default value: 0x00)
	 * Bits (7-0): BOOT FIFO_EN WTM_EN 0 0 HP_CLICK HPIS1 HPIS2
	 * BOOT - Reboot memory content (0: normal, 1: reboot)
	 * FIFO_EN - Fifo enable (0: disable, 1: enable)
	 * WTM_EN - FIFO watermark enable (0: disable, 1: enable)
	 * HP_CLICK - HPF enabled for click (0: filter bypassed, 1: enabled)
	 * HPIS1 - HPF enabled for interrupt generator 1 (0: bypassed, 1: enabled)
	 * HPIS2 - HPF enabled for interrupt generator 2 (0: bypassed, 1 enabled)
	 *
	 * CTRL_REG1_XM (Default value: 0x07)
	 * Bits (7-0): AODR3 AODR2 AODR1 AODR0 BDU AZEN AYEN AXEN
	 * AODR[3:0] - select the acceleration data rate:
	 * 			0000=power down, 0001=3.125Hz, 0010=6.25Hz, 0011=12.5Hz,
	 * 			0100=25Hz, 0101=50Hz, 0110=100Hz, 0111=200Hz, 1000=400Hz,
	 * 			1001=800Hz, 1010=1600Hz, (remaining combinations undefined).
	 * BDU - block data update for accel AND mag
	 * 			0: Continuous update
	 * 			1: Output registers aren't updated until MSB and LSB have been read.
	 * AZEN, AYEN, and AXEN - Acceleration x/y/z-axis enabled.
	 * 			0: Axis disabled, 1: Axis enabled
	 *
	 * CTRL_REG2_XM (Default value: 0x00)
	 * Bits (7-0): ABW1 ABW0 AFS2 AFS1 AFS0 AST1 AST0 SIM
	 * ABW[1:0] - Accelerometer anti-alias filter bandwidth
	 * 			00=773Hz, 01=194Hz, 10=362Hz, 11=50Hz
	 * AFS[2:0] - Accel full-scale selection
	 * 			000=+/-2g, 001=+/-4g, 010=+/-6g, 011=+/-8g, 100=+/-16g
	 * AST[1:0] - Accel self-test enable
	 * 			00=normal (no self-test), 01=positive st, 10=negative st, 11=not allowed
	 * SIM - SPI mode selection
	 * 			0=4-wire, 1=3-wire
	 *
	 * CTRL_REG3_XM is used to set interrupt generators on INT1_XM
	 * Bits (7-0): P1_BOOT P1_TAP P1_INT1 P1_INT2 P1_INTM P1_DRDYA P1_DRDYM P1_EMPTY
	 *
	 * CTRL_REG4_XM is used to set interrupt generators on INT2_XM
	 * Bits (7-0): P2_TAP P2_INT1 P2_INT2 P2_INTM P2_DRDYA P2_DRDYM P2_Overrun P2_WTM
	 *
	 * CTRL_REG5_XM enables temp sensor, sets mag resolution and data rate
	 * Bits (7-0): TEMP_EN M_RES1 M_RES0 M_ODR2 M_ODR1 M_ODR0 LIR2 LIR1
	 * TEMP_EN - Enable temperature sensor (0=disabled, 1=enabled)
	 * M_RES[1:0] - Magnetometer resolution select (0=low, 3=high)
	 * M_ODR[2:0] - Magnetometer data rate select
	 * 			000=3.125Hz, 001=6.25Hz, 010=12.5Hz, 011=25Hz, 100=50Hz, 101=100Hz
	 * LIR2 - Latch interrupt request on INT2_SRC (cleared by reading INT2_SRC)
	 * 			0=interrupt request not latched, 1=interrupt request latched
	 * LIR1 - Latch interrupt request on INT1_SRC (cleared by readging INT1_SRC)
	 * 			0=irq not latched, 1=irq latched
	 *
	 * CTRL_REG6_XM sets the magnetometer full-scale
	 * Bits (7-0): 0 MFS1 MFS0 0 0 0 0 0
	 * MFS[1:0] - Magnetic full-scale selection
	 * 			00:+/-2Gauss, 01:+/-4Gs, 10:+/-8Gs, 11:+/-12Gs
	 *
	 * CTRL_REG7_XM sets magnetic sensor mode, low power mode, and filters
	 * AHPM1 AHPM0 AFDS 0 0 MLP MD1 MD0
	 * AHPM[1:0] - HPF mode selection
	 * 			00=normal (resets reference registers), 01=reference signal for filtering,
	 * 			10=normal, 11=autoreset on interrupt event
	 * AFDS - Filtered acceleration data selection
	 * 			0=internal filter bypassed, 1=data from internal filter sent to FIFO
	 * MLP - Magnetic data low-power mode
	 * 			0=data rate is set by M_ODR bits in CTRL_REG5
	 * 			1=data rate is set to 3.125Hz
	 * MD[1:0] - Magnetic sensor mode selection (default 10)
	 * 			00=continuous-conversion, 01=single-conversion, 10 and 11=power-down
	 */
	comm.selectSPISlave(XM);
	int ret = spi_bus.write(write_ctrl, 9);
	comm.disableSPISlaves();
	return ret;
}

void SenseIMU::readGyro(int16_t *data, bool use_offset)
{
	uint8_t readCom = OUT_GYRO | READ_FLAG | MS_FLAG;
	comm.selectSPISlave(GYRO);
	spi_bus.write(&readCom, 1);
	spi_bus.read((uint8_t*)data, 6);
	comm.disableSPISlaves();

	if (use_offset)
		for (int i = 0; i < 3; i++) data[i] -= gyro_offset[i];
}

void SenseIMU::readAcc(int16_t *data, bool use_offset)
{
	uint8_t readCom = OUT_ACC | READ_FLAG | MS_FLAG;
	comm.selectSPISlave(XM);
	spi_bus.write(&readCom, 1);
	spi_bus.read((uint8_t*)data, 6);
	comm.disableSPISlaves();

	if (use_offset)
		for (int i = 0; i < 3; i++) data[i] -= acc_offset[i];

}

void SenseIMU::readMag(int16_t *data, bool use_offset)
{
	uint8_t readCom = OUT_MAG | READ_FLAG | MS_FLAG;
	comm.selectSPISlave(XM);
	spi_bus.write(&readCom, 1);
	spi_bus.read((uint8_t*)data, 6);
	comm.disableSPISlaves();

	if (use_offset)
		for (int i = 0; i < 3; i++) data[i] -= mag_offset[i];

}

void SenseIMU::readTemp(int16_t *data)
{
	uint8_t readCom = OUT_TEMP | READ_FLAG | MS_FLAG;
	comm.selectSPISlave(XM);
	spi_bus.write(&readCom, 1);
	spi_bus.read((uint8_t*)data, 2);
	comm.disableSPISlaves();

	*data = (*data & 0x800) ? (-1 ^ 0xFFF) | *data : *data; // oonvert 12 bit signed to 16 bit signed
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

void SenseIMU::calibrateAcc()
{
	setPeriodicBeat(0, 1*MILLISECONDS);
	int32_t sum[3] = {0};
	int16_t tmp[3];
	const int CALIB_COUNT = 1000;
	for (int i = 0; i < CALIB_COUNT; i++)
	{
		readAcc(tmp, false);
		for (int j = 0; j < 3; j++) sum[j] += tmp[j];
		suspendUntilNextBeat();
	}

	acc_offset[0] = sum[0] / CALIB_COUNT;
	acc_offset[1] = sum[1] / CALIB_COUNT;
	acc_offset[2] = sum[2] / CALIB_COUNT + 1.0/ACC_FACTOR_2G; // if discovery board up, else replace + with -
}

void SenseIMU::calibrateMag()
{
	// TODO; Board/Satellite has to be moved while calibrating
}

void SenseIMU::run()
{
	uint8_t read_waig = WHO_AM_I_G | READ_FLAG;
	comm.selectSPISlave(GYRO);
	uint8_t waig;
	spi_bus.write(&read_waig, 1);
	spi_bus.read(&waig, 1);
	comm.disableSPISlaves();

	uint8_t read_waix = WHO_AM_I_XM | READ_FLAG;
	comm.selectSPISlave(XM);
	uint8_t waix;
	spi_bus.write(&read_waix, 1);
	spi_bus.read(&waix, 1);
	comm.disableSPISlaves();

	initGyro();
	initXM();
	calibrateGyro();
	calibrateAcc();
	setPeriodicBeat(0, 100*MILLISECONDS);
	while(1)
	{
		PRINTF("Gyro WHO_AM_I: %d\n", waig);

		PRINTF("XM WHO_AM_I: %d\n", waix);

		int16_t gyroData[3], accData[3], magData[3], tempData;
		readGyro(gyroData);
		readAcc(accData);
		readMag(magData, false);
		readTemp(&tempData);
		PRINTF("Gyro Raw: %f, %f, %f\n", gyroData[0]*GYRO_FACTOR_245DPS, gyroData[1]*GYRO_FACTOR_245DPS, gyroData[2]*GYRO_FACTOR_245DPS);
		PRINTF("Acc Raw: %f, %f, %f\n", accData[0]*ACC_FACTOR_2G, accData[1]*ACC_FACTOR_2G, accData[2]*ACC_FACTOR_2G);
		PRINTF("Mag Raw: %f, %f, %f\n\n", magData[0]*MAG_FACTOR_2GA, magData[1]*MAG_FACTOR_2GA, magData[2]*MAG_FACTOR_2GA);
		PRINTF("Temperature: %f\n", tempData*TEMP_FACTOR);
		suspendUntilNextBeat();
	}
}
