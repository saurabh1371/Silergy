/***************************************************************************
 * DESCRIPTION: POWER METER - Access Flash InfoBlock (Trim).
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * $Id: info_block.h 23351 2020-02-22 00:23:33Z rvandewa $ 
 ***************************************************************************/
#ifndef INFO_BLOCK_H
#define INFO_BLOCK_H 1

// Firmware Device Driver version
extern const char fw_ver_info_block[]; 

#define CHIPID_INDEX 1

// Info block shadow address starts at INFO_BLK_OFFSET in flash
#define INFO_BLK_OFFSET 0x40 

// Access Info Block, flash address (FLASH->ADDR) BIT31 must be set
#define FLASH_INFO_BLOCK BIT31 

/* Read from Flash Info Block */
int info_block_read(int index);

/* Write to Flash Info Block */
int info_block_write(int index, int value);

/* Fuse refresh control */
int cmd_fuse_refresh(char cmd);
	
/* Refresh trim ISR */
void refresh_trim_isr(void);

/***************************************************************************
 * 2005 August 17; First Version. 
 * Copyright (C) 2005 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // INFO_BLOCK_H

