/*
 * SensePower.cpp
 *
 *  Created on: 07.11.2017
 *      Author: TobiZ
 */

#include "SensePower.h"

#include "CommInterfaces.h"
#include "Topics.h"

#define CONFIG_REG		0x00
#define SHUNT_VOLT_REG	0x01
#define BUS_VOLT_REG	0x02
#define POWER_REG		0x03
#define	CURRENT_REG		0x04
#define	CALIB_REG		0x05

#define SHUNT_VOLT_FACTOR	0.01 // mV/bit
#define BUS_VOLT_FACTOR		0.004 // V/bit

SensePower sensePower;

SensePower::SensePower()
{
	// TODO Auto-generated constructor stub

}

void SensePower::initSensor(uint8_t sensorAddr)
{
	uint8_t writeConfig[] = {CONFIG_REG, 0x17, 0xFF};
	/*
	 * 0x17FF == 0b0001011111111111: 16V FSR, +-160mV (PGA :4), 128/68.10ms Bus+Shunt, Bus+Shunt cont. conversion
	 * BIT #	D15 D14 D13	 D12 D11  D10   D9	  D8    D7    D6    D5    D4    D3    D2    D1    D0
	 * BIT NAME RST  —	BRNG PG1 PG0 BADC4 BADC3 BADC2 BADC1 SADC4 SADC3 SADC2 SADC1 MODE3 MODE2 MODE1
	 *
	 * RST: Reset Bit
	 * Bit 15 Setting this bit to '1' generates a system reset that is the same as power-on reset.
	 * 		Resets all registers to default values; this bit self-clears.
	 * BRNG: Bus Voltage Range
	 * Bit 13 0 = 16V FSR
	 * 		  1 = 32V FSR (default value)
	 *
	 * PG: PGA (Shunt Voltage Only)
	 * Bits 11, 12 Sets PGA gain and range. Note that the PGA defaults to ÷8 (320mV range).
	 * Table 5 shows the gain and range for the various product gain settings.
	 *
	 * PG1 PG0 GAIN RANGE
	 *  0   0    1  ±40mV
	 *  0   1   ÷2  ±80mV
	 *  1   0   ÷4  ±160mV
	 *  1   1   ÷8  ±320mV
	 *
	 * BADC: BADC Bus ADC Resolution/Averaging
	 * Bits 7–10 These bits adjust the Bus ADC resolution (9-, 10-, 11-, or 12-bit)
	 * or set the number of samples used when averaging results for the Bus Voltage Register (02h).
	 *
	 * SADC: SADC Shunt ADC Resolution/Averaging
	 * Bits 3–6 These bits adjust the Shunt ADC resolution (9-, 10-, 11-, or 12-bit)
	 * or set the number of samples used when averaging results for the Shunt Voltage Register (01h).
	 * BADC (Bus) and SADC (Shunt) ADC resolution/averaging and conversion time settings are shown in Table 6.
	 *
	 * ADC4 ADC3 ADC2 ADC1 MODE/SAMPLES CONVERSION TIME
	 *  0    X    0    0       9-bit          84mys
	 *  0    X    0    1      10-bit         148mys
	 *  0    X    1    0      11-bit         276mys
	 *  0    X    1    1      12-bit         532mys
	 *  1    0    0    0      12-bit         532mys
	 *  1    0    0    1         2           1.06ms
	 *  1    0    1    0         4           2.13ms
	 *  1    0    1    1         8           4.26ms
	 *  1    1    0    0        16           8.51ms
	 *  1    1    0    1        32          17.02ms
	 *  1    1    1    0        64          34.05ms
	 *  1    1    1    1       128          68.10ms
	 *
	 *  MODE: Operating Mode
	 *  Bits 0–2 Selects continuous, triggered, or power-down mode of operation.
	 *  These bits default to continuous shunt and bus measurement mode.
	 *  The mode settings are shown in Table 7.
	 *
	 *  MODE3 MODE2 MODE1 MODE
	 *    0     0     0   Power-Down
	 *    0     0     1   Shunt Voltage, Triggered
	 *    0     1     0   Bus Voltage, Triggered
	 *    0     1     1   Shunt and Bus, Triggered
	 *    1     0     0   ADC Off (disabled)
	 *    1     0     1   Shunt Voltage, Continuous
	 *    1     1     0   Bus Voltage, Continuous
	 *    1     1     1   Shunt and Bus, Continuous
	 */

	uint8_t writeCalib[] = {CALIB_REG, 0xFA, 0x00};
	/*
	 * BIT # 	D15	 D14  D13  D12  D11  D10  D9  D8  D7  D6  D5  D4  D3  D2  D1  D0
	 * BIT NAME FS15 FS14 FS13 FS12 FS11 FS10 FS9 FS8 FS7 FS6 FS5 FS4 FS3 FS2 FS1 FS0
	 */

	i2c_bus.write(sensorAddr, writeConfig, 3);
	i2c_bus.write(sensorAddr, writeCalib, 3);
}

int16_t SensePower::readReg(uint8_t sensorAddr, uint8_t reg)
{
	int16_t res;
	i2c_bus.writeRead(sensorAddr, &reg, 1, (uint8_t*)&res, 2);
	if (reg == BUS_VOLT_REG)
		return swap16(res) >> 3;

	return swap16(res);
}

void SensePower::run()
{
	setPeriodicBeat(2*MILLISECONDS, 100*MILLISECONDS);
	initSensor(CURR_BATT_I2C_ADDR);
	while(1)
	{
		int16_t shuntVolt = readReg(CURR_BATT_I2C_ADDR, SHUNT_VOLT_REG);
		int16_t busVolt = readReg(CURR_BATT_I2C_ADDR, BUS_VOLT_REG);
		int16_t current = readReg(CURR_BATT_I2C_ADDR, CURRENT_REG);
		int16_t power = readReg(CURR_BATT_I2C_ADDR, POWER_REG);

		//PRINTF("Shunt Voltage: %fmV; Bus Voltage: %fV\n", shuntVolt*SHUNT_VOLT_FACTOR, busVolt*BUS_VOLT_FACTOR);
		//PRINTF("Current: %fmA, Power: %fmW, %fW\n", current*0.32 - 165, busVolt*BUS_VOLT_FACTOR*(current*0.32 - 165), power*1.0);

		PowerData pd;
		pd.batt_voltage = busVolt;
		pd.batt_current = current;

		itPowerData.publish(pd);

		//PRINTF("Power data published: %d, %d\n", pd.batt_voltage, pd.batt_current);

		suspendUntilNextBeat();
	}
}

