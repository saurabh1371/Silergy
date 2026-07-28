/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This code implements the daylight savings time and
 * time zone strings of the Russian political region.
 *
 * $Id: dst_ru.c 16026 2017-11-23 02:49:23Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "rtc.h"
#include "dst.h"

/*** Version ****/
const char fw_ver_dst[] =
"dst_ru" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// Russian calculation for daylight savings time.
int dst(struct tm *tm_ptr)
{
    return 0; // No DST
}

// Russian time zones.
const char *tz_ary[] = {
    "EET",       // Eastern European Time, Kaliningrad, UTC+2
    "MSK",       // Moscow Standard Time, UTC+3
    "SAMT",      // Samara Time, UTC+4
    "YEKT",      // Yekaterinburg Time, UTC+5
    "OMST",      // Omsk Time, UTC+6
    "KRAT",      // Krasnoyarsk Time, UTC+7
    // "NOVT",      // Novosibirsk Time, UTC+7
    "IRKT",      // Irkutsk Time, UTC+8
    "YAKT",      // Yakutsk Time, UTC+9
    "VLAT",      // Vladivostok Time, UTC+10
    "MAGT",      // Magadan Time, UTC+11
    //"SAKT",      // Sakhalin Time, UTC+11
    //"SRET",      // Srednekolymsk Time, UTC+11
    "PETT"       // Kamchatka Time, UTC+12
    //"ANAT"       // Anadyr Time, UTC+12
};

// Find the Russian time zones.
const char *dst_tz(struct tm *tm_ptr)
{
    int gmt_offset = tm_ptr->gmt_offset;
    int idx;
    
    if(gmt_offset < 120 || 720 < gmt_offset)
    	return "";
  
    idx = (gmt_offset/60) - 2;

    return tz_ary[idx];
}

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

