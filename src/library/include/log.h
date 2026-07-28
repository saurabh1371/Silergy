/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp.  All Rights Reserved.
 * DESCRIPTION: POWER METER - error recording
 * The data is kept in nonvolatile area. There are both compact
 * bit-based logs and log records with date, etc.
 * Space is more limited for the records, so only the most important (highest
 * numbered) events get records.
 * The CLI has output routines.
 *
 * $Id: log.h 23908 2020-03-20 03:13:04Z rvandewa $
 ***************************************************************************/
// File: error.h

#ifndef LOG_H
#define LOG_H	1


// Library version
extern const char fw_ver_log[]; 


// Default number of log records.
// It can be redefined in mappcb.h More records may make the register data too
// big to fit in NVRAM.
#ifndef LOG_REC_CNT
#define LOG_REC_CNT 5
#endif

// If zero, the log is not valid. Save directly to the NVRAM.
extern uint8_t log_valid;

// Log record. 
typedef struct
{
    uint8_t ev, year, mon, mday, hour, min, cnt;
} Log_rec_t;
typedef struct
{
    uint32_t bits, bits_sticky;
    #if 0!=LOG_REC_CNT
    Log_rec_t rec_ary[LOG_REC_CNT];
    #endif
} Log_t;

// Define events by increasing importance
// To change priorities of events, reorder this list.
typedef enum {
    // ev_none is required by design to be the lowest-priority event.
    EV_NONE = 0,        // Indicates no event or an empty log record.
    EV_LINE_BAD,        // Power is measured from neutral.
    EV_COM,             // Communication occurred.
    EV_RTC_ALARM,       // RTC alarm
    EV_TEMP_ALARM,      // Temperature range alarm
    // Events following this comment are nonvolatile. (See EV_NONVOLATILE)
    EV_SOFTWARE,        // Unknown software failure
    EV_WATCHDOG,        // Software watchdog reset.
    EV_OVERVOLTAGE,     // AC Mains voltage above design limit.
    EV_OVERCURRENT,     // AC Mains current above design limit.
    EV_HARDFAULT,       // Hard fault.
    EV_HARDWARE,        // Unknown hardware failure
    EV_SPURIOUS,        // Spurious interrupt
    EV_FUSEFAIL,        // Fuse failure detected
    EV_RTC_CORRUPT,     // RTC found corrupted, set from previous time.
    EV_VLCT_BAD,        // Voltage-line cut power pin voltage not run the CPU.
    EV_CAL_BAD,         // Calibration had CRC failure or was unreadable.
    EV_BAT_BAD,         // Battery voltage is too low to run the CPU.
    EV_SAG,             // AC Mains voltage below tariff limit.
    EV_RTC_UNSET,       // RTC has not been set.
    EV_DEBUGGER,        // Debugger was detected.
    EV_RTC_TAMPER,      // RTC set more than two hours from previous time.
    EV_MAG_TAMPER,      // Magnetic tamper.
    EV_I_NO_V,          // One-wire mode detected.
    EV_TERM_TAMPER,     // Mechanical tamper with terminal block cover.
    EV_CASE_TAMPER,     // Mechanical case tamper.
    EV_REG_BAD,         // Billing registers had CRC failure or was unreadable.
    // ev_log_bad should be the highest-priority event.
    // If the log is bad, it affects interpretation of all the events.
    EV_LOG_BAD,         // Log had a CRC failure or was unreadable.
} Ev_t;
#define EV_MIN (EV_NONE)
#define EV_MAX (EV_LOG_BAD)
// At start-up, events from EV_MIN to EV_NONVOLATILE are deleted.
#define EV_NONVOLATILE (EV_SOFTWARE)

// Define tamper events. (A bitwise 'and' of this mask with the bit log
// permits the LCD driver to display log events easily.)
#define M_I_NO_V (1UL << (int)EV_I_NO_V)
#define M_MAG_TAMPER (1UL << (int)EV_MAG_TAMPER)
#define M_CASE_TAMPER (1UL << (int)EV_CASE_TAMPER)
#define M_TERM_TAMPER (1UL << (int)EV_TERM_TAMPER)
#define M_RTC_TAMPER (1UL << (int)EV_RTC_TAMPER)
#define M_DEBUGGER (1UL << (int)EV_DEBUGGER)
#define LOG_TAMPER_MASK ( \
    M_I_NO_V \
    | M_RTC_TAMPER \
    | M_MAG_TAMPER \
    | M_CASE_TAMPER \
    | M_TERM_TAMPER \
) // tampers

#if EVMAX>31
#error Too many types of events to fit in 32-bit word.
#endif

// Log a software failure.
void log_software(void);

// record events.
void log_event (Ev_t ev);

// record and clear states.
void log_state (Ev_t ev, bool on);

// clear all the old events.
void log_clear (void);

// clear one old event.
void log_clear_one (Ev_t ev);

// Test to see if a condition is recorded.
bool log_active (Ev_t ev);

// Test to see if a condition was recorded since
// the log was last cleared.
bool log_historic (Ev_t ev);

// Get a log record, if it exists.
Log_rec_t *log_get_rec (Ev_t ev);

// initialize log. 
// Volatile events from previous start-ups are cleared. 
// Events after reset, but before this call are logged. 
void log_init (void);

// Error system is unavailable when this variable is set
// to any random value, like 0, 1 or 0xff.
extern uint8_t log_lock;
#define MAX_EV_CNT UINT8_MAX

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif /* error.h */
