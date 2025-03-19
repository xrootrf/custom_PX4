/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file batmon.h
 *
 * BatMon module for Smart Battery utilizing SBS 1.1 specifications
 * Setup/usage information: https://rotoye.com/batmon-tutorial/
 *
 * @author Eohan George <eohan@rotoye.com>
 * @author Nick Belanger <nbelanger@mail.skymul.com>
 */

#pragma once

#include <px4_platform_common/i2c_spi_buses.h>
#include <px4_platform_common/module.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/battery_status.h>

#define BATMON_DEFAULT_SMBUS_ADDR                       0x55            ///< Default 7 bit address I2C address. 8 bit = 0x16

#define BATT_SMBUS_MEASUREMENT_INTERVAL_US              100//_ms         ///< time in microseconds, measure at 10Hz

#define MAC_DATA_BUFFER_SIZE                            32

#define BATT_CELL_VOLTAGE_THRESHOLD_RTL                 0.5f            ///< Threshold in volts to RTL if cells are imbalanced
#define BATT_CELL_VOLTAGE_THRESHOLD_FAILED              1.5f            ///< Threshold in volts to Land if cells are imbalanced

#define BATT_CURRENT_UNDERVOLTAGE_THRESHOLD             5.0f            ///< Threshold in amps to disable undervoltage protection
#define BATT_VOLTAGE_UNDERVOLTAGE_THRESHOLD             3.4f            ///< Threshold in volts to re-enable undervoltage protection

#define BATT_SMBUS_ADDR                                 0x55            ///< Default 7 bit address I2C address. 8 bit = 0x16

#define BATT_SMBUS_TEMP                                 0x0C            ///< temperature register
#define BATT_SMBUS_VOLTAGE                              0x08            ///< voltage register
#define BATT_SMBUS_CURRENT                              0x10            ///< current register
#define BATT_SMBUS_AVERAGE_CURRENT                      0x0A            ///< average current register
#define BATT_SMBUS_MAX_ERROR                            0x03            ///< max error
// #define BATT_SMBUS_RELATIVE_SOC                         0x0D            ///< Relative State Of Charge
#define BATT_SMBUS_ABSOLUTE_SOC                         0x02            ///< Absolute State of charge
#define BATT_SMBUS_REMAINING_CAPACITY                   0x04            ///< predicted remaining battery capacity as a percentage
#define BATT_SMBUS_FULL_CHARGE_CAPACITY                 0x06            ///< capacity when fully charged
// #define BATT_SMBUS_RUN_TIME_TO_EMPTY                    0x11            ///< predicted remaining battery capacity based on the present rate of discharge in min
#define BATT_SMBUS_AVERAGE_TIME_TO_EMPTY                0x18            ///< predicted remaining battery capacity based on the present rate of discharge in min
#define BATT_SMBUS_CYCLE_COUNT                          0x2C            ///< number of cycles the battery has experienced
#define BATT_SMBUS_DESIGN_CAPACITY                      0x3C            ///< design capacity register
// #define BATT_SMBUS_DESIGN_VOLTAGE                       0x19            ///< design voltage register
// #define BATT_SMBUS_MANUFACTURER_NAME                    0x20            ///< manufacturer name
// #define BATT_SMBUS_MANUFACTURER_NAME_SIZE               21              ///< manufacturer name data size
// #define BATT_SMBUS_MANUFACTURE_DATE                     0x1B            ///< manufacture date register
#define BATT_SMBUS_SERIAL_NUMBER                        0x28            ///< serial number register

// #define BATT_SMBUS_BQ40Z50_CELL_4_VOLTAGE               0x3C
// #define BATT_SMBUS_BQ40Z50_CELL_3_VOLTAGE               0x3D
// #define BATT_SMBUS_BQ40Z50_CELL_2_VOLTAGE               0x3E
// #define BATT_SMBUS_BQ40Z50_CELL_1_VOLTAGE               0x3F

// #define BATT_SMBUS_BQ40Z80_CELL_7_VOLTAGE               0x3C
// #define BATT_SMBUS_BQ40Z80_CELL_6_VOLTAGE               0x3D
// #define BATT_SMBUS_BQ40Z80_CELL_5_VOLTAGE               0x3E
// #define BATT_SMBUS_BQ40Z80_CELL_4_VOLTAGE               0x3F

#define BATT_SMBUS_STATE_OF_HEALTH                      0x2E            ///< State of Health. The SOH information of the battery in percentage of Design Capacity

// #define BATT_SMBUS_MANUFACTURER_ACCESS                  0x00
// #define BATT_SMBUS_MANUFACTURER_DATA                    0x23
// #define BATT_SMBUS_MANUFACTURER_BLOCK_ACCESS            0x44

// #define BATT_SMBUS_SECURITY_KEYS                        0x0035

// #define BATT_SMBUS_DEVICE_TYPE                          0x0001
// #define BATT_SMBUS_LIFETIME_FLUSH                       0x002E
// #define BATT_SMBUS_LIFETIME_BLOCK_ONE                   0x0060
// #define BATT_SMBUS_ENABLED_PROTECTIONS_A_ADDRESS        0x4938
// #define BATT_SMBUS_SEAL                                 0x0030
// #define BATT_SMBUS_DASTATUS1                            0x0071
// #define BATT_SMBUS_DASTATUS2                            0x0072
// #define BATT_SMBUS_DASTATUS3                            0x007B

// #define BATT_SMBUS_ENABLED_PROTECTIONS_A_DEFAULT        0xcf
// #define BATT_SMBUS_ENABLED_PROTECTIONS_A_CUV_DISABLED   0xce

class BATMAN
{
public:
	virtual ~BATMAN() = default;

	virtual int init() = 0;

	// read reg value
	virtual int get_reg(uint8_t addr, uint8_t *value) = 0;

	// bulk read reg value
	virtual int get_reg_buf(uint8_t addr, uint8_t *buf, uint8_t len) = 0;

	// write reg value
	virtual int set_reg(uint8_t value, uint8_t addr) = 0;

	// // bulk read of calibration data into buffer, return same pointer
	// virtual calibration_s *get_calibration(uint8_t addr) = 0;

	// virtual uint32_t get_device_id() const = 0;

	// virtual uint8_t get_device_address() const = 0;

	// virtual void set_device_type(uint8_t devtype) = 0;
};

class Batmon : public I2CSPIDriver<Batmon>
{

public:
	Batmon(const I2CSPIDriverConfig &config, BATMAN *interface);
	~Batmon() = default;

	static I2CSPIDriverBase *instantiate(const I2CSPIDriverConfig &config, int runtime_instance);

	static void print_usage();

	void RunImpl();


private:

	uORB::PublicationMulti<battery_status_s> _battery_status_pub{ORB_ID(battery_status)};
	orb_advert_t _batt_topic{nullptr};
	BATMAN			*_interface{nullptr};

	float _max_cell_voltage_delta{0};

	float _min_cell_voltage{0};

	/** @param _last_report Last published report, used finding v deltas */
	battery_status_s _last_report{};

	void custom_method(const BusCLIArguments &cli) override;

	int get_cell_voltages();

	int get_batmon_startup_info();

	/** @param _crit_thr Critical battery threshold param. */
	float _crit_thr{0.f};

	/** @param _emergency_thr Emergency battery threshold param. */
	float _emergency_thr{0.f};

	/** @param _low_thr Low battery threshold param. */
	float _low_thr{0.f};

};

extern BATMAN *batmon_i2c_interface(uint8_t busnum, uint32_t device, int bus_frequency);

