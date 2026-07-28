/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: SPI Master
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: spi.c 23249 2020-02-17 03:04:10Z rvandewa $
 ****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Needed by rtc.h
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "gpio.h"		// GPIO function declarations 
#include "spi.h"	    // EEPROM SPI Dual-Timer function declarations 
 
// SPI Chip Select; Should be controlled by the higher-level driver.
// Why? A: The polarity differs in different ICs.
// B: There can be several ICs on one SPI, with different chip selects.
 
const char fw_ver_spi[] =
"spi"    /* Device Driver name */
" v"     /* ver string */
"1.01"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

static void spi_busy_wait(void);

/*****************************************************************************
* Description:  
*        Initialize SPI master interface.
* Parameters:
*   Input:
*         None. 
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
void spi_init(void)
{
    SSI->EEX_b.dio_eex = 0x03;  //Set 3 wire (SPI_SCL, SPI_MOSI, SPI_MISO)
    gpio_alt_on(spi_mosi_seg);  // alt=spi_mosi
    gpio_alt_on(spi_scl_seg);	// alt=spi_scl 
    gpio_alt_on(spi_miso_seg);	// alt=spi_miso  
    SSI->INT_b.ssi_ie = 0;		// Disable interrupt
    SSI->INT_b.ssi_i = 0;		// clear interrupt
}


/*****************************************************************************
* Description:  
*        De-initialize SPI master interface.
* Parameters:
*   Input:
*         None. 
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
void spi_deinit(void)
{
    SSI->EEX_b.dio_eex = 0x00;  // Clear 3 wire I/f
    gpio_dir_in(spi_cs_seg);    // CS, high impedance
    gpio_dir_in(spi_mosi_seg);	// alt=spi_mosi, high impedance
    gpio_dir_in(spi_scl_seg);	// alt=spi_scl, high impedance
    gpio_dir_in(spi_miso_seg);	// alt=spi_miso, high impedance
    SSI->INT_b.ssi_ie = 0;		// Disable interrupt
    SSI->INT_b.ssi_i = 0;		// clear interrupt
}


/*****************************************************************************
* Description:  
*        SPI data Write.
* Parameters:
*   Input:
*         wr_buff: pointer to data to send
*		  size: number of bytes to send
*   Output: 
*         None.
* 	Return Status: 
*         None
*****************************************************************************/
void spi_write (uint8_t *wr_buff, uint16_t size)
{
    int i;
    spi_busy_wait();

    // Send data byte(s) to be written to SPI device
    for(i = 0; i < size; i++)
    {
        SSI->DATA = wr_buff[i];
        SSI->MSPI_CNTL = 0x8; // 8 bits to be written;
        spi_busy_wait();
    }
}


/*****************************************************************************
* Description:  
*        Write a SPI data Write
* Parameters:
*   Input:
*         wr_byte: data to write
*   Output: 
*         None.
* 	Return Status: 
*         None
*****************************************************************************/
void spi_wr_byte(uint8_t wr_byte)
{
    spi_busy_wait();
    SSI->DATA = wr_byte;
    SSI->MSPI_CNTL = 0x8; // 8 bits to be written;
    spi_busy_wait();
}


/*****************************************************************************
* Description:  
*        Write last SPI data Write followed by data HiZ (high impedance)
* Parameters:
*   Input:
*         wr_byte: data to write
*   Output: 
*         None.
* 	Return Status: 
*         None
*****************************************************************************/
void spi_wr_byte_hiz(uint8_t wr_byte)
{
    spi_busy_wait();
    SSI->DATA = wr_byte;
    SSI->MSPI_CNTL = 0x28; //Write the last 8 bits with data Hi-Z
    spi_busy_wait();
}

/*****************************************************************************
* Description:  
*        SPI Data read
* Parameters:
*   Input:
*         addr: EEPROM Memory address (2 bytes)
*         wr_buff: pointer to buffer to save read bytes
*		  size: number of bytes to read
*   Output: 
*         None.
* 	Return Status: 
*         None
*****************************************************************************/
void spi_read (uint8_t *rd_buff, uint16_t size)
{
    int i;

    spi_busy_wait(); 

    // Command to read byte(s) from SPI device
    for(i=0; i < size; i++)
    {	
        SSI->MSPI_CNTL = 0x8; // 8 bits to be read
        spi_busy_wait();
        rd_buff[i] = (uint8_t)SSI->DATA;
    }
}


/*****************************************************************************
* Description:  
*        SPI Data byte read 
* Parameters:
*   Input:
*       None
*   Output: 
*         Byte.
* 	Return Status: 
*         None
*****************************************************************************/
uint8_t spi_read_byte(void)
{
    uint8_t rd_data;
    spi_busy_wait(); 

    // Command to read byte(s) from SPI device
    SSI->MSPI_CNTL = 0x18; // 8 bits to be read
    spi_busy_wait();
    rd_data = SSI->DATA;
    return rd_data; 
}


/*****************************************************************************
* Description:  
*        Wait in while loop until I2C bus is busy
* Parameters:
*   Input:
*         None. 
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
void spi_busy_wait(void)
{
    while(SSI->MSPI_CNTL_b.busy)
    {
    }
    return; 
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 
 
