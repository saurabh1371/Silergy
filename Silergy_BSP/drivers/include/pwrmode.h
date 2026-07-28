/***************************************************************************
 * DESCRIPTION: POWER METER - Battery Mode Routines.
 * This code controls the battery modes.
 *
 * Copyright (C) 2013-2017 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * $Id: pwrmode.h 24510 2020-04-21 19:30:02Z rvandewa $
 ***************************************************************************/
#ifndef PWRMODE_H
#define PWRMODE_H 1

/*** Version ****/
extern const char fw_ver_pwrmode[];

#define WF_BADVDD_MASK   0x00000100
#define WF_OSCFAIL_MASK  0x00000200
#define WF_WDOF_MASK     0x00000800
#define WF_RSTBIT_MASK   0x00001000
#define WF_RST_MASK      0x00002000
#define WF_CSTART_MASK   0x00004000
#define WF_PB0_MASK      0x00080000
#define WF_WK0_MASK      0x00100000
#define WF_WK1_MASK      0x00200000
#define WF_RX0_MASK      0x00400000
#define WF_TMR_MASK      0x00800000
#define WF_TMP_MASK      0x01000000
#define WF_VSYS_MASK     0x02000000
#define WF_VLCT_MASK     0x04000000


// Read the power state.
//  true = SOC switched to battery power, false = running on mains
bool pwrmode_is_brownout(void);

// Read the battery's power state.
// true = battery is ok, false = battery is bad.
int pwrmode_vbat_is_ok(void);

// Read the VLCT pin's (i.e. alternate power supply) power state.
// Only in SY7T_M11-series parts.
// true = VLCT is ok, false = it's bad.
int pwrmode_vlct_is_ok(void);

// Read the power state.
//  true = battery (& VLCT) are failing (Vdd is out of regulation), 
//  false = power is ok, somewhere
bool pwrmode_is_fail(void);

// Set RTC_WAKE.
void pwrmode_set_wake(int rtcwake);

// Arrange a timer to leave sleep or LCD only mode.
//  Time in wake timer's native units.
void pwrmode_wait(int time);
#define PWRMODE_US(_x_) ((_x_ + 125000) / 250000)
#define PWRMODE_MS(_x_) ((_x_ + 125) / 250)
#define PWRMODE_SEC(_x_) (_x_* 8)

// These routines do not exit. They enter a low-power mode.
// The wake bits determine events that will cause the CPU to start.
// When the CPU starts, it starts from the reset vector.

// Enter sleep mode.
// Sets the wake bits.
void pwrmode_sleep(int wake);

// Enter LCD-only mode.
//  Sets the wake input.
void pwrmode_lcd(int wake);

// Enter voltage line cut-mode (VLCT) mode. (Only in SY7T_M11 parts.)
//  Sets the wake input.
void pwrmode_vlct(int wake);

// Serve the power fail interrupt.
// If there is no power (i.e. VDD is out of regulation), it enters sleep.
void pwrmode_fail_Handler(void);

#endif // PWRMODE_H
/***************************************************************************
 * Copyright (C) 2014-2020 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

