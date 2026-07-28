/***************************************************************************
 * DESCRIPTION: POWER METER - Battery Mode Routines.
 * This code controls the battery modes.
 *
 * Copyright (C) 2013-2020 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * $Id: pwrmode.c 24708 2020-04-29 20:36:29Z rvandewa $
 ***************************************************************************/
#include <stdbool.h>    // Needed by rtc.h
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "delay.h"      // for hysteresis in power failure detection.
#include "pwrmode.h"    // Function prototypes.

/** Dependencies ***/
// SystemInit
// sys-shadow-busy-wait()
// sys-tmp-busy-wait()

/*** Version ****/
const char fw_ver_pwrmode[] =
"pwrmode"  /* Device Driver name */
" v"       /* ver string */
"3.00"     /* Major_ver.Minor_ver */
"\r\n"     /* CR + LF */
;


int32_t wake_time;   // time to wake in timer units.


/****************************************************************************
* Description:
*       Read the power state.
*       If the power is too low to run the metrology, notice it.
*
* Parameters:
*   Input:
*         None.
*   Output:
*         true = SoC has lost mains power. false = power is normal.
****************************************************************************/
bool pwrmode_is_brownout(void)
{
    // Did the power up state machine switch V3P3RTC away from V3P3SYS?
    if (0 != SYS->STAT_b.v3p3rtc_src_nvsys)
        return true;
    else
        return false;
} // pwrmode_is_brownout


/****************************************************************************
* Description:
*       Read the health of the power sources from the auxiliary power pins:
*       Vbat and VLCT.
*
* Parameters:
*   Input:
*         None.
*   Output:
*         true = power source is OK.
****************************************************************************/
int pwrmode_vbat_is_ok(void)
{
    // Is the battery voltage bit valid?
    if (SYS->STAT_b.v3a_nok) 
        // Valid, so is the battery voltage ok?
        if (SYS->STAT_b.vbat_nok)
            return false;
        else
            return true;
    else // not valid, running from V3P3
        return true; // It's OK as far as we can tell.
} // pwrmode_vbat_ok()
int pwrmode_vlct_is_ok(void)
{
    // Is the voltage-line-cut supply pin ok?
    if (SYS->STAT_b.vlct_nok)
       return false;
    else
       return true;
} // pwrmode_vlct_ok


/****************************************************************************
* Description:
*       Read the power state.
*       Tests if the digital power can be regulated.
*       If not, battery power is failing.
*       The system intentionally does not test the BADVDD case;
*       BADVDD is too near digital failure levels.
*       If the analog front end is running, it should have adclk.8, LOWBIAS
*       set.  This test returns false near to the voltage at which the AFE with 
*       LOWBIAS stops measuring power accurately.
*
* Parameters:
*   Input:
*         None.
*   Output:
*         true = digital voltage is too low for its regulator to work.
****************************************************************************/
bool pwrmode_is_fail(void)
{
    // Is the system losing regulation of digital power?.
    if (SYS->STAT_b.v3p3d_nok)
        return true;
    else
        return false;
} // pwrmode_fail


/****************************************************************************
* Description:
*       Set RTCWAKE. Returns true on failure.
*
* Parameters:
*   Input:
*         New value for register RTCWAKE.
* Output:
*        None
* Return Status:
*        None
****************************************************************************/
void pwrmode_set_wake(int rtcwake)
{
    // Wait till the RTC is free
    sys_shadow_busy_wait();

    // Freeze the shadow registers.
    SYS->WAKE_SRC = SHADOW_UNLOCK;

    SYS->WAKE = rtcwake;

    // Unfreeze the shadow registers.
    SYS->WAKE_SRC = 0;

    sys_shadow_busy_wait();
} // pwrmode_set_rtcwake


/****************************************************************************
* Description:
*		 Arrange a timer to leave sleep or LCD only mode.
*		 The calculations are in pwrmode_set_rtcwake_tempcntl()
*
* Parameters:
*   Input:
*         Time in wake timer's native units.
*   Output:
*         None.
****************************************************************************/
void pwrmode_wait(int time)
{ 
    wake_time = time;
} // pwrmode_wait()


/****************************************************************************
* Description:
*       Set RTCWAKE, including the wake timer.
*
* Parameters:
*   Input:
*         Sets the wake input.
*   Output:
*         none
****************************************************************************/
static void make_pwrmode(int enable_wake)
{
    int wake;

    // Set the wake timer if needed.
    if(wake_time)
    {
        enable_wake &= 0xFFFF8000;// clear existing wake_tmr to set new wake_time
        wake = wake_time | enable_wake | WF_TMR_MASK; // Bit24 enables the timer.
        wake_time = 0;
    } else { // wake timer is not needed.
        wake = enable_wake;
    }

    // Enable brownout mode.
    SYS->PWR_MOD_b.brn_en = 1;

    // Set the wake flags.
    pwrmode_set_wake(wake);

    // Safety code, shared for sleep, lcd-only and vlct modes.
    // The set of the wake register, above waits for shadow registers to lock.
    sys_tmp_busy_wait();        // Wait for temperature measurement to end.
} // make_pwrmode()


/****************************************************************************
* Description:
*		 Enter sleep mode.
*		 This routine does not exit. It enters a low-power mode.
*		 The wake bits determine events that will cause the CPU to start.
*		 When the CPU starts, it starts from the reset vector.
*
* Parameters:
*   Input:
*         Sets the wake input.
*   Output:
*         None.
****************************************************************************/
void pwrmode_sleep(int wake)
{
    make_pwrmode(wake);
    // Wait for the state machine to switch to sleep.
    while(true)
    {
        SYS->MOD_CNTL = 1;
        delay(DELAY_MS(1));     // Wait for the switch, then retry.
    }
} // void pwrmode_sleep(void)


/****************************************************************************
* Description:
*		 Enter LCD-only mode.
*        The memory remains powered.
*		 This routine does not exit. It enters a low-power mode.
*		 The wake bits determine events that will cause the CPU to start.
*		 When the CPU starts, it starts from the reset vector.
* Parameters:
*   Input:
*         Sets the wake input.
*   Output:
*         None.
****************************************************************************/
void pwrmode_lcd(int wake)
{
    make_pwrmode(wake);
    // Wait for the state machine to switch to LCD-only mode.
    while(true)
    {
        SYS->MOD_CNTL = 3;
        delay(DELAY_MS(1));     // Wait for the switch, then retry.
    }
} // void pwrmode_lcd(void)


/****************************************************************************
* Description:
*		 Enter voltage line cut-mode (VLCT) mode.
*		 This routine does not exit. It enters a low-power mode.
*		 The wake bits determine events that will cause the CPU to start.
*		 When the CPU starts, it starts from the reset vector.
* Parameters:
*   Input:
*         Wake reason bit-mask.
*   Output:
*         None.
****************************************************************************/
void pwrmode_vlct(int wake)
{
    make_pwrmode(wake);
    // If it's not in VLCT mode, try to put it there.
    while (3 != (SYS->MOD_CNTL & 3))    // in VLCT mode already?
    {   // No- force VLCT mode.
        SYS->PWR_MOD_b.lct_en = 1;
        SYS->MOD_CNTL = 3;              // keep RAM powered, in LCD-only mode.
        //SYS->MOD_CNTL = 1;              // Or, RAM unpowered, in sleep mode.
        delay(DELAY_MS(1));             // Wait for the switch, then retry.
    }
} // pwrmode_vlct()


/***************************************************************************
* Description:
*        Serve the power fail interrupt.
* Parameters:
*   Input:
*         None.
*   Output:
*         If there is no power, it shuts down everything.
* Return Status:
*         None
***************************************************************************/
void pwrmode_fail_Handler(void)
{
    if(pwrmode_is_fail())   // digital power is out of regulation!
    {
        pwrmode_sleep(0);   // sleep until system power (V3P3SYS) returns.
    }
}


/***************************************************************************
 * Copyright (C) 2014-2020 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

