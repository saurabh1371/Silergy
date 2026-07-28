/*****************************************************************************
 * DESCRIPTION: billing register save and restore system.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2012...2013, Silergy Corp.   All rights reserved.
 *
 * $Id: reg.h 18859 2018-09-26 23:02:54Z rvandewa $
 ****************************************************************************/
#ifndef REG_H
#define REG_H 1


// Library version
extern const char fw_ver_reg[]; 


// Tells which copy to save. true = save the RAM copy.
// false = save the stable copy in nonvolatile RAM.
extern volatile bool save_reg_data;

// Sets the CRC and manages the double-buffering that the billing
// data requires in order to be saved at any instant by a power-failure.
// That is, there is always a valid copy of the billing data, and
// reg_put can be called at any time.
void reg_crc(void);

// Restores the best available set of billing-register data.
// Multiple sets exist to enhance the reliability of the system.
// The sets are stored in different places, at different times.
// In testing with EMI, about 1 / 500 register saves failed.
// Using 6 copies reduces this to (1/500)^6 = 6.4e-17, about 1000
// times better than the reliability of CMOS gates.
// This should be called before interrupts are enabled, so that if
// a power failure occurs, a valid copy of current register data is
// available in RAM whenever interrupts are enabled.
void reg_get(void);

// Saves one (1) copy of the billing-register data.
// This is reentrant. It can be called from an interrupt.
// Each call saves the data to a different place than the preceding call.
// The locations rotate on a schedule: 0102010301020104010201030102010
// So, location 4 is used half as often as 3, which is half as often as 2, etc.
void reg_put (void);

// Clears billing registers. Not reentrant, and not synchronized with
// data collection cycle.  That is, it must be called from a particular
// time in the main loop.
void reg_reset(void);

// Register data. Keep it small so it can be saved quickly
// to EEPROM during a power failure.
// A very simple meter could cut everything except
// state_bit_ary, wh_cnt, ver_hash and data_ok_reg.
struct reg_s
{
    Log_t log;              // Event log is here so it's nonvolatile.
    int32_t wh_cnt;         // )21, Wh, pulse count. Mandatory.
    int32_t wh_cnt_ex;      // )22, Wh, exported, pulse count. Optional.
    int32_t varh_cnt;       // )23, VARh, pulse count. Optional.
    int32_t varh_cnt_ex;    // )24, VARh, exported, pulse count. Optional.
    int32_t dmd_max;        // )25, Max demand, W. Optional.
    int32_t tamper_sec;     // )2B, Seconds of tamper. Optional.
    int32_t sag_sec;        // )2C, Seconds Vmain < v_min. Optional.
    int32_t operating_sec;  // )2E, Seconds of operation. Optional.
    tm_t tm;                // Most recent clock time. Mandatory for RTC.
    int8_t dmd_mon;         // )26, month of max. demand
    int8_t dmd_mday;        // )27, month-day of max. demand
    int8_t dmd_hour;        // )28, hour of maximum demand
    int8_t dmd_min;         // )29, minute of maximum demand
    int8_t reg_cnt;         // )2D, Count of register saves.  Optional.
    uint8_t ver_hash;       // )2E, hash of firmware version. Mandatory.
    uint8_t data_ok_reg[2]; // Check-data for registers. Mandatory.
};
typedef struct reg_s Reg_t;


/***************************************************************************
 * Copyright (C) 2011..2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // reg_h
