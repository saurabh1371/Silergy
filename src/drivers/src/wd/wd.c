/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Watchdog Timer. 
 *
 * Copyright 2020, Silergy Corp. All rights reserved.
 *
 *
 * $Id: wd.c 24475 2020-04-20 12:25:09Z dhb $
 ****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "wd.h"         // Check definitions.

/*** Version ****/
const char fw_ver_wd[] =
"wd"      /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

/****************************************************************************
* Description:  
*       Reset Watch Dog  timer.
* Parameters:
*   Input:
*         None
*   Output:
*         None.
* Return Status:
*         None
****************************************************************************/
void wd_reset(void)
{
    TEMP->CNTL_b.wd_clr = 1;  // Set the watchdog clear bit
}


/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
 
