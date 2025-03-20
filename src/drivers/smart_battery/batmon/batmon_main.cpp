/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
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
 * @file batmon_i2c.cpp
 *
 * I2C interface for BATMON
 */

#include "batmon.h"
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>
#include <parameters/param.h>


void Batmon::print_usage()
{
	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Driver for SMBUS Communication with BatMon enabled smart-battery
Setup/usage information: https://rotoye.com/batmon-tutorial/
### Examples
To start at address 0x0B, on bus 4
$ batmon start -X -a 11 -b 4

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("batmon", "driver");

	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x55);

	PRINT_MODULE_USAGE_COMMAND_DESCR("man_info", "Prints manufacturer info.");
	PRINT_MODULE_USAGE_COMMAND_DESCR("suspend", "Suspends the driver from rescheduling the cycle.");
	PRINT_MODULE_USAGE_COMMAND_DESCR("resume", "Resumes the driver from suspension.");

	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}


extern "C" __EXPORT int batmon_main(int argc, char *argv[])
{
	using ThisDriver = Batmon;
	 
	// Setting the BAT_SOURCE to "external"
	int32_t battsource = 1;
	param_set(param_find("BAT_SOURCE"), &battsource);
	BusCLIArguments cli{true, false};
	cli.default_i2c_frequency = 400000;

	int32_t batmon_addr_batt1 = BATMON_DEFAULT_SMBUS_ADDR;
	param_get(param_find("BATMON_ADDR_DFLT"), &batmon_addr_batt1);
	cli.i2c_address = batmon_addr_batt1;

	const char *verb = cli.parseDefaultArguments(argc, argv);
	if (!verb) {
		ThisDriver::print_usage();
		return -1;
	}

	BusInstanceIterator iterator(MODULE_NAME, cli, DRV_BAT_DEVTYPE_BATMON_SMBUS);

	if (!strcmp(verb, "start")) {
		return ThisDriver::module_start(cli, iterator);
	}

	if (!strcmp(verb, "stop")) {
		return ThisDriver::module_stop(iterator);
	}

	if (!strcmp(verb, "status")) {
		return ThisDriver::module_status(iterator);
	}

	if (!strcmp(verb, "man_info")) {
		cli.custom1 = 1;
		return ThisDriver::module_custom_method(cli);
	}
	if (!strcmp(verb, "suspend")) {
		cli.custom1 = 4;
		return ThisDriver::module_custom_method(cli);
	}
	if (!strcmp(verb, "resume")) {
		cli.custom1 = 5;
		return ThisDriver::module_custom_method(cli);
	}

	ThisDriver::print_usage();
	return -1;
}

