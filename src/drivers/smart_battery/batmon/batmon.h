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

#include <drivers/drv_hrt.h>
#include <lib/drivers/device/i2c.h>
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

// #define BATT_SMBUS_RUN_TIME_TO_EMPTY                    0x11            ///< predicted remaining battery capacity based on the present rate of discharge in min
// #define BATT_SMBUS_DESIGN_VOLTAGE                       0x19            ///< design voltage register

// #define BATT_SMBUS_BQ40Z50_CELL_4_VOLTAGE               0x3C
// #define BATT_SMBUS_BQ40Z50_CELL_3_VOLTAGE               0x3D
// #define BATT_SMBUS_BQ40Z50_CELL_2_VOLTAGE               0x3E
// #define BATT_SMBUS_BQ40Z50_CELL_1_VOLTAGE               0x3F

// #define BATT_SMBUS_BQ40Z80_CELL_7_VOLTAGE               0x3C
// #define BATT_SMBUS_BQ40Z80_CELL_6_VOLTAGE               0x3D
// #define BATT_SMBUS_BQ40Z80_CELL_5_VOLTAGE               0x3E
// #define BATT_SMBUS_BQ40Z80_CELL_4_VOLTAGE               0x3F


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

class Batmon : public device::I2C, public I2CSPIDriver<Batmon>
{

public:
	Batmon(const I2CSPIDriverConfig &config);
	~Batmon() override;

	static void print_usage();

	void RunImpl();


private:

	uORB::PublicationMulti<battery_status_s> _battery_status_pub{ORB_ID(battery_status)};

	/** @param _last_report Last published report, used finding v deltas */
	battery_status_s _last_report{};

	orb_advert_t _batt_topic{nullptr};

	// void custom_method(const BusCLIArguments &cli) override;

	int get_cell_voltages();

	/** @param _crit_thr Critical battery threshold param. */
	float _crit_thr{0.f};

	/** @param _emergency_thr Emergency battery threshold param. */
	float _emergency_thr{0.f};

	/** @param _low_thr Low battery threshold param. */
	float _low_thr{0.f};

};
