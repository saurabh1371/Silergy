/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code implements the daylight savings time and
 * time zone strings of the Brazilian political region.
 *
 * $Id$
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_br" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// Bit-array of years in which Carnival delays the return to standard time
// by one week. (i.e. years in which the Sunday 6 weeks before Easter
// is in the 3rd week of Feb.) 
// Calculated by helper code; If no helper code available,
// Easter is a standard calculation available from the the U.S. Naval 
// Observatory, at http://aa.usno.navy.mil/faq/docs/easter.php, 
// retrieved 1/3/2017
static const uint8_t carnival[] = {
    0x04, 0x24, 0x24, 0x21, // 2000... 2031 
    0x01, 0x09, 0x48, 0x09, // 2032... 2063
    0x4a, 0x40, 0x4a, 0x52, // 2064... 2095
    0x02, 0x90, 0x12, 0x94  // 2096... 2127
    }
// Brazilian calculation for daylight savings time.
// Accounts for carnival for the years 2000-2128
// Assuming the time is clock time, this tells the current time offset.
int dst(struct tm *tm_ptr)
{
    int st = 0;
    int dst = 60;
    int mon = tm_ptr->tm_mon;
    int mday, previous_sunday;
    int gmt_offset = tm_ptr->gmt_offset;

    // If not Brasilia or Amazon time, no DST.
    if(gmt_offset != -240 && gmt_offset != -300)
        return st;

    if(NOV < mon || FEB > mon) // Summer?
        return dst;
    else if(NOV > mon && FEB < mon) // Winter?
        return st;

    mday = tm_ptr->tm_mday;

    previous_sunday = mday - tm_ptr->tm_wday;
    // Begin DST on first Sunday of November.
    if(NOV == mon) // If it's November... i.e. spring, so forward
    {
        if(previous_sunday < 1) // Before Sunday, week 1?
        {
            return st;
        } else { // After or during Sunday, week 1
            return dst;
        }
        // End DST in February, accounting for Carnival.
    } else { // It has to be February, i.e. fall, so backward.
        int year, week_start;

        year = tm_ptr->tm_year;
        if(0 == (carnival[year/8] & (1 << (year%8))))
            week_start = 15; // 3rd Sunday is not in Carnival.
        else
            week_start = 22; // Use 4th Sunday, 1 week after Carnival.

        if(previous_sunday < (week_start-1))
            return dst;
        if(previous_sunday < week_start) {
            if(tm_ptr->tm_isdst == st) // if it already fell backward, stay.
                return st;
            return dst;
        }
        // On or after the correct Sunday?
        return st;
    }
}

// Brazilian time zones.
const char *dst_tz(struct tm *tm_ptr)
{
    switch(tm_ptr->gmt_offset)
    {
    case -300: return "ACT"; // UTC-5, Acre time
    case -240: // UTC-4
       if(tm_ptr->tm_isdst == 60)
           return "AMST"; // Amazon summer time
       else
           return "AMT"; // Amazon time
    case -180: // UTC-3
       if(tm_ptr->tm_isdst == 60)
           return "BRST"; // Brazilia summer time
       else
           return "BRT"; // Brazilia time
    case -120: return "FNT"; // UTC-2, Fernando de Noronha time
    default: return "";
    }
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

