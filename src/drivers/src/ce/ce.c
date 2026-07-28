/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: These are the parts of CE initialization and start-up
 * that are portable.
 *
 * Copyright 2017, Silergy Corp.   All rights reserved.
 * $Id: ce.c 24638 2020-04-27 22:35:35Z rvandewa $
 ****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Booleans.
#include <string.h>     // C standard string library.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "mmath.h"      // meter math (for lmin())
#include "afe.h"        // calls ce_init();
#include "delay.h"      // loop delay
#include "gpio.h"       // use LCD interfaces to force pulse outputs to idle.
#include "bitdef.h"     // Bit definition macros
#include "ce.h"         // Verify the function prototypes.

/*** Version ****/
const char fw_ver_ce[] =
"ce"         /* Device Driver name */
" v"         /* ver string */
"3.00"       /* Major_ver.Minor_ver */
"\r\n"       /* CR + LF */
;


volatile bool ce_xfer_done;                // true means data is available.
bool sag_int_enabled;   // true means YPLS interrupt is enabled 

// Interrupt call-back variables.
void (*fn_ptr_xfer)(void);
void (*fn_ptr_ce_busy)(void);
void (*fn_ptr_vp)(void);
void (*fn_ptr_wp)(void);
void (*fn_ptr_xp)(void);
void (*fn_ptr_yp)(void);

/***************************************************************************   
* Description:  
*       ce_init() is customized for each CE code.
*       See the file ce<CE number>.c and ce<CE number>.h
***************************************************************************/

/***************************************************************************   
* Description:  
*       ce_deinit().
* Parameters:
*   Input:
*         None.
*   Output: 
*         CE register are initialized to be off, in a minimum power state.
* Return Status: 
*         None
***************************************************************************/
void ce_deinit(void)
{
    ce_disable();

    ADC->VREF_CNTL_b.adcm_en = 0; // Disable multiplexing ADC
    ADC->VREF_CNTL_b.adcs_en = 0; // Disable staring ADC
    CE->INT_CNTL = 0;   // Disable and clear all CE interrupts.
    CE->PLS_CNTL=0;     // configure pulse output (disabled)
    ADC->FIR_LEN=0;     // No slots.
    RMT->CNTL=0;        // disable Remotes
    ADC->SLOT_CNTL=0;     
}


/***************************************************************************   
* Description:  
*       ce_disable().
* Parameters:
*   Input:
*         None.
*   Output: 
*         CE register are initialized to be off, in a minimum power state.
* Return Status: 
*         None
***************************************************************************/
void ce_disable(void)
{
    // Disable pulses before the CE is initialized.
    // More pulse disable/enable logic is in afe.c
    gpio_alt_off(wpulse_seg);
    if(VPULSE_OK)
    {
        gpio_alt_off(vpulse_seg);
    } else {
        gpio_alt_on(vpulse_seg);
    }
    #ifdef xpulse_seg
    gpio_alt_off(xpulse_seg);
    #endif
    #ifdef ypulse_seg
    gpio_alt_off(ypulse_seg);
    #endif

    ADC->CNTL_b.mux_div = 0;    // Disable the analog front end's main clock
    delay(DELAY_MS(4));         // More than two samples, so it stops correctly.
    CE->CNTL_b.ce_en = 0;       // Disable the CE
}


/***************************************************************************   
* Description:  
*       ce_load_code(). Also disable pulses.
* Parameters:
*   Input:
*         Starting address CE code.
*   Output: 
*         Loads CE code
* Return Status: 
*         None
***************************************************************************/
void ce_load_code(const unsigned int *ce_code_ptr)
{
    gpio_alt_off(wpulse_seg);
    gpio_set_state(wpulse_seg,1);
    gpio_dir_in(wpulse_seg);

    if(VPULSE_OK)
    {
        gpio_alt_off(vpulse_seg);
        gpio_set_state(vpulse_seg,1);
        gpio_dir_in(vpulse_seg);
    }

    #ifdef xpulse_seg
    gpio_alt_off(xpulse_seg);
    gpio_set_state(xpulse_seg,1);
    gpio_dir_in(xpulse_seg);
    #endif
    #ifdef ypulse_seg
    gpio_alt_off(ypulse_seg);
    gpio_set_state(ypulse_seg,1);
    gpio_dir_in(ypulse_seg);
    #endif

    CE->LCTN = (unsigned int)ce_code_ptr; 
}


/***************************************************************************   
* Description:  
*       Enables ISRs for the CE.
* Parameters:
*   Input:
*         Indicate which interrupt with an ISR enable mask, from sy7xx.h
*           Note: Interrupts that interfere with accuracy can't be enabled.
*           e.g. those synchronized with mains or the sample freq.
*         A function pointer, for a function to be executed.
*           A null pointer does nothing.
*   Output: 
*       The CE's ISR will execute the enabled interrupt.
* Return Status: 
*       Nonzero for failure, zero for success
***************************************************************************/
int ce_int_enable(int mask, void (*fn_ptr)(void))
{
    int failed = false;
	
	  // Preserve interrupt enable bits but don't set interrupt flag bits.
	  // Writing 1 to interrupt flag will clear the int. flag.
	  uint32_t ce_ie_mask = CE->INT_CNTL & 0xFF00; 

    switch(mask)
    {
        case XFER_N_I:
            fn_ptr_xfer = fn_ptr;   // Set the callback
            CE->INT_CNTL = ce_ie_mask | XFER_N_IE; // Enable the interrupt
            break;
        case CE_BUSY_I:
            fn_ptr_ce_busy = fn_ptr;   // Set the callback
            CE->INT_CNTL = ce_ie_mask | CE_BUSY_IE; // Enable the interrupt
            break;
        case VPLS_I:
            fn_ptr_vp = fn_ptr;     // Set the callback
            CE->INT_CNTL = ce_ie_mask | VPLS_IE; // Enable the interrupt
            break;
        case WPLS_I:
            fn_ptr_wp = fn_ptr;     // Set the callback
            CE->INT_CNTL = ce_ie_mask | WPLS_IE; // Enable the interrupt
            break;
        case XPLS_I:
            fn_ptr_xp = fn_ptr;     // Set the callback
            CE->INT_CNTL = ce_ie_mask | XPLS_IE; // Enable the interrupt
            break;
        case YPLS_I:
            fn_ptr_yp = fn_ptr;     // Set the callback
            CE->INT_CNTL = ce_ie_mask | YPLS_IE; // Enable the interrupt
            sag_int_enabled = true;
            break;
        default:
            failed = true;
            break;
    }

    NVIC_EnableIRQ(CE_IRQn);  // enabling the CE causes a hang.

    return failed;
} // ce_int_enable()


/***************************************************************************   
* Description:  
*       Disables ISRs for the CE.
* Parameters:
*   Input:
*         Indicate which interrupt with an ISR enable mask, from sy7xx.h
*           Note: Interrupts that interfere with accuracy can't be enabled.
*           e.g. those synchronized with mains or the sample freq.
*   Output: 
*       The CE's ISR will not run the enabled interrupt.
* Return Status: 
*       None
***************************************************************************/
void ce_int_disable(int mask)
{
    // Preserve interrupt enable bits but don't set interrupt flag bits.
    // Writing 1 to interrupt flag will clear the int. flag.
    uint32_t ce_ie_mask = CE->INT_CNTL & 0xFF00; 
	
    switch(mask)
    {
        case XFER_N_I:
            CE->INT_CNTL = (ce_ie_mask & ~(XFER_N_IE));// Disable the interrupt
            fn_ptr_xfer = NULL;     // Disable the call-back.
            break;
        case CE_BUSY_I:
            CE->INT_CNTL = (ce_ie_mask & ~(CE_BUSY_IE));// Disable the interrupt
            fn_ptr_ce_busy = NULL;     // Disable the call-back.
            break;
        case VPLS_I:
            CE->INT_CNTL = (ce_ie_mask & ~(VPLS_IE));// Disable the interrupt
            fn_ptr_vp = NULL;       // Disable the callback.
            break;
        case WPLS_I:
            CE->INT_CNTL = (ce_ie_mask & ~(WPLS_IE));// Disable the interrupt
            fn_ptr_wp = NULL;       // Disable the callback.
            break;
        case XPLS_I:
            CE->INT_CNTL = (ce_ie_mask & ~(XPLS_IE));// Disable the interrupt
            fn_ptr_xp = NULL;       // Disable the callback.
            break;
        case YPLS_I:
            CE->INT_CNTL = (ce_ie_mask & ~(YPLS_IE));// Disable the interrupt
            fn_ptr_yp = NULL;       // Disable the callback.
            sag_int_enabled = false;
            break;
        default:
            // Disable any other CE interrupt.
            CE->INT_CNTL = (ce_ie_mask & ~(FIR_DONE_IE));// Disable the interrupt
            CE->INT_CNTL = (ce_ie_mask & ~(MUX_SYNC_IE));// Disable the interrupt
            break;
    }
} // ce_int_disable()


/****************************************************************************
* Description:
*       CE Interrupt Service Routine : process interrupt from CE
*       and will set status bits related to CE.
* Parameters:
*   Input:
*         None
*   Output:
*         None.
* Return Status:
*         None
*****************************************************************************/
void CE_Handler(void)   // CE ISR
{
    uint32_t ce_state,ce_int_enable;
    uint32_t ce_ie_mask; // CE int. enable mask

    ce_state = CE->INT_CNTL;

    // Bits 15..8 of the interrupt control are interrupt enable bits.
    ce_int_enable = ce_state >> 8;  // To service only ISRs which are enabled.
    ce_ie_mask = 0xFF00 & ce_state; // Mask for CE int. enable bits
    // Mask CE Interrupt flags with Interrupt enable bits.
    ce_state = ce_state & ce_int_enable;

    // Masking prevents spurious interrupt acknowledges, when flags are set
    // and other interrupts run.

    if(ce_state & CE_BUSY_I)
    {
        CE->INT_CNTL = ce_ie_mask | CE_BUSY_I; // Clear the interrupt
        if(NULL != fn_ptr_ce_busy)       // If callback is set
            (*fn_ptr_ce_busy)();         // run it
    }

    if(ce_state & XFER_N_I)
    {
        CE->INT_CNTL = ce_ie_mask | XFER_N_I; // Clear the interrupt
        if(NULL != fn_ptr_xfer)         // If callback is set
            (*fn_ptr_xfer)();           // run it
    }

    // Handle the other cases.
    if(ce_state & VPLS_I)
    {
        CE->INT_CNTL = ce_ie_mask | VPLS_I; // Clear the interrupt
        if(NULL != fn_ptr_vp)           // If callback is set
            (*fn_ptr_vp)();             // run it
    }

    if(ce_state & WPLS_I)
    {
        CE->INT_CNTL = ce_ie_mask | WPLS_I; // Clear the interrupt
        if(NULL != fn_ptr_wp)           // If callback is set
            (*fn_ptr_wp)();             // run it.
    }

    if(ce_state & XPLS_I)
    {
        CE->INT_CNTL = ce_ie_mask | XPLS_I; // Clear the interrupt
        if(NULL != fn_ptr_xp)           // If callback is set
            (*fn_ptr_xp)();             // run it.
    }

    if(ce_state & YPLS_I)
    {
        CE->INT_CNTL = ce_ie_mask | YPLS_I; // Clear the interrupt
        if(NULL != fn_ptr_yp)           // If callback is set
            (*fn_ptr_yp)();             // run it.
    }

    if(ce_state & FIR_DONE_I)           // Safety code in case of EMI.
    {
        // Clear the interrupt and Disable the interrupt
        CE->INT_CNTL = (ce_ie_mask & ~(FIR_DONE_IE)) | FIR_DONE_I; 
    }

    if(ce_state & MUX_SYNC_I)           // Safety code in case of EMI.
    {
        // Clear the interrupt and Disable the interrupt
        CE->INT_CNTL = (ce_ie_mask & ~(MUX_SYNC_IE)) | MUX_SYNC_I; 
    }
}


/***************************************************************************   
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
 
