/*****************************************************************************
 * DESCRIPTION: Billing register save and restore logic.
 * A crucial system design problem is that the billing registers must be
 * completely restored and valid before interrupts are enabled.
 * Otherwise, a power failure could save a register data structure that is
 * not valid.
 *
 * This code saves billing registers to multiple EEPROM pages.
 * It allocates the copies by indexing the copies with the
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
 * $Id: reg_eeprom.c 18785 2018-09-17 18:23:39Z ptanwar $
 ****************************************************************************/
#include <string.h>
#include "options.h"    // Define the IC, CE code and other options.
#include "crc.h"        // Checksum/CRC of data structures.
#include "stm.h"        // Software timer
#include "eeprom_common.h"     // Common EEPROM definitions.
#include "eeprom.h"     // To access the EEPROM.
#include "nvram.h"      // Access NVRAM
#include "hash_str.h"	// Makes a number from a string.
#include "version.h"    // Software version strings.
#include "irq.h"        // Interrupt management.
#include "reg.h"        // Manage register data.

/*** Version ****/
const char fw_ver_reg[] =
"reg_eeprom" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


// tells which copy to save. true = save global.reg (The RAM copy)
// false = save nvm.reg, the stable copy in nonvolatile RAM.
       volatile bool save_global_reg;
static volatile bool lock;  // Defaults to false when cleared to zero.


/*****************************************************************************
* Description:
*       Calculate the addresses of the copies of power registers in EEPROM
* Parameters:
*   Input:
*         An index, 0..n selecting a set of billing registers.
*   Output:
* Return Status:
*         An EEPROM address of the start of that set of billing registers.
*****************************************************************************/
#define EEPROM_REG_SET_CNT 5
#define REG_SET_SIZE ((((sizeof(Reg_t) + EEPROM_PAGE_SIZE - 1) \
      / EEPROM_PAGE_SIZE) * EEPROM_PAGE_SIZE)+60416)//ANIL RAJIV

static uint32_t reg_set_adr(int idx)
{
    if (idx >= EEPROM_REG_SET_CNT)
        idx = 0;
    return (EEPROM_REGISTERS + (idx * REG_SET_SIZE));
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
    // Mark the register image with the software ID.
  // global.reg.ver_hash = hash_code_str (software_version);

    // This counts the register saves so that the register-gets
    // always get the most current register set.
    // save_cnt is from the last call of reg_get().
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

    log_state ( EV_REG_BAD, false );

    // Set the start of the maximum demand interval so far.
    // (This is in the register data.)
    global.reg.dmd_mon = nvm.reg.tm.tm_mon;
    global.reg.dmd_mday = nvm.reg.tm.tm_mday;
    global.reg.dmd_hour = nvm.reg.tm.tm_hour;
    global.reg.dmd_min = nvm.reg.tm.tm_min;

    // Unique value for this version is made from the date and time.
   // global.reg.ver_hash = hash_code_str (software_version);

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

    // Store the cleared data to all the EEPROM copies.
    eeprom_init();
    for (reg_idx = 0; reg_idx < EEPROM_REG_SET_CNT; ++reg_idx)
    {
        eeprom_write (
                // address to store to
                reg_set_adr(reg_idx),
                // The start of the data
                (uint8_t *)&nvm.reg,
                sizeof(Reg_t));
    }
    eeprom_deinit();
}


/***************************************************************************
* Description:
*           Get the best registers, RAM or EEPROM.
*           On exit, saving the registers has to work!
*           So flags and data used by put_reg() have to be set-up.
*           Also, get_reg interacts with the main loop, which prepares this 
*           data.
*           See the calls to "reg_crc" in the main loop.
*           Put the register data into EEPROM. In EFT, put reg can fail.
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
     bool bad_reg = true;    // Assume that no billing register data is good.
//    uint8_t reg_idx;        // index of the set of billing registers.
//    uint8_t my_ver_hash;    // Hash of the firmware's version string.

    // Unique value for this version is made from the date and time.
   // my_ver_hash = hash_code_str (software_version);

    // global.reg is used for testing candidate copies of the billing data, 
    // nvm.reg for best current data. (It is usually the most recent data.)

    // Check registers in NVRAM.
    // Don't reread NVRAM. It has already been read in the main loop.
//    if (    // firmware version mismatch?
//           // (my_ver_hash != nvm.reg.ver_hash)
//            // Is the CRC good?
//           // || 
//		(!crc16 ((uint8_t *) &nvm.reg, sizeof (Reg_t), false))
//       )
    {
     //   extern const struct tm starting_tm; // constant structure in time.c
        // NVRAM is bad.  Put a default register image in it for now.
        // On exit, nvm.reg must have valid data, so saving the registers
        // will work!
     //   memset ((uint8_t*)&nvm.reg,0,sizeof(Reg_t));
    //    nvm.reg.tm = starting_tm;       // If NVM is bad, old time is bad, too.
      //  nvm.reg.log = global.reg.log;   // Log is already init. in main loop.
//
        // Get the best EEPROM registers better than nvm reg.
			
			
        eeprom_init();
      //  for (reg_idx = 0; reg_idx < EEPROM_REG_SET_CNT; ++reg_idx)
        //{
        //    int bad_read;

            // read in an EEPROM value
//            bad_read = eeprom_read ( reg_set_adr(0),
//                    (uint8_t*)&global.reg,    // source
//                    sizeof (Reg_t));

//            // check it for validity.
//            if ( (0 != bad_read) // eeprom read failed
//                    // Firmware version mismatch?
//                    || (global.reg.ver_hash != my_ver_hash)
//                    // CRC bad?
//                    || (!crc16 ((uint8_t *) &global.reg,
//                            sizeof (Reg_t), false))
//               )
//            {
//                bad_read = true; /* something is wrong with it. */
//            } else {
//                if (!bad_reg) /* Already found a valid register set? */
//                {
//                    // The signed arithmetic of this comparison preserves ordering
//                    // when the save count wraps around. e.g. 0xff = -1, so 0xff - 0 = -1.
//                    // This would fail if the register saves were ever more than 127
//                    // counts apart, but they cannot be: The put_reg's repeating
//                    // pattern (below) is only 16 saves long.
//                    // is previous good reg. set later, i.e. more Wh?
//                    if (0 <= (nvm.reg.reg_cnt - global.reg.reg_cnt)) {
//                        bad_read = true;        // don't use this reg. set.
//                    }
//                    // else bad_read = false;   // Already handled above.
//                }
//                // else bad_read = false;    // Already handled above.
//            } // done with tests; bad_read = false for a good register set.

//            if(!bad_read) // good register set?
//            {   // Keep the latest surviving register set in NVM.
//                // This preserves Wh even if the save count wraps around.
               // nvm.reg = global.reg;  /* new reg. is the best! Keep it! */
//                bad_reg = false;
//            }
        //}
        eeprom_deinit();
    } 
//		else {
//        // NVRAM has valid data.  Assume it's the latest.  Keep it.
//        bad_reg = false;
//    }

    // The log data and old time data has to be preserved as well
    // as the billing data.
    // At this point these all should be from a valid copy of reg.
    // Or, they should be the best available data if none existed.

    // Force the version hash to be new.
  //  nvm.reg.ver_hash = my_ver_hash;

    // Update the checksum after updating errors.
    crc16 ((uint8_t *) &nvm.reg, sizeof (Reg_t), true);

    // Make both register copies the same.
    global.reg = nvm.reg;

    // By default save the stable NVRAM register set, nvm.reg
    save_global_reg = false;

    // There is no need to write the data here. There is no new
    // data, and a save here will just use up space. 
    // New data will be written to NVRAM in the main loop 
    // in meter-run() within a second or two. Also it will 
    // be written to EEPROM in the next hour or on the 
    // next power failure.

    // This counts the register saves so that the register-gets
    // always get the most current register set.
    // The reg-get handles overflow in this counter
    // by finding the greatest difference.
    global.misc.save_cnt = nvm.reg.reg_cnt;

    // Set up the CE pulse counters from the saved data.
    ce_data.wpulse_ctr = nvm.reg.wh_cnt;
    ce_data.wsum_accum = 0x40000000;    // Round down.
    ce_data.vpulse_ctr = nvm.reg.varh_cnt;
    ce_data.vsum_accum = 0x40000000;    // Round down.

    // Record errors here.
    log_state ( EV_REG_BAD , bad_reg );

    lock = false; // Safety code.
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
    int tmp;
    Reg_t *reg_ptr;
    uint8_t reg_idx;
    IRQ_DEFINES;  // Prepare to restore interrupt state.

    // prevent reentrant calling of this routine
    IRQ_DISABLE();               // Disable interrupts
    if (lock)
    {
        // If it goes here, then the routine has been called 
        // already, so data has been saved.
        IRQ_ENABLE();     // Restore interrupt state.
        return;
    }
    lock = true;

    // This counts the register saves so that reg-get()
    // always get the most current register set.
    // reg-get() handles overflow in this counter
    // by finding the greatest difference.
    // Make the "read" save count incremented.
    global.misc.save_cnt = global.reg.reg_cnt;

    IRQ_ENABLE();         // Restore the interrupt state.

    // save the data that is OK right now.
    if (save_global_reg)
        reg_ptr = &global.reg;  // NVRAM is being updated.
    else
        reg_ptr = &nvm.reg;     // Save NVRAM data.

    // Save sequence is 0102010301020104, and then it repeats
    // to get diversity in time and space.
    // So a single event in time or space can't
    // corrupt all the register sets.
    // There's also a register set in NVRAM, so it's
    // also saved in multiple devices.
    tmp = reg_ptr->reg_cnt;
    if ((tmp & 0x0F) == 0x0F)
        reg_idx = 4; // register set 4
    else if ((tmp & 0x07) == 0x07)
        reg_idx = 3; // register set 3
    else if ((tmp & 0x03) == 0x03)
        reg_idx = 2; // register set 2
    else if ((tmp & 0x01) == 0x01)
        reg_idx = 1; // register set 1
    else
        reg_idx = 0; // register set 0

    // Write the billing registers to EEPROM.
    // No need to write to NVRAM: Done in meter-run()
    // in meter.c
//    eeprom_init ();
//    eeprom_write (
//            reg_set_adr(reg_idx),   // destination
//            (uint8_t*)reg_ptr,      // source
//            sizeof(Reg_t));         // length
//    eeprom_deinit ();

    lock = false; // Unlock
}


/***************************************************************************
 * Copyright (C) 2011..2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

