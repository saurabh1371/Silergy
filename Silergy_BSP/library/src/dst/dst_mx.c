/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code implements the daylight savings time and
 * time zone strings of the Mexican political region.
 * Note: For districts that keep U.S. time, use dst_us.c
 *
 * $Id: dst_mx.c 16026 2017-11-23 02:49:23Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_mx" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// Mexican calculation for daylight savings time.
// Assuming the time is clock time, this tells the current time offset.
int dst(struct tm *tm_ptr)
{
    int st = 0;
    int dst = 60;
    int mon = tm_ptr->tm_mon;
    int mday, previous_sunday;

    if(-300 == tm_ptr->gmt_offset) // Quintana Roo?
        return st;

    if(APR < mon && OCT > mon) // Summer?
        return dst;
    else if(APR > mon || OCT < mon) // Winter?
        return st;

    mday = tm_ptr->tm_mday;

    previous_sunday = mday - tm_ptr->tm_wday;
    // Begin DST on 1st Sunday, April
    if(APR == mon) // If it's April... i.e. spring, so forward
    {
        if(previous_sunday < 8) // Before Sunday, week 2?
        {
            return st;
        } else { // After week 1
            if (previous_sunday < 15) // In week 1?
            {
                // Sunday? In week 2?
                if(previous_sunday == mday)
                {
                    if(tm_ptr->tm_hour < 2)
                        return st;
                    else
                        return dst;
                } else {
                    return dst;
                }
            } else {
                return dst;
            }
        }
    // End DST on last Sunday, October
    } else { // It has to be October, i.e. fall, so backward.
        if(previous_sunday < 25) // before last week?
        {
            return dst;
        } else {
            // Sunday in last week?
            if(previous_sunday == mday)
            {
                if (tm_ptr->tm_hour < 1)
                    return dst;
                // If it's already ST, stay ST.
                else if(tm_ptr->tm_hour < 2 && tm_ptr->tm_isdst == st)
                    return st;
                else if(tm_ptr->tm_hour < 2)
                    return dst;
                else
                    return st;
            } else {
                return st;
            }
        }
    }
}

// Mexican time zones.
const char *tz_ary[] = {
    "HE", "HV",         // Unknown, non-Mexican time zone.
    "HEE", "HVE",       // Hora del este, UTC - 4
    "HEC", "HCA",       // Hora central, UTC - 5
    "HEM", "HVM",       // Hora de la montana, UTC - 7
    "HEP", "HVP"        // Hora de pacifico, UTC - 8
};

// Find the time zones.
const char *dst_tz(struct tm *tm_ptr)
{
	int idx;
	
	switch(tm_ptr->gmt_offset)
	{
		case -240: idx = 2; break;
		case -300: idx = 4; break;
		case -420: idx = 6; break;
		case -480: idx = 8; break;
		default:   idx = 0; break;
	}

    if(0 < tm_ptr->tm_isdst)
        idx++;

    return tz_ary[idx];
}

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

