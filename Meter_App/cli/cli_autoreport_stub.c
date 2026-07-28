/***************************************************************************
 * DESCRIPTION: SoC POWER METER - Command Line Interface Routines.
 * This autoreports meter data on serial.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * $Id: cli_autoreport_stub.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
#include <ctype.h>
#include "options.h"    // System options for compiler, hardware, etc.
#include "afe.h"        // For meter data to display.
#include "cli.h"        // This uses the CLI's serial system.
#include "cli_buf.h"    // To use the CLI's I/O routines and buffering.
#include "cli_autoreport.h"     // Test function prototypes.

int ar_cli_idx;


/***************************************************************************   
* Description:  
*       Enable autoreporting on the indexed CLI device.  
*       It sets up the first field.
* Parameters:
*   Input:
*       The CLI index of the CLI I/O device to get the data. 
*   Output: 
*       It enables autoreporting starting on the next accumulation interval.
* Return Status: 
*       none
***************************************************************************/
void cli_ar_enable(int cli_idx)
{
} // cli_ar_enable()


/***************************************************************************   
* Description:  
*       Disable autoreporting on the indexed CLI device.  
* Parameters:
*   Input:
*       None- it assumes that only one channel is autoreporting.
*   Output: 
*       It sets a halt flag, which the autoreporting detects at the
*       end of the next line.
* Return Status: 
*       none
***************************************************************************/
void cli_ar_disable(int cli_idx)
{
} // cli_ar_disable()


/***************************************************************************   
* Description:  
*       Do autoreporting on the indexed CLI device.  
* Parameters:
*   Input:
*       Any CLI index of a CLI I/O device.
*   Output: 
*       If the cli device is the devie that enabled reporting, it
*       sends fields suitable for a comma-delimited file.
* Return Status: 
*       none
***************************************************************************/
void cli_ar_run(int cli_idx)
{
} // cli_ar_run()


/***************************************************************************
 * Copyright (C) 2005..2013 Silergy Corp. 
 * All Rights Reserved.
 * This program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/

