/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Timer1 device driver.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: timer1.c 17447 2018-04-25 20:28:26Z ptanwar $
 ****************************************************************************/
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "timer1.h"

const char fw_ver_timer1[] =
"timer1"  /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

// Static declaration
static void (*pEventHandler)(void) = {NULL};    // timer event handler

/****************************************************************************
* Description:
*    Starts auto-reload timer and registers a event handler function.
*    Event handler function is called when timer expires.
* Parameters:
*   Input:
*         uint32_t time_msec: Timer delay in millisecs.
*         void(*func)(void) : Pointer to the call-back function.
*   Output:
*         None.
* Return Status:
*        None.
****************************************************************************/
void timer1_start(uint32_t time_msec, void(*func)(void))	
{
	uint32_t ticks = MS2TICKS(time_msec);
	if (NULL != func)
	{
    // Assign function pointer to event handler.
    pEventHandler = func;
	}	
  /* Enable Timer Interrupt */
	NVIC_EnableIRQ(TIMER1_IRQn);                                                    
  TIMER1->RELOAD = ticks - 1;
  TIMER1->VAL  = ticks - 1;
	/* Timer enabled in IRQ Mode*/
	TIMER1->CNTL_b.timer_en = 1;
	TIMER1->CNTL_b.timer_ie = 1;
}

/*****************************************************************************
* Description:
*    Stops auto-reload timer.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
*****************************************************************************/
void timer1_stop(void)
{
	TIMER1->CNTL  = 0;   // Stop timer , Disable timer interrupt.
	NVIC_DisableIRQ(TIMER1_IRQn);  
	
	// clear timer counter : Reload, Compare and value
	TIMER1->RELOAD = 0;
	TIMER1->VAL  = 0;
}

/*****************************************************************************
* Description:
*         Timer interrupt handler. Clears Timer Overflow bit and
*          calls user-defined event handler function.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
******************************************************************************/
void TIMER1_Handler(void)
{
  TIMER1->INT = 1;
	//  now call user-defined event handler, if any
	if (NULL != pEventHandler)
	{
			(*pEventHandler)();
	}	
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/

