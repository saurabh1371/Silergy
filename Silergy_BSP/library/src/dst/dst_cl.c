/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code implements the daylight savings time and
 * time zone strings of the Chilean political region.
 *
 * $Id$
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_cl" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// Chilean calculation for daylight savings time.
// Assuming the time is clock time, this tells the current time offset.
int dst(struct tm *tm_ptr)
{
    int st = 0;
    int dst = 60;
    int mon = tm_ptr->tm_mon;
    int mday, previous_sunday;

    if(AUG < mon || MAY > mon) // Summer?
        return dst;
    else if(AUG > mon && MAY < mon) // Winter?
        return st;

    mday = tm_ptr->tm_mday;

    previous_sunday = mday - tm_ptr->tm_wday;
    // Begin DST on second Sunday of August.
    if(AUG == mon) // If it's August... i.e. spring, so forward
    {
        if(previous_sunday < 8) // Before Sunday, week 2?
        {
            return st;
        } else { // After or during Sunday, week 2
            return dst;
        }
        // End DST on second Sunday of May.
    } else { // It has to be May, i.e. fall, so backward.
        if(previous_sunday < 8)
        {
            if(tm_ptr->tm_isdst == st) // if it already fell backward, stay.
                return st;
            return dst;
        } else {
            // On or after Sunday in week 2?
            return st;
        }
    }
}

// Chilean time zones.
const char *dst_tz(struct tm *tm_ptr)
{
    switch(tm_ptr->gmt_offset)
    {
    case -180: // UTC-3
       if(tm_ptr->tm_isdst == 60)
           return "CLST"; // Chile summer time
       else
           return "CLT"; // Chile time
    default: return "";
    }
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

