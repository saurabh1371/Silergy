/***************************************************************************
 * DESCRIPTION: POWER METER SoC - Command Line Interface Routines.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * $Id: lcd_BQ14053_1phase.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
#include "options.h"    // System options for compiler, hardware, etc.
#include <math.h>       // for fabs()
#include "afe.h"        // For meter data to display.
#include "bat.h"        // For battery readings to display.
#include "pwrmode.h"    // To read the battery mode.
#include "LCD_BQ14053.h"
#include "lcd_strings.h"// Display labels.
#include "lcd.h"        // Test function prototypes.


#define TIMER_LCD_SECS 7    //  Seconds per automatic LCD display
#define BUTTON_LCD_SECS 14   //  Seconds per button-selected display


/***************************************************************************
* Description:
*       Write the software's ID to the LCD.
*       This has to be formatted to a particular display, so it's in
*       this module.
*   Input:
*       Software ID string.
* Parameters:
*       None
*   Output:
*       Software ID string appears on the display.
* Return Status:
*       None
***************************************************************************/
void lcd_put_sw_id (void)
{
    lcd_put_str(LINE1,hi_str);
}


/***************************************************************************
* Description:
*       Write units to display.
*   Input:
* Parameters:
*         None
*   Output:
* Return Status:
*         None
***************************************************************************/
static void lcd_put_v (void)
{
    lcd_put_icon(v);            // V
}
static void lcd_put_w (void)
{
    lcd_put_v();                // V of W
    lcd_put_icon(w);            // W part of W
}
static void lcd_put_wh (void)
{
    lcd_put_w();                // W
    lcd_put_icon(h);            // h
}
static void lcd_put_kwh (void)
{
    lcd_put_icon(kk);           // K
    lcd_put_wh();
}
static void lcd_put_a (void)
{
    lcd_put_icon(A);            // long / of A
}
static void lcd_put_va (void)
{
    lcd_put_icon(v);            // V of W
    lcd_put_a();                // A
}
static void lcd_put_var (void)
{
    lcd_put_va();               // VA
    lcd_put_icon(R);            // r
}
static void lcd_put_varh (void)
{
    lcd_put_var();              // VAr
    lcd_put_icon(h);            // h
}
static void lcd_put_kvarh (void)
{
    lcd_put_icon(kk);           // K
    lcd_put_varh ();
}
static void lcd_put_vah (void)
{
    lcd_put_va();               // VA
    lcd_put_icon(h);            // h
}
static void lcd_put_kvah (void)
{
    lcd_put_icon(kk);           // K
    lcd_put_vah ();
}


/***************************************************************************
* Description:
*       Update the LCD front panel.
*   Input:
*       The current value of lcd_idx, the LCD display index, selects
*       which screen to show on the LCD.
*       It displays data from the global meter data and billing registers.
* Parameters:
*         None
*   Output:
*         It displays data on the LCD.
* Return Status:
*         None
***************************************************************************/
void lcd_update(void)
{
    bool timer_scroll = false;
	bool button_scroll = false;
    bool blink = true;      // blinks unit to show activity.

    // Blink display items in mission mode.
    if (0 == (global.misc.acc_cnt & 1) && !pwrmode_is_brownout() ) {
        blink = false;
    }

    /* Scroll display, if configured to do so. */
    if (0 == global.cal.lcd_bit)   /* bit mask selects displays. */
    {
        global.misc.lcd_timer = 0;  /* stop timer */
    } else if (0 == global.misc.lcd_timer) {    /* timer expires */
        timer_scroll = true;
        global.misc.lcd_timer = TIMER_LCD_SECS; /* time display is on */
    }

    // Button pressed?  It's tested here so
    // it's easy to control.  Lift the button
    // after seeing the desired display.
    if (0 != pb_read())
    {
        // Button-commanded LCD screen time
        button_scroll = true;
        global.misc.lcd_timer = BUTTON_LCD_SECS;    
    }
    if (timer_scroll || button_scroll)
    {
        // A timer scroll searches for an enabled entry.
        // A button scroll shows all entries.
        do {
            switch(global.cal.lcd_idx)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                // 11 has to skip 12
                // 12 is voltage phase angle, meaningless in single-phase
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                    ++global.cal.lcd_idx;
                    break;
                case 11:
                    global.cal.lcd_idx = 13;   // skip 12
                    break;
                case 22:
                    global.cal.lcd_idx = 0;    // go back to start
                    break;
                default:
                    global.cal.lcd_idx = 3;
                    break;
            }
            // Search for the next permitted display.
        } while (
                // Button scroll does every display.
                (!button_scroll)
                // If timer is disabled, do every display.
                && (0 != global.cal.lcd_bit)
                // Find timed displays.
                && (0 == (global.cal.lcd_bit & (1L << global.cal.lcd_idx))));
    } // If timer or button scroll

    // Write the number and units. 
    {
        int idx;
        bool show_phase = false;// true = show measurement's phase

        global.misc.lcd_idx_old = global.cal.lcd_idx;

        // Show the display number.
        idx = global.cal.lcd_idx % 100;

        // Make the display number
        // and tamper indication blink.
        lcd_clear();

        if (blink)
            lcd_put_num(LINE2,2,idx);    // item number.

        // Select the data.
        switch(global.cal.lcd_idx)
        {
            // ID. 00000000
            case 0:
                lcd_put_num(LINE1,LINE1_LEN,global.cal.meter_id);
                break;

            case 1: // C.   -476.0
                lcd_put_num(LINE1+2, 4, global.misc.temp_c);
                lcd_put_icon(P7);
                lcd_put_icon(c);
                break;

            case 2: // Hz 60.0
                lcd_put_num  (LINE1,3,lroundf(10*afe_frequency()));
                lcd_put_icon(D2);
                lcd_put_str(LINE1+3,hz_str);
                break;

                // KWh, etc. 0.00000
            case 3: 
                if (global.cal.cfg & DISPLAY_KWH)
                {
                    // This code is frequently requested;
                    // To see the display, set )1=1
                    lcd_put_num(LINE1+1,LINE1_LEN-1,kwh(&global.reg.wh_cnt));
                    lcd_put_kwh ();
                } else {
                    lcd_put_num(LINE1+1,LINE1_LEN-1,wh(&global.reg.wh_cnt));
                    lcd_put_wh ();
                }
                break;

            case 4:
                if (global.cal.cfg & DISPLAY_KWH)
                {
                    // This code is frequently requested;
                    // To see the display, set )1=1
                    lcd_put_num(LINE1+1,LINE1_LEN-1,kwh(&global.reg.wh_cnt_ex));
                    lcd_put_kwh ();
                } else {
                    lcd_put_num(LINE1+1,LINE1_LEN-1,wh(&global.reg.wh_cnt_ex));
                    lcd_put_wh ();
                }
                break;

            case 5:
                if (global.cal.cfg & DISPLAY_KWH)
                {
                    // This code is frequently requested;
                    // To see the display, set )1=1
                    lcd_put_num(LINE1+1,LINE1_LEN-1,kwh(&global.reg.varh_cnt));
                    lcd_put_kvarh ();
                } else {
                    lcd_put_num(LINE1+1,LINE1_LEN-1,wh(&global.reg.varh_cnt));
                    lcd_put_varh ();
                }
                break;

            case 6:
                if (global.cal.cfg & DISPLAY_KWH)
                {
                    // This code is frequently requested;
                    // To see the display, set )1=1
                    lcd_put_num(LINE1+1,LINE1_LEN-1,kwh(&global.reg.varh_cnt_ex));
                    lcd_put_kvarh ();
                } else {
                    lcd_put_num(LINE1+1,LINE1_LEN-1,wh(&global.reg.varh_cnt_ex));
                    lcd_put_varh ();
                }
                break;

            case 7: 
                if (global.cal.cfg & DISPLAY_KWH)
                {
                    // This code is frequently requested;
                    // To see the display, set )1=1
                    lcd_put_num(LINE1+1,LINE1_LEN-1,kwh(&global.misc.vah_cnt));
                    lcd_put_kvah ();
                } else {
                    lcd_put_num(LINE1+1,LINE1_LEN-1,wh(&global.misc.vah_cnt));
                    lcd_put_vah ();
                }
                break;

            case 8: // Display hours of operation.
                // RTC 00000.0
                lcd_put_num(LINE1,6,global.reg.operating_sec/360);
                lcd_put_icon(P7);
                lcd_put_icon(h);        // h(ours)
                break;
            case 9: // Display time.
                // 00:00:00
                lcd_put_num(LINE1  ,2,(int32_t)global.reg.tm.tm_hour);
                lcd_put_num(LINE1+2,2,(int32_t)global.reg.tm.tm_min);
                lcd_put_num(LINE1+4,2,(int32_t)global.reg.tm.tm_sec);
                if (blink)
								{
                    lcd_put_icon(COL1);
									  lcd_put_icon(P6);
								}
                break;
            case 10: // Display date.
                // 00-00-00
                lcd_put_num(LINE1  ,2,(int32_t)global.reg.tm.tm_year);
                lcd_put_num(LINE1+2,2,1+(int32_t)global.reg.tm.tm_mon);
                lcd_put_num(LINE1+4,2,(int32_t)global.reg.tm.tm_mday);
                if (blink)
                {
                    lcd_put_icon(P6);
                    lcd_put_icon(D2);
                }
                break;

                // Power factor. -1.00
            case 11:    // For all phases.
                lcd_put_num(LINE1,5,
                        lroundf(100.0*afe_power_factor(0))
                        );
                lcd_put_icon(P5);
                lcd_put_icon(phi);
                break;
            case 111:   // For phase 1.
                show_phase = true;          // show measurement phase
                lcd_put_num(LINE1,5,
                        lroundf(100.0*afe_power_factor(1))
                        );
                lcd_put_icon(P5);
                lcd_put_icon(phi);
                break;
            case 211:   // For phase 2.
                show_phase = true;          // show measurement phase
                lcd_put_num(LINE1,5,
                        lroundf(100.0*afe_power_factor(2))
                        );
                lcd_put_icon(P5);
                lcd_put_icon(phi);
                break;

            case 13: // edge count. 120,100 or 0
                // the AFE module applies a creep test.
                lcd_put_num(LINE1,3,afe_zero_crossings());
                break;
            case 113:// edge count. total
                global.misc.zc_cnt%=100000000L;
                lcd_put_num(LINE1,LINE1_LEN,global.misc.zc_cnt);
                break;

            case 14: // Hours of bad power. 00000.0
                lcd_put_num(LINE1,LINE1_LEN,global.reg.sag_sec/360);
                lcd_put_icon(P7);
                lcd_put_icon(h);            // h(ours)
                break;
            case 114: // Hours of tamper. 00000.0
                lcd_put_num(LINE1,LINE1_LEN,global.reg.tamper_sec/360);
                lcd_put_icon(P7);
                lcd_put_icon(h);            // h(ours)
                break;

            case 15: // A 208.00 all phases.
                show_phase = true;          // show measurement phase
                // Falls through intentionally.
                lcd_put_num(LINE1,LINE1_LEN, lroundf(100.0 * afe_arms(0)) );
                lcd_put_icon(P6);
                lcd_put_a();                // A
                break;
            case 115:   // Phase 1
                show_phase = true;          // show measurement phase
                // Falls through intentionally.
                lcd_put_num(LINE1,LINE1_LEN, lroundf(100.0 * afe_arms(1)) );
                lcd_put_icon(P6);
                lcd_put_a();                // A
                break;
            case 215:   // Phase 2
                show_phase = true;          // show measurement phase
                lcd_put_num(LINE1,LINE1_LEN, lroundf(100.0 * afe_arms(2)) );
                lcd_put_icon(P6);
                lcd_put_a();                // A
                break;

            case 16: // V 240.00
            case 116:   // Phase 1
            case 216:   // Phase 2
                // Falls through intentionally.
                lcd_put_num(LINE1,LINE1_LEN, lroundf(100.0 * afe_vrms(1)) );
                lcd_put_icon(P6);
                lcd_put_v ();
                break;

                // V. Bat.  Last measure. 000.00
            case 17:
                // Falls through intentionally.
            case 117:
                lcd_put_num(LINE1+3,3, lroundf(100.0 * bat_volts()) );
                lcd_put_icon(P6);
                lcd_put_v ();
                break;
            case 217:
                lcd_put_num(LINE1+3,3, lroundf(100.0 * bat_volts_rtc()) );
                lcd_put_icon(P6);
                lcd_put_v ();
                break;

            case 18: // w 000000
                lcd_put_num(LINE1, LINE1_LEN, lroundf(afe_w(0)) );
                lcd_put_w();
                break;
            case 118:   // Phase 1
                show_phase = true;          // show measurement phase
                lcd_put_num(LINE1, LINE1_LEN, lroundf(afe_w(1)) );
                lcd_put_w();
                break;
            case 218:   // Phase 2
                show_phase = true;          // show measurement phase
                lcd_put_num(LINE1, LINE1_LEN, lroundf(afe_w(2)) );
                lcd_put_w();
                break;

            case 19: // var 000000
                lcd_put_num(LINE1, LINE1_LEN, lroundf(afe_var(0)) );
                lcd_put_var();
                break;
            case 119:   // Phase 1
                show_phase = true;          // show measurement phase
                lcd_put_num(LINE1, LINE1_LEN, lroundf(afe_var(1)) );
                lcd_put_var();
                break;
            case 219:   // Phase 2
                show_phase = true;          // show measurement phase
                lcd_put_num(LINE1, LINE1_LEN, lroundf(afe_var(2)) );
                lcd_put_var();
                break;

            case 20: // va 000000
                lcd_put_num(LINE1, LINE1_LEN, lroundf(afe_va(0)) );
                lcd_put_va();
                break;
            case 120:   // Phase 1
                show_phase = true;          // show measurement phase
                lcd_put_num(LINE1, LINE1_LEN, lroundf(afe_va(1)) );
                lcd_put_va();
                break;
            case 220:   // Phase 2
                show_phase = true;          // show measurement phase
                lcd_put_num(LINE1, LINE1_LEN, lroundf(afe_va(2)) );
                lcd_put_va();
                break;

            case 21: // Demand.
                // Kw of demand: 000000
                lcd_put_num(LINE1, LINE1_LEN, (int32_t)global.reg.dmd_max );
                lcd_put_w();
                break;
            case 121:
                // 00:00, time of demand.
                lcd_put_num(LINE1  ,2,
                        (int32_t)global.reg.dmd_hour);
                lcd_put_num(LINE1+2,2,
                        (int32_t)global.reg.dmd_min);
                lcd_put_icon(COL1);
                break;
            case 221:   // date of maximum demand
                lcd_put_num(LINE1  ,2,1+(int32_t)global.reg.dmd_mon);
                lcd_put_num(LINE1+2,2,(int32_t)(global.reg.dmd_mday));
                lcd_put_icon(D1);
                break;

            case 22: {
                int i;
                // Display all the segments except trademarks.
                for(i = 1; i <= DIGIT_CNT; ++i) {
                    int j;
                    const uint16_t * seg_ptr = lcd_chrs[ i ];
  
                    /* For every possible segment, write it. */
                    for (j = 0; j < lcd_chr_seg_cnts [ i ]; ++j)
                    {
                        /* get segment data, go to the next segment */
                        int seg = *seg_ptr++;

                        /* write the segment */
                        lcd_put_seg (seg, 1);
                    }                   
                }
                for(i = 0; i <= ICON_LAST; ++i)
                    if(i != 11 && i != 19) // Avoid the trademarks.
                        lcd_put_icon(i);
                }
                break;

            default: // Wh, etc. 000.00
                global.cal.lcd_idx = 3; break;
        } // Select the data.

    // Display line status
    if(!(meter_state & CREEP_MASK))
    {
        int w;

        if (show_phase) {
            if (2 == (global.cal.lcd_idx/100)) {    // measuring line 2?
                lcd_put_icon(L2);
            } else {
                lcd_put_icon(L1);
            }
        }

        w = afe_wsum(ALL_PHASES);
        if(0 < w ) {
            lcd_put_icon(S2);
        } else if (0 > w ) {
                lcd_put_icon(S1);
            }
        }
    }

    // Display tamper if tamper was ever discovered.
    if (global.reg.log.bits_sticky & LOG_TAMPER_MASK) {
        lcd_put_icon(EXCLAIM);      // Yes, put an exclaimation mark "!"
    }

    // Display battery status
    if (log_active(EV_BAT_BAD)) {
        lcd_put_icon(LOWBAT);       // bad battery
    }

    // Display communication status
    if (log_active(EV_COM)) {
        log_state(EV_COM, false);
        lcd_put_icon(COMS);          // Active communication
    }
} /* Update the LCD front panel */

/***************************************************************************
* Description:
*       Update the LCD front panel in brownout mode.
*       This can be very different from the normal mode, so it has
*       its own routine.
*       Also, it is for a particular display.
*   Input:
* Parameters:
*       The index, selects which screen to show on the LCD.
*       It displays data from the global meter data and billing registers.
*   Output:
*       It displays data on the LCD.
* Return Status:
*       None
***************************************************************************/
void lcd_update_brownout(int index)
{
    lcd_clear();
    lcd_put_num(LINE2,2,index);    // item number.
    switch(index) {

        // KWh, etc. 0.00000
        case 3: 
            if (global.cal.cfg & DISPLAY_KWH)
            {
                // This code is frequently requested;
                // To see the display, set )1=1
                lcd_put_num(LINE1+1,7,kwh(&nvm.reg.wh_cnt));
                lcd_put_kwh ();
            } else {
                lcd_put_num(LINE1+1,7,wh(&nvm.reg.wh_cnt));
                lcd_put_wh ();
            }
            break;

        case 4:
            if (global.cal.cfg & DISPLAY_KWH)
            {
                // This code is frequently requested;
                // To see the display, set )1=1
                lcd_put_num(LINE1+1,7,kwh(&nvm.reg.wh_cnt_ex));
                lcd_put_kwh ();
            } else {
                lcd_put_num(LINE1+1,7,wh(&nvm.reg.wh_cnt_ex));
                lcd_put_wh ();
            }
            break;

        case 5:
            if (global.cal.cfg & DISPLAY_KWH)
            {
                // This code is frequently requested;
                // To see the display, set )1=1
                lcd_put_num(LINE1+1,7,kwh(&nvm.reg.varh_cnt));
                lcd_put_kvarh ();
            } else {
                lcd_put_num(LINE1+1,7,wh(&nvm.reg.varh_cnt));
                lcd_put_varh ();
            }
            break;

        case 6:
            if (global.cal.cfg & DISPLAY_KWH)
            {
                // This code is frequently requested;
                // To see the display, set )1=1
                lcd_put_num(LINE1+1,7,kwh(&nvm.reg.varh_cnt_ex));
                lcd_put_kvarh ();
            } else {
                lcd_put_num(LINE1+1,7,wh(&nvm.reg.varh_cnt_ex));
                lcd_put_varh ();
            }
            break;

        default:
            break;
    }

    // Display tamper
    if (nvm.reg.log.bits_sticky & LOG_TAMPER_MASK) {// Was tamper ever discovered?
        lcd_put_icon(EXCLAIM);      // Yes, put an exclaimation mark "!"
    }

    // Display battery status
    if (log_active(EV_BAT_BAD)) {
        lcd_put_icon(LOWBAT);       // bad battery
    }
}

/***************************************************************************
 * Copyright (C) 2005..2017 Silergy Corp. 
 * All Rights Reserved.
 * This program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/


