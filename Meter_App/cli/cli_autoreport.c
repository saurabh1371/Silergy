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
 * $Id: cli_autoreport.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
#include <ctype.h>
#include "options.h"    // System options for compiler, hardware, etc.
#include "afe.h"        // For meter data to display.
#include "cli.h"        // This uses the CLI's serial system.
#include "cli_buf.h"    // To use the CLI's I/O routines and buffering.
#include "cli_autoreport.h"     // Test function prototypes.

#define SHOW_B 1

// Sequence of autoreporting. Rearrange it to change it.
// Don't forget to change the header string in the enable fn.
enum  ar_field_enum_e {
    invalid_field_idx = 0,
    acc_cnt_field_idx,
    va_field_idx,
    ia_field_idx,
    wa_field_idx,
    wha_field_idx,
    vara_field_idx,
    varha_field_idx,
#if SHOW_B
    vb_field_idx,
    ib_field_idx,
    wb_field_idx,
    whb_field_idx,
    varb_field_idx,
    varhb_field_idx,
#endif
    last_field_idx
};
typedef enum ar_field_enum_e ar_field_idx_t;
#define first_field_idx acc_cnt_field_idx

// -1 is an impossible index for a CLI I/O device, so that halts output.
       int ar_cli_idx = -1; // The cli device for output (i.e. starts halted).
static bool ar_halt = true; // force it to halt if it does start.
static int ar_acc_cnt; // The last dumped accumulation interval.
static ar_field_idx_t ar_field_idx; // Index of the next data field.


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
    ar_halt = false; // don't halt autoreporting
    ar_field_idx = first_field_idx; // Start with the first field.
    ar_acc_cnt = 0;
    ar_cli_idx = cli_idx; // Select the output to get the data.

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
    if(ar_cli_idx == cli_idx)
    {
        ar_halt = true; // Stop output after the end of the line.
        if(global.misc.acc_cnt == ar_acc_cnt) // if not running
            ar_cli_idx = -1; // halt it immediately.
    }
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
    if(cli_idx != ar_cli_idx)
        return;

    // New data?
    if(global.misc.acc_cnt != ar_acc_cnt)
    {
        if(0 == ar_acc_cnt) // just started?
        {
            cli_put_str( // Put out a header for the spreadsheet's first line.
                "\r\n"
                "acc. cnt, "
                "VA, IA, WA, WhA, VARA, VARhA, "
                #if SHOW_B
                "VB, IB, WB, WhB, VARB, VARhB "
                #endif
                "\r\n"
                );

            ar_acc_cnt = global.misc.acc_cnt;   // Resume when there is new data.
            return;
        }
        switch(ar_field_idx)
        {
            case acc_cnt_field_idx: cli_put_32b10(global.misc.acc_cnt); break; // Accumulation Count
            case va_field_idx: cli_put_fb10(afe_vrms(PHASE_A),2); break; // VA
            case ia_field_idx: cli_put_fb10(afe_arms(PHASE_A),3); break; // IA
            case wa_field_idx: cli_put_fb10(afe_w(PHASE_A),2); break; // WA
            case wha_field_idx: cli_put_fb10(afe_wh(PHASE_A),5); break; // WhA
            case vara_field_idx: cli_put_fb10(afe_var(PHASE_A),2); break; // VARA
            case varha_field_idx: cli_put_fb10(afe_varh(PHASE_A),5); break; // VARhA
            #if SHOW_B
            case vb_field_idx: cli_put_fb10(afe_vrms(PHASE_B),2); break; // VB
            case ib_field_idx: cli_put_fb10(afe_arms(PHASE_B),3); break; // IB
            case wb_field_idx: cli_put_fb10(afe_w(PHASE_B),2); break; // WB
            case whb_field_idx: cli_put_fb10(afe_wh(PHASE_B),5); break; // WhB
            case varb_field_idx: cli_put_fb10(afe_var(PHASE_B),2); break; // VARB
            case varhb_field_idx: cli_put_fb10(afe_varh(PHASE_B),5); break; // VARhB
            #endif
            default: // wait for next time... and recover from a bad field index.
                ar_acc_cnt = global.misc.acc_cnt;   // Resume when there is new data.
                ar_field_idx = invalid_field_idx;     // Start with the first data field.
                break;
        } // end of switch

        if(first_field_idx <= ar_field_idx) { // End of current field
            cli_put_chr(',');
            ar_field_idx++;  // Next output field.

        } else { // end of line.

            cli_put_crlf();

            // Halt after the line is done.
            if(ar_halt) // Halt commanded?
                ar_cli_idx = -1;    // No cli device is valid for output.

            // Restart the line
            ar_field_idx = first_field_idx;
        }
    } // end if it's new data
} // cli_ar_run()


/***************************************************************************
 * Copyright (C) 2005..2013 Silergy Corp. 
 * All Rights Reserved.
 * This program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/

