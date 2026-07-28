/***************************************************************************
 * This code and information is provided "as is" without warranty of any   *
 * kind, either expressed or implied, including but not limited to the     *
 * implied warranties of merchantability and/or fitness for a particular   *
 * purpose.                                                                *
 *
 * DESCRIPTION: This implements a timeout routine using a loop counter 
 *              and refreshes Watch-Dog timer.
 *
 * Author : RP, RGV
 *
 * $Id: timeout.c 16212 2017-12-08 00:31:28Z ptanwar $
 ****************************************************************************/

#include "options.h"

volatile int32_t timeout_counter;

/*****************************************************************************   
* Description:  
*    Initialize timeout counter and refreshes WatchDog timer.
* Parameters:
*   Input:
*         int32_t init_count :  Initial timeout count
*   Output: 
*         None.
* Return Status: 
*         None.
*******************************************************************************/
void timeout_start (int32_t init_count)
{
    reset_wd();
    timeout_counter = init_count;
}


/*****************************************************************************   
* Description:  
*    Check if timeout  and also refreshes WatchDog timer.
* Parameters:
*   Input:
*         None
*   Output: 
*         None.
* Return Status: 
*         0 - When timeout. 
*         1 - otherwise  (no timeout).
*******************************************************************************/
int timeout_ok (void)
{
    --timeout_counter;
    if (timeout_counter < 0)
    {
        timeout_counter = 0;
        return 0;     //  TIMEOUT , timeout_counter expired. 
    }
    
// reset watchdog less often
    if ((timeout_counter & 0xFF) == 0)
        reset_wd();

    return 1;      // NO timeout 
}


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/


