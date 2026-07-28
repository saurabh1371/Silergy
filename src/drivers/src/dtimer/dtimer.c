/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Dual timer ISR routine.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: dtimer.c 17148 2018-03-29 15:41:30Z ptanwar $
 ****************************************************************************/
#include <stdbool.h>    // Booleans.
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "dtimer.h"

// Static declaration

// Dual timer0 event handler
void (*pDtimerEventHandler0)(void) = {NULL};   
// Dual timer1 event handler
void (*pDtimerEventHandler1)(void) = {NULL}; 


/*****************************************************************************
* Description:
*         Dual Timer interrupt handler. Clears Timer interrupt bit and
*          calls user-defined event handler function.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
******************************************************************************/
void DTIMER_Handler(void)
{

    if(DTIMER0->STAT_b.timer_i) 
    {
        DTIMER0->INT = 1; // Clear DTIMER Interrupt.

        // Call user-defined event handler, if any
        if (NULL != pDtimerEventHandler0)
            (*pDtimerEventHandler0)();
    }     

    if(DTIMER1->STAT_b.timer_i) 
    {
        DTIMER1->INT = 1; // Clear DTIMER Interrupt.

        // Call user-defined event handler, if any
        if (NULL != pDtimerEventHandler1)
            (*pDtimerEventHandler1)();
    }     
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
