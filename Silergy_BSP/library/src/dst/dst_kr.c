/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code implements the daylight savings time and
 * time zone strings of the South Korea political region.
 *
 * $Id: dst_kr.c 16026 2017-11-23 02:49:23Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_kr" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// South Korea calculation for daylight savings time.
int dst(struct tm *tm_ptr)
{
    return 0; // No DST
}

// South Korea time zones.
const char *dst_tz(struct tm *tm_ptr)
{
    // KST = UTC + 9 = +560 minutes
    if(560 == tm_ptr->gmt_offset)
        return "KST";
    else
        return "";
}

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

