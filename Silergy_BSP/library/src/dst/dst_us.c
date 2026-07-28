/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code implements the daylight savings time and
 * time zone strings of the U.S. political region.
 *
 * $Id: dst_us.c 24062 2020-03-26 23:35:19Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_us" /* Library file name */
" v"     /* ver string */
"1.01"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


// US calculation for daylight savings time.
// Assuming the time is clock time, this tells the current time offset.
int dst(struct tm *tm_ptr)
{
    int st = 0;
    int dst = 60;
    int mon = tm_ptr->tm_mon;
    int mday, previous_sunday;

    if(MAR < mon && NOV > mon) // Summer?
        return dst;
    else if(MAR > mon || NOV < mon) // Winter?
        return st;

    mday = tm_ptr->tm_mday;

    previous_sunday = mday - tm_ptr->tm_wday;
    // Begin DST on 2nd Sunday of March.
    if(MAR == mon) // If it's March... i.e. spring, so forward
    {
        if(previous_sunday < 8) // Before Sunday, week 2?
        {
            return st;
        } else { // After week 1
            if (previous_sunday < 15) // In week 2?
            {   // Sunday? In week 2?
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
    // Begin ST on 1st Sunday of November.
    } else { // It has to be November, i.e. fall, so backward.
        if(previous_sunday < 1) // before week 1?
        {
            return dst;
        } else {
            if(previous_sunday < 8)
            {   // Sunday in week 1?
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
            } else {
                return st;
            }
        }
    }
}

// US time zones.
static const char *tz_ary[] = {
	"ST", "DT",         // Unknown, non-US time zone.
	"AST", "ADT",       // Atlantic time, UTC - 4 (-240)
	"EST", "EDT",       // Eastern time, UTC - 5 (-300)
	"CST", "CDT",       // Central time, UTC - 6 (-360)
	"MST", "MDT",       // Mountain time, UTC - 7 (-420)
	"PST", "PDT",       // Pacific time, UTC - 8 (-480)
	"AKST", "AKDT",     // Alaska time, UTC - 9 (-540)
	"HAST", "HADT",     // Hawaiian-Aleutian time, UTC - 10 (-600)
	"SST", "SDT"        // Samoa time, UTC - 11 (-660)
};

// Find the time zones.
const char *dst_tz(struct tm *tm_ptr)
{
    int idx;

    // Linear solution.
    idx = (tm_ptr->gmt_offset + 180)/-30;
    if(16 < idx || 2 > idx || 0 != (idx & 1))
        idx = 0;

    if(0 < tm_ptr->tm_isdst)
        idx++;

    return tz_ary[idx];
}

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

