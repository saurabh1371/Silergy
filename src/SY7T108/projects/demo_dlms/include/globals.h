/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: This file has math functions that are not present in
 * the standard library.
 * 
 * DESCRIPTION: This defines global data structures for an electricity 
 * metering system.
 *
 * $Id: globals.h 25648 2020-06-10 11:00:29Z phuddar $
 **************************************************************************/
#ifndef GLOBALS_H
#define GLOBALS_H 1


// Miscellaneous data. Used for globals
typedef struct
{
    int32_t acc_cnt;        // )3F, Count of accumulation intervals.
    volatile int32_t last_wh_cnt_saved; // 0 = power registers are unlocked.
    volatile int32_t last_vah_cnt_saved; // 0 = power registers are unlocked.
    volatile int lcd_timer; // time to next scroll of LCD.
    int32_t wh_frac;        // Wh "fractional pulse counter"
    int32_t varh_frac;      // VARh "fractional pulse counter"
    int32_t wh_frac_ex;     // Wh export fractonal count
    int32_t varh_frac_ex;   // VARh export fractonal count
    int32_t vah_cnt;        // VAh count
    int32_t vah_frac;       // VAh fractional count
    int32_t dmd_cnt;        // demand count Wh, VAh, whatever
    int32_t dmd_frac;       // demand fractional count
    int32_t dmd_acc_cnt;    // acc_cnt from start of demand interval.
    int32_t zc_cnt;         // Count of zero crossings in main.
    uint32_t last_sec;      // detect change of second.
    int16_t temp_c;         // )16..1A, temp, LSB = 0.1C
    uint16_t lcd_idx_old;   // detect change of LCD.
    tm_t dmd_tm;            // Start of most recent demand interval.
    uint8_t last_min;       // detect change of minute.
    uint8_t last_hr;        // detect change of hour.
    uint8_t in_cnt;         // I neutral hysteresis (down counter)
    uint8_t wd_want;        // watchdog events expected (bit per event).
    uint8_t wd_set;         // watchdog events so far (set as bit).
    uint8_t mains_subsec;   // count mains cycles for s/w RTC
    uint8_t wake_secs;      // count of wake seconds.
    int8_t remote_idx;      // remote whose temperature is being read.
    int8_t save_cnt;        // Last read register save count
} Misc_t;

//Global data in nonvolatile memory.
typedef struct
{
    // A log is included in the reg structure.
    Reg_t reg;              // from main.h stable copy of registers.
    uint32_t mission_mode_ended; // Julian time that mission mode ended.
    uint32_t last_wake_src; // Last reset's wake source register value.
    uint32_t last_wake;     // Last reset's wake register value.
    uint8_t wd_fail;        // Which watchdog events failed?
} Nvm_t;
// Defined in main.c
extern Nvm_t nvm;

// Global data.
typedef struct
{
    // Calibration data from cal.h, mpu's calibration data.
    Cal_t cal;
    // Registers from reg.h, a scratch copy of registers.
    Reg_t reg;
    // Miscellaneous global data from this file.
    Misc_t misc;
} Global_t;
extern Global_t global;

// global.cal.cfg system configuration bits:
#define DISPLAY_KWH 0x01    // Display registers in KWh, ")1=1" Save the config.
#define CLEAR_REG   0x02    // Clear the registers ")1=2"
#define RESET_DEMAND 0x04   // Reset demand ")1=4"
#define CE_RAW      0x08    // CE raw values; 1=MPU does not change them.
// unused           0x10    //
#define MANUAL_TEMP 0x20    // Set temperature manually. Used to test software Vref calcs.
#define AUTO_CAL    0x40    // Data collection mode of autocalibration
#define TAMPER_DET  0x80    // Enabled tamper detection (unused in alpha demo)


extern const ce_t *ce_ptr;  // The current CE. Defined in main.c


/***************************************************************************
 * Copyright (C) 2016 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif /* GLOBALS_H */
