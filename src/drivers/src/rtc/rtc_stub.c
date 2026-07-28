/**************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2019 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION:Real Time Clock Routines.
 * Silergy recommends using a hardware real-time clock with battery back-up.
 * This is just a simulation of a real-time clock, but it does
 * illustrate all the interfaces to the rest of the meter code,
 * and includes useful date handling code.
 * This code reads, displays and writes a time-of-day clock.
 *
 * The timebase is the AFE's sumcycle.
 * (The AFE's sumcycle is not a recommended time base)
 *
 * The code assumes that the clock is read exactly once per sumcycle.
 * At reset, if the value is invalid, it resets the clock.
 * It includes routines for detecting common clock tampers,
 * as well as calculations for date and civil time adjustment.
 * Clock compensation is a fixed drift rate (y_cal0) set in parts per billion.
 * There is no temperature compensation.
 *
 * $Id: rtc_stub.c 21454 2019-06-27 23:52:54Z rvandewa $
 *************************************************************************/
#include <stdbool.h>    // Needed by rtc.h
#include <stdint.h>     // Standard integer definitions.
#include <stdlib.h>     // Standard library.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "delay.h"      // Delay loop
#include "rtc.h"        // Function prototypes.

// nanoseconds per call of rtc_read ().
#define BASE_TRIM 1000000000L

const char fw_ver_rtc[] =
"rtc_stub" /* Device Driver name */
" v"       /* ver string */
"1.00"     /* Major_ver.Minor_ver */
"\r\n"     /* CR + LF */
;

int rtc_fail;

struct tm sim_tm;

int sim_alarm_sec;
int sim_alarm_min;
int sim_alarm_hour;
int sim_tcab;
int sim_tccd;
int sim_trim;

void rtc_compensation(void); // Perform compensation.



/****************************************************************************
* Description:
*       Is the year a leap year?
* Input:
* Parameters:
*       tm_ptr points to a C tm_t structure with the time of day and date.
* Output:
* Return Status:
*       true if the date is in a leap-year.
*****************************************************************************/
int rtc_leap_year (tm_t *tm_ptr)
{
    int year = (int)tm_ptr->tm_year;
    if (0 == (year & 3))
    {   /* divisible by 4 */
        if (0 == (year % 100))      /* divisible by 100? */
        {
            if (0 == (year % 400))
                return true;        /* divisible by 400 */
            else
                return false;       /* divisible only by 100 */
        } else {
            return true;            /* divisible only by 4 */
        }
    } else {
        return false;               /* most years */
    }
} // rtc_leap_year ()


/****************************************************************************
* Description:
*       Find the day of the year.
*       The day of the year is a convenient, compact way to store calendars
*       and discover if the current civil time is daylight savings time.
*       Normally meters should not store algorithms that depend on
*       legislatively-set standards, such as daylight savings times,
*       tariff seasons, etc.  These should be controlled by a downloadable
*       calendar.
* Input:
* Parameters:
*       tm_ptr points to a C tm_t structure with the time of day and date.
* Output:
* Return Status:
*       The day of year of the date in tm_ptr->tm_t
*           Note that years can have 365 or 366 days, so the day of the year
*           can be 1...366; The day of year is suitable for an array index.
*****************************************************************************/
const int16_t yday_by_mon[12] = { /* months are 0..11 */
    -1,   30,  58,  89, 119, 150,  // Add MDAY to get day of year.
    180, 211, 242, 272, 303, 333 };
int rtc_get_yday (tm_t *tm_ptr) /* 1 - 366 */
{
    int mon, doy;  // Month, doy = Day Of Year

    mon = tm_ptr->tm_mon;

    /* Day Of Year is the start of the month,
     * plus the day of the month. */
    doy = ((int)yday_by_mon[mon]) + ((int)tm_ptr->tm_mday);

    /* Past February, add 1 on leap years. */
    if (mon > FEB) {
       doy += ( rtc_leap_year(tm_ptr) ) ? 1 : 0;
    }
    return doy;
} // rtc_get_yday ()


/****************************************************************************
* Description:
*       Initialize the RTC. 
* Input Parameters: none
* Output:
* Return Status:
*       nonzero indicates that the RTC is not usable.
*****************************************************************************/
int rtc_init(void)
{
    struct tm tm_tmp;
    int fail;

    // Clear alarm to avoid a spurious interrupt from it.
    rtc_alarm_clear();


    // Test the RTC's value.
    rtc_read(&tm_tmp);
    fail = rtc_invalid(&tm_tmp);
    rtc_fail = fail;
    return fail;    // A bit is set for each failure reason.
}


/****************************************************************************
* Description:
*       Read RTC time and validity from the registers.
* Parameters:
*       pointer to a time structure. It fills it.
*       Not set: is_dst; the daylight savings time state is not in the RTC.
*****************************************************************************/
void (*rtc_handler)(void); // Cleared to NULL by startup code.
const int16_t last_mon_day[12] = { 31, 28, 31, 30, 31, 30, 31,
    31, 30, 31, 30, 31 };
void rtc_read (tm_t *tm_ptr)
{
    int trim = BASE_TRIM;   // Happens to be same as base_trim; Lucky.

    // Usually read once per second.
    sim_trim += trim;
    if(sim_trim >= BASE_TRIM)
    {
        sim_trim -= BASE_TRIM;
        if (++sim_tm.tm_sec > 59)
        {
            sim_tm.tm_sec = 0;
            if(++sim_tm.tm_min > 59)
            {
                sim_tm.tm_min = 0;
                if(++sim_tm.tm_hour > 23)
                {
                    int last_day = last_mon_day[sim_tm.tm_mon];

                    sim_tm.tm_hour = 0;

                    if(++sim_tm.tm_wday > SAT)
                        sim_tm.tm_wday = SUN;

                    if(FEB == sim_tm.tm_mon)
                        last_day += rtc_leap_year(&sim_tm);

                    if(++sim_tm.tm_mday > last_day)
                    {
                        sim_tm.tm_mday = 1;

                        if(++sim_tm.tm_mon > DEC)
                        {
                            sim_tm.tm_mon = JAN;

                            if(++sim_tm.tm_year > 99)
                                sim_tm.tm_year = 0;
                        }
                    }
                }
            }
        }
    }

    // Get the day of the year.
    sim_tm.tm_yday = (int16_t)rtc_get_yday(&sim_tm);

    if(
        sim_tm.tm_sec == sim_alarm_sec
        && sim_tm.tm_min == sim_alarm_min
        && sim_tm.tm_hour == sim_alarm_hour
    )
    {
        if(NULL != rtc_handler)
            (*rtc_handler)();
    }

    // Write new RTC value to the shadow registers.
    *tm_ptr = sim_tm;
} // rtc_read ()


/****************************************************************************
* Description:
*       Write to simulated RTC.
* Input Parameters:
*       tm_ptr is a time-of-day structure. By convention the RTC has local time.
* Output:
*       The simulated RTC's registers are set.
* Return Status:
*       none
*****************************************************************************/
void rtc_write(struct tm *tm_ptr)
{
    sim_tm = *tm_ptr;
} // rtc_write()


/****************************************************************************
* Description:
*       Write to RTCALRM register to set the alarm.
*       When the RTCSEC value matches RTCALRM an RTC alarm event is triggered.
* Input Parameters:
*       hour, minute, second.
* Output:
*       RTCALRM register is set.
*****************************************************************************/
void rtc_alarm_write (int hours, int minutes, int seconds)
{
	sim_alarm_sec = seconds;
	sim_alarm_min = minutes;
	sim_alarm_hour = hours;
} /* rtc_alarm_write () */


/****************************************************************************
* Description:
*       Unset the alarm clock.
* Input:
* Parameters:
*       None.
* Output:
*       The alarm is set to expire at the end of time, i.e. 2^32-1 seconds
*       after 00:00 January 1, 2000, sometime in 2136AD.
* Return Status:
*       None.
*****************************************************************************/
void rtc_alarm_clear (void)
{
    rtc_alarm_write(0x1f, 0x3f, 0x3f); // Write an impossible alarm time.
} // rtc_alarm_clear ()


/****************************************************************************
* Description:
*       Test a tm_t structure for validity. 
* Input Parameters: pointer to a tm-t
* Output:
* Return Status:
*       nonzero indicates that the tm_t structure is invalid.
*****************************************************************************/
int rtc_invalid(struct tm *ptr)
{
    int fail = 0;
    int t;

    t = ptr->tm_year;   // Check year.
    if(t < 0 || t > 99)
        fail |= 1;

    t = ptr->tm_mon;    // Check month.
    if(t < 0 || t > 11)
        fail |= 2;

    t = ptr->tm_mday;   // Check day of month.
    if(t < 1 || t > 31)
        fail |= 4;

    t = ptr->tm_hour;   // Check hour of day.
    if(t < 0 || t > 23)
        fail |= 8;

    t = ptr->tm_min;    // Check minute of hour.
    if(t < 0 || t > 59)
        fail |= 0x10;

    t = ptr->tm_sec;    // Check second of minute.
    if(t < 0 || t > 59)
        fail |= 0x20;

    t = (int)ptr->tm_wday;   // Check day of week.
    if(t < 0 || t > 6)
        fail |= 0x40;

    t = ptr->tm_yday;   // Check day of year.
    if(t < 0 || t > 365)
        fail |= 0x80;

    t = ptr->tm_isdst;  // Check daylight savings time.
    if(t < -1 || t > 60)
        fail |= 0x100;

    t = ptr->gmt_offset;  // Check GMT offset.
    if(t < -720 || t > 780)
        fail |= 0x200;

    // Validity is known only when the clock is set.
    //t = ptr->tm_isvalid;  // Check validity flag.
    //if(t != true)
    //    fail |= 0x400;

    return fail;
}


/****************************************************************************
* Description:
*       Read the RTC Temp. compensation and calibration registers (TCAB,TCCD and RTCCAL).
* Parameters:
*       Register value is returned by a pointer.
*       int32_t *tcab - Pointer, value of TCAB returned.
*       int32_t *tccd - Pointer, value of TCCD returned.
* Return Status:
*       None
*****************************************************************************/
void rtc_trim_read (int32_t *_tcab, int32_t *_tccd)
{
    *_tcab = sim_tcab;
    *_tccd = sim_tccd;
} // int32_t rtc_get_trim (int32_t *tcab, int32_t *tccd, int32_t *rtc_cal)


/****************************************************************************
* Description:
*       Set the RTC Temp. compensation and calibration registers (TCAB,TCCD).
*       The values are usually the part of the Calibration Data.
*
* Parameters:
*       int32_t tcab - TCAB register value to write.
*       int32_t tccd - TCCD register value to write.
* Output:
*       The RTC hardware registers TCAB and TCCD are set.
*       If any of these values are non-zero, RTC Temperature Compensation 
*       is activated.
* Return Status:
*       None
*****************************************************************************/
void rtc_trim_write (int32_t _tcab, int32_t _tccd)
{
    sim_tcab = _tcab;
    sim_tccd = _tccd;
} // rtc_set_trim ()


/****************************************************************************
* Description:
*       Set compensation adjustment values to default
* Input:
*       Default value.    
* Parameters:
*       None.
* Output:
*       The RTC hardware registers TCAB and TCCD are set to zero.
* Return Status:
*       None.
*****************************************************************************/
void rtc_default_trim (void)
{
   // Write Trim value to default
   // TCAB =0, TCCD=0
   rtc_trim_write (0, 0);
	
} // void rtc_default_trim (void)


/****************************************************************************
* Description: Wait for time to pass.
*
* Parameters:
*   Input:
*         Count of 1/256 seconds.
*   Output:
*         none
****************************************************************************/
void rtc_wait (int cnt)
{
    for(;cnt > 0;--cnt)
    {
        delay(DELAY_US((1000000/256)));
    }
}


/****************************************************************************
* Description: Read RTC as a count of subseconds since midnight.
* This is handy for high resolution wall-clock timing.
* To extend beyond one day, use julian.c application code.
* Parameters:
*   Input:
*         None
*   Output:
*         Count of subseconds since midnight.
****************************************************************************/
static int subsec;
int rtc_subsec_read (void)
{
    ++subsec;
    return subsec;
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

