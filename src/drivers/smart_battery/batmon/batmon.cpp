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
 * @file batmon.cpp
 *
 * BatMon module for Smart Battery utilizing SBS 1.1 specifications
 * Setup/usage information: https://rotoye.com/batmon-tutorial/
 *
 * @author Eohan George <eohan@rotoye.com>
 * @author Nick Belanger <nbelanger@mail.skymul.com>
 */

#include "batmon.h"
#include <mathlib/mathlib.h>
#include <lib/atmosphere/atmosphere.h>
#include <parameters/param.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/i2c_spi_buses.h>

uint8_t BATT_SMBUS_TEMP                   =              0x0C;            ///< temperature register
uint8_t	BATT_SMBUS_VOLTAGE 		  =		 0x08;            ///< voltage register
uint8_t	BATT_SMBUS_CURRENT                =              0x10;            ///< current register
uint8_t	BATT_SMBUS_ABSOLUTE_SOC           =              0x02;            ///< Absolute State of charge
uint8_t	BATT_SMBUS_REMAINING_CAPACITY     =              0x04;            ///< predicted remaining battery capacity as a percentage
uint8_t	BATT_SMBUS_AVERAGE_TIME_TO_EMPTY  =              0x18;            ///< predicted remaining battery capacity based on the present rate of discharge in min
uint8_t	BATT_SMBUS_CYCLE_COUNT            =              0x2C;            ///< number of cycles the battery has experienced
uint8_t	BATT_SMBUS_MANUFACTURER_NAME      =              0x20;            ///< manufacturer name
uint8_t	BATT_SMBUS_MANUFACTURER_NAME_SIZE =              21;            ///< manufacturer name data size
uint8_t	BATT_SMBUS_MANUFACTURE_DATE       =              0x1B;            ///< manufacture date register
uint8_t	BATT_SMBUS_SERIAL_NUMBER          =              0x28;            ///< serial number register
uint8_t	BATT_SMBUS_STATE_OF_HEALTH        =              0x2E;            ///< State of Health. The SOH information of the battery in percentage of Design Capacity


// extern "C" __EXPORT int batmon_main(int argc, char *argv[]);

Batmon::Batmon(const I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config)
{
}

Batmon::~Batmon()
{

}

// I2CSPIDriverBase *Batmon::instantiate(const I2CSPIDriverConfig &config, int runtime_instance)
// {
// 	BATMAN *interface = batmon_i2c_interface(config.bus, config.i2c_address, config.bus_frequency);


// 	int32_t batmon_en_param = 0;
// 	param_get(param_find("BATMON_DRIVER_EN"), &batmon_en_param);

// 	if (batmon_en_param == 0) {	// BATMON_DRIVER_EN is set to disabled. Do not start driver
// 		return nullptr;        // TODO: add option for autodetect I2C address
// 	}

// 	if (interface == nullptr) {
// 		PX4_ERR("alloc failed");
// 		return nullptr;
// 	}

// 	Batmon *instance = new Batmon(config, interface);

// 	if (instance == nullptr) {
// 		PX4_ERR("alloc failed");
// 		return nullptr;
// 	}

// 	// int ret = instance->get_startup_info();
// 	// ret |= instance->get_batmon_startup_info();

// 	// if (ret != PX4_OK) {
// 	// 	delete instance;
// 	// 	return nullptr;
// 	// }


// 	// Setting the BAT_SOURCE to "external"
// 	int32_t battsource = 1;
// 	param_set(param_find("BAT_SOURCE"), &battsource);

// 	instance->ScheduleOnInterval(BATT_SMBUS_MEASUREMENT_INTERVAL_US);

// 	return instance;
// }


void Batmon::RunImpl()
{
	int ret = PX4_OK;

	// Temporary variable for storing SMBUS reads.
	uint8_t resultL;		//prev was uint16_t
	uint8_t resultH;

	// Read data from sensor.
	battery_status_s new_report = {};

	new_report.id = 1;

	// Set time of reading.
	new_report.timestamp = hrt_absolute_time();
	new_report.connected = true;

	ret |= transfer(&BATT_SMBUS_VOLTAGE, sizeof(BATT_SMBUS_VOLTAGE), &resultL, sizeof(resultL));
	ret |= transfer(&BATT_SMBUS_VOLTAGE+1, sizeof(BATT_SMBUS_VOLTAGE+1), &resultH, sizeof(resultH));
	new_report.voltage_v = (resultL<<8)+resultH;

	PX4_INFO("batmon_ret %d", ret);
	PX4_INFO("batmon_resultL %d", resultL);

	// for (int i = 0; i < _cell_count; i++) {
	// 	new_report.voltage_cell_v[i] = 65535;
	// }

	// // Read current.
	ret |= transfer(&BATT_SMBUS_CURRENT, sizeof(BATT_SMBUS_CURRENT), &resultL, sizeof(resultL));
	ret |= transfer(&BATT_SMBUS_CURRENT+1, sizeof(BATT_SMBUS_CURRENT+1), &resultH, sizeof(resultH));

	new_report.current_a = (resultL<<8)+resultH;

	// Read average time to empty (minutes).
	ret |= transfer(&BATT_SMBUS_AVERAGE_TIME_TO_EMPTY, sizeof(BATT_SMBUS_AVERAGE_TIME_TO_EMPTY) , &resultL, sizeof(resultL));
	ret |= transfer(&BATT_SMBUS_AVERAGE_TIME_TO_EMPTY+1, sizeof(BATT_SMBUS_AVERAGE_TIME_TO_EMPTY+1) , &resultH, sizeof(resultH));
	new_report.average_time_to_empty = (resultL<<8)+resultH;

	// Read remaining capacity.
	ret |= transfer(&BATT_SMBUS_REMAINING_CAPACITY, sizeof(BATT_SMBUS_REMAINING_CAPACITY), &resultL, sizeof(resultL));
	ret |= transfer(&BATT_SMBUS_REMAINING_CAPACITY+1, sizeof(BATT_SMBUS_REMAINING_CAPACITY+1), &resultH, sizeof(resultH));
	new_report.remaining_capacity_wh = (resultL<<8)+resultH;

	// Read Absolute SOC.
	ret |= transfer(&BATT_SMBUS_ABSOLUTE_SOC, sizeof(BATT_SMBUS_ABSOLUTE_SOC), &resultL, sizeof(resultL));
	ret |= transfer(&BATT_SMBUS_ABSOLUTE_SOC+1, sizeof(BATT_SMBUS_ABSOLUTE_SOC+1), &resultH, sizeof(resultH));
	// Normalize 0.0 to 1.0
	new_report.remaining = ((float)(resultL<<8)+resultH) / 100.0f;

	// Read battery temperature and covert to Celsius.
	ret |= transfer(&BATT_SMBUS_TEMP, sizeof(BATT_SMBUS_TEMP), &resultL, sizeof(resultL));
	ret |= transfer(&BATT_SMBUS_TEMP+1, sizeof(BATT_SMBUS_TEMP+1), &resultH, sizeof(resultH));
	new_report.temperature = (((float)(resultL<<8)+resultH )/ 10.0f) + atmosphere::kAbsoluteNullCelsius;

	// // Only publish if no errors.
	// if (ret == PX4_OK) {
	// 	// new_report.capacity = _batt_capacity;
	// 	// new_report.cycle_count = _cycle_count;
	// 	// new_report.serial_number = _serial_number;
	// 	new_report.max_cell_voltage_delta = _max_cell_voltage_delta;
	// 	// new_report.cell_count = _cell_count;
	// 	// new_report.state_of_health = _state_of_health;

	// 	// TODO: This critical setting should be set with BMS info or through a paramter
	// 	// Setting a hard coded BATT_CELL_VOLTAGE_THRESHOLD_FAILED may not be appropriate
	// 	//if (_lifetime_max_delta_cell_voltage > BATT_CELL_VOLTAGE_THRESHOLD_FAILED) {
	// 	//	new_report.warning = battery_status_s::BATTERY_WARNING_CRITICAL;

	// 	if (new_report.remaining > _low_thr) {
	// 		new_report.warning = battery_status_s::BATTERY_WARNING_NONE;

	// 	} else if (new_report.remaining > _crit_thr) {
	// 		new_report.warning = battery_status_s::BATTERY_WARNING_LOW;

	// 	} else if (new_report.remaining > _emergency_thr) {
	// 		new_report.warning = battery_status_s::BATTERY_WARNING_CRITICAL;

	// 	} else {
	// 		new_report.warning = battery_status_s::BATTERY_WARNING_EMERGENCY;
	// 	}

	// 	// new_report.interface_error = perf_event_count(_interface->_interface_errors);

		_battery_status_pub.publish(new_report);

		_last_report = new_report;
	// }
}

// int Batmon::get_batmon_startup_info()
// {
// 	int ret = PX4_OK;

// 	// Read battery threshold params on startup.
// 	param_get(param_find("BAT_CRIT_THR"), &_crit_thr);
// 	param_get(param_find("BAT_LOW_THR"), &_low_thr);
// 	param_get(param_find("BAT_EMERGEN_THR"), &_emergency_thr);

// 	// Read BatMon specific data	do later
// 	// uint16_t num_cells;
// 	// ret = transfer(BATT_SMBUS_CELL_COUNT, num_cells);
// 	// _cell_count = math::min((uint8_t)num_cells, (uint8_t)MAX_CELL_COUNT);

// 	// int32_t _num_cells = num_cells;
// 	// param_set(param_find("BAT_N_CELLS"), &_num_cells);

// 	return ret;
// }

// void Batmon::custom_method(const BusCLIArguments &cli)
// {
// 	switch(cli.custom1) {
// 		case 1:
// 			// TODO: analyze why these statements are not printed
// 			// PX4_INFO("The manufacturer name: %s", _manufacturer_name);
// 			// PX4_INFO("The manufacturer date: %d", _manufacture_date);
// 			// PX4_INFO("The serial number: %d", _serial_number);
// 			break;
// 		case 4:
// 			// suspend();
// 			break;
// 		case 5:
// 			// resume();
// 			break;
// 	}
// }

