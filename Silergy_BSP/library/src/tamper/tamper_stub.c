/*****************************************************************************
 * DESCRIPTION: Tamper detection logic.
 * This code manages a couple of burglar alarms in a meter.
 * The idea is to use whatever tamper features are permitted by the PCB.
 * If a tamper GPIO is defined, it is polled.
 * If a tamper interrupt is defined, it is enabled.
 * If a tamper wake is defined, it is selected before sleep.
 * The tamper enables and detection are "smart", so that if a tamper 
 * already exists, the tamper interrupt or wake is not invoked.
 * This is so that the meter can seem to function normally, while still
 * recording a tamper.
 * The tamper recording sets a bit EV_TAMPER_MASK in state , 
 * a global variable.
 * The demo code, in main(), uses the EV_TAMPER_MASK & state
 * to count seconds of tamper.
 * The LCD display can poll this bit and produce a display if needed.
 * Be careful to display a tamper indication in a way that a meter-reader
 * can recognize, but not a utility customer.  If a tamper is clearly shown
 * on a meter, further vandalism often occurs.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2016, Silergy Corp. All rights reserved.
 *
 * $Id: tamper_stub.c 16466 2018-01-22 20:58:07Z rvandewa $
 ****************************************************************************/
#include "options.h"    // Define the PCB, IC, CE code and other options.
#include "tamper.h"     // Check the definitions.


/*** Version ****/
const char fw_ver_tamper[] =
"tamper_stub"     /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


/***************************************************************************
* Description:
*        Tamper detection initialization.
* Parameters:
*   Input:
*         None.
*   Output:
*         It sets the tamper detection.
* Return Status:
*         None
***************************************************************************/
void tamper_init(void)
{
}

/***************************************************************************
* Description:
*        Tamper detection polling.
*        Note that it polls the GPIOs in all cases, whether or not
*        the interrupts are on, off, whatever.
* Parameters:
*   Input:
*         None.
*   Output:
*         It sets the tamper detection call calling the error logic.
* Return Status:
*         Returns nonzero if it detects a tamper.
***************************************************************************/
int tamper_run(void)
{
    return 0;
}

/***************************************************************************
* Description:
*        Tamper detection interrupt(s).
* Parameters:
*   Input:
*         None.
*   Output:
*         It sets the tamper detection call calling the error logic.
* Return Status:
*         Returns nonzero if it detects a tamper.
***************************************************************************/
void tamper_isr(void)
{
}


/***************************************************************************
* Description:
*        Tamper detection by waking from sleep.
*        This adds any tamper detection bits to a bit-array of wake-requests.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
***************************************************************************/
int tamper_wakes(int in)
{
    int out = in;

    return out;
}


/***************************************************************************
 * Copyright (C) 2011..2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

