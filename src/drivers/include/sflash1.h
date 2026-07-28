/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Gigabyte flash driver header.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: ar1000.h 29 2017-11-15 19:47:16Z rpoonia $
 ****************************************************************************/

#ifndef SFLASH1_H
#define SFLASH1_H

// Initialize SPI-Flash SPI comm. interface.
void sflash1_init(void);

// De-initialize SPI-Flash SPI comm. interface.
void sflash1_deinit(void);

// Write Enable 
void sflash1_write_enable(void);

// Write Disable 
void sflash1_write_disable(void);

// The Read Status Register (RDSR) command
uint8_t sflash1_read_status(void);

// Write to the Status Register.
void sflash1_write_status(void);

// Read SPI Flash Identification (ID)
uint32_t sflash1_read_id(void);

// Read SPI flash byte to buffer.
void sflash1_read_data(uint32_t addr, uint32_t size, uint8_t *rd_buff);

// Program SPI_flash page(256 bytes) memory. 
void sflash1_program_page(uint32_t addr, uint32_t size, uint8_t *wr_buff);

// Chip Erase
void sflash1_erase_chip(void); 

// Sector Erase
void sflash1_sector_chip(uint32_t addr);

// Wait while SPI Flash is busy 
void sflash1_busy_wait(void);

#endif //SFLASH1_H

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
