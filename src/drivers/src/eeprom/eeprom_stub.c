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
 * $Id: eeprom_i2c.c 17490 2018-04-27 19:40:40Z rvandewa $
 ****************************************************************************/
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Booleans.
#include <string.h>     // memset
#include "options.h"    // PCB & System definitions, device, pins, etc.
#include "eeprom_common.h"     // Common EEPROM definitions.
#include "eeprom.h"	    // EEPROM I2C function declarations 
 
const char fw_ver_eeprom[] =
"eeprom_stub"   /* Device Driver name */
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
  return 0;
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
int eeprom_erase (void)
{
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
  memset((void*)rd_buff, 0xff, size);
  return 0;
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 
 
