/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code implements the daylight savings time and
 * time zone strings of the Australian political region.
 *
 * $Id: dst_au.c 16319 2017-12-26 22:30:25Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_au" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// Australian calculation for daylight savings time.
// Assuming the time is clock time, this tells the current time offset.
int dst(struct tm *tm_ptr)
{
    int st = 0;
    int dst = 60;
    int mon = tm_ptr->tm_mon;
    int mday, previous_sunday;

    if(480 == tm_ptr->gmt_offset) // Western Australia?
        return st; // Always standard time.    

    if(OCT < mon || APR > mon) // Summer?
        return dst;
    else if(OCT > mon && APR < mon) // Winter?
        return st;

    mday = tm_ptr->tm_mday;

    previous_sunday = mday - tm_ptr->tm_wday;
    // Begin DST on first Sunday of October.
    if(OCT == mon) // If it's October... i.e. spring, so forward
    {
        if(previous_sunday < 1) // Before Sunday, week 1?
        {
            return st;
        } else { // After week 1
            if (previous_sunday == mday) // In week 1?
            {   // Sunday in week 1.
                if(tm_ptr->tm_hour < 2)
                    return st;
                else
                    return dst;
            } else {
                return dst;
            }
        }
        // End DST on first Sunday of April.
    } else { // It has to be April, i.e. fall, so backward.
        if(previous_sunday < 1) // before week 1?
        {
            return dst;
        } else {
            if(previous_sunday < 8)
            {
                // Sunday in week 1?
                if(previous_sunday == mday)
                {
                    // If it's already ST, stay ST.
                    if(tm_ptr->tm_hour < 2 && tm_ptr->tm_isdst == st)
                        return st;
                    else if(tm_ptr->tm_hour < 2)
                        return dst;
                    else
                        return st;
                } else {
                    return st;
                }
            } else {
                return st;
            }
        }
    }
}

// Australian time zones.
const char *tz_ary[] = {
    "ST", "DT",         // Unknown, non-Australian time zone.
    "AEST", "AEDT",     // Aus. Eastern time, UTC + 10 (600)
    "ACST", "ACDT",     // Aus. Central time, UTC + 9.5 (570)
    // Note: AWDT is to detect erroneous setting of DST. (Should never be!)
    "AWST", "AWDT"      // Aus. Western time, UTC + 8 (480)
};

// Find the time zones.
const char *dst_tz(struct tm *tm_ptr)
{
    int idx;

    // A linear calculation is impossible.
    switch(tm_ptr->gmt_offset)
    {
    case 600: idx = 2; break;
    case 570: idx = 4; break;
    case 480: idx = 6; break;
    default: idx = 0; break;
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

