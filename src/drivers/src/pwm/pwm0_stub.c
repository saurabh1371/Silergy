/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: PWM0 Output driver.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: pwm0.c 17447 2018-04-25 20:28:26Z ptanwar $
 ****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "pwm0.h"       // Function prototypes.

/*** Dependencies ***/
// None

/*** Version ****/
const char fw_ver_pwm0[] =
"pwm0_stub" /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

/*****************************************************************************
* Description:
*    Starts PWM output at desired frequency and Duty Cycle.
* Parameters:
* Input:
*        uint32_t freq_hz  : Sets PWM output frequency in Hz.
*        uint8_t duty_cycle: Sets PWM output Duty Cycle.
*                             Input value range from 1 to 100 for 1% to 100%
*                             duty Cycle.
* Output:
*         None.
* Return Status:
*         int: 0 for success
*              1 for failure
*****************************************************************************/
int pwm0_start (uint32_t freq_hz, uint32_t duty_cycle)
{
    return 0;
}


/*****************************************************************************
* Description:
*    Stops PWM output.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
*****************************************************************************/
void pwm0_stop (void)
{
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

