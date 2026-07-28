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
 * $Id: tmp.h 16212 2017-12-08 00:31:28Z ptanwar $
 ***************************************************************************/
#ifndef TMP_H
#define TMP_H 1

// Firmware Device Driver version
extern const char fw_ver_tmp[]; 

// Initialize the temperature. Returns first temperature measurement.
float tmp_init(void);

// Deinitialize (shut down) the temperature, used for sleep.
void tmp_deinit(void);

// Measure the temperature.  Actually may take the previous
// measurement, and start a new measurement.
float tmp(void);

/* busy-wait for the temperaure measurement to complete. */
int32_t tmp_busy_wait (void);


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // undefined TMP_H
