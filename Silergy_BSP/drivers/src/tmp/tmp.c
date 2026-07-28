/***************************************************************************
 * DESCRIPTION: POWER METER - Temperature Routines.
 * This code reads the temperature.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * $Id: tmp.c 21473 2019-07-08 23:31:47Z rvandewa $
 ***************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Needed by rtc.h
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "irq.h"        // Interrupt management.
#include "pwrmode.h"    // Detect the power source.
#include "delay.h"      // call to delay()
#include "tmp.h"        // Check definitions.

/*** Dependencies ***/

static int16_t last_stemp; // Last measured Temperature.

/*** Version ****/
const char fw_ver_tmp[] =
"tmp"      /* Device Driver name */
" v"       /* ver string */
"1.00"     /* Major_ver.Minor_ver */
"\r\n"     /* CR + LF */
;



/****************************************************************************
* Description:
*       Initialize the temperature measurement.
* Input:
* Parameters:
*       None.
* Output:
*       Temperature measurements are started.
* Return Status:
*       First temperature measurement.
*****************************************************************************/
static void init(void) 
{
    // Wait till the RTC is free
    sys_shadow_busy_wait();

    SYS->WAKE_SRC = SHADOW_UNLOCK;      // Enable writing.

    // In demo code, the source of power is determined by the power mode.
    if(pwrmode_is_brownout()) {
        SYS->WAKE_b.temp_pwr = 0;           // use power from battery (VBAT)
        SYS->PWR_MOD_b.tper_dis = 0;        // Measure automatically.
    } else {
        SYS->WAKE_b.temp_pwr = 1;           // use power from mains (V3P3SYS)
        SYS->PWR_MOD_b.tper_dis = 1;        // Measure by software command.
    }

    // Set TEMP_PER (Temperature Period), an interval between 
    // temperature measurements for RTC compensation. 
    // See Drivers/Include/system_xx.h 
    SYS->PWR_MOD_b.temp_per = TEMP_PER_DEFAULT; 
    SYS->WAKE_b.temp_bsel = 0;          // Clear selection to VBat
    SYS->WAKE_b.temp_bat = 0;           // measure temperature

    SYS->WAKE_SRC = 0;                  // Relock the shadow registers.
    sys_shadow_busy_wait();             // wait until set.
}
float tmp_init(void) 
{
    // Finish the first measurement.
    return tmp();
}


/****************************************************************************
* Description:
*       Read a valid STEMP
* Input:
* Parameters:
*       None.
* Output:
* Return Status:
*       A valid 16-bit signed value read from STEMP
*****************************************************************************/
int16_t tmp_stemp(void)
{
    int stemp;
    IRQ_DEFINES;

    init(); // Safety code to assure it is initialized.

    if (SYS->PWR_MOD_b.tper_dis)        // If software controlled measurement.
    {
        sys_tmp_busy_wait();            // Wait for any measurement to complete.

        IRQ_DISABLE();                  // Global Interrupt Disable

        TEMP->CNTL_b.temp_start = 1;    // Start measurement.

        IRQ_ENABLE();                   // Restore Global Interrupt state
    }

    sys_tmp_busy_wait();            // Wait for measurement to complete.

    stemp = TEMP->BAT_TEMP_b.stemp; // Right-justify the measurement.

    if(TEMP->CNTL_b.temp_err || stemp == 0x7FFF) // Error condition
    {        
        delay(DELAY_MS(48));            // 48msec wait.

        IRQ_DISABLE();                  // Global Interrupt Disable

        TEMP->CNTL_b.temp_start = 1;    // Restart Temperature measurement.

        IRQ_ENABLE();                   // Restore Global Interrupt state

        sys_tmp_busy_wait();            // Wait till conversion is over. 

        delay(DELAY_MS(48));            // 48msec wait.

        TEMP->CNTL = 0;                 // Clear Temperature reset and Start.
        
        stemp = 0x7fff;             // An error was detected.
    }   

    return (int16_t)stemp;          // Return the measurement.
}


/****************************************************************************
* Description:
*       Deinitialize (shut down) the temperature, used for sleep.
* Input:
* Parameters:
*       None.
* Output:
*       All the clocks are turned off.
* Return Status:
*       None.
*****************************************************************************/
void tmp_deinit(void)
{
    /* Wait till the RTC/shadow register logic is free */
    sys_shadow_busy_wait();

    SYS->WAKE_SRC = SHADOW_UNLOCK;     // Enable writing.

    // Set an interval between temperature measurements 
    SYS->PWR_MOD_b.temp_per = TEMP_PER_DEFAULT; 
    SYS->PWR_MOD_b.tper_dis = 0;       // Measure automatically.

    // Power from VBAT_RTC.
    SYS->WAKE_b.temp_pwr = 0;

    // End writing.
    SYS->WAKE_SRC = 0;

    sys_shadow_busy_wait();
}


/****************************************************************************
* Description:
*       Measure the temperature.
*       the timing is that this always uses the measurement from the
*       previous call.  When the routine is called repetitively, as it
*       usually is, this wastes less time.
* Input:
*       Hardware register, stemp.
*       Uses the CE's calibrated values for STEMP22 and the LSB.
* Parameters:
*       none.
* Output:
* Return Status:
*       Returns a floating-point temperature.
*****************************************************************************/
float tmp(void)
{
    float out_val_f;
    uint32_t tmps;

    tmps = tmp_stemp(); // Shared code to safely get the current data.

    if(0x7FFF != tmps)  // If no error, use the value.
    {        
        last_stemp = tmps;
    }   

    // Calculate temperature. From SY7Txxxx/Drivers/Include/system_SY7xx.h
    out_val_f = COMPUTE_TEMP(last_stemp);

    return  out_val_f;   // Float encodes decimal point.
}


/***************************************************************************
 * Copyright (C) 2013-2016 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

