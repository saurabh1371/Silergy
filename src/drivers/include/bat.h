/***************************************************************************
 * DESCRIPTION: POWER METER - Battery Routines.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * $Id: bat.h 16212 2017-12-08 00:31:28Z ptanwar $
 ***************************************************************************/
#ifndef BAT_H
#define BAT_H 1

// Firmware Device Driver version
extern const char fw_ver_bat[]; 

// Initialize the battery measurement.
void bat_init(void);

// Deinitialize (shut down) the battery measurement, used for sleep.
void bat_deinit(void);

// Measure the battery voltage under a 100uA load.
float bat_volts(void);

// Measure the RTC battery's voltage under a 100uA load.
float bat_volts_rtc(void);

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // undefined BAT_H
