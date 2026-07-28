/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Sends and receives on UART1.
 *
 * The driver has a special problem for transmit: It has to be able to send
 * very large strings of ASCII help text.  A conventional circular buffer
 * would need to be several thousand bytes. So, it queues zero-terminated
 * string pointers in a circular buffer of strings.
 * The receive queue is a conventional small circular queue of characters.
 * The receive queue is drained as the main loop edits the command line.
 * Both queues are indexed, and wrap the indices around by masking with an
 * "and" operation.
 * Both queues have a count of entries, and use this for flow control.
 *
 * Copyright 2013, Silergy Corp. All rights reserved.
 * $Id: serc0.c 16212 2017-12-08 00:31:28Z ptanwar $
 ****************************************************************************/
#include <ctype.h>        // get iscntrl() 
#include "options.h"      // Specific device HW register definitions.
#include "cli.h"          // CLI driver definitions.
#include "irq.h"          // Interrupt management.
#include "ser_common.h"   // Common serial definitions.
#include "stm.h"          // software timer


/*** Version ****/
const char fw_ver_serc1[] =
"serc1_stub"   /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

/*** Version ****/
const char fw_ver_ser1[] =
"ser1 not in build (stubbed serc1)"   /* Library file name */
"\r\n"   /* CR + LF */
;

static volatile int ser_tx_busy = 0;

static void init (void) { } // Nothing to set up.
static char *get_str (char **buf_ptr, int *len_ptr) // Never any input.
{
    *len_ptr = -1;
    return NULL;
}
static int put_str (const char *str) // Output to the bit-bucket.
{
    str++;
    return 0;
}
/****************************************************************************
* Description:
*       Fn pointer for structure to get polymorphic interface to cli driver.
=*****************************************************************************/
const cli_driver_t serc1_cli_driver = {
	init, get_str, put_str, &ser_tx_busy
};


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

