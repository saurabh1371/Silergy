/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code reads and sets the Real Time Clock (RTC), and 
 * provides Julian Calendar implementation.
 *
 * $Id: time.h 16026 2017-11-23 02:49:23Z rvandewa $
 ***************************************************************************/

#ifndef TIME_H
#define TIME_H 1		// 1 = enable calendar logic.
#include "rtc.h"    // defines struct tm

// Library version string.
extern const char fw_ver_time[];

// Start-up RTC reliably.
void time_init(void);

// set the clock from globals.reg.tm, i.e. global.reg.tm
void time_read (struct tm *tm_ptr);

// set the clock from globals.reg.tm, i.e. global.reg.tm
void time_write (struct tm *tm_ptr);

// Adjust daylight savings time, if needed.
// Returns a nonzero if the time in *tm_ptr changed.
int time_to_dst(struct tm *tm_ptr);

// Returns the number of seconds (Julian seconds) from midnight, 
// January 1, 2000 GMT to the passed civil time.
uint32_t mktime (struct tm *tm_ptr);

// Get time and date from a count of seconds since 2000-01-01 00:00 GMT
void gmtime (struct tm *tm_ptr, uint32_t j);

// Get local time and date from a count of seconds since 2000-01-01 00:00 GMT
void localtime (struct tm *tm_ptr, uint32_t j);

// Finds the number of seconds between two dates and times.
int32_t time_delta (struct tm *start_tm_ptr, struct tm *end_tm_ptr);

// Returns nonzero if the structure is consistent.
// Sets the day of week and day of year.
int time_valid (struct tm *tm_ptr);

// calculate the ISO day of week. (Monday = 1)
int time_iso_dow (struct tm *tm_ptr);

// calculate the ISO week number. (First week of year = 1)
int time_get_iso_woy (struct tm *tm_ptr);

/***************************************************************************
 * Copyright (C) 2013-2017 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // undefined TIME_H
