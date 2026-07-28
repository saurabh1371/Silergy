/***************************************************************************
 * DESCRIPTION: POWER METER - Temperature Routines.
 * This code reads the temperature.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * $Id: tmp_stub.c 16212 2017-12-08 00:31:28Z ptanwar $
 ***************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "tmp.h"

// Firmware Device Driver version
const char fw_ver_tmp[] =
"tmp_stub"      /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

// Initialize the temperature. Returns first temperature measurement.
float tmp_init(void) { return tmp(); }

// Read the binary value.
int16_t tmp_stemp(void) { return 0; }

// Deinitialize (shut down) the temperature, used for sleep.
void tmp_deinit(void) { }

// Measure the temperature.  Actually may take the previous
// measurement, and start a new measurement.
float tmp(void) { return 0.0; }

/* busy-wait for the temperaure measurement to complete. */
int32_t tmp_busy_wait (void) { return 0; }


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

