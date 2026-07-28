/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Mailbox management for Slave SPI
 *
 *
 * Copyright 2013, Silergy Corp. All rights reserved.
 * $Id: sspi0.c 16429 2018-01-13 08:00:11Z rvandewa $
 ****************************************************************************/
#include <ctype.h>        // get iscntrl() 
#include "options.h"      // Specific device HW register definitions.
#include "cli.h"          // CLI driver definitions.
#include "irq.h"          // Interrupt management.
#include "sspi.h"         // Slave SPI low level driver
#include "sspi0.h"        // Slave SPI CLI driver declaration
#include "stm.h"          // software timer


/*** Version ****/
const char fw_ver_sspi0[] =
"sspi0_stub"   /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

/*** Version ****/
const char fw_ver_sspi[] =
"sspi not in build (sspi0 stubbed)"    /* Device Driver name */
"\r\n"    /* CR + LF */
;

static volatile int rx_busy = 0;

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
const cli_driver_t sspi0_cli_driver = {
    init, get_str, put_str, &rx_busy
};


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

