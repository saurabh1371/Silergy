/*****************************************************************************
 * DESCRIPTION: Billing register save and restore logic.
 * A crucial system design problem is that the billing registers must be
 * completely restored and valid before interrupts are enabled.
 * Otherwise, a power failure could save a register data structure that is
 * not valid.
 *
 * This code saves billing registers to (usually) two flash pages.
 * The SY7-series flash has a write limit of 100,000.  To get to a write limit
 * of 1,000,000, this code must store at least 10 copies of the register
 * data to dilute the writes.
 * It allocates the copies by indexing the copies in the flash pages with the
 * register save count.
 * When it reads the new register data, it locates the latest by finding
 * the valid copy with the latest register save count.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2012...2013, Silergy Corp. All rights reserved.
 *
 * $Id: reg_flash.c 17911 2018-06-02 05:34:34Z rvandewa $
 ****************************************************************************/
#include <string.h>
#include "options.h"    // Define the IC, CE code and other options.
#include "crc.h"        // Checksum/CRC of data structures.
#include "stm.h"        // Software timer
#include "flash.h"      // To read and write the flash.
#include "delay.h"      // A calibrated delay.
#include "nvram.h"      // Access NVRAM
#include "hash_str.h"	// Makes a number from a string.
#include "version.h"    // Software version strings.
#include "irq.h"        // Interrupt management.
#include "reg.h"        // Manage register data.

/*** Version ****/
const char fw_ver_reg[] =
"reg_flash" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


// Where to save the next set of billing registers.
volatile int8_t allocation_idx;        // allocation index
volatile int8_t allocation_idx_inv;    // to test validity

// tells which copy to save. true = save global.reg (The RAM copy)
// false = save nvm.reg, the stable copy in nonvolatile RAM.
       volatile bool save_global_reg;
static volatile bool lock;  // Defaults to false when cleared to zero.


/*****************************************************************************
* Description:
*       Calculate the addresses of the copies of power registers in flash
* Parameters:
*   Input:
*         An index, 0..n selecting a set of billing registers.
*   Output:
* Return Status:
*         A flash address of the start of that set of billing registers.
*****************************************************************************/
#define REG_SET_SIZE ((sizeof(Reg_t) + 3)&0x7FFFFFFC) // To next-largest word
#define REG_PER_PAGE (FPAGE_SIZE/REG_SET_SIZE)
#define FLASH_REG_PAGE_CNT (FLASH_BILLING_PAGE_CNT)
#define FLASH_REG_SET_CNT (FLASH_REG_PAGE_CNT*REG_PER_PAGE)
#define IS_FLASH_PAGE_BASE(_x_) (0 == ((FPAGE_SIZE-1) & (int)_x_))
#define FLASH_PAGES(_cnt_)       (_cnt_ * FPAGE_SIZE)
static int32_t reg_set_adr(int idx)
{
    int idx_wrapped = idx % FLASH_REG_SET_CNT;
    int page_idx = idx_wrapped / REG_PER_PAGE;
    int reg_in_page_idx = idx_wrapped % REG_PER_PAGE;
    uint32_t page = FLASH_BILLING_DATA; // Systems billing data base, flash.h
    page += FLASH_PAGES(page_idx) + (reg_in_page_idx * REG_SET_SIZE);
    // Detect bad configurations. 10 is minimum for 1x10^6 writes; >256
    // will overflow allocation_idx
    if(FLASH_REG_SET_CNT < 10 || FLASH_REG_SET_CNT > 256)
        return -1;
    return page;
}

/***************************************************************************
* Description:
*         Puts the protection and validation information in the
*         register data.
* Parameters:
*   Input:
*         Valid billing data in the scratch register area, global.reg.
*   Output:
*         Valid billing data with a CRC in global.reg and nvm.reg.
* Return Status:
*         None
***************************************************************************/
void reg_crc(void)
{
    // If RAM is corrupted, regenerate the billing registers and
    // allocation index.
    if (allocation_idx != ~allocation_idx_inv)
        reg_get();

    // Mark the register image with the software ID.
    global.reg.ver_hash = hash_code_str (software_version);

    // This counts the register saves so that the register-gets
    // always get the most current register set.
    // misc. save_cnt is from the last call of reg_get().
    // reg_cnt will be saved the next call of reg_put().
    // The reg_get handles overflow in this counter
    // by finding the greatest difference.
    global.reg.reg_cnt = (int8_t)(global.misc.save_cnt + 1);

    // Figure the checksum on the scratch register data.
    crc16((uint8_t *)&global.reg, sizeof(Reg_t), true);

    // Distribute the data in a thread-safe way without 
    // contention and without disabling interrupts (which 
    // can interfere with AMR).

    // Change which set to save, if there is a power failure interrupt.
    save_global_reg = true;     // global.reg is stable, and should be saved.
    nvm.reg = global.reg;       // Copy the billing data.
    save_global_reg = false;    // nvm.reg is stable, and should be saved.

    // Data is saved to NVRAM just after this routine is called, 
    // in meter-run(). Why there? So that the nvram-enable() and disable
    // can be called one "if" away from the nvram-write(), to assure that 
    // the NVRAM is safe from EMI corruption of the PC.
    // Billing data is in a state in which errors can be discovered.

    lock = false; // Safety code.
}


/***************************************************************************
* Description:
*           Clears the billing registers. All of them!  Everywhere!
*           This has to be called at particular place in the main loop,
*           so that the registers are not being updated.
*           On exit, saving the registers has to work!
*           See the calls to "reg_crc" in the main loop.
*           Clear the register data in EEPROM.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
***************************************************************************/
void reg_reset(void)
{
    int reg_idx;

    // Clear register data.
    memset((uint8_t*)&global.reg, 0, sizeof(Reg_t));

    // Save the clock time. Especially whether it has been set.
    global.reg.tm = nvm.reg.tm;

    // Save the log.
    global.reg.log = nvm.reg.log;

    log_state ( EV_REG_BAD, false );    // The register data is now bad.

    // Set the start of the maximum demand interval so far.
    global.reg.dmd_mon = nvm.reg.tm.tm_mon;
    global.reg.dmd_mday = nvm.reg.tm.tm_mday;
    global.reg.dmd_hour = nvm.reg.tm.tm_hour;
    global.reg.dmd_min = nvm.reg.tm.tm_min;

    // Unique value for this version is made from the date and time.
    global.reg.ver_hash = hash_code_str (software_version);

    // Figure the checksum.
    crc16((uint8_t *)&global.reg, sizeof(Reg_t), true);

    // Update the stable copy of registers in a thread-safe way
    // that does not disable interrupts.
    save_global_reg = true;     // global.reg is the stable register data.
    nvm.reg = global.reg;       // Move it to the nvram image.
    save_global_reg = false;    // nvm.reg is once again stable data.

    // Save the cleared register data to NVRAM.
    // Note that NVRAM write is enabled in meter-run(), one "if"
    // away from this call, so that this is safe if EMI corrupts
    // the PC. PCB_* symbols are from the PCB's .map file.
    nvram_write(PCB_NVRAM_ADR_NVM, (uint8_t*)&nvm, sizeof(Nvm_t));

    // Erase all the flash pages.
    for(reg_idx = 0; reg_idx < FLASH_REG_PAGE_CNT; reg_idx += REG_PER_PAGE) {
        flash_page_erase(FLASH_PAGES(reg_idx)+FLASH_BILLING_DATA);
    }

    allocation_idx = 0; // Save to the first slot.
    allocation_idx_inv = ~allocation_idx;   // Set the check value.
}


/***************************************************************************
* Description:
*           Get the best registers, RAM or flash.
*           On exit, saving the registers has to work!
*           So flags and data used by put_reg() have to be set-up.
*           Also, get_reg interacts with the main loop, which prepares this 
*           data.
*           See the calls to "reg_crc" in the main loop.
*           Put the register data into flash. In EFT, put reg can fail.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
***************************************************************************/
void reg_get(void)
{
    extern const struct tm starting_tm; // constant structure in time.c
    int good_reg_data = false;  // Assume that no billing register data is good.
    uint8_t reg_idx;        // index of the set of billing registers.
    uint8_t my_ver_hash;    // Hash of the firmware's version string.
    int8_t best_alloc_idx = -1;

    // Unique value for this version is made from the date and time.
    my_ver_hash = hash_code_str (software_version);

    // Get the best flash registers.
    // Also, set the allocation index
    for (reg_idx = 0; reg_idx < FLASH_REG_SET_CNT; ++reg_idx)
    {
        int good_reg_idx;
        const Reg_t *reg_ptr = (const Reg_t *)reg_set_adr(reg_idx);

        if(reg_ptr->ver_hash != my_ver_hash) { // firmware version mismatch
            good_reg_idx = false; // It's a bad register image.
        } else { // Check the CRC
            if (!crc16 ((uint8_t *) reg_ptr, sizeof (Reg_t), false)) { // CRC?
                good_reg_idx = false; /* something is wrong with it. */
            } else { // Version & CRC are good, so is it the last reg. set?
                if (good_reg_data) { /* Already found a valid register set? */
                    // Find the last reg. set.
                    // The signed modular math handles wrap-arounds
                    // as long as there are less than 128 reg. sets.
                    // And, the number of reg. sets is about 10 by design.
                    if (0 < ((global.reg.reg_cnt) - (reg_ptr->reg_cnt))) {
                        good_reg_idx = false; /* don't use this reg. set */
                    } else { // This is a later reg. set.
                        good_reg_idx = true; /* use this reg. set */
                    }
                } else { // No previous reg. set was found.
                    good_reg_idx = true;    /* use this reg. set */
                }
                if (good_reg_idx) {
                    global.reg = *reg_ptr;  // Save the best reg. set so far.
                    // Find the following slot, which is empty or erasable
                    // if this is the last reg. set.
                    best_alloc_idx = (1 + reg_idx) % FLASH_REG_SET_CNT; 
                    good_reg_data = true;   /* usable reg. set found */
                } // good data in global.reg, allocation index set...
            } // CRC was good...
        } // hash was good...
    } // end loop over all the reg sets; good_reg_data = true if any found.

    // Save the best allocation index.
    if(good_reg_data) {
        allocation_idx = best_alloc_idx; // Save to the next slot.
    } else {
        allocation_idx = 0; // All the flash register sets are bad; use zero.
    }
    allocation_idx_inv = ~allocation_idx;   // Set the error-checking.

    // Check the register set in NVRAM. It's the most recent if it's OK.
    if ( (my_ver_hash == nvm.reg.ver_hash) // firmware version OK?
      && (crc16 ((uint8_t *) &nvm.reg, sizeof (Reg_t), false)) // CRC OK?
    ) {                                 // Yes, NVRAM has valid data.
        good_reg_data = true;           // Keep it.
    } else {                            // NVRAM has bad data.
        if(good_reg_data) {             // Was some flash data OK?
            nvm.reg = global.reg;       // Put the OK flash data in NVRAM.
        } else {                        // NVRAM is bad too... get a default.
            memset((void*)&nvm.reg, 0, sizeof (Reg_t)); // Default to zeros.
            nvm.reg.tm = starting_tm;       // Stored time is bad; Get default.
            nvm.reg.log = global.reg.log;   // Log is from main loop.
            nvm.reg.ver_hash = my_ver_hash; // Get a new version hash.
            crc16 ((uint8_t *) &nvm.reg, sizeof (Reg_t), true); // Fix the CRC.
        } // make a default reg image.
    } // NVRAM reg. set is now valid.
    global.reg = nvm.reg;               // Make both register copies the same.

    // By default save the stable NVRAM register set, nvm.reg
    save_global_reg = false;

    // Count the register saves to get the most current register set.
    global.misc.save_cnt = nvm.reg.reg_cnt;

    // Set up the CE pulse counters from the saved data.
    ce_data.wpulse_ctr = nvm.reg.wh_cnt;
    ce_data.wsum_accum = 0xC0000000;    // Round down.
    ce_data.vpulse_ctr = nvm.reg.varh_cnt;
    ce_data.vsum_accum = 0xC0000000;    // Round down.

    log_state ( EV_REG_BAD , !good_reg_data ); // Record a failure.

    lock = false; // Reentrant monitor is done.
}


/***************************************************************************
* Description:
*          Put the register data into EEPROM. In EMI, reg-put can fail.
*          In experiments with EMI, reg-put failed about 1 per 500 times.
*          The meter saves five (6 if NVRAM has a battery) copies at different 
*          times, so if reg-put fails, the meter doesn't lose money.
*          (1/500) ^ 5 = 8.4x10^-14, i.e. more reliable than the IC's 
*          CMOS logic.
*          Also, reg-put interacts with the main loop, which prepares this data.
*          See the calls to "reg-crc()" in meter-run(), called from the main loop.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
***************************************************************************/
void reg_put (void)
{
    Reg_t *reg_ptr;
    uint32_t reg_flash_ptr;
    IRQ_DEFINES;  // Prepare to restore interrupt state.

    // prevent reentrant calling of this routine
    IRQ_DISABLE();               // Disable interrupts
    if (lock) {
        // If it goes here, then the routine has been called 
        // already, so data has been saved.
        IRQ_ENABLE();     // Restore interrupt state.
        return;
    }
    lock = true;

    // This counts the register saves so that reg-get()
    // always get the most current register set.
    global.misc.save_cnt = global.reg.reg_cnt;

    IRQ_ENABLE();         // Restore the interrupt state.

    // save the data that is OK right now.
    if (save_global_reg)
        reg_ptr = &global.reg;  // NVRAM is being updated.
    else
        reg_ptr = &nvm.reg;     // Save NVRAM data.

    // Find the next free slot.
    if(allocation_idx != ~allocation_idx_inv) {
        reg_get(); // regenerate the allocation index.
    }

    reg_flash_ptr = reg_set_adr(allocation_idx);

    if(IS_FLASH_PAGE_BASE(reg_flash_ptr)) { // Is the slot erased?
        if (0xffffffff != *((const uint32_t *)reg_flash_ptr)) {
            flash_page_erase( reg_flash_ptr ); // Not erased, so erase it.
        } // now erased.
    } // if it was a page base, now it's erased.

    // Write the billing registers to flash.
    flash_write(reg_flash_ptr, (uint32_t*)reg_ptr, REG_SET_SIZE);

    // Increment the allocation index, and wrap around.
    allocation_idx = (1 + allocation_idx) % FLASH_REG_SET_CNT;
    allocation_idx_inv = ~allocation_idx; // Set the check-value.

    lock = false; // Reentrant monitor is done.
}


/***************************************************************************
 * Copyright (C) 2011..2018 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

