/***************************************************************************
 * DESCRIPTION: POWER METER - Battery Routines.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * $Id: bat_stub.c 16212 2017-12-08 00:31:28Z ptanwar $
 ***************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "bat.h"        // Check function prototypes.

// Firmware Device Driver version
const char fw_ver_bat[] =
"bat_stub"      /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

// Initialize the battery measurement.
void bat_init(void) { }

// Deinitialize (shut down) the battery measurement, used for sleep.
void bat_deinit(void) { }

// Measure the battery voltage under a 100uA load.
float bat_volts(void) { return 0.0; }

// Measure the RTC battery's voltage under a 100uA load.
float bat_volts_rtc(void) { return 0.0; }

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

