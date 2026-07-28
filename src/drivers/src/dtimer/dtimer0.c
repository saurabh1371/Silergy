/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Dual timer 0 driver.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: dtimer0.c 17148 2018-03-29 15:41:30Z ptanwar $
 ****************************************************************************/
#include <stdbool.h>    // Booleans.
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "dtimer.h"


/*** Version ****/
const char fw_ver_dtimer0[] =
"dtimer0"  /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

/****************************************************************************
* Description:
*    Starts Dual timer and registers a event handler function.
*    Event handler function is called when timer expires.
* Parameters:
*   Input:
*         uint32_t ticks: Dual-Timer ticks
*         tmode tm: Timer Modes - FREERUN, PERIODIC, ONESHOT
*         void(*func)(void) : Pointer to the call-back function.
*   Output:
*         None.
* Return Status:
*        None.
****************************************************************************/
void dtimer0_start(uint32_t ticks, tmode tm, void(*func)(void))    
{
    // Stop Dual-timer
    dtimer0_stop(); 

    if (NULL != func)
    {
        pDtimerEventHandler0 = func; // Assign function pointer to event handler.
    }


    switch(tm)
    {
        case FREERUN:  // Free running and wrapping
            DTIMER0->CNTL = 0; 
        break;

        case PERIODIC: // Periodic timer mode
            DTIMER0->CNTL_b.timer_mode = 1;    // Periodic mode
        break;

        case ONESHOT:  // One-shot timer mode
            // DTIMER0->CNTL_b.timer_mode = 1;    // Periodic mode
            DTIMER0->CNTL_b.count_mode = 1;    // One-shot count mode
        break;
    }

    DTIMER0->LOAD = ticks; 
    DTIMER0->CNTL_b.timer_pre  = 0;  // Prescale, clk/1, default 
    DTIMER0->CNTL_b.timer_size  = 1; // 32-bit counter
    DTIMER0->CNTL_b.timer_ie  = 1;   // Enable Dual-timer Interrupt 
    DTIMER0->CNTL_b.timer_en = 1;    // Enable Dual-timer

    NVIC_EnableIRQ(DTIMER_IRQn); // Enable DTIMER_IRQn  
}

/*****************************************************************************
* Description:
*    Stops the Dual Timer.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
*****************************************************************************/
void dtimer0_stop(void)
{
    DTIMER0->CNTL = 0;   // Disable dual-Timer 
    DTIMER0->BGLOAD = 0; 
    DTIMER0->LOAD = 0;  
    DTIMER0->INT = 1;   // Clear any pending interrupt
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
