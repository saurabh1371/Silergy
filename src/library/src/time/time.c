/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION: This implements Julian conversion, moving between a C-standard 
 * struct tm, and a julian count of seconds since 00:00:00 January 1, 2000. 
 * (1970 is standard, but may turn over in a practical meter lifetime.)
 * Unlike the UNIX code, this keeps local time in the hardware RTC, so
 * that reading the time takes minimal power and code.
 * The calendar code is a C integer adaptation of the U.S. Naval 
 * Observatory Fortran from http://aa.usno.navy.mil/faq/docs/JD_Formula.php
 * (Retrieved in 2016).
 *
 * $Id: time.c 24061 2020-03-26 23:34:34Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "dst.h"		// Daylight savings time API
#include "rtc.h"        // RTC driver
#include "time.h"       // check function prototypes and constants


/*** Version ****/
const char fw_ver_time[] =
"time"   /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

// Various counts of seconds
#define ONE_MINUTE (60)
#define ONE_HOUR (60 * ONE_MINUTE)
#define TWO_HOURS (2*ONE_HOUR)
#define ONE_DAY (24 * ONE_HOUR)
// 1y ~= 365.2425d
#define ONE_YEAR ((365 * ONE_DAY) + ((ONE_DAY * 2425) / 10000))
#define BATTERY_LIFE (3 * ONE_YEAR) // A typical battery time.
#define METER_LIFE (20 * ONE_YEAR)  // A typical meter lifetime.
// Most crystals have a maximum drift of 10ppm.
// So, the maximum drift of the clock.
#define MAX_CLOCK_DRIFT (10*(METER_LIFE / 1000000))


// The default time: 1/1/2020 00:00:00 was a Wednesday, day 1 of the year,
// with calculable daylight savings time, and it is not valid (false).
const struct tm starting_tm = {
	"PST", // Time Zone (from gmt_offset)
	0, // yday
	-480, // gmt_offset (US-PST)
	0, 0, 0, 1, JAN, 20, // 00:00:00 1, Jan, 2020 
	WED, // week day
    0, // Standard time (60=DST, -1=unknown)
    0, // subsec
    false // Not valid.
};


/****************************************************************************
* Description:
*       Run after reset.  It resets the RTC to defaults, if necessary.
*       This code is really tricky, because if the RTC is corrupted by
*       EMC, EMI or a battery failure, this is the routine that notices.
*       It also usually gets the blame: 'Why does time_init corrupt the
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
    int write_rtc = 0;   // flag if need to correct RTC failure.

    // If previous clock data invalid?
    if(log_active(EV_REG_BAD) || !nvm.reg.tm.tm_isvalid ) {
        write_rtc |= GET_DEFAULT;  // There has never been a good RTC value.

    } else if ( rtc_init() ) { // RTC hardware failed?
        write_rtc |= GET_OLD_COPY; // RTC failed, so get copy from NVM.

    } else { // No known clock problem, so read RTC.
        // Get GMT offset and daylight savings time data.
        global.reg.tm = nvm.reg.tm;
        rtc_read(&global.reg.tm);

        if( ! time_valid(&global.reg.tm) ) // Good value from hardware?
            write_rtc |= GET_OLD_COPY; // Bad hardware value, softcopy OK.
    }

    // Need to get default copy? (Because a set RTC value is not available.)
    if ( write_rtc & GET_DEFAULT ) { 
        global.reg.tm = starting_tm;   // Set a default value.
        log_state(EV_RTC_UNSET, true);
    } else if ( write_rtc & GET_OLD_COPY ) {
        global.reg.tm = nvm.reg.tm;  // Get an old RTC value from NVM
        log_state(EV_RTC_CORRUPT, true);
    }
    // else time read from hardware is just fine.

    // To or from daylight savings time?
    if(time_to_dst(&global.reg.tm))
        write_rtc |= SET_RTC;

    // Does the hardware need to be set?
    if(write_rtc) {
        /* Set the value. Don't clear the subseconds. */
        rtc_write(&global.reg.tm);
        // The NVM copy is set in the meter's data cycle.
    }

    // Setting the RTC calibration is part of the initialization,
    // and one would expect it to happen here.
    // However, trim values tcab, tccd, are calibration values.
    // They are set by get_cal() in cal.c .
    // which calls rtc_set_trim(), below.
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
    int changed = false;
    // Move to and from daylight savings time.
    int dst_min = dst(tm_ptr);
    int dst_delta = dst_min - tm_ptr->tm_isdst;
    if(0 != dst_delta)
    {
        int gmt_offset = tm_ptr->gmt_offset;
        // Convert to seconds. Add the offset.         
        uint32_t new_now;

        new_now = mktime(tm_ptr);
        // Convert back to time.
        new_now += (dst_min + gmt_offset) * ONE_MINUTE;
        gmtime(tm_ptr, new_now);
        tm_ptr->gmt_offset = gmt_offset;

        changed = true; // The clock needs to be set.

        // Prevent jitter from setting the clock.
        if(0 == tm_ptr->tm_subsec)
            tm_ptr->tm_subsec = 1; // !=0 -> driver does not set subsec.
    }
    tm_ptr->tm_isdst = dst_min;
    tm_ptr->tz = dst_tz(tm_ptr);

    return changed;
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
*       Finds seconds since midnight, January 1, 2000 .
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
uint32_t mktime (struct tm *tm_ptr)
{
    int32_t a, y, m, j, civil_offset, dst;

    a = (13 - tm_ptr->tm_mon) / 12L;
    y = (int32_t)((uint32_t)((tm_ptr->tm_year) + 6800L) - a);
    m = (int32_t)(((uint32_t)tm_ptr->tm_mon + (12 * a)) - 2);

    /* julian days since Jan 1, 2000; 5.8e6 years range in 32-bit signed int */
    j = (int32_t)(
        (uint32_t)tm_ptr->tm_mday
        + (((153 * m) + 2)/5)
        + (365 * y)
        + (y / 4)
        - (y / 100)
        + (y / 400)
        - 2483590
      );

    /* calculate julian seconds since Jan 1, 2000; valid until j wraps
     * around in 2068 */
    j = (int32_t)(
        (j * 86400)
      + ((((uint32_t)tm_ptr->tm_hour) - 12L) * 3600)
      + ((uint32_t) tm_ptr->tm_min * 60L)
      + (uint32_t) tm_ptr->tm_sec
      + 43200L
    );

    // Subtract the time zone (in minutes)
    civil_offset = tm_ptr->gmt_offset;
    // And daylight savings time. (also in minutes)
    dst = tm_ptr->tm_isdst;
    if(0 < dst) // Is there daylight savings time?
    {
        civil_offset += dst;
    }
    j -= ONE_MINUTE * civil_offset;

    return j; // seconds since 00:00:00 January 1, 2000 UTC
} // mktime ()


/****************************************************************************
* Description:
*       Get time and date from a count of seconds since 2000-01-01 00:00
*       UTC. Math is from U.S. Naval Observatory Fortran, ported to C
*       and using integer math.
* Input:
* Parameters:
*       tm_ptr points to a C structure in RAM that gets the time and date.
*       j is a Julian count of seconds since 00:00 January 1, 2000
* Output:
*       The time and date is written to the tm_t C structure pointed-to by
*           tm_ptr.
* Return Status:
*       None.
*****************************************************************************/
void gmtime (struct tm *tm_ptr, uint32_t j)
{
    uint32_t jd, w, x, a, b, c, d, e, f, month;

    tm_ptr->tm_subsec = 128; // minimize jitter
    tm_ptr->tm_sec = (uint8_t)(j % 60);
    tm_ptr->tm_min = (uint8_t)((j / 60) % 60);
    tm_ptr->tm_hour = (uint8_t)((j / 3600) % 24);
    jd = (uint32_t)((j / 86400L) + 2451545L); /* jd = julian days */

    /* Calculate day of week: 0 = Sunday; January 1, 2000 was Saturday */
    tm_ptr->tm_wday = (int8_t)((jd + 1) % 7);

    /* This obscure logic is the standard arithmetic for calculating a date
     * from a julian day, from the U.S. Naval Observatory. */

    /* In standard calculation w = int((jd - 1867216.25)/36524.25); */
    w = ((4L * jd) - 7468865L) / 146097L;
    x = w / 4L;
    a = (jd + w + 1L) - x;
    b = a + 1524L;

    /* In standard calculation c = int((b - 122.1) / 365.25); */
    c = ((20L * b) - 2442L) / 7305L;
    /* In standard calculation d = int(c * 365.25); */
    d = (c * 1461L) / 4;
    /* In standard calculation e = int((b - d)/30.6001); */
    e = ((b - d) * 10000L)/306001L; /* b - d is a few hundred, so no overflow */
    f = (306001L * e)/10000L; /* e is less than 25, so no overflow */

    tm_ptr->tm_mday = (uint8_t)((b - d) - f);
    tm_ptr->tm_mon = (int8_t)(month = (e < 14) ? (e - 2) : (e - 14));
    tm_ptr->tm_year = (int8_t)((month < 2) ? (c - 6715) : (c - 6716));
    tm_ptr->tm_yday = rtc_get_yday(tm_ptr);

    tm_ptr->tm_isdst = 0;
    tm_ptr->gmt_offset = 0;
    tm_ptr->tz = "Z";
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
    // Add the Local time-zone's GM offset.
    j += ONE_MINUTE * global.cal.gmt_offset;
    gmtime(tm_ptr, j);

    // Describe the time zone.
    tm_ptr->gmt_offset = global.cal.gmt_offset;
    tm_ptr->tm_isdst = 0;

    // Update to daylight savings time.
    time_to_dst(tm_ptr);
} // localtime ()


/****************************************************************************
* Description:
*       Finds the number of seconds between two dates and times.
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
    return (
        (int32_t)mktime(end_tm_ptr)
      - (int32_t)mktime(start_tm_ptr));  /* delta time. */
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
    struct tm test;
    int now, then, valid;

    valid = rtc_invalid (tm_ptr) ? false : true;

    /* Can the clock be set to new date and time?
     * Test the decoded form in global.reg.tm.  
     * Bad values in the structure make 
     * different dates and times. This tests that when the clock
     * is set, the values make sense. */
    now = (int)mktime(tm_ptr); /* make seconds since Jan 1, 2000 */
    localtime (&test, now);

    tm_ptr->tm_wday = test.tm_wday;  /* Use calculated day of week */
    tm_ptr->tm_yday = test.tm_yday;  /* Use calculated day of year */

    if ( // if any other value does not match...
            test.tm_sec  != tm_ptr->tm_sec
            || test.tm_min  != tm_ptr->tm_min
            || test.tm_mday != tm_ptr->tm_mday
            || test.tm_mon  != tm_ptr->tm_mon
            || test.tm_year != tm_ptr->tm_year
            || test.tm_year <  starting_tm.tm_year // before firmware written
       )
    {
        valid = false;  // the date and time time structure is invalid.
    }
    if(test.tm_isdst == tm_ptr->tm_isdst)
    {
        if(test.tm_hour != tm_ptr->tm_hour)
            valid = false;
        // In the hours before and after changing daylight savings times
        // there are two permissible times.
    } else if (1 != labs(test.tm_hour - tm_ptr->tm_hour)) {
        valid = false;
    }
    // If valid == true, the structure is self-consistent.

    if(nvm.reg.tm.tm_isvalid) // Was the copy in billing data set?
    {
        int dt;
        // Get the time saved in the billing data.
        then = mktime(&nvm.reg.tm); /* in seconds since Jan 1, 2000 */

        dt = now - then; // Get the difference. (and handle clock roll-over.)

        // Can the clock be set to this new value?
        // Maximum clock drift depends on if RTC drift is set, meter reader
        // sets the clock, etc.
        if(dt < -MAX_CLOCK_DRIFT) // setting clock to an earlier time?
        {   // Permit setting standard time to undo daylight savings time.
            if(0 == tm_ptr->tm_isdst && 0 < nvm.reg.tm.tm_isdst)
            {
                if(dt < -TWO_HOURS) // Only permit two hours back at most.
                    valid = false;  // The clock can't be set backwards so much.
            } else { // if not changing to standard time.
                valid = false;  // The clock can't be set backwards so much.
            }
        }

        // How long can power be off before the clock fails?
        if(dt > METER_LIFE)     // In some systems maybe BATTERY-LIFE.
        {
            valid = false;      // The clock can't be so far in the future.
        }
        if (!valid)
            *tm_ptr = nvm.reg.tm;
    }
    else
    {   // No copy-B to revert.
        tm_ptr->tm_isvalid = valid;
    }

    return valid;
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

