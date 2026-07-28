/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code implements the daylight savings time and
 * time zone strings of the New Zealand political region.
 *
 * $Id$
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_nz" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// New Zealand calculation for daylight savings time.
// Assuming the time is clock time, this tells the current time offset.
int dst(struct tm *tm_ptr)
{
    int st = 0;
    int dst = 60;
    int mon = tm_ptr->tm_mon;
    int mday, previous_sunday;

    if(SEP < mon || APR > mon) // Summer?
        return dst;
    else if(SEP > mon && APR < mon) // Winter?
        return st;

    mday = tm_ptr->tm_mday;

    previous_sunday = mday - tm_ptr->tm_wday;
    // Begin DST on last Sunday of September.
    if(SEP == mon) // If it's September... i.e. spring, so forward
    {
        if(previous_sunday < 24) // Before last Sunday?
        {
            return st;
        } else { // After last week
            if (previous_sunday == mday) // Is it sunday?
            {   // Sunday in last week.
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
                    if(tm_ptr->tm_hour < 3 && tm_ptr->tm_isdst == st)
                        return st;
                    else if(tm_ptr->tm_hour < 3)
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

// New Zealand time zones.
const char *dst_tz(struct tm *tm_ptr)
{
    // A linear calculation is impossible.
    switch(tm_ptr->gmt_offset)
    {
    case 780:
        if(60 == tm_ptr->tm_isdst)
            return "NZDT";
        else
            return "NZT";
    case 825: 
        if(60 == tm_ptr->tm_isdst)
            return "CHADT";
        else
            return "CHAT";
    default: return "";
    }
}

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

