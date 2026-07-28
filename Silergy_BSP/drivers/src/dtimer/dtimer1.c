/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Dual timer 1 driver.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: dtimer1.c 17148 2018-03-29 15:41:30Z ptanwar $
 ****************************************************************************/
#include <stdbool.h>    // Booleans.
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "dtimer.h"


/*** Version ****/
const char fw_ver_dtimer1[] =
"dtimer1"  /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;


// Static declaration
static bool timeExpired;            // timer Expired flag
static void clrTimeoutFlag(void);   // Clear timeout flag.
static void setTimeoutFlag(void);   // Set timeout flag.


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
void dtimer1_start(uint32_t ticks, tmode tm, void(*func)(void))    
{

    // Stop Dual-timer
    dtimer1_stop(); 

    if (NULL != func)
    {
        pDtimerEventHandler1 = func; // Assign function pointer to event handler.
    }

    switch(tm)
    {
        case FREERUN:  // Free running and wrapping
            DTIMER1->CNTL = 0; 
        break;

        case PERIODIC: // Periodic timer mode
            DTIMER1->CNTL_b.timer_mode = 1;    // Periodic mode
        break;

        case ONESHOT:  // One-shot timer mode
            // DTIMER1->CNTL_b.timer_mode = 1;    // Periodic mode
            DTIMER1->CNTL_b.count_mode = 1;    // One-shot count mode
        break;
    }

    DTIMER1->LOAD = ticks; 
    DTIMER1->CNTL_b.timer_pre  = 0;  // Prescale, clk/1, default 
    DTIMER1->CNTL_b.timer_size  = 1; // 32-bit counter
    DTIMER1->CNTL_b.timer_ie  = 1;   // Enable Dual-timer Interrupt 
    DTIMER1->CNTL_b.timer_en = 1;    // Enable Dual-timer

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
void dtimer1_stop(void)
{
    DTIMER1->CNTL = 0;   // Disable dual-Timer 
    DTIMER1->BGLOAD = 0; 
    DTIMER1->LOAD = 0;  
    DTIMER1->INT = 1;   // Clear any pending interrupt
}


/*****************************************************************************
* Description:
*         User-defined event handler function, sets timeExpired flag.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
******************************************************************************/
static void setTimeoutFlag(void)
{
    timeExpired = 1;
}


/*****************************************************************************
* Description:
*         Clears timeExpired flag.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
******************************************************************************/
static void clrTimeoutFlag(void)
{
    timeExpired = 0;
}


/*****************************************************************************
* Description:
*         Microsec Delay implementation using dtimer1.
* Parameters:
*   Input:
*         uint32_t msec_delay : Delay in millisecs.
*   Output:
*         None.
* Return Status:
*         None.
******************************************************************************/
void delay_clocks(uint32_t msec)
{
    // Add execution time for tmr_stop
    dtimer1_stop();  
    
    // Clear timeout flag.
    clrTimeoutFlag();

    // if Dual Timer start
    dtimer1_start(MS2TICKS(msec), ONESHOT, &setTimeoutFlag);

    // now spin
    while(0 == timeExpired) ;

    // Add execution time for tmr_stop
    dtimer1_stop();
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
