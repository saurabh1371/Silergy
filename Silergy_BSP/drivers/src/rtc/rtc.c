/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION:POWER METER - Real Time Clock (RTC) driver Code.
 * This code provide utility routines to access RTC and Trim registers.
 *
 * $Id: rtc.c 21453 2019-06-27 23:51:55Z rvandewa $
 ***************************************************************************/
#include <stdbool.h>    // Needed by rtc.h
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "rtc.h"        // Function prototypes.

/** Dependencies ***/
// SystemInit
//#define SYSTEM_CLOCK SystemCoreClock

/*** Version ****/
const char fw_ver_rtc[] =
"rtc"      /* Device Driver name */
" v"       /* ver string */
"1.00"     /* Major_ver.Minor_ver */
"\r\n"     /* CR + LF */
;

// Internal variables.
int rtc_fail; // Bit mask showing current failures.

unsigned char d_yr,d_mnth,d_day,t_hr,t_min,t_sec;
unsigned long real_time, real_date;//,prev_real_date;

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

    // Unfreeze the shadow registers.
    SYS->WAKE_SRC_b.rtc_wr = 0;
    SYS->WAKE_SRC_b.rtc_rd = 0;
    sys_shadow_busy_wait();

    // Wait for the subsecond to become nonzero, indicating that the RTC
    // is readable.
    while(0 == RTC->SUB)
        ;

    // Test the RTC's value.
    rtc_read(&tm_tmp);
    // Set defaults for the test.
    tm_tmp.gmt_offset = 0;
    tm_tmp.tm_isdst = -1;
    tm_tmp.tz = "";
    tm_tmp.tm_isvalid = true;
    // Test the values.
    fail = rtc_invalid(&tm_tmp);
    // Test the hardware state.
    if(SYS->WAKE_SRC_b.rtc_fail)    // RTC failure
        fail |= 0x800;
    if(SYS->WAKE_SRC_b.er_oscfail)  // Oscillator failed while CPU off.
        fail |= 0x1000;
    if (SYS->STAT_b.v3a_nok) {      // Is the battery voltage bit valid?
        if (SYS->STAT_b.vbat_nok) { // Valid, so is the battery voltage bad?
            fail |= 0x2000;         // Battery is bad, so clock is bad.
    }   }
    // failure reason 0x4000, 0x8000 are reserved, unused.

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
void rtc_read (struct tm *tm_ptr)
{
	uint32_t tmp_long;
    // Wait till the RTC is free; i.e. the shadow registers are
    // valid and not busy. 
    sys_shadow_busy_wait();

    // Freeze the shadow registers.
    SYS->WAKE_SRC_b.rtc_rd = 1;

    // Write new RTC value to the shadow registers.
    tm_ptr->tm_subsec = RTC->SUB_b.rtc_sub;
    tm_ptr->tm_sec = t_sec = RTC->TIME_b.rtc_sec;
    tm_ptr->tm_min = t_min = RTC->TIME_b.rtc_min;
    tm_ptr->tm_hour = t_hr = RTC->TIME_b.rtc_hour;
    tm_ptr->tm_mday = d_day = RTC->DATE_b.rtc_mday;
    tm_ptr->tm_mon = d_mnth = (RTC->DATE_b.rtc_month)-1;d_mnth=d_mnth+1; // Hardware counts 1..12
    tm_ptr->tm_year = d_yr = RTC->DATE_b.rtc_year;
    tm_ptr->tm_wday = (RTC->WDAY)-1; // Hardware counts 1..7
	
		tmp_long=t_hr;
		tmp_long=tmp_long*100+t_min;
		real_time=tmp_long*100+t_sec;
								
		tmp_long=d_day;
		tmp_long=tmp_long*100+d_mnth;
		real_date=tmp_long*100+d_yr;         

    // Unfreeze the shadow registers.
    SYS->WAKE_SRC_b.rtc_rd = 0;

    if(SYS->WAKE_SRC_b.rtc_fail)
        tm_ptr->tm_isvalid = false;

    // Get the day of the year.
    tm_ptr->tm_yday = (int16_t)rtc_get_yday(tm_ptr);

    // Wait till the RTC is free; i.e. the shadow registers are
    // valid and not busy. 
    sys_shadow_busy_wait();
} // rtc_read ()


/****************************************************************************
* Description:
*       Write to RTC data and time registers to set the clock/Calendar. 
*       The SUBSEC register is cleared to zero.
* Input Parameters:
*       tm_ptr is a time-of-day structure. By convention the RTC has local time.
* Output:
*       The RTC's registers are set.
* Return Status:
*       none
*****************************************************************************/
void rtc_write(struct tm *tm_ptr)
{
    // Wait till the RTC is free; i.e. the shadow registers are
    // valid and not busy. 
    sys_shadow_busy_wait();

    // Freeze the shadow registers.
    SYS->WAKE_SRC = SHADOW_UNLOCK;

    // Write new RTC value to the shadow registers.
    RTC->TIME_b.rtc_sec = tm_ptr->tm_sec;
    RTC->TIME_b.rtc_min = tm_ptr->tm_min;
    RTC->TIME_b.rtc_hour = tm_ptr->tm_hour;
    RTC->DATE_b.rtc_mday = tm_ptr->tm_mday;
    RTC->DATE_b.rtc_month = 1 + tm_ptr->tm_mon; // hardware counts 1..12
    RTC->DATE_b.rtc_year = tm_ptr->tm_year;
    RTC->WDAY = 1 + tm_ptr->tm_wday;  // hardware counts 1..7

    // If the subsecond is not cleared, leave it alone to reduce jitter.    
    if(0 == tm_ptr->tm_subsec)
        RTC->SUB_b.rtc_sub = 0;

    // Unfreeze the shadow registers.
    SYS->WAKE_SRC_b.rtc_wr = 0;

    // Wait till the RTC is free; i.e. the shadow registers are
    // valid and not busy. 
    sys_shadow_busy_wait();
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
    // Write new alarm value synchronously.
    int rtc_alarm;
    rtc_alarm = seconds;
    rtc_alarm |= (minutes << 8);
    rtc_alarm |= (hours << 16);

    RTC->ALARM = rtc_alarm;
} /* rtc_write_alarm () */


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
    SYS->STAT = 0x80000101; // Clear the flag and disable the interrupt.
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
    // Wait till the RTC is free
    sys_shadow_busy_wait();

    // Freeze the shadow registers.
    SYS->WAKE_SRC_b.rtc_rd = 1;

    *_tcab = RTC->TC_AB;
    *_tccd = RTC->TC_CD;

    // Unfreeze the shadow registers.
    SYS->WAKE_SRC_b.rtc_rd = 0;
	
    sys_shadow_busy_wait();
} // rtc_trim_read ()


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
    // Wait till the RTC is free; i.e. the shadow registers are
    // valid and not busy. 
    sys_shadow_busy_wait();
    
    // write it to the RTC
    *(int *)(&SYS->WAKE_SRC) = SHADOW_UNLOCK; // Enable writing.

    RTC->TC_AB = _tcab;            // Set the trim values.
    RTC->TC_CD = _tccd;

    SYS->WAKE_SRC_b.rtc_wr = 0;  // End Writing
	
    sys_shadow_busy_wait();
} // rtc_trim_write ()


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
    int old_tck_cnt, tck_cnt, rtc_sub;

    old_tck_cnt = RTC->SUB_b.rtc_sub;

    // RTCSUB is accurate in all clock speeds and battery modes.
    while (0 != cnt)
    {
        // Has the time changed? (often not)
        rtc_sub = RTC->SUB_b.rtc_sub;
        if (old_tck_cnt != rtc_sub)
        {
            tck_cnt = 0xff & (rtc_sub - old_tck_cnt); 

            if (tck_cnt < cnt) // Timer expired?
            {
                // count timer down.
                cnt = cnt - (int)tck_cnt;
            } else {
                // The count expired, so stop the timer.
                cnt = 0;
            }   

            old_tck_cnt = rtc_sub;
        }
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
int rtc_subsec_read (void)
{
    int rtc_time;
    int sub_sec_a, sub_sec_b;
    int tmp;

    do {
        sub_sec_a = RTC->SUB;
        rtc_time = RTC->TIME;
        sub_sec_b = RTC->SUB;
    } while (sub_sec_a != sub_sec_b);

    tmp = 60 * (0xff & (rtc_time >> 16));
    tmp += 0xff & (rtc_time >> 8);
    tmp *= 60;
    tmp += 0xff & rtc_time;
    tmp *= SUBSEC_PER_SEC;
    tmp += RTC->SUB_b.rtc_sub;

    return tmp;
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

