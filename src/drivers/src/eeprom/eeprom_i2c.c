/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: EEPROM I2C device driver. EEPROM AT24C256C :
 *      http://www.atmel.com/images/Atmel-8568-SEEPROM-AT24C256C-Datasheet.pdf 
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: eeprom_i2c.c 24374 2020-04-13 17:41:11Z ptanwar $
 ****************************************************************************/
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Booleans.
#include "options.h"    // PCB & System definitions, device, pins, etc.
#include "delay.h"      // Loop delay
#include "eeprom_common.h"     // Common EEPROM definitions.
#include "eeprom.h"	    // EEPROM I2C function declarations 
#include "gpio.h"	    // GPIO function declarations 
#include "i2c_master.h" // I2C driver
 #include "Defines.h"
 
const char fw_ver_eeprom[] =
"eeprom_i2c"    /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

/*****************************************************************************
* Description:  
*        Initialize I2C master interface.
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
	i2c_master_init();
}

/*****************************************************************************
* Description:  
*        De-initialize I2C master interface, saves power, maybe.
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
	i2c_master_deinit();
}

/***************************************************************************   
* Description:  
*        Write data bytes to a EEPROM page.
* Parameters:
*   Input:
*       dst : consists of EEPROM device address(8-bit) 
*           + data word address (16-bit)
*       const uint8_t *pchOut : pointer to RAM data byte array.
*       int cnt     : number of data bytes to write within EEPROM page.
*   Output: 
*       None
* Return Status: 
*       bool 0 for success 1 for failure
****************************************************************************/
static bool EPWritePage(int addr, uint8_t *wr_buff, int size)
{
    int index;
    uint8_t data;
	
    // send START command
    if(i2c_master_start())
    {
        i2c_master_stop();
        return 1;
    }

    // Send EEPROM Device Address (WR)
    i2c_master_tx_byte(I2C_EE_WR_ADDR  | (0x02 & (addr >> 15)));

    // Send EEPROM Memory address (2 bytes)
    data = (uint8_t)(addr >> 8 & 0xFF);
    if(i2c_master_tx_byte(data))
    {
        i2c_master_stop();
        return 1;
    }
    data = (uint8_t)(addr & 0xFF);
    if(i2c_master_tx_byte(data))
    {
        i2c_master_stop();
        return 1;
    }

    // Send data byte(s) to be written
    for(index = 0; index < size; index++)
    {
        data = wr_buff[index];
        if(i2c_master_tx_byte(data))
        {
            i2c_master_stop();
            return 1;
        }
    }

    // Send stop command
    i2c_master_stop();

    delay(I2C_EE_WAIT); 
    delay(DELAY_MS(EEPROM_PAGE_WR_DELAY));

    return 0; // no error
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
*         0 = OK
*****************************************************************************/
int eeprom_write(int addr, uint8_t *wr_buff, int size)
{
	int wr_len;
	int wr_len_left; //number of bytes left in the current page
	
	// Address is beyond the EEPROM size
	if(addr >= EEPROM_SIZE)
	{
		return 1;
	}
	
	// write until we are finished writing complete buffer
	while(size > 0)
	{
		if(0 == (addr % EEPROM_PAGE_SIZE ))
		{ // If addr aligned with page begin
			// and bytes to be written are less than or equal to page size
			if(size <= EEPROM_PAGE_SIZE)
			{ // write all remaining bytes
				wr_len = size;
			}
			else
			{ // write a page
				wr_len = EEPROM_PAGE_SIZE;
			}
		}
		else
		{ 
      // get partial page length
			wr_len = size % EEPROM_PAGE_SIZE;
			// get number of bytes can be written in the current page
			wr_len_left = (((addr/EEPROM_PAGE_SIZE)+1)*EEPROM_PAGE_SIZE) - addr;
			// check if partial page length can fit in current page 
			if(wr_len_left <= wr_len || 0 == wr_len)
			{
				wr_len = wr_len_left;
			}
		}
		// Write bytes of size of the page (or smaller than page) 
		if(EPWritePage(addr,wr_buff,wr_len))
            return 1;
		addr += wr_len;
		wr_buff += wr_len;
		size -= wr_len;
	} // while ((size != 0))
  return 0;
}

/***************************************************************************   
* Description:  
*       Erase an EEPROM page. (This EEPROM has to be explicitly written to
*       0xff)
* Parameters:
*   Input:
*       dst : consists of EEPROM device address(8-bit) 
*           + data word address (16-bit)
*   Output: 
*       A page is erased.
* Return Status: 
*       bool 0 for success 1 for failure
****************************************************************************/
bool EPClearPage(int addr) //static
{
    int index;
    uint8_t data;

    // send START command
    if(i2c_master_start())
    {
        i2c_master_stop();
        return 1;
    }


    // Send EEPROM Device Address (WR)
    if(i2c_master_tx_byte(I2C_EE_WR_ADDR  | (0x02 & (addr >> 15))))
    {
        i2c_master_stop();
        return 1;
    }

    // Send EEPROM Memory address (2 bytes)
    data = (uint8_t)(addr >> 8 & 0xFF);
    if(i2c_master_tx_byte(data))
    {
        i2c_master_stop();
        return 1;
    }
    data = (uint8_t)(addr & 0xFF);
    if(i2c_master_tx_byte(data))
    {
        i2c_master_stop();
        return 1;
    }

    // Send data byte(s) to be written
    for(index = 0; index < EEPROM_PAGE_SIZE; index++)
    {
        data = 0x00; //0xFF; // erase byte by writing 0xFF
        if(i2c_master_tx_byte(data))
        {
            i2c_master_stop();
            return 1;
        }
    }

    // Send stop command
    i2c_master_stop();

    delay(I2C_EE_WAIT); 
    delay(DELAY_MS(EEPROM_PAGE_WR_DELAY));

    return 0; // no error
}

/***************************************************************************   
* Description:  
*       Erase an EEPROM. (This EEPROM has to be explicitly written to
*       0xff)
* Parameters:
*   Input:
*       None.
*   Output: 
*       The EEPROM is erased.
* Return Status: 
				bool 0 for success 1 for failure
****************************************************************************/
int eeprom_erase (unsigned char clr_type)
{
    uint32_t len = EEPROM_SIZE;
    uint32_t dst = 0;

		if(clr_type!=0)
			dst=dst+EEPROM_PAGE_SIZE;
	
    // write pages as quickly as possible
    while (len > 0)
    {
        // write pages
        if(EPClearPage(dst))
            return 1; // return the error
        dst += EEPROM_PAGE_SIZE;
        len -= EEPROM_PAGE_SIZE;
				scratch=dst;
				scratch1=len;
    }
		
//write default calibration constants

		
		
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
*         0 = OK
*****************************************************************************/
int eeprom_read (int addr, uint8_t *rd_buff, int size)
{
    int index = 0;
    uint8_t data;

		// Address is beyond the EEPROM size
		if(addr >= EEPROM_SIZE)
		{
			return 1;
		}

    // send START command
    if(i2c_master_start())
    {
        i2c_master_stop();
        return 1;
    }

    // Send EEPROM Device Address (WR)
    if(i2c_master_tx_byte(I2C_EE_WR_ADDR | (0x02 & (addr >> 15))))
    {
        i2c_master_stop();
        return 1;
    }

    // Send EEPROM Memory address (2 bytes)
    data = (uint8_t)(addr >> 8 & 0xFF);
    if(i2c_master_tx_byte(data))
    {
        i2c_master_stop();
        return 1;
    }
    data = (uint8_t)(addr & 0xFF);
    if(i2c_master_tx_byte(data))
    {
        i2c_master_stop();
        return 1;
    }

    // send (re)START command
    if(i2c_master_start())
    {
        i2c_master_stop();
        return 1;
    }

    // Send EEPROM Device Address (RD)
    if(i2c_master_tx_byte(I2C_EE_RD_ADDR | (0x02 & (addr >> 15))))
    {
        i2c_master_stop();
        return 1;
    }

    // Command to read bytes (but not the last byte) with ACK
    for(index = 0; index < size - 1; index++)
    {	
        rd_buff[index] = i2c_master_rx_byte(true);
    }
    // Command to read the last byte with NACK
    rd_buff[index] = i2c_master_rx_byte(false);

    // Send stop command
    i2c_master_stop();

    return 0;
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 
 
