/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION:POWER METER - Real Time Clock (RTC) driver Code.
 * This code provide utility routines to access RTC, its interrupt
 * and Trim registers.
 * Unlike a UNIX, the meter keeps local time in its RTC hardware, to
 * reduce the access time and power. The auxiliary information to calculate
 * GM/Z is in the calendar software and its configuration.
 * 
 * $Id: rtc.h 17045 2018-03-15 22:51:49Z rvandewa $
 ***************************************************************************/
#ifndef SY7T2xxx_RTC_H_1
#define SY7T2xxx_RTC_H_1 1

/* remove the effects of standard library definitions */
#ifdef tm_t
#undef tm_t
#endif

#define RTC_ERA 2000    // The upper two digits of the year. Valid until 2128

// Month of year (US Standard Numbering, used in the RTC.)
enum Month_e {
    JAN = 0, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC
};
typedef enum Month_e Month_t;

// Day of week (US Standard Numbering, used in the RTC.)
enum Day_e {
    SUN = 0, MON, TUE, WED, THU, FRI, SAT
};
typedef enum Day_e Day_t;

// Daylight savings time flag (UNIX standard)
// >0 = minutes added (yes), 0 = no not DST, -1 = unknown

struct tm
{
    const char      *tz;   // "" for none.
    // Standard US numbering for a year is 1..366, instead, do 0..365
    int16_t         tm_yday;    // 0 - 365 Jan 1 = 0 (for array index)
    int16_t         gmt_offset; // Minutes of offset
    int8_t          tm_sec;     // 0 - 59
    int8_t          tm_min;     // 0 - 59
    int8_t          tm_hour;    // 0 - 23
    int8_t          tm_mday;    // 1 - 31
    int8_t          tm_mon;     // 0 - 11 (JAN..DEC)
    int8_t          tm_year;    // Years since 1900
    int8_t          tm_wday;    // 0 - 6, Sun = 0 (RTC uses US standard)
    // In this implementation, isdst is the current minutes of savings.
    int8_t          tm_isdst;   // <0 unknown, 0 = standard, >0 = daylight
    // When the clock is set, clear subsecs = 0
    uint8_t         tm_subsec; // 0 - 255, counts 1/256 second.
    bool            tm_isvalid; // true = valid
};

#define __tm_t_defined
typedef struct tm tm_t;

// Firmware Device Driver version
extern const char fw_ver_rtc[];

// Returns nonzero if the year in *tm_ptr is a leap-year.
int rtc_leap_year (struct tm *tm_ptr);

// Returns the day in the year of the date in tm_ptr (1..366)
int rtc_get_yday (struct tm *tm_ptr);

// Checks that RTC is usable. Returns nonzero for errors.
// Does not change the RTC value.
int rtc_init(void);

// Read RTC into a timer structure. Does not set the daylight savings time.
void rtc_read(struct tm *tm_ptr);

// Write to RTC registers to set the clock/Calender.
// The subsecond is cleared. In automatic adjustments this causes jitter.
void rtc_write(struct tm *tm_ptr);

// Write to RTC's alarm register to set the alarm.
// The interrupt is enabled by a separate call to sys_int_enable ()
void rtc_alarm_write (int tm_hour, int tm_min, int tm_sec);

// Unset the alarm clock.
void rtc_alarm_clear (void);

// Test a tm_t structure for validity.
// Returns zero if all ranges of each item in the time structure are valid.
// Does not check relations between items, e.g. no test if 1/1/2001 is Monday.
// Common errors: wday = 0, mday = 0, year = 17, not 2017
// The time.c library code can do more testing.
int rtc_invalid(struct tm *tm_ptr);

// Read and write the RTC's trim (i.e. temperature compensation) registers.
void rtc_trim_read (int32_t *tcab, int32_t *tccd);
void rtc_trim_write (int32_t tcab, int32_t tccd);
void rtc_default_trim(void);

// Delay loop that waits for RTC time.
// The resolution is 1/256 second.
void rtc_wait (int cnt);
#define RTC_US(_x_) (((_x_*8) + 15625)/31250) // Microseconds
#define RTC_MS(_x_) (((_x_*64) + 125)/250) // Milliseconds
#define RTC_S(_x_)  (_x_*256) // Seconds

// Read RTC as a count of subseconds since midnight.
// This is handy for high resolution wall-clock timing.
// To extend beyond one day, use time.c library code.
#define SUBSEC_PER_SEC 256
int rtc_subsec_read(void);


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // undefined RTC_H
