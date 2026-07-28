/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION:POWER METER - Daylight Savings Time Code.
 * 
 * $Id: dst.h 16212 2017-12-08 00:31:28Z ptanwar $
 ***************************************************************************/
#ifndef DST_H
#define DST_H 1

/*** Version ****/
extern const char fw_ver_dst[];

// Calculate if the time has daylight savings time.
int dst(struct tm *tm_ptr);

// Find the time zone string.
const char *dst_tz(struct tm *tm_ptr);


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // undefined RTC_H
