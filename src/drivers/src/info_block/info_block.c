/***************************************************************************
 * DESCRIPTION: POWER METER - Access Flash InfoBlock (Trim).
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * $Id: info_block.c 22661 2020-01-08 19:26:08Z ptanwar $ 
 ***************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "irq.h"        // Interrupt management.
#include "bitdef.h"
#include "flash.h"
#include "info_block.h" // Check the definitions.

// Firmware Device Driver version
const char fw_ver_info_block[] =
"info_block" /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

static void wait_info_block_busy(void);

/* Read from Flash Info Block */
int info_block_read(int index)
{
    int tmp;
    int addr = index;
	  unsigned int flash_prot;
    IRQ_DEFINES;

    IRQ_DISABLE();
    // Info block shadow address starts at INFO_BLK_OFFSET in flash
    index = index | INFO_BLK_OFFSET;
    addr = FLASH_INFO_BLOCK | index;
	
    flash_prot = FLASH->PROT;
    flash_prot |= (1<<8); // set to 1 to allow write/erase flash transactions
    FLASH->PROT = flash_prot;
	
    FLASH->ADDR = addr;
    FLASH->CNTL = (FLASH_UNLOCK | FLASH_INFBLK_RD);
    tmp = FLASH->DATA;
	
    /* Clear Flash control register */
    FLASH->ADDR  = ADD_OUTOFBOUND;
    FLASH->CNTL = 0;
    flash_prot = FLASH->PROT;
    flash_prot &= ~(1<<8); // disable write/erase flash transactions
    FLASH->PROT = flash_prot;	

    IRQ_ENABLE();

    return tmp;
} // info-block-read()


/* Write to Flash Info Block */
int info_block_write(int index, int value)
{
    int addr = index;
    unsigned int flash_prot;
    IRQ_DEFINES;

    IRQ_DISABLE();
    // Info block shadow address starts at INFO_BLK_OFFSET in flash
    index = index | INFO_BLK_OFFSET;
    addr = FLASH_INFO_BLOCK | index;

    flash_prot = FLASH->PROT;
    flash_prot |= (1<<8); // set to 1 to allow write/erase flash transactions
    FLASH->PROT = flash_prot;


    FLASH->ADDR = addr;
    FLASH->DATA = value;
    FLASH->CNTL = (FLASH_UNLOCK | FLASH_IBBLK_WR);
    wait_info_block_busy();

    /* Clear Flash control register */
    FLASH->ADDR  = ADD_OUTOFBOUND;
    FLASH->CNTL = 0;
    flash_prot = FLASH->PROT;
    flash_prot &= ~(1<<8); // disable write/erase flash transactions
    FLASH->PROT = flash_prot;

    IRQ_ENABLE();

    return 0;
} // info-block-write()

/* Read from Flash Info Block */
int cmd_fuse_refresh(char cmd)
{
	if ('R' == cmd) // start a fuse refresh
	{
		// Command the SoC to reread fuse registers from the infoblock.
		FLASH->CNTL = (FLASH_UNLOCK | FLASH_FUSE_REFRESH); 
	}
	
	return 0;
}//cmd_fuse_refresh()


// Wait for flash operation to complete.
static void wait_info_block_busy(void)
{
    while( FLASH_BUSY_BAR != (FLASH->CNTL & FLASH_BUSY_BAR))
    {
    } // loop here till flash controller is busy
} // wait-flash-busy()

/***************************************************************************
 * 2017 August 17; First Version. 
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

