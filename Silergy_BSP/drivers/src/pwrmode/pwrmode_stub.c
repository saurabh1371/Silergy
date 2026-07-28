/***************************************************************************
 * DESCRIPTION: POWER METER - Battery Mode Routines.
 * This code controls the battery modes.
 *
 * Copyright (C) 2013-2020 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * $Id: pwrmode_stub.c 24708 2020-04-29 20:36:29Z rvandewa $
 ***************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "pwrmode.h"    // Function prototypes.

/** Dependencies ***/

/*** Version ****/
const char fw_ver_pwrmode[] =
"pwrmode_stub"  /* Device Driver name */
" v"       /* ver string */
"3.00"     /* Major_ver.Minor_ver */
"\r\n"     /* CR + LF */
;


// Read the power state.
//  true = SOC can't measure power, false = SOC can measure power
bool pwrmode_is_brownout(void) { return false; }

// Read the battery's power state.
// true = battery is ok, false = battery is bad.
int pwrmode_vbat_ok(void) { return true; }

// Read the VLCT pin's (i.e. alternate power supply) power state.
// true = VLCT is ok, false = it's bad.
int pwrmode_vlct_ok(void) { return true; }

// Read the power state.
//  true = battery & VLCT are failing (Vdd is out of regulation), 
//  false = power is ok
bool pwrmode_is_fail(void) { return false; }

// Set RTCWAKE.
void pwrmode_set_wake(int rtcwake) { rtcwake++; }

// Arrange a timer to leave sleep or LCD only mode.
//  Time in wake timer's native units.
void pwrmode_wait(int time) { time++; }

// These routines do not exit. They enter a low-power mode.
// The wake bits determine events that will cause the CPU to start.
// When the CPU starts, it starts from the reset vector.

// Enter sleep mode.
// Sets the wake bits.
void pwrmode_sleep(int wake) { ++wake; SYS->MOD_CNTL_b.reset = 1; }

// Enter LCD-only mode.
//  Sets the wake input.
void pwrmode_lcd(int wake) { ++wake; SYS->MOD_CNTL_b.reset = 1; }

// Enter voltage line cut-mode (VLCT) mode.
//  Sets the wake input.
void pwrmode_vlct(int wake) { ++wake; SYS->MOD_CNTL_b.reset = 1; }

// Serve the power fail interrupt.
// If there is no power (i.e. VDD is out of regulation), it enters sleep.
void power_fail_Handler(void) { }

/***************************************************************************
 * Copyright (C) 2014-2020 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

