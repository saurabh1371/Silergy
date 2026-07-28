/*----------------------------------------------------------------------------
 * Purpose: Flash routines which run from RAM
 * Note(s):
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * $Id: flash.c 17447 2018-04-25 20:28:26Z ptanwar $
 *----------------------------------------------------------------------------*/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "flash.h"      // Flash definitions

const char fw_ver_flash[] =
"flash"   /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

// flash variables.
// Safety code: Contains the unlock code for the flash erase & write routines.
 int flash_passcode;
uint32_t flash_cn; // To keep a copy of flash controller (FLASH->FLASH_CN)
uint32_t flash_prot; // To keep a copy of flash controller (FLASH->FLASH_PROT)
const int retry_cnt = 1; // flash write retry count

// Internal functions.
static void wait_flash_busy (void);


/*****************************************************************************
* Description:
*  Enable and disable flash operations.
* Parameters:
*   Input:
*         None.
*   Output:
*         Flash is enabled or disabled
* Return:
*         None
*****************************************************************************/
void flash_enable(void) { flash_passcode = FLASH_UNLOCK; }
void flash_disable(void) { flash_passcode = 0; }


/*****************************************************************************
* Description:
*  Perform Mass erase of the flash
* Parameters:
*   Input:
*         None.
*   Output:
*         Erase flash array of entire chip
* Return:
*         0 - OK,  1 - Failed
*****************************************************************************/
int flash_chip_erase (void)
{

    flash_cn  = FLASH->CNTL;
    flash_cn &= ~FLASH_MASS_ERASE;
    // Write mass erase code
    flash_cn |= (flash_passcode | FLASH_MASS_ERASE);

    flash_prot = FLASH->PROT;
    flash_prot |= (1<<8); // set to 1 to allow write/erase flash transactions
    FLASH->PROT = flash_prot;

    // Issue mass erase command
    FLASH->CNTL = flash_cn;

    // Wait for erasure to complete
    wait_flash_busy();// loop here till flash controller is busy

    /* Clear Flash control register */
    FLASH->ADDR  = ADD_OUTOFBOUND;
    FLASH->CNTL = 0;
    flash_prot = FLASH->PROT;
    flash_prot &= ~(1<<8); // disable write/erase flash transactions
    FLASH->PROT = flash_prot;

    return (0);   // Finished without Errors
} // flash-chip-erase()


/*****************************************************************************
* Description:
*  Erase Page in Flash Memory
* Parameters:
*   Input:
*         adr:  Page Address.
*   Output:
*         Erase flash array of entire chip
* Return:
*         0 - OK,  1 - Failed
*****************************************************************************/
int flash_page_erase (uint32_t adr)
{

    //adr must be in the 4-byte boundaries, e.g. 0, 4, 8 etc.
    if ((adr % WORD_SZ) != 0)
        return 1;

    flash_cn  = FLASH->CNTL;
    flash_cn &= ~FLASH_PAGE_ERASE;
    // Write page erase code
    flash_cn |= (flash_passcode | FLASH_PAGE_ERASE);

    flash_prot = FLASH->PROT;
    flash_prot |= (1<<8); // set to 1 to allow write/erase flash transactions
    FLASH->PROT = flash_prot;

    // Specify page address
    FLASH->ADDR  = FLASH_ADDR_LONG_WORD(adr);

    // Issue page erase command
    FLASH->CNTL = flash_cn;

    // Wait for erasure to complete
    wait_flash_busy();// loop here till flash controller is busy

    /* Clear Flash control register */
    FLASH->ADDR  = ADD_OUTOFBOUND;
    FLASH->CNTL = 0;
    flash_prot = FLASH->PROT;
    flash_prot &= ~(1<<8); // disable write/erase flash transactions
    FLASH->PROT = flash_prot;
    return (0);       // Finished without Errors
} // flash-page-erase()


/*****************************************************************************
* Description:
*  Program Flash Memory for given buffer and address
* Parameters:
*   Input:
*         adr:  Start Address
*         buf:  Write buffer (pattern)
*          sz:  Size of write buffer
*    passcode:  Magic number to enable operation.
*   Output:
*         Erase flash array of entire chip
* Return:
*         0 - OK,  1 - Failed
*****************************************************************************/
int flash_write(uint32_t adr, uint32_t *buf, int sz)
{
    int retries = retry_cnt;
    uint32_t *data = (uint32_t *)adr;

    //adr must be in the 4-byte boundaries, e.g. 0, 4, 8 etc.
    if ((adr % WORD_SZ) != 0)
        return 1;
    if (sz >= WORD_SZ)
    {    //Must be writing in long Word
        // write in 32-bit units while we can
        flash_cn  = FLASH->CNTL;
        flash_cn &= ~FLASH_WR;
        flash_cn |= (flash_passcode | FLASH_WR);

        flash_prot = FLASH->PROT;
        // set to 1 to allow write/erase flash transactions
        flash_prot |= (1<<8);
        FLASH->PROT = flash_prot;

        while (sz >= WORD_SZ)
        {
            while(*data != *buf && retries-- > 0)
            {
                FLASH->ADDR  = FLASH_ADDR_LONG_WORD(adr);
                FLASH->DATA = *buf;
                flash_cn |= (flash_passcode | FLASH_WR);
                FLASH->CNTL = flash_cn;

                wait_flash_busy();// loop here till flash controller is busy
            }

            /* Wait until flash operation is complete */

            buf++;
            data++;
            adr += WORD_SZ;
            sz  -= WORD_SZ;
            retries = retry_cnt;
        }

        flash_cn  = FLASH->CNTL;
        flash_cn &= ~(flash_passcode | FLASH_WR); // clear 'FLO', disable flash op.
        FLASH->CNTL = flash_cn;

        flash_prot = FLASH->PROT;
        flash_prot &= ~(1<<8); // disable write/erase flash transactions
        FLASH->PROT = flash_prot;

    }
    return (0);        // Finished without Errors
} // flash-write()


// Wait for flash operation to complete.
static void wait_flash_busy(void)
{
    while( FLASH_BUSY_BAR != (FLASH->CNTL & FLASH_BUSY_BAR))
    {
    } // loop here till flash controller is busy
} // wait-flash-busy()


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

