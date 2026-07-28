/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Description : This implements a looped delay. 
 * The delay unit is one cycle of the crystal.
 * It adapts to clock rate changes.
 *
 * $Id: delay.c 24473 2020-04-20 12:24:13Z dhb $
 **************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "wd.h"
#include "SY7_device.h"
#include "delay.h"

/*** Version ****/
const char fw_ver_delay[] =
"delay"  /* Device Driver name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

volatile int32_t delay_cnt;


/*****************************************************************************
* Description:
*    Loop Delay implementation
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
*****************************************************************************/
void delay (int32_t cnt)
{
    cnt /= SystemCoreClockDivisor;
    while ((--cnt) > 0)
    {
        delay_cnt = cnt;
        if(0 == (cnt & 0xff))
        {
            wd_reset();
        }
    }
}

/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/


