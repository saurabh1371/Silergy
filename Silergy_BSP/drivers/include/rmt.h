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
* Description: SYM017/AM97 Capacitive Remote interface header file.
*
 * $Id: rmt.h 17420 2018-04-21 00:40:44Z rvandewa $:
*****************************************************************************/
#ifndef RMT_H
#define RMT_H 1

// Firmware Device Driver version
extern const char fw_ver_rm[]; 

// Reliably read a register from remote IC.
int rmt_get_reg(int adr, int reg_idx);

// Register index, in 16-bit pieces
#define RMT_OTP0HIGH 0
#define RMT_OTP0LOW 1
#define RMT_OTP1HIGH 2
#define RMT_OTP1LOW 3
#define RMT_VER_VOLTS 4
#define RMT_STEMP 5

// Reliably read a register from remote IC.
int rmt_get_reg(int adr, int cmd);

// Get error counts. Clears sticky error bits.
int rmt_error (void);

// Displays Device and Version info for all the remote ICs. (code is in CLI.C)
void rmt_put_id (void);

// get the part no. flash string. (Code is in cli.c)
const char *rmt_get_part_no(int idx);

// The version letter is bits Major rev: 7:4, 1=A, Minor rev is 3:0
int rmt_get_version(int idx);

// Return a normalized tc1 and tc2 for a remote.
// If it returns nonzero, it timed out.
int rmt_get_ppmc(float *tc1_ptr, float *tc2_ptr, int chan_idx);


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
#endif  //  RMT_H

