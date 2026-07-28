/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: This file has remote interface functions.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: rmt.c 16212 2017-12-08 00:31:28Z ptanwar $
 ****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "delay.h"
#include "rmt.h"

#ifndef REMOTES
#warning Needed symbol 'REMOTES' is not defined.
#endif
#ifndef RMT_H
#warning Needed symbol 'RMT_H' is not defined.
#endif

// Firmware Device Driver version
const char fw_ver_rmt[] =
"rmt"           /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

int rmt_parity_error_cnt;
int rmt_frame_error_cnt;
int rmt_timeout_cnt;


/***************************************************************************   
* Description:  
*  Get error cases by reading RMTSTAT register. 
*  Error bit are sticky and need to be cleared.
* Parameters:
*   Input:
*         none
*   Output: 
*         32-bit error code from the remote IC - MAX7107X(AM83)
*         
* Return Status: 
*         none
***************************************************************************/
int rmt_error (void)
{
    rmt_frame_error_cnt += RMT->FRAME_b.mframe_err_cnt;
    RMT->FRAME_b.mframe_err_cnt = 7; // Clear the manchest frame error count.
    if(RMT->SIDE_DATA_b.parity)
        ++rmt_parity_error_cnt;
    RMT->SIDE_DATA_b.parity = 1;    // Clear the parity error flag.
    return rmt_frame_error_cnt + rmt_parity_error_cnt + rmt_timeout_cnt;
}


/***************************************************************************   
* Description:  
*   Reliably read a register from remote IC - MAX7107X(AM83).
* Parameters:
*   Input:
*         Cmd : command format - RD_CODE[6:4] , TMUX[3:1], test[0] 
*         adr : 1, 2 or 3 for Remote Channel A, B or C.       
*
*   Output: 
*         16-bit data from the remote IC MAX7107X(AM83) 
* Return Status: 
*         none
***************************************************************************/
int rmt_get_reg(int adr, int cmd)
{
    int side_flags;
    int timeout = SystemCoreClock / 512; 

    rmt_error ();

    // 2 bits index 4 remotes.
    adr &= 3;
    // Bottom 3 bits select the data from the remote.
    cmd &= 7;
    cmd |= adr << 3;

    RMT->CNTL_b.side_sel = adr;

    // Wait for the the data to come in.
    do
    {
        side_flags = RMT->SIDE_DATA_b.side_flag;
        side_flags >>= adr;
        side_flags &= 1;
        --timeout;
    } while (0 != side_flags && 0 < timeout);

    if(0 == timeout)
        ++rmt_timeout_cnt;

    rmt_error ();

    return (int)(RMT->SIDE_DATA_b.side_dat);
}


/***************************************************************************   
* Description:  
*   Displays device ID of the remote IC - MAX7107X(AM83).
* Parameters:
*   Input:
*         idx : 1, 2 or 3 for Remote Channel A, B or C.       
*
*   Output: 
*         Device ID string on the UART 
* Return Status: 
*         none
***************************************************************************/
const char *rmt_get_part_no (int idx)
{
    idx++;
    return "SY7M017";
}


/***************************************************************************   
* Description:  
*   Displays Version info the remote IC - MAX7107X(AM83).
* Parameters:
*   Input:
*         idx : 0..3 for Remote Channel A,... D.       
*
*   Output: 
*   The hex digit at bits 7..4 is the revision letter (1=A)
*   The hex digit at bits 3..0 is the revision number (1=1)
* Return Status: 
*         none
***************************************************************************/
int rmt_get_version (int idx)
{
    int r;
    r = rmt_get_reg(idx,RMT_VER_VOLTS);  // Read Version Info [7:0]
    r >>= 8;
    return r;
}


/***************************************************************************   
* Description:  
*   Reliably get tc1, tc2 from remote IC- MAX71071.
* Parameters:
*   Input:
*         adr : 1, 2 or 3 for Remote Channel A, B or C.       
*
*   Output: 
*         tc1 = tc1 * tc1-lsb = tc1 * (1/DegC)
*         tc2 = tc2 * tc2-lsb = tc1 * (1/DegC^2)
* Return Status: 
*         none
***************************************************************************/
#define VREF_NOM (1.20e6f) // 1.20V (from data sheet), in microvolts
#define TRIMT_DEFAULT 60    // The production median, better than nothing.
// but the standard deviation is 40, so it's not very good, either.
int rmt_get_ppmc(float *tc1_ptr, float *tc2_ptr, int rmt_idx)
{
    int trimt, failed, timer;
    float tc1, tc2;
    float trimt_f;

    // Wait till the remotes are powered-up.
    // If it hangs, the meter is broken.
    timer = 150; // >300ms
    failed = false;
    trimt = 0;
    while(0 == trimt)
    {
        delay(DELAY_MS(2));
        // Read the trim value from the remote.
        trimt = 0xff & rmt_get_reg(rmt_idx, RMT_OTP0HIGH);
        if(0 != trimt)
            break;
        if(0 == --timer)
        {
            failed = true;
            trimt = TRIMT_DEFAULT; // Better than nothing.
            break; 
        }
    }

    // Convert it first to a 2s complement integer.
    if (trimt & 0x80) trimt -= 256;  // trimt is 2s complement
    if(trimt <= 0) // Is it invalid?
    {
        trimt = TRIMT_DEFAULT; // Better than nothing.
        failed = true;
    }
    trimt_f = (float)trimt; // convert it to a float.

    // Set the coefficients for the integer compensation 
    // polynomial. 
    // tc1 in uV/C = 316 - (5.94 * trim)
    // tc2 in uV/C^2 = -0.582 + ( 0.00132  * trim)
    tc1 = (316 - (5.94 * trimt_f)); // tc1 in uV/DegC, from data sheet
    tc2 = -0.582 + ( 0.00132  * trimt_f); // tc2 in uV/(DegC^2), from data sheet

    tc1 /= VREF_NOM;    // normalize so linear gain = tc1*(DegC-22C)
    tc2 /= VREF_NOM;    // normalize so quadratic gain = tc2*((DegC-22C)^2)
    tc1 = 0;
    tc2 = 0;

    *tc1_ptr = tc1;     // return tc1 and tc2
    *tc2_ptr = tc2;

    return failed;      // nonzero = failure
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

