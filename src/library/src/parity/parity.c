/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: calculates 8-bit parity.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 * $Id: parity.c 16400 2018-01-09 23:57:40Z rvandewa $
 ****************************************************************************/
#include "options.h"    // System definitions, device, pins, etc.
#include "parity.h"

/*** Version ****/
const char fw_ver_parity[] =
"parity" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


// Returns 1 if even parity.
int parity_even (uint8_t b)
{
    int i = (b ^ (b >> 4)) & 0x0f;
    return 1 & (0x9669 >> i);
}

// Returns 1 if odd parity.
int parity_odd (uint8_t b)
{
    int i = (b ^ (b >> 4)) & 0x0f;
    return 1 & (0x6996 >> i);
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

