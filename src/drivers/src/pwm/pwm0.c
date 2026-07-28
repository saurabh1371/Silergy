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
"pwm0"    /* Device Driver name */
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
    uint32_t per, wid;

    pwm0_stop();

    if(duty_cycle > 100)  // valid range from 1 to 100
        return 1;  // Error


    if(ADC->CNTL_b.adc_vls) // If VLS bit is set, SYSCLK is NOT auto-compensated. 
    {    
        // Set the output period.  It sets the width of a half-cycle.  
        // round-off to nearest positive integer
        per = (int) ((SYSTEM_CLOCK_F/((float)freq_hz * 2.0 * 13.0)) + 0.5);
    }    
    else // SYSCLK is auto-compensated. 
    {
        // Set the output period.  It sets the width of a half-cycle.  
        // round-off to nearest positive integer
        per = (int) ((SYSTEM_CLOCK_F/((float)freq_hz * 2.0)) + 0.5) ;
    }       

    // Set the output pulse width. If WIDTH > PERIOD, output is NOT asserted.
    wid = (int) (((float)per * (100.0 - (float)duty_cycle))/100.0 + 0.5); 

    // Return Error, if per is greater than 16-bit counter value
    if((per >= 0xFFFF))
        return 1; // Error

    // Set both alternate && dir for DIO32 to work as PWM0
    LCD->DIO_b[ pwm0_seg ].dir = 1;    
    LCD->DIO_b[ pwm0_seg ].alt = 1;

    // Set Width and Duty Cycle.
    PWM->PWM0_b.period = per;
    PWM->PWM0_b.width = wid;
    PWM->CNTL_b.pwm0_en = 1; 
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
    PWM->PWM0 = 0;
    PWM->CNTL_b.pwm0_en = 0; 
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

