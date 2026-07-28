/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code implements the daylight savings time and
 * time zone strings of the European Union political region.
 *
 * $Id: dst_eu.c 16026 2017-11-23 02:49:23Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "time.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_eu" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// EU calculation for daylight savings time.
// Assuming the utc is current, this tells the current time offset.
int dst(struct tm *ptr)
{
    int st = 0;
    int dst = 60;
    struct tm tmT;
    struct tm *tm_ptr = &tmT;
    int mon;
    int mday;
    int previous_sunday;

    // Calculate it in UTC.
    gmtime(tm_ptr, mktime(ptr)); // Convert to UTC.

    mon = tm_ptr->tm_mon;
    if(MAR < mon && OCT > mon) 
        return dst;
    else if(MAR > mon || OCT < mon)
        return st;

    mday = tm_ptr->tm_mday;

    previous_sunday = mday - tm_ptr->tm_wday;
    // DST begins on the last Sunday of March.
    if(MAR == mon) // If it's March... i.e. spring, so forward
    {  
        if(previous_sunday < 25) // Before last week of month?
        {
            return st;
        } else { // In last week of month.

            // Sunday in last week of month?
            if(previous_sunday == mday)
            {
               if (tm_ptr->tm_hour < 1)
                   return st;
               else
                   return dst;
            } else {
                return dst;
            }
        }
    // DST ends on the last Sunday of October.
    } else { // It has to be October, i.e. fall, so backward.
        if(previous_sunday < 25) // before the last week?
        {
            return dst;
        } else {
            // Is it sunday in the last week?
            if(previous_sunday == mday)
            {
                // If it's already ST...
                if(tm_ptr->tm_hour < 1 && ptr->tm_isdst == st)
                    return st; // It's still ST.
                else if(tm_ptr->tm_hour < 1)
                    return dst; // It's still DST.
                else
                    return st;
            } else {
                return st;
            }
        }
    }
}

// EU time zones.
const char *tz_ary[] = {
    "T",    "ST",        // Unknown, non-EU time zone.
    "GT",  "GST",       // Greenland Time, UTC-3 (-180)
    //"FKT", "FKST",      // Falklands Time, UTC-3
    "GST", "GSST",      // South Georgia & Sandwich Islands Time, UTC-2 (-120)
    "EGT", "EGST",      // Eastern Greenland Time, UTC-1 (-60)
    //"AZOT", "AZOST",    // Azores Time, UTC-1
    "WET", "WEST",      // Western European Time, UTC+0 (0)
    "CET", "CEST",      // Central European Time, UTC+1 (60)
    "EET", "EEST",      // Eastern European Time, UTC+2 (120)
    "FET", "FEST",      // Further Eastern European Time, UTC+3 (180)
    "GET", "GEST"       // Georgia Time, UTC+4 (240)
};

// Find the EU time zones.
const char *dst_tz(struct tm *tm_ptr)
{
    int idx;
    int dst = tm_ptr->tm_isdst;
    int gmt_offset = tm_ptr->gmt_offset;

    if(dst < 0 || gmt_offset < -120 || 240 < gmt_offset) // Unknown?
        return "";

    // Linear solution to index time-zone names.
    idx = 4 + ((tm_ptr->gmt_offset + 120)/30);
    idx &= 0xfe;

    if(0 < dst) // Daylight savings time?
        idx++;

    return tz_ary[idx];
}
/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

