/*----------------------------------------------------------------------------
 * Purpose: Driver for TMUX
 * $Id: tmux.c 17447 2018-04-25 20:28:26Z ptanwar $ 
 *----------------------------------------------------------------------------
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *----------------------------------------------------------------------------*/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "gpio.h"
#include "tmux.h"

/*** Version ****/
const char fw_ver_tmux[] =
"tmux"    /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

/****************************************************************************
* Description:
*       Configure TMUX0 for the test signal
* Input:
* Parameters:
*       signal is the enum for TMUX0 test signal
* Output:
* Return Status:
*       None.
*****************************************************************************/
void tmux0_config(e_tmux0_signal signal)
{
    if(tmux0_low == signal)
        gpio_alt_off(tmux0_seg);
    else
        gpio_alt_on(tmux0_seg); // Alternate mode -> TMUX0

	TMUX->TMUXSEL_b.tmux0_sel = signal; // Assign signal to be out on test pin
}

/****************************************************************************
* Description:
*       Configure TMUX1 for the test signal
* Input:
* Parameters:
*       signal is the enum for TMUX1 test signal
* Output:
* Return Status:
*       None.
*****************************************************************************/
void tmux1_config(e_tmux1_signal signal)
{
    if(tmux1_low == signal)
        gpio_alt_off(tmux1_seg);
    else
        gpio_alt_on(tmux1_seg); // Alternate mode -> TMUX0

	TMUX->TMUXSEL_b.tmux1_sel = signal; // Assign signal to be out on test pin
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 

