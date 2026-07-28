/****************************************************************************
* Copyright (C) 2013 Silergy Corp. All rights Reserved. 
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to 
* any party or parties not specified in the license agreement and/or 
* nondisclosure agreement is expressly prohibited. 
*
* Description: MAX7107X Remote interface header file.
*
 * $Id: rmt_stub.c 16212 2017-12-08 00:31:28Z ptanwar $:
*****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "rmt.h"

// Firmware Device Driver version
const char fw_ver_rmt[] =
"rmt_stub"      /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

// Reliably read a register from remote IC.
int rmt_get_reg(int adr, int cmd) { return adr+cmd; }

// Get error cases by reading RMTSTAT register. 
// Error bits are sticky and need to be cleared.
int rmt_error (void) { return 0; }

// Displays Device and Version info for all the remote ICs. (code is in CLI.C)
void rmt_put_id (void) { }

// get the part no. flash string. (Code is in cli.c)
const char *rmt_get_part_no(int idx) { return "rmt_stub"; }

// The version letter is bits 7:4, 1=A, number is 3:0
int rmt_get_version(int idx);

// Return a normalized tc1 and tc2 for a remote.
// If it returns nonzero, it timed out.
int rmt_get_ppmc(float *tc1_ptr, float *tc2_ptr, int chan_idx)
{
    *tc1_ptr = 0.0;
    *tc2_ptr = 0.0;
    return chan_idx;
}


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/

