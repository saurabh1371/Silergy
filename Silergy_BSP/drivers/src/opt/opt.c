/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Optical UART (IRDA and Optical PWM) device driver
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: opt.c 17447 2018-04-25 20:28:26Z ptanwar $
 ****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "gpio.h"  
#include "opt.h"

const char fw_ver_opt[] =
"opt"     /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

/****************************************************************************
* Description:
*    Initialize Optical Receiver (IRDA).
* Parameters:
*   Input:
*         uint32_t baud: Set baud rate
*   Output:
*         None.
* Return Status:
*         int: 0 for success
*              1 for failure
*****************************************************************************/
int opt_rx_init (uint32_t baud)
{
    uint32_t setbaud;  

    if(ADC->CNTL_b.adc_vls) // If VLS bit is set, SYSCLK is NOT auto-compensated.
        setbaud = (int)(SYSTEM_CLOCK_F/(float)(baud * 13.0) + 0.5); // round-off to integer.
    else
        setbaud = (int)(SYSTEM_CLOCK_F/(float)(baud) + 0.5); // round-off to integer.

    if(setbaud > 0x3FFF)  // Exeeedes LOW_COUNTER
        return 1; // Error


    gpio_alt_on(opt_rx_seg);// Set Alternate Func
    // Set Width and Duty Cycle.
    OPT->RX_CNTL_b.low_counter = setbaud; // Set Baud 
    OPT->RX_CNTL_b.rx_en = 1; // Enable IRDA

    return 0; // Success
}


/****************************************************************************
* Description:
*    Initialize Optical Transmitter (OPT PWM)
* Parameters:
*   Input:
*         uint32_t baud: Set baud rate
*         uint32_t pwidth: Pulse Width
*   Output:
*         None.
* Return Status:
*         int: 0 for success
*              1 for failure
*****************************************************************************/
int opt_tx_init(int baud, float pwidth)
{
    int setbaud,setwidth;   

    // Set the output period.  It sets the width of a half-cycle.  
    if(ADC->CNTL_b.adc_vls) // If VLS bit is set, SYSCLK is NOT auto-compensated.
        setbaud = (int)(SYSTEM_CLOCK_F/(float)(baud * 2.0 * 13.0) + 0.5);  
    else 
        setbaud = (int)(SYSTEM_CLOCK_F/(float)(baud * 2.0) + 0.5);  

    // setpwidth = (int)((float)setbaud*(1.0 - pwidth) + 0.5);
    setwidth = (int)((float)setbaud*(1.0 - pwidth));

    if((setbaud > 0xFFF) || (setwidth >= setbaud))
    {
        return 1;   // Error
    }	 

    gpio_alt_on(opt_tx_seg);// Set Alternate Func

    // Set Width and Duty Cycle.
    OPT->WP_b.period = setbaud; 
    OPT->WP_b.width = setwidth; 
    OPT->TX_CNTL_b.tx_en = 1; 

    return 0; // Success
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
