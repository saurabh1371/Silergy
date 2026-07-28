/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: SPI Flash driver. GigaDevice- Uniform Sector Standard
 *              and Dual Serial Flash GD25WD40C/20C
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: sflash.c 29 2017-11-15 19:47:16Z rpoonia $
 ****************************************************************************/
#include <stdio.h>        /* prototype declarations for I/O functions  */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "SY7_device.h"
#include "delay.h"
#include "spi.h"
#include "sflash.h"

// Port to SPI0, just called "SPI"
//#define spi_init spi0_init
//#define spi_deinit spi0_deinit
//#define spi_write spi0_write
//#define spi_write_byte_hiz spi0_write_byte_hiz
//#define spi_write_byte spi0_write_byte
//#define spi_read spi0_read
//#define spi_read_byte spi0_read_byte
//#define spi_cs_seg spi0_cs_seg

#ifndef SFLASH_CS_PCB // Does the PCB have a different CS?
#define SFLASH_CS_PCB spi_cs_seg
#endif
#define CS_PIN        LCD->DIO_b[SFLASH_CS_PCB]
#define CS_DIO        (LCD->DIO_b[SFLASH_CS_PCB].alt = 0)
#define CS_INPUT      (LCD->DIO_b[SFLASH_CS_PCB].dir = 0)
#define CS_OUTPUT     (LCD->DIO_b[SFLASH_CS_PCB].dir = 1)
#define CS_HIGH       (LCD->DIO_b[SFLASH_CS_PCB].data = 1)
#define CS_LOW        (LCD->DIO_b[SFLASH_CS_PCB].data = 0)
#define SELECT        CS_LOW
#define DESELECT      CS_HIGH

// STATUS REGISTER
#define SRP     0x80
#define BP_MASK 0x1C
#define BP2     0x10
#define BP1     0x08
#define BP0     0x04
#define WEL     0x02
#define WIP     0x01

// COMMANDS DESCRIPTION
#define SFLASH_WREN             0x06  // Write Enable
#define SFLASH_WRDI             0x04  // Write Disable
#define SFLASH_RDSR             0x05  // Read Status Register
#define SFLASH_WRSR             0x01  // Write Status Register
#define SFLASH_READ             0x03  // Read Data Bytes
#define SFLASH_FAST_READ        0x0B
#define SFLASH_DO_FAST_READ     0x3B
#define SFLASH_PP               0x02  // PAGE_PROGRAM
#define SFLASH_SECTOR_ERASE     0x20
#define SFLASH_BLOCK_ERASE_32K  0x52
#define SFLASH_BLOCK_ERASE_64K  0xD8
#define SFLASH_CHIP_ERASE       0xC7  // CHIP_ERASE
#define SFLASH_DEEP_POWERDOWN   0xB9
#define SFLASH_REL_DEEPPOWER    0xAB
#define SFLASH_REL_DEEPPOWER_ID 0xAB
#define SFLASH_READ_DEVICE_ID   0xAB
#define SFLASH_MANUFACTURER_ID  0x90
#define SFLASH_RDID             0x9F  // Read Identification

#define SFLASH_PAGE_SIZE        256

/*  Gigadevice - Uniform Sector Standard and
    Dual Serial Flash GD25WD40C/20C

    Timing and parameters:

    Clock Frequency - 40MHz for standard speed,
                      50Mhz fast read with 30PF load.

        - 256 bytes per programmable page.
        - Page Program time: 1.6ms typical
        - Sector(4Kb) Erase time: 150ms typical
        - Block(32/64KB) Erase time: 0.5/0.8s typical
        - Chip Erase(256/512KB) time: 5/2.5s typical
*/

const char fw_ver_sflash[] =
"sflash"    /* Device Driver name */
" v"        /* ver string */
"1.00"      /* Major_ver.Minor_ver */
"\r\n"      /* CR + LF */
;


/*****************************************************************************
* Description:
*        Initialize SPI-Flash SPI comm. interface.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
*****************************************************************************/
void sflash_init(void)
{
    DESELECT; // Output 1
    CS_OUTPUT; // Direction is out
    CS_DIO; // Make it a DIO
    spi_init(); // Initialize the master SPI.
}


/*****************************************************************************
* Description:
*        De-initialize SPI-Flash SPI comm. interface.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
*****************************************************************************/
void sflash_deinit(void)
{
    spi_deinit();
    DESELECT; // Output 1
    CS_INPUT; // High impedance
}


/*****************************************************************************
* Description:
*   SPI-Flash Write Enable (WREN) (06H)
*   The Write Enable (WREN) command sets Write Enable Latch (WEL) bit to 1.
*   WEL bit must be set prior to Page Program (PP), Sector Erase (SE),
*   Block Erase (BE), Chip Erase (CE) and Write Status Register (WRSR) command.
*
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
*****************************************************************************/
void sflash_write_enable(void)
{
    SELECT;
    spi_wr_byte_hiz(SFLASH_WREN);
    DESELECT;
}


/*****************************************************************************
* Description:
*    SPI-Flash Write Disable (WRDI) (04H)
*    The Write Disable command resets the Write Enable Latch (WEL) bit to 0.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
*****************************************************************************/
void sflash_write_disable(void)
{
    SELECT;
    spi_wr_byte_hiz(SFLASH_WRDI);
    DESELECT;
}


/*****************************************************************************
* Description:
*    The Read Status Register (RDSR) command
*    The Status Register may be read at any time, even while a Program, Erase
*    or Write Status Register cycle is in progress. When one of these cycles is
*    in progress, it is recommended to check the Write In Progress (WIP) bit before
*    sending a new command to the device.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         Returns command status
*****************************************************************************/
uint8_t sflash_read_status(void)
{
    uint8_t status;

    SELECT;
    spi_wr_byte(SFLASH_RDSR);
    status = spi_read_byte();
    DESELECT;

    return status;
}


/*****************************************************************************
* Description:
*    The Write Status Register (WRSR) instruction allows new values to be written
*    to the Status Register.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         Returns command status
*****************************************************************************/
void sflash_write_status(void)
{
    SELECT;
    spi_wr_byte_hiz(SFLASH_WRSR);
    DESELECT;
}


/*****************************************************************************
* Description:
*   Read SPI flash byte to buffer.
*   The Read Data Bytes (READ) command is followed by a 3-byte address (A23-A0),
*   Then the memory content, at that address, is shifted out on SO,
*   Max frequency fR, 40 MHz
*
* Parameters:
*   Input:
*         addr : SPI flash address to read from.
*         size : num of bytes to read.
*       wr_buff: buffer to fill from read data.
*   Output:
*         None.
* Return Status:
*         Returns command status
*****************************************************************************/
void sflash_read_data(uint32_t addr, uint32_t size, uint8_t *rd_buff)
{
    SELECT;
    spi_wr_byte(SFLASH_READ); // READ Command
    spi_wr_byte(addr >> 16);     // Address
    spi_wr_byte(addr >> 8);
    spi_wr_byte_hiz(addr);
    spi_read(rd_buff,size);      // Read multiple bytes
    DESELECT;
}


/*****************************************************************************
* Description:
*   Program SPI_flash page(256 bytes) memory.
*   A Write Enable (WREN) command must previously have been executed to set
*   the Write Enable Latch (WEL) bit before sending the Page Program command.
*
*   CS# goes low --> sending Page Program command --> 3-byte address on SI
*   --> at least 1 byte data on SI --> CS# goes high
*
* Parameters:
*   Input:
*         addr : SPI flash page address to write
*         size : num of bytes to write in page.
*       wr_buff: buffer that contains the page write data
*   Output:
*         None.
* Return Status:
*         Returns command status
*****************************************************************************/
void sflash_program_page(uint32_t addr, uint32_t size, uint8_t *wr_buff)
{
    sflash_busy_wait();  // Wait for Write or Erase to complete

    sflash_write_enable(); // Enable Write

    if(size > SFLASH_PAGE_SIZE) // Limit Write bytes to page size
        size = SFLASH_PAGE_SIZE;

    SELECT;
    spi_wr_byte(SFLASH_PP);   // Page Program Command
    spi_wr_byte(addr >> 16);     // Address
    spi_wr_byte(addr >> 8);
    spi_wr_byte(addr);
    spi_write(wr_buff,size);  // Read multiple bytes
    DESELECT;

    sflash_busy_wait();  // Wait for page write to complete
}


/*****************************************************************************
* Description:
*   The Chip Erase (CE) command is for erasing the all data of the chip.
*   A Write Enable (WREN) command must previously have been executed to set
*   the Write Enable Latch (WEL) bit

*  Chip Erase(256/512KB) time: 5/2.5s typical

* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         Returns command status
*****************************************************************************/
void sflash_erase_chip(void)
{
    sflash_busy_wait();  // Wait for pending write/erase to complete

    sflash_write_enable(); // Write Enable Latch (WEL) is set 1, write enable

    SELECT;
    spi_wr_byte_hiz(SFLASH_CHIP_ERASE);  // Chip Erase command
    DESELECT;

    sflash_busy_wait();  // Wait for chip erase to complete
}


/*****************************************************************************
* Description:
*   The Sector Erase (SE) command is for erasing the all data of the sector.
*   A Write Enable (WREN) command must previously have been executed to set
*   the Write Enable Latch (WEL) bit

*  Sector Erase(4KB) time: 150msec typical, max 500 ms

* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         Returns command status
*****************************************************************************/
void sflash_sector_chip(uint32_t addr)
{
    sflash_busy_wait();  // Wait for pending write/erase to complete

    sflash_write_enable(); // Write Enable Latch (WEL) is set 1, write enable

    SELECT;
    spi_wr_byte(SFLASH_SECTOR_ERASE);  // Sector Erase command
    spi_wr_byte(addr >> 16);     // Address
    spi_wr_byte(addr >> 8);
    spi_wr_byte_hiz(addr);
    DESELECT;

    sflash_busy_wait();  // Wait for chip erase to complete
}


/*****************************************************************************
* Description:
*   Wait while SPI Flash is busy writing or erasing.

* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         Returns command status
*****************************************************************************/
void sflash_busy_wait(void)
{
    uint8_t status;

    SELECT;
    spi_wr_byte(SFLASH_RDSR); // Read Status register Command
    do
    {
        status = spi_read_byte();
    } while(status & WIP); // Loop while WIP bit is set.
    DESELECT;
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
