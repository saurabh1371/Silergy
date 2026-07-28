/***************************************************************************
 * DESCRIPTION: SoC POWER METER - Command Line Interface Routines.
 * This  displays a three-phase ISO-style front panel meter display on serial.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * $Id: cli_display_1_phase_iso.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
#include <ctype.h>
#include "options.h"    // System options for compiler, hardware, etc.
#include "afe.h"        // For meter data to display.
#include "bat.h"        // For battery readings to display.
#include "cli.h"        // This uses the CLI's serial system.
#include "cli_buf.h"    // To use the CLI's I/O routines and buffering.
#include "lcd_strings.h"// Display labels.
#include "cli_display.h"// Test function prototypes.

/***************************************************************************   
* Description:  
*       Put the phase index out in string form, so it
*        uses no RAM for the number conversion. 
* Parameters:
*   Input:
*         int phase_idx : Phase index  
*   Output: 
*         None.
* Return Status: 
*         None
***************************************************************************/
static const char one_str[]     = " L1";
static const char two_str[]     = " L2";
static const char slash_str[]     = "/";

void put_phase(int phase_idx)
{
    switch(phase_idx) {
        case 0:
        case 1: cli_put_str(one_str); break;
        case 2: cli_put_str(two_str); break;
        default: break;
    }
}


/***************************************************************************   
* Description:  
*        update the front panel simulated on the serial line
* Parameters:
*   Input:
*         None.
*   Output: 
*         None.
* Return Status: 
*         None
***************************************************************************/
static const char id_str[] = " ID ";
//static const char period_str[] = ".";
static const char colon_str[] = ":";
static const char sec_tamper_str[] = " Seconds of Tamper";
static const char sec_sag_str[] = " Seconds of Sag";
static const char w_demand_str[] = " W. Demand ";
static const char time_demand_str[] = " Demand Time ";
static const char date_demand_str[] = " Demand Date ";

void cli_display(void)
{
    int phase_idx;
    float ftmp;

    phase_idx = global.cal.lcd_idx/100;

    switch(global.cal.lcd_idx)
    {
            // ID. 00000000
        case 0: cli_put_32b10(global.cal.meter_id);
            cli_put_str(id_str);
            break;
        case 1:     // Temperature -23.0 C
        case 101:   // Phase 1's Vref
        case 201:   // Phase 2's Vref
            // Exact data used by temperature compensation.
            ftmp = ((float)global.misc.temp_c)/10.0;
            cli_put_fb10(ftmp,1);
            cli_put_str(c_str);
            break;

        case 2: // Hz 60.0
            ftmp = afe_frequency();
            cli_put_fb10(ftmp,1);
            cli_put_str(hz_str);
            break;

        default: global.cal.lcd_idx = 3; break;
        case 3:
            if (global.cal.cfg & DISPLAY_KWH)
            {
                // KWh, etc. 000000
                cli_put_32(kwh(&global.reg.wh_cnt),6,10);
                put_phase(0);
                cli_put_str(kwh_str);
            } else {
                // Wh, etc. 000.000
                ftmp = (float)wh(&global.reg.wh_cnt);
                cli_put_fb10(ftmp,0);
                put_phase(0);
                cli_put_str(wh_str);
            }
            break;
        case 4:
            if (global.cal.cfg & DISPLAY_KWH)
            {
                // Exported KWh, etc. 000000
                cli_put_32(kwh(&global.reg.wh_cnt_ex),6,10);
                put_phase(0);
                cli_put_str(kwh_exp_str);
            } else {
                // Exported Wh, etc. 0.
                ftmp = (float)wh(&global.reg.wh_cnt_ex);
                cli_put_fb10(ftmp,0);
                put_phase(0);
                cli_put_str(wh_exp_str);
            }
            break;
        case 5:
            if (global.cal.cfg & DISPLAY_KWH)
            {
                // Inductive KVARh, etc. 000000
                cli_put_32(kwh(&global.reg.varh_cnt),6,10);
                put_phase(0);
                cli_put_str(kvarh_str);
            } else {
                // Inductive VARh, etc. 000.000
                ftmp = (float)wh(&global.reg.varh_cnt);
                cli_put_fb10(ftmp,0);
                put_phase(0);
                cli_put_str(varh_str);
            }
            break;
        case 6:
            if (global.cal.cfg & DISPLAY_KWH)
            {
                // Capacitive KVARh, etc. 000000
                cli_put_32(kwh(&global.reg.varh_cnt_ex),6,10);
                put_phase(0);
                cli_put_str(kvarh_exp_str);
            } else {
                // Capacitive VARh, etc. 000.000
                ftmp = (float)wh(&global.reg.varh_cnt_ex);
                cli_put_fb10(ftmp,0);
                put_phase(0);
                cli_put_str(varh_exp_str);
            }
            break;
        case 7:
            if (global.cal.cfg & DISPLAY_KWH)
            {
                // KVAh, etc. 000000
                cli_put_32(kwh(&global.misc.vah_cnt),6,10);
                put_phase(0);
                cli_put_str(kvah_str);
            } else {
                // VAh, etc. 000.
                ftmp = (float)wh(&global.misc.vah_cnt);
                cli_put_fb10(ftmp,0);
                put_phase(0);
                cli_put_str(vah_str);
            }
            break;
        case 8:
            // Hours of operation.
            cli_put_fb10(((float)global.reg.operating_sec)/3600.0,2);
            cli_put_str(hr_str);
            break;
        case 9:
        case 10:    // Date and time; fall-through is intentional.
            cli_put_tm (&global.reg.tm);
            break;

        case 11:    // Power factor, total
        case 111:   // Phase 1
        case 211:   // Phase 2
            ftmp = afe_power_factor(phase_idx);
            cli_put_fb10(ftmp,2);
            put_phase(phase_idx);
            cli_put_str(cos_phi_str);
            break;

        #if EQUATION==EQUATION2
        case 12:    // Angle 
        case 112:   // Phase a to b 
            ftmp = afe_a_to_b();
            cli_put_fb10(ftmp,2);
            cli_put_str(deg_str);
            break;
        #endif

        case 13: // edge count. 120,100 or 0
            cli_put_32b10(afe_zero_crossings());
            cli_put_str(zc_str);
            break;
        case 113:// edge count. 120,100 or 0
        case 213:// Running zero crossing count.
            cli_put_32b10(global.misc.zc_cnt);
            cli_put_str(zc_str);
            break;

            // Seconds of bad power. 000000
        case 14:
            cli_put_32b10(global.reg.sag_sec);
            cli_put_str(sec_sag_str);
            break;
             // Seconds of tamper. 000000
        case 114:
            {
                int32_t s = global.reg.tamper_sec;
                // "-" = tamper is happening now!
                if(log_active(EV_CASE_TAMPER) 
                        || log_active(EV_TERM_TAMPER))
                    s = -s;
                cli_put_32b10(s);
                cli_put_str(sec_tamper_str);
            }
            break;

        case  15: // A 208.00, total
        case 115: // Phase 1
        case 215: // Phase 2
            ftmp = afe_arms(phase_idx);
            cli_put_fb10(ftmp,3);
            put_phase(phase_idx);
            cli_put_str(a_str);
            break;

        case  16: // All phases
        case 116: // Phase 1
        #if EQUATION==EQUATION2
        case 216: // Phase 2
        #endif //EQUATION==EQUATION2
            ftmp = afe_vrms(phase_idx);
            cli_put_fb10(ftmp,3);
            put_phase(phase_idx);
            cli_put_str(v_str);
            break;

            // V. Bat.  Last measure. 000.00
        case  17: // VBat = 0
        case 117: // VBat_RTC = 1;
            if (1 == phase_idx) {
                cli_put_fb10(bat_volts_rtc(),2);
                cli_put_str(vbat_rtc_str);
            } else {
                cli_put_fb10(bat_volts(),2);
                cli_put_str(vbat_str);
            }
            break;

        case 18:  // w 000000., total
        case 118: // Phase 1
        case 218: // Phase 2
            ftmp = afe_w(phase_idx);
            cli_put_fb10(ftmp,-1);
            put_phase(phase_idx);
            cli_put_str(w_str);
            break;

        case 19:  // var 000000., total
        case 119: // Phase 1
        case 219: // Phase 2
            ftmp = afe_var(phase_idx);
            cli_put_fb10(ftmp,-1);
            put_phase(phase_idx);
            cli_put_str(var_str);
            break;

        case 20:  // va 000000., total
        case 120: // Phase 1
        case 220: // Phase 2
            ftmp = afe_va(phase_idx);
            cli_put_fb10(ftmp,-1);
            put_phase(phase_idx);
            cli_put_str(va_str);
            break;

            // Demand.
        case 21:
            // Watts of demand
            cli_put_32b10(global.reg.dmd_max);
            cli_put_str(w_demand_str);
            break;
        case 121:
            // h:m, time of demand.
            cli_put_32((int)global.reg.dmd_hour,2,10);
            cli_put_str(colon_str);
            cli_put_32((int)global.reg.dmd_min,2,10);
            cli_put_str(time_demand_str);
            break;
        case 221:
            // m/d, date of demand.
            cli_put_32(1+global.reg.dmd_mon,2,10);
            cli_put_str(slash_str);
            cli_put_32(global.reg.dmd_mday,2,10);
            cli_put_str(date_demand_str);
            break;
        case 22:
            cli_put_str(test_str);
            break;
    } /* end switch on LCD index */
} /* cli_display () */


/***************************************************************************
 * Copyright (C) 2005..2013 Silergy Corp. 
 * All Rights Reserved.
 * This program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/


