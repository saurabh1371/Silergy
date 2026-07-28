/***************************************************************************
 * DESCRIPTION: POWER METER - Battery Measurement Routines.
 * This code reads the battery.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Dependencies: uses register-write routines from rtc.c
 *
 * $Id: bat.c 17045 2018-03-15 22:51:49Z rvandewa $
 ***************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Needed by rtc.h
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "irq.h"        // interrupt management.
#include "delay.h"      // Call to delay()
#include "bat.h"        // Check function prototypes.

// Firmware Device Driver version
const char fw_ver_bat[] =
"bat"      /* Device Driver name */
" v"       /* ver string */
"1.00"     /* Major_ver.Minor_ver */
"\r\n"     /* CR + LF */
;

/*** Dependencies ***/
//  sys_shadow_wait(), waits for the shadow registers to unlock.
//  sys_tmp_wait(), waits for the temperature measurement to complete.



/****************************************************************************
* Description:
*       Initialize the battery measurement.
*
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
****************************************************************************/
void bat_init(void) 
{
    // sys-shadow-busy-wait() is from Device-name\Drivers\Src\sys\system_name.c
    sys_shadow_busy_wait();

    SYS->WAKE_SRC = SHADOW_UNLOCK;
    SYS->WAKE_b.temp_pwr = 1;    // Power from mains.

    // Set TEMP_PER (Temperature Period), an interval between 
    // temperature measurements for RTC compensation. 
    // See Drivers/Include/system_xx.h 
    SYS->PWR_MOD_b.temp_per = TEMP_PER_DEFAULT; 

    SYS->WAKE_SRC = 0;           // End writing.

    sys_shadow_busy_wait();

    // Measure the batteries, and record any errors.
    bat_volts();
    bat_volts_rtc();
} // bat_init


/****************************************************************************
* Description:
*       Deinitialize (shut down) battery measurement, used for sleep.
*       Reduce power use, if any.
*
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
****************************************************************************/
void bat_deinit(void)
{ 
    SYS->WAKE_b.temp_bcur = 0; // turn off BCURR, battery current drain.
} // bat_deinit


/****************************************************************************
* Description:
*       Measure a battery voltage under a 100uA load.
*
* Parameters:
*   Input:
*         true = measure vbat_rtc, false = measure vbat
*   Output:
*         volts
****************************************************************************/
static float get_volts(bool vbat_rtc)
{ 
    float ftmp;
    int tmpT;
    int cnt = 3;
    IRQ_DEFINES;

    // if temperature measurement does not time out
    if( !sys_shadow_busy_wait() ) {
        sys_tmp_busy_wait();

        SYS->WAKE_SRC = SHADOW_UNLOCK;      // Enable writing.

        SYS->WAKE_b.temp_bsel = vbat_rtc ? 0 : 1;
        SYS->WAKE_b.temp_bat = 1;           // measure battery
        SYS->WAKE_b.temp_pwr = 1;           // use power from mains (V3P3SYS)

        SYS->WAKE_SRC = 0;                  // Relock the shadow registers.

        sys_shadow_busy_wait();             // wait until set.

        IRQ_DISABLE();                   // Global Interrupt Disable

        SYS->WAKE_b.temp_bcur = 1;          // Enable battery current drain.
        TEMP->CNTL_b.temp_start = 1;        // Start measurement.

        IRQ_ENABLE();             // Restore Global Interrupt state

        sys_tmp_busy_wait();                // Wait till conversion is over.

        SYS->WAKE_b.temp_bcur = 0;          // Disable battery current drain.

        SYS->WAKE_SRC = SHADOW_UNLOCK;      // Enable writing.

        SYS->WAKE_b.temp_bat = 0;           // Restore to measure temperature
        SYS->WAKE_b.temp_bsel = 1;          // Restore to measure VBAT

        SYS->WAKE_SRC = 0;                  // Relock the shadow registers.

        sys_shadow_busy_wait();             // wait until set.
    }

    // Read the battery voltage register.
    tmpT = TEMP->BAT_TEMP_b.bsense;

    while( ( TEMP->CNTL_b.temp_err || tmpT == 0x7FFF)
            && cnt-- > 0) // Error condition
    {        
        delay(DELAY_MS(48));            // ~48msec wait.

        IRQ_DISABLE();                  // Global Interrupt Disable

        TEMP->CNTL_b.temp_start = 1;    // Restart Temperature measurement.

        IRQ_ENABLE();                   // Restore Global Interrupt state

        sys_tmp_busy_wait();            // Wait till conversion is over. 

        delay(DELAY_MS(48));            // ~48msec wait.

        TEMP->CNTL = 0;                 // Clear Temperature reset and Start.

        // Read the battery voltage register.
        tmpT = TEMP->BAT_TEMP_b.bsense;
    }
    if (0 == cnt)
        tmpT = 0;

    // Macro for VBAT computation 
    ftmp = COMPUTE_VBAT((float)tmpT);

    return ftmp;
} // get_volts


/****************************************************************************
* Description:
*       Measure the battery's voltage under a 100uA load.
*
* Parameters:
*   Input:
*         None.
*   Output:
*         Volts
****************************************************************************/
float bat_volts(void)
{
    return get_volts(false); 
} // bat_volts


/****************************************************************************
* Description:
*       Measure the RTC battery's voltage under a 100uA load.
*       return samples per accumulation interval.
*
* Parameters:
*   Input:
*         None.
*   Output:
*         Volts
****************************************************************************/
float bat_volts_rtc(void)
{
    return get_volts(true); 
} // bat_volts_rtc


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

