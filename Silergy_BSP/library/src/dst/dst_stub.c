/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code stubs the daylight savings time calculation and
 * time zone strings and works in any political region.
 *
 * $Id: dst_stub.c 22228 2019-11-07 23:40:40Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_stub" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// No daylight savings time.
int dst(struct tm *tm_ptr)
{
    return 0;
}

// No time zone name.
const char *dst_tz(struct tm *tm_ptr)
{
    return "";
}


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

