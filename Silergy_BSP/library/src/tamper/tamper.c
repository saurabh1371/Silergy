/*****************************************************************************
 * DESCRIPTION: Tamper detection logic.
 * This code manages a couple of burglar alarms in a meter.
 * The idea is to use whatever tamper features are permitted by the PCB.
 * DIOs, interrupts or wake inputs are defined in the PCB's map file.
 * If a tamper GPIO is defined, it is polled.
 * If a tamper interrupt is defined, it is enabled.
 * If a tamper wake is defined, it is selected before sleep.
 * The tamper enables and detection are "smart", so that if a tamper 
 * already exists, the tamper interrupt or wake is not invoked.
 * This is so that the meter can seem to function normally, while still
 * recording a tamper.
 * The tamper recording sets a bit EV_TAMPER_MASK in state , 
 * a global variable.
 * The demo code, in main(), uses the EV_TAMPER_MASK & state
 * to count seconds of tamper.
 * The LCD display can poll this bit and produce a display if needed.
 * Be careful to display a tamper indication in a way that a meter-reader
 * can recognize, but not a utility customer.  If a tamper is clearly shown
 * on a meter, further vandalism often occurs.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2016, Silergy Corp. All rights reserved.
 * 
 * $Id: tamper.c 24039 2020-03-25 23:44:25Z rvandewa $
 ****************************************************************************/
#include "options.h"    // Define the PCB, IC, CE code and other options.
#include "delay.h"      // Loop delay
#include "external_int.h" // External interrupts
#include "gpio.h"       // Define LCD mapping of a pin.
#include "log.h"        // Report tampering.
#include "tamper.h"     // Check the definitions.


/*** Version ****/
const char fw_ver_tamper[] =
"tamper" /* Library file name */
" v"     /* ver string */
"1.01"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


/***************************************************************************
* Description:
*        Tamper detection interrupt(s).
* Parameters:
*   Input:
*         None.
*   Output:
*         It sets the tamper detection call calling the error logic.
* Return Status:
*         Returns nonzero if it detects a tamper.
***************************************************************************/
// If the case tamper pin is defined.
// Note: #-defines beginning PCB_* are defined in the application's PCB map*.h
#ifdef PCB_CASE_TAMPER_SEG
void tamper_case_isr(void)
{
    // EV_* symbols are in the log.h file. 
    log_state(EV_CASE_TAMPER,true);
}
#endif
#ifdef PCB_TERM_TAMPER_SEG
void tamper_term_isr(void)
{
    // EV_* symbols are in the log.h file. 
    log_state(EV_TERM_TAMPER,true);
}
#endif
#ifdef PCB_MAGNET_TAMPER_SEG
void tamper_magnet_isr(void)
{
    // EV_* symbols are in the log.h file. 
    log_state(EV_MAG_TAMPER,true);
}
#endif


/***************************************************************************
* Description:
*        Tamper detection initialization.
* Parameters:
*   Input:
*         None.
*   Output:
*         It sets the tamper detection.
* Return Status:
*         None
***************************************************************************/
void tamper_init(void)
{
    //int tamper_hw_exists = false;

    // Set-up tampers
    // Two are handled; Some meters detect tampers on a wiring block
    // as well a case.

    // The case-open tamper.
    // PCB_* symbols are in the PC mapping file included in options.h
    #ifdef PCB_CASE_TAMPER_SEG 
    gpio_alt_off(PCB_CASE_TAMPER_SEG);
    gpio_dir_in(PCB_CASE_TAMPER_SEG);
    tamper_hw_exists = true;
    #endif

    // The terminal block tamper.
    #ifdef PCB_TERM_TAMPER_SEG 
    gpio_alt_off(PCB_TERM_TAMPER_SEG);
    gpio_dir_in(PCB_TERM_TAMPER_SEG);
    tamper_hw_exists = true;
    #endif

    // Magnet tamper detected via a reed switch and DIO.
    #ifdef PCB_MAGNET_TAMPER_SEG
    gpio_alt_off(PCB_MAGNET_TAMPER_SEG);
    gpio_dir_in(PCB_MAGNET_TAMPER_SEG);
    tamper_hw_exists = true;
    #endif
    
    // Log a watchdog reset.
//    if(SYS->WAKE_SRC_b.ws_wdof) {
//        log_event(EV_WATCHDOG);
//    }

    // If there is any tamper detection, it may have very high impedance
    // pull-ups, to minimize battery current in sleep modes.
//    if(tamper_hw_exists) {
//        delay(DELAY_MS(3)); // Wait for tampers to settle.
//    }

    // Poll for a tamper, and initialize tamper interrupts.
  //  tamper_run();
}

/***************************************************************************
* Description:
*        Tamper detection polling.
*        Note that it polls the GPIOs in all cases, whether or not
*        the interrupts are on, off, whatever.
* Parameters:
*   Input:
*         None.
*   Output:
*         It sets the tamper detection call calling the error logic.
* Return Status:
*         Returns nonzero if it detects a tamper.
***************************************************************************/
int tamper_run(void)
{
    int tamper = false;
    int insecure = false;

    // Reset interrupts, or detect tamper.
    #ifdef PCB_CASE_TAMPER_INT
    if(PCB_CASE_TAMPER == gpio_get_state(PCB_CASE_TAMPER_SEG)) {
        tamper_case_isr();    // Tamper detected.
    } else {
        log_state(EV_CASE_TAMPER, false);
        // level-triggered, on PCB_CASE_TAMPER state.
        external_int_init(PCB_CASE_TAMPER_INT, true, 
            PCB_CASE_TAMPER, tamper_case_isr); // Tamper int.
    }
    #else
    #ifdef PCB_CASE_TAMPER_SEG
    if(PCB_CASE_TAMPER == gpio_get_state(PCB_CASE_TAMPER_SEG)) {
        tamper_case_isr();    // Tamper detect if no interrupt.
    } else {
        log_state(EV_CASE_TAMPER, false);
    }
    #endif
    #endif

    // Reset interrupts, or detect tamper.
    #ifdef PCB_TERM_TAMPER_INT
    if(PCB_TERM_TAMPER == gpio_get_state(PCB_TERM_TAMPER_SEG)) {
        tamper_term_isr();    // Tamper detected.
    } else {
        log_state(EV_TERM_TAMPER, false);
        // level-triggered, on PCB_TERM_TAMPER state.
        external_int_init(PCB_TERM_TAMPER_INT, true, 
            PCB_TERM_TAMPER, tamper_term_isr); // Tamper int.
    }
    #else
    #ifdef PCB_TERM_TAMPER_SEG
    if(PCB_TERM_TAMPER == gpio_get_state(PCB_TERM_TAMPER_SEG)) {
        tamper_term_isr();    // Tamper detect if no interrupt.
    } else {
        log_state(EV_TERM_TAMPER, false);
    }
    #endif
    #endif

    // Reset interrupts, or detect tamper.
    #ifdef PCB_MAGNET_TAMPER_INT
    if(PCB_MAGNET_TAMPER == gpio_get_state(PCB_MAGNET_TAMPER_SEG)) {
        tamper_magnet_isr();    // Tamper detected.
    } else {
        log_state(EV_MAG_TAMPER, false);
        // level-triggered, on PCB_MAGNET_TAMPER state.
        external_int_init(PCB_MAGNET_TAMPER_INT, true, 
            PCB_MAGNET_TAMPER, tamper_magnet_isr); // Tamper int.
    }
    #else
    #ifdef PCB_MAGNET_TAMPER_SEG
    if(PCB_MAGNET_TAMPER == gpio_get_state(PCB_MAGNET_TAMPER_SEG)) {
        tamper_magnet_isr();    // Tamper detect if no interrupt.
    } else {
        log_state(EV_MAG_TAMPER, false);
    }
    #endif
    #endif

    // Detect an insecure debugger.
    insecure = false;
    if(   (1UL == SV->VERSION_b.test)   // Debugger is attached, or
       || (0 == FLASH->PROT_b.dbg_lock) // Debugger is unlocked, or
       || (0 == FLASH->PROT_b.secure)   // flash is not secured
      )
        insecure = true;
    // Or if SSPI is insecure.
    if(1UL == SSPI->CNTL_b.sspi_en) // If it's enabled
        if(1UL != SSPI->CNTL_b.sspi_safe) // and unsafe
            insecure = true;
    log_state(EV_DEBUGGER, insecure);
    
    log_state(EV_FUSEFAIL, (SYS->STAT & FUSE_FAIL_INT));

    #ifdef PCB_CASE_TAMPER_SEG
    // EV_* enumerations are in log.h
    tamper |= log_active(EV_CASE_TAMPER);
    #endif
    #ifdef PCB_TERM_TAMPER_SEG
    tamper |= log_active(EV_TERM_TAMPER);
    #endif
    #ifdef PCB_MAGNET_TAMPER_SEG
    tamper |= log_active(EV_MAG_TAMPER);
    #endif

    return tamper;
}


/***************************************************************************
* Description:
*        Tamper detection by waking from sleep.
*        This adds any tamper detection wake bits to a bit-array of 
*        wake-requests.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
***************************************************************************/
int tamper_wakes(int in)
{
    int out = in;

    #ifdef PCB_CASE_TAMPER_WF
    // PCB_* symbols are in the PC mapping file included in options.h
    out |= PCB_CASE_TAMPER_WF;
    #endif

    #ifdef PCB_TERM_TAMPER_WF
    out |= PCB_TERM_TAMPER_WF;
    #endif

    #ifdef PCB_MAGNET_TAMPER_WF
    out |= PCB_MAGNET_TAMPER_WF;
    #endif

    return out;
}


/***************************************************************************
 * Copyright (C) 2011..2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

