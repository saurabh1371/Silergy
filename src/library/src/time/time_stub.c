/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This implements stubs for a Julian calendar. The main object
 * is to save space.
 *
 * $Id: time.c 18251 2018-07-10 23:17:36Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "dst.h"		// Daylight savings time API
#include "rtc.h"        // RTC driver
#include "time.h"       // check function prototypes and constants


/*** Version ****/
const char fw_ver_time[] =
"time_stub"   /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;



// The default time: 1/1/2020 00:00:00 was a Wednesday, day 1 of the year,
// with unknown daylight savings time, and it is not valid (false).
const struct tm starting_tm = {
    "", // Time Zone (from gmt_offset)
    0, // yday
    0, // gmt_offset (US-PST)
	0, 0, 0, 1, JAN, 20, // 00:00:00 1, Jan, 2020 
	WED, // week day
    -1, // Standard time (60=DST, -1=unknown)
    0, // subsec
    false // Not valid.
};


/****************************************************************************
* Description:
*       Run after reset.  It resets the RTC to defaults, if necessary.
*       This code is really tricky, because if the RTC is corrupted by
*       EMC, EMI or a battery failure, this is the routine that notices.
*       It also usually gets the blame: 'Why does rtc_init corrupt the
*       RTC!!?'. Because the value is really bad. Probably because of EMI.
*       In general, it tries to leave the RTC's value alone if it could
*       possibly be correct.
* Input:
*       The RTC hardware's data.
* Parameters:
*       None.
* Output:
*       The global RTC value saved in the RAM C structure global.reg.tm
* Return Status:
*       None.
*****************************************************************************/
#define GET_OLD_COPY 0x1
#define GET_DEFAULT 0x2
#define SET_RTC 0x4
void time_init (void)
{
    if ( rtc_init() ) { // RTC hardware failed?
        global.reg.tm = starting_tm; // RTC failed, so get default.
        rtc_write(&global.reg.tm);

    } else { // No known clock problem, so read RTC.
        // Get the time.
        global.reg.tm = starting_tm; // Set the default tz, gmt_offset, etc.
        rtc_read(&global.reg.tm);
        global.reg.tm.tm_isvalid = true; // As far as we can tell.
    }
} // void time_init (void)


/****************************************************************************
* Description:
*       Adjust daylight savings time, if needed.
* Input:
* Parameters:
*       A pointer to struct tm in global RAM
* Output:
*       The struct's time will contain daylight savings time.
* Return Status:
*       Nonzero if the struct's time is adjusted.
*****************************************************************************/
int time_to_dst(struct tm *tm_ptr)
{
    return false;
}


/****************************************************************************
* Description:
*       Set the calendar correctly.
*       The clock data is converted to a count of seconds since Jan 1, 2000.
*       This routine should be called only during calibration, or maybe
*       when the clock is set in the field.
* Input:
*       global.reg.tm, a C structure in global RAM, must contain a valid time.
*       nvm.reg.tm, a C structure in stable global RAM, must contain
*          the previous valid time.
* Parameters:
*       None.
* Output:
*       The RTC's registers are set to the time in global.reg.tm.
*       Any "RTC_UNSET" or "RTC_CORRUPT" status is cleared.
*       If the time is set more than a couple of hours away,
*       the routine does it, but sets a clock tamper error.
* Return Status:
*       None.
*****************************************************************************/
void time_write (struct tm *tm_ptr)
{
    // Set the time zone offset in minutes.
    tm_ptr->gmt_offset = global.cal.gmt_offset;
    // Set the daylight savings time.
    tm_ptr->tm_isdst = dst(tm_ptr);
    tm_ptr->tz = dst_tz(tm_ptr);
    // Since it's set, it is a valid time.
    tm_ptr->tm_isvalid = true;
    // Set the global value.
    global.reg.tm = *tm_ptr;
    // The NVM copy is set later in the meter's data cycle.
    // Set the RTC.
    rtc_write(tm_ptr);

    /* The clock is now set. */
    log_clear_one(EV_RTC_UNSET);
    log_clear_one(EV_RTC_CORRUPT);
} // time_set ()


/****************************************************************************
* Description:
*       Get the time efficiently, with low power and CPU resources.
* Input:
* Parameters:
*       Pointer to an empty (or reusable) time structure.
* Output:
* Return Status:
*       The time structure is set to the current local time.
*****************************************************************************/
void time_read (struct tm *tm_ptr)
{
    // Set the time zone offset in minutes.
    tm_ptr->gmt_offset = global.cal.gmt_offset;
    // Set the daylight savings time.
    tm_ptr->tm_isdst = global.reg.tm.tm_isdst;
    tm_ptr->tz = global.reg.tm.tz;
    tm_ptr->tm_isvalid = global.reg.tm.tm_isvalid;
    rtc_read(tm_ptr);
} // time_get ()


/****************************************************************************
* Description:
*       Finds seconds since midnight, january 1, 2000 .
*       Does no time-zone or daylight savings time calculations.
*       Math is from U.S. Naval Observatory Fortran, ported to C, using 
*       integer math.
* Input:
* Parameters:
*       tm_ptr is a pointer to a tm_t structure, which contains the time
*           of day and date.
* Output:
* Return Status:
*       time_t is a count of seconds since 00:00;2000-01-01
*****************************************************************************/
struct tm fake_tm;
uint32_t mktime (struct tm *tm_ptr)
{
    fake_tm = *tm_ptr;
    return 0; // seconds since 00:00:00 January 1, 2000 UTC
} // mktime ()


/****************************************************************************
* Description:
*       Get time and date from a count of seconds since 2000-01-01 00:00
*       UTC. Math is from U.S. Naval Observatory Fortran, ported to C
*       and using integer math.
* Input:
* Parameters:
*       tm_ptr points to a C structure in RAM that gets the time and date.
*       j is a julian count of seconds since 00:00 January 1, 2000
* Output:
*       The time and date is written to the tm_t C structure pointed-to by
*           tm_ptr.
* Return Status:
*       None.
*****************************************************************************/
void gmtime (struct tm *tm_ptr, uint32_t j)
{
    *tm_ptr = fake_tm;
} // gmtime ()


/****************************************************************************
* Description:
*       Get local time and date from a julian count of seconds
*       since January 1, 2000, local time.
* Input:
*       It also uses the local_time offset in the global calibration
*           structure Cal_t
* Parameters:
*       tm_ptr points to a C structure in RAM that gets the time and date.
*       j is a julian count of seconds since 00:00 January 1, 2000 local time
* Output:
*       The coordinated universal time and date (GMT) is written to the
*       tm_t C structure pointed-to by tm_ptr.
* Return Status:
*       None.
*****************************************************************************/
void localtime (tm_t *tm_ptr, uint32_t j)
{
    *tm_ptr = fake_tm;
} // localtime ()


/****************************************************************************
* Description:
*       Finds the number of seconds between to dates and times.
* Input:
* Parameters:
*       The starting and ending tm_ptrs which each point to a time of day
*       and date.
* Output:
* Return Status:
*       A count of seconds from the start to the end; If the start is AFTER
*       the ending time, the count will be NEGATIVE.
*****************************************************************************/
int32_t time_delta (struct tm *start_tm_ptr, struct tm *end_tm_ptr)
{
    return 0;  /* delta time. */
} // time_delta ()


/****************************************************************************
* Description:
*       Is the data in the main copy of the RTC data a good current clock time?
*       First it checks that the time structure has valid numbers.
*       Then if the time was already set, it tests against the last saved time.
*       The input is a time structure, which may be invalid (e.g. -1 hours)
*       The validation converts the date and time to julian seconds, 
*       and then back.  If all the fields are the same, the numbers are valid
*       If any of the fields have crazy values, the date and time do not match.
*       This routine forces the day of week and day of year to valid values,
*       and since this is called when setting the clock, it is not possible
*       to set invalid days of the week or days of the year.
*       If the C structure is invalid, it is marked invalid.
* Input:
*       global.reg.tm, the global date and time of day in a RAM C structure.
* Parameters:
*        None.
* Output:
*       global.reg.tm, the global date and time of day, has its day of week and
*       day of year set.
*       If the C structure is invalid, it is marked invalid.
* Return Status:
*       nonzero = the data is valid.
*****************************************************************************/
int time_valid (struct tm *tm_ptr)
{
    return rtc_invalid (tm_ptr) ? false : true;
} // time_valid ()


/****************************************************************************
* Description:
*       Find the ISO day of week from the date.
* Input:
* Parameters:
*       tm_ptr points to a C tm_t structure with the time of day and date.
* Output:
* Return Status:
*       The day of week, 1..7 1 = Monday
*****************************************************************************/
int time_iso_wday (struct tm *tm_ptr)
{
    /* tm_ptr->tm_wday: 0 - 6, Sunday = 0 (UNIX std) */
    /* ISO day of week: 1 - 7, Monday = 1 (ISO std) */
    return ((((int)tm_ptr->tm_wday) + 6) % 7) + 1;
} // time_iso_wday ()

/****************************************************************************
* Description:
*       Find the ISO week number from the date.
* Input:
* Parameters:
*       tm_ptr points to a C tm_t structure with the time of day and date.
* Output:
* Return Status:
*       The week of the year, 1..52 or 1..53
*****************************************************************************/
int time_iso_yweek (struct tm *tm_ptr) /* 1 - 52 or 53 */
{
    int yday, wday, yweek;
  
    yday = tm_ptr->tm_yday + 1; /* ISO std */
    wday = time_iso_wday(tm_ptr); /* 1 - 7, Monday = 1 (ISO std) */
    yweek = ((yday - wday) + 10) / 7; /* calculate the week of the year */
    return yweek;
} // calendar_iso_yweek (tm_t *tm_ptr)


/***************************************************************************
 * Copyright (C) 2013..2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

