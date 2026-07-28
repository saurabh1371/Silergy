/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: EEPROM SPI device driver. EEPROM AT25080A :
 *        http://ww1.microchip.com/downloads/en/DeviceDoc/doc3347.pdf
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: eeprom_spi.c 24630 2020-04-27 17:39:51Z ptanwar $
 ****************************************************************************/
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Booleans.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "delay.h"
#include "spi.h"        // SPI master
#include "eeprom.h"	    // EEPROM function declarations 
#include "gpio.h"		// GPIO function declarations 
 
// SPI Chip Select
#define SPI_CS(val) (LCD->DIO_b[spi_cs_seg].data = !(val)) 

// SPI EEPROM definitons for 
// EEPROM IC: http://ww1.microchip.com/downloads/en/DeviceDoc/doc3347.pdf
// COMMANDS DESCRIPTION
#define SPI_EE_WREN 						0x06  // Write Enable
#define SPI_EE_WRDI 						0x04  // Write Disable
#define SPI_EE_RDSR        			0x05  // Read Status Register
#define SPI_EE_WRSR 						0x01  // Write Status Register
#define SPI_EE_RD 				    	0x03  // Read Data Bytes
#define SPI_EE_WR						    0x02  // Write Data Bytes
// Read Status Register 
// Bit 1= “0” indicates the device is not WRITE ENABLED. 
// Bit 1 = “1” indicates the device is write enabled.
#define SPI_EE_SR_WEN						0x02   

const char fw_ver_eeprom[] =
"eeprom_spi"    /* Device Driver name */
" v"      		/* ver string */
"1.01"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

static bool eeprom_spi_wr_en(void);

/*****************************************************************************
* Description:  
*        Initialize EEPROM SPI master interface.
* Parameters:
*   Input:
*         None. 
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
void eeprom_init(void)
{
    SPI_CS(false);                  // Initialize with Chip Deselected
    gpio_dir_out(spi_cs_seg);       // CS , output
    spi_init();
	  eeprom_spi_wr_en();
}


/*****************************************************************************
* Description:  
*        Disable the SPI EEPROM and place it in a low power state.
* Parameters:
*   Input:
*         None. 
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
void eeprom_deinit(void)
{
	SPI_CS(false);              // Deselect with Chip Select as false
	gpio_dir_in(spi_cs_seg);    // CS, high impedance
  spi_deinit();
}


/*****************************************************************************
* Description:  
*        write enable
* Parameters:
*   Input:
*         addr: EEPROM Memory address (2 bytes)
*         wr_buff: pointer to data to send
*		  size: number of bytes to send
*   Output: 
*         None.
* 	Return Status: 
*         None
*****************************************************************************/
static bool eeprom_spi_wr_en(void)
{
	bool wr_enable = false;
	volatile uint8_t rx_data;
	
	SPI_CS(true);
  spi_wr_byte(SPI_EE_WREN); // WR Enanble command
	SPI_CS(false);	
	
	delay(0x50);
	
	SPI_CS(true);
	spi_wr_byte(SPI_EE_RDSR); // RD status register
	rx_data = spi_read_byte();
	SPI_CS(false);
	
	if((0xFF != rx_data) 
      && (SPI_EE_SR_WEN == (rx_data & SPI_EE_SR_WEN))
    )// Write is enabled in the SPI
	{
		wr_enable = true;
	}	
	
	return wr_enable;
}
	

/*****************************************************************************
* Description:  
*        Sends command and data to the slave address on the I2C bus.
* Parameters:
*   Input:
*         addr: EEPROM Memory address (2 bytes)
*         wr_buff: pointer to data to send
*		  size: number of bytes to send
*   Output: 
*         None.
* 	Return Status: 
*         0 = success
*****************************************************************************/
int eeprom_write(int addr, uint8_t *wr_buff, int size)
{
	uint32_t index;
	SPI_CS(true);
	spi_wr_byte(SPI_EE_WR); // CMD type: Write data

	spi_wr_byte((uint8_t)(addr >> 8 & 0xFF)); // Write addr MSB

	spi_wr_byte((uint8_t)(addr & 0xFF)); // Write addr LSB
	
	// Send data byte(s) to be written
	for(index = 0; index < size; index++)
	{
     spi_wr_byte(wr_buff[index]); // Write addr LSB
	}

	SPI_CS(false);
  return 0;
}


/*****************************************************************************
* Description:  
*        Sends command and reads data to the slave address on the I2C bus.
* Parameters:
*   Input:
*         addr: EEPROM Memory address (2 bytes)
*         wr_buff: pointer to buffer to save read bytes
*		  size: number of bytes to read
*   Output: 
*         None.
* 	Return Status: 
*         0 = ok
*****************************************************************************/
int eeprom_read (int addr, uint8_t *rd_buff, int size)
{
	uint32_t index;
	
	SPI_CS(true);
	
	spi_wr_byte(SPI_EE_RD); // CMD type: Read data
	spi_wr_byte((uint8_t)(addr >> 8 & 0xFF)); // Write addr MSB
	spi_wr_byte((uint8_t)(addr & 0xFF)); // Write addr LSB
	
	// Command to read byte(s) from SPI EEPROM
	for(index = 0; index < size; index++)
	{	
    rd_buff[index] = spi_read_byte();
	}
	SPI_CS(false);

  return 0;
}

// Provided by spi.c
/*****************************************************************************
* Description:  
*        EEPROM erase
* Parameters:
*   Input:
*         None. 
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
int eeprom_erase(void)
{
	// All programming cycles are completely selftimed, 
	// and no separate erase cycle is required before write.
	// http://ww1.microchip.com/downloads/en/DeviceDoc/doc3347.pdf
	return 0; 
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 
