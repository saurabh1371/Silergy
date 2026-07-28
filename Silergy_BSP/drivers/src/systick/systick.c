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
#include <stdlib.h>				 
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "systick.h"

// The maximum value of the systick timer's counter.
#define SYSTICK_MAX (SysTick_LOAD_RELOAD_Msk)

const char fw_ver_systick[] =
"systick" /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

void (*systick_isr)(void); // Callback for Systick interrupt.

/****************************************************************************
* Description:
*    The systick interrupt handler.
*    This calls the callback function pointer, if any.
*
* Parameters:
*   Input:
*         None
*   Output:
*         Count of interrupts.
*   Return Status:
*         None.
****************************************************************************/
void SysTick_Handler(void)
{
    if(NULL!= systick_isr)
    {
        (*systick_isr)();
    }
}

/****************************************************************************
* Description:
*    Starts the timer as an interrupting periodic timer.
*    The period is in cycles.
*    Very similar to CMSIS's systick config, except it has a callback, and
*    it doesn't check its parameter.
*
* Parameters:
*   Input:
*       The function pointer for the interrupt routine. Use NULL for none.
*       Period in cycles.
*   Output:
*   Return Status:
*       None.
****************************************************************************/
void systick_interrupt(void (*fn_ptr)(), int clock_cnt)
{
    SysTick->CTRL = 0;              // Stop the timer.
    systick_isr = fn_ptr;           // Set the interrupt callback.
    SysTick->LOAD = clock_cnt - 1;  // Set the reload value.
    SysTick->VAL = 0;               // Force an immediate reload.
    // set Priority for Systick Interrupt
    NVIC_SetPriority (SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    // Start counting, and enable the interrupt.
    SysTick->CTRL = 
          SysTick_CTRL_ENABLE_Msk    // Start counting.
        | SysTick_CTRL_CLKSOURCE_Msk // Use the processor clock.
        | SysTick_CTRL_TICKINT_Msk;  // Enable the interrupt.
}

/****************************************************************************
* Description:
*    Returns true if the systick timer is running.
*    This does not check if the timer is interrupting.
*
* Parameters:
*   Input:
*       None. Reads the value and control registers.
*   Output:
*   Return Status:
*       As named.
****************************************************************************/
int systick_is_on(void)
{
    int ctrl;

   	ctrl = SysTick->CTRL; // Read the control register. This clears status.
    // Is timer expired?
    if ( 0 != ( ctrl & SysTick_CTRL_COUNTFLAG_Msk ) ) // If it reached 0
    {
        SysTick->CTRL = 0;  // Stop counting.
        return false;       // Timer turned over, i.e. expired.
    }
    return true;            // Timer has not turned over, i.e. not expired.
}
int systick_is_off(void) { return (false == systick_is_on()); }

/****************************************************************************
* Description:
*    Starts the timer with a time. It counts down to zero.
*    The interrupt is not enabled.
*
* Parameters:
*   Input:
*         Cycle count.
*   Output:
*   Return Status:
*         None.
****************************************************************************/
void systick_start(int clock_cnt)
{
    SysTick->CTRL = 0;              // Stop the timer.
    SysTick->LOAD = clock_cnt - 1;  // Set the count
    SysTick->VAL = 0;               // Immediate reload.
    SysTick->CTRL =
        SysTick_CTRL_ENABLE_Msk     // Start counting.
      | SysTick_CTRL_CLKSOURCE_Msk; // Use the processor clock.
}

/****************************************************************************
* Description:
*    Starts the timer to measure the elapsed time in cycles.
*    The interrupt is not enabled.
*
* Parameters:
*   Input:
*         None.
*   Output:
*   Return Status:
*         None.
****************************************************************************/
void systick_measure(void)
{
    SysTick->CTRL = 0;              // Stop the timer.
    SysTick->LOAD = SYSTICK_MAX;    // Maximum counter.
    SysTick->VAL = 0;               // Immediate reload.
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk
        | SysTick_CTRL_CLKSOURCE_Msk; // Start counting from internal clock.
}

/****************************************************************************
* Description:
*    Get the elapsed time in cycles since systick_measure() was called.
*    The timer does not stop.
*
* Parameters:
*   Input:
*         None.
*   Output:
*   Return Status:
*        elapsed time in clock cycles.
****************************************************************************/
int systick_elapsed_adjust = 0x0c; // Overhead for interval measurement.
int systick_elapsed(void)
{
    int q, ctrl;

    q	= SysTick->VAL;     // Read the down-counter.
    ctrl = SysTick->CTRL;   // Read the control register. This clears status.
    // If it hasn't reached 0
    if ( 0 == (ctrl & SysTick_CTRL_COUNTFLAG_Msk)  
         // and it is still enabled.
         && 0 != (ctrl & SysTick_CTRL_ENABLE_Msk))    
    {
        // Get the time from the down-counter.
        // Assumes it was set with the maximum value.
        return (SYSTICK_MAX - (q - systick_elapsed_adjust));
    }
    // Else, the counter overflowed.
    SysTick->CTRL = 0;              // Stop the timer.
    SysTick->VAL = 0;               // Maximum elapsed time...
    return SYSTICK_MAX;
}

/****************************************************************************
* Description:
*    Get the elapsed time in cycles since systick_measure() was called.
*    Stops the timer.
*
* Parameters:
*   Input:
*         None.
*   Output:
*   Return Status:
*        elapsed time in clock cycles.
****************************************************************************/
int systick_stop_adjust = 0x01e;    // Overhead for stop's measurement.
int systick_stop(void)
{
    // Get the elapsed time.
    int cycle_cnt = systick_elapsed() - systick_stop_adjust;

    SysTick->CTRL = 0;              // Stop the timer.
    SysTick->LOAD = 0;              // Prevent reload
    SysTick->VAL = 0;               // Return maximum elapsed time.

    return cycle_cnt;
}

/****************************************************************************
* Description:
*    Calibrate the measurements that use systick.
*
* Parameters:
*   Input:
*       None. 
*   Output:
*   Return Status:
*       None. 
****************************************************************************/
void systick_cal(void)
{
    systick_elapsed_adjust = 0;
    systick_stop_adjust = 0;

    // Interval between start and measurement should be zero.
    // So, measure the overhead, so it can be subtracted.
    systick_measure();      // Start systick
    systick_elapsed_adjust = systick_elapsed(); // Get the measurement.

    systick_measure();
    systick_stop_adjust = systick_stop();
}

/****************************************************************************
* Description:
*    Delays for a precise time in cycles. Starts and Stops the timer.
*
* Parameters:
*   Input:
*       Time to delay in cycles.
*   Output:
*   Return Status:
*       None.
****************************************************************************/
void systick_delay(int clock_cnt)
{
    SysTick->CTRL = 0;              // Stop the timer.
    SysTick->LOAD = clock_cnt - 1;  // Set the reload value.
    SysTick->VAL = 0;               // Immediate reload.
    SysTick->CTRL =
        SysTick_CTRL_ENABLE_Msk     // Start counting.
        | SysTick_CTRL_CLKSOURCE_Msk; // Use the processor clock.

    // COUNTFLAG is a bit that is set to 1 when counter reaches 0.
    // It's automatically cleared when read.
    while ( 0 == (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) )
    {
    }

    SysTick->CTRL = 0;              // Stop the timer.
    SysTick->LOAD = 0;              // Prevent reload
    SysTick->VAL = 0;               // Set the count value.
}

/***************************************************************************
 * Copyright (C) 2018 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/

