/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: SPI header.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: spi.h 23252 2020-02-17 03:05:43Z rvandewa $
 ****************************************************************************/
#ifndef SPI_H
#define SPI_H

// Version string.
extern const char fw_ver_spi[];

// Initialize SPI master interface
void spi_init(void);

// De-initialize SPI master interface
void spi_deinit(void);

// SPI data Write
void spi_write(uint8_t *wr_buff, uint16_t size);

// SPI data Write, followed by HiZ
void spi_wr_byte_hiz(uint8_t wr_byte);

// SPI data Write
void spi_wr_byte(uint8_t wr_byte);

// SPI Data Read
void spi_read(uint8_t *rd_buff, uint16_t size);

// SPI Read byte
uint8_t spi_read_byte(void);

// SPI busy wait
void spi1_busy_wait(void);

#endif // SPI_H

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
