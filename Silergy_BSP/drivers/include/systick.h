/***************************************************************************
 * Name:    systick.c
 * Purpose: Functions to use the systick timer.
 * Use the SystemCoreClock variable to get the current clock speed in Hz.
 * The inputs for clock and time are all in terms of the clock speed.
 *
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2018 Silergy Corp. All Rights Reserved.
 * $Id$  
 ***************************************************************************/
#ifndef SYSTICK_H
#define SYSTICK_H 1

// Units to clock cycles.
// SystemCoreClock is set-up by the CMSIS startup code.
#define SYSTICK_HZ(_x_) (SystemCoreClock / _x_)
#define SYSTICK_MS(_x_) ((SystemCoreClock/1000) * _x_)
#define SYSTICK_US(_x_) ((SystemCoreClock/1000000) * _x_)
// Clock cycles (e.g. from systick elapsed() ) to Units.
#define SYSTICK_TO_HZ(_x_) (SystemCoreClock / _x_)
#define SYSTICK_TO_MS(_x_) ( ( _x_ * 250 ) / ( SystemCoreClock / 4 ) )
#define SYSTICK_TO_US(_x_) ( ( _x_ * 250 ) / ( SystemCoreClock / 4000 ) )

extern const char fw_ver_systick[]; // The version.

// The systick interrupt handler.
void SysTick_Handler(void);

// Starts the systick timer as an interrupting periodic timer.
// The input is a call-back for the interrupt, and the period in cycles.
// When this is used, the timer, elapsed and delay fns below can't work.
void systick_interrupt(void (*fn_ptr)(void), int clock_cnt);

// Detects if the systick timer is running.
// Note that this does not check if the interrupt is enabled.
int systick_is_on(void);
int systick_is_off(void);

// Starts the systick timer as a down-counter.
// The systick interrupt is not enabled or needed.
// When this is used, the timer interrupt can't work.
// The maximum interval is clock-hz / SYSTICK_MAX
// When this is used, the timer, elapsed and delay fns below can't work.
void systick_start(int cycle_cnt);

// Starts the systick timer to measure elapsed time in cycles.
// The systick interrupt is not enabled or needed.
// When this is used, the timer interrupt can't work.
// The maximum interval is clock-hz / SYSTICK_MAX
void systick_measure(void);

// Get the elapsed time in cycles since systick_timer() was called.
// The timer does not stop. The timer interrupt is not enabled or needed.
// If the timer is stopped, the returned number is the maximum.
int systick_elapsed(void);

// Get the elapsed time in cycles since systick_timer() was called.
// The timer stops. The timer interrupt is not enabled or needed.
// If the timer was already stopped, the returned number is the maximum.
int systick_stop(void);

// Measures the program overhead for systick elapsed and stop,
// which then subtract the overhead automatically.
void systick_cal(void);

// Delays for a precise time in cycles. Starts and Stops the timer.
// The timer interrupt is not enabled or needed.
// When this is used, the timer interrupt can't work.
// The maximum interval is clock-hz / SYSTICK_MAX
void systick_delay(int clock_cnt);

/***************************************************************************
 * Copyright (C) 2018 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
#endif

