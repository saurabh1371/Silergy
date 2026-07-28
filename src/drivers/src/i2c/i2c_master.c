/*----------------------------------------------------------------------------
 * Purpose: Driver for SY7-series I2C EEPROM Master (2-wire)
 * Note(s): Slave EEPROM is AT24C256C
 *           http://www.atmel.com/images/Atmel-8568-SEEPROM-AT24C256C-Datasheet.pdf 
 * $Id: i2c_master.c 22661 2020-01-08 19:26:08Z ptanwar $ 
 *----------------------------------------------------------------------------
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *----------------------------------------------------------------------------*/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Standard boolean declaration
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "delay.h"
#include "gpio.h"
#include "i2c_master.h"
#include "wd.h"


const char fw_ver_i2c_master[] =
"i2c_master"    /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

int i2c_error;


static int i2c_busy_wait(void);
//static int i2c_ack_wait(void);

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
void i2c_master_init(void)
{
    i2c_error = 0;
	SSI->EEX = 0x01; 	// Set 2 wire (DIO20 as I2C_SCL and DIO19 as I2C_SDA)
	gpio_alt_on(i2c_sda_seg);			// dio19 alt=i2c_sda 
	gpio_alt_on(i2c_scl_seg);			// dio20 alt=i2c_scl  
	SSI->INT = 0;					// disable interrupt
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
void i2c_master_deinit(void)
{
    i2c_error = 0;
	gpio_dir_in(i2c_sda_seg);   // alt=i2c_sda is high impedance
	gpio_dir_in(i2c_scl_seg);   // alt=i2c_scl is high impedance  
	SSI->EEX = 0x00; 	// diable i2c master
	SSI->INT = 0; 		// disable interrupt
}

/*****************************************************************************
* Description:  
*        	Sends start command to the I2C slave.
* Parameters:
*   Input:
*					None.
*   Output: 
*         None.
* 	Return Status: 
* 	      nonzero is failure.
*****************************************************************************/
int i2c_master_start(void)
{
	// send start command
	SSI->I2C_CNTL = I2C_CMD_START;
	return i2c_busy_wait();
}


/*****************************************************************************
* Description:  
*        	Sends stop command to the I2C slave.
* Parameters:
*   Input:
*					None.
*   Output: 
*         None.
* 	Return Status: 
*         nonzero is failure.
*****************************************************************************/
int i2c_master_stop(void)
{
	// Send stop command
	SSI->I2C_CNTL = I2C_CMD_STOP;
	return i2c_busy_wait();	
}

/*****************************************************************************
* Description:  
*        Sends command (or data) to the slave address on the I2C bus.
* Parameters:
*   Input:
*         tx_byte: Data (address, or command) to be sent 
*   Output: 
*         None.
* 	Return Status: 
*         nonzero is failure.
*****************************************************************************/
int i2c_master_tx_byte(uint8_t tx_byte)
{
	// Send a byte to the I2C slave
	SSI->DATA = tx_byte;
	SSI->I2C_CNTL = I2C_CMD_TX_BYTE;
    i2c_error = 0;
    i2c_busy_wait();
//	i2c_ack_wait();
//	delay(0x20);
    return i2c_error;
}

/*****************************************************************************
* Description:  
*        Receive a byte from the slave.
* Parameters:
*   Input:
*         ack: If 'true' then : receive a byte from EEPROM and send ACK
*							 If 'false' then : receive a byte from EEPROM and send NACK
*   Output: 
*         None.
* 	Return Status: 
*        	The byte received from the slave.
*****************************************************************************/
uint8_t i2c_master_rx_byte(bool ack)
{
	uint8_t data;

    if(i2c_error)
        return 0;

	if(true == ack)
		SSI->I2C_CNTL = I2C_CMD_RX_BYTE_TX_ACK;
	else
		SSI->I2C_CNTL = I2C_CMD_RX_BYTE_TX_NACK;
	i2c_busy_wait();
	data = (uint8_t)SSI->DATA;
	delay(0x20);
	return data;
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
int i2c_busy_wait(void)
{
    int timeout = 50000; // 50.000 milliseconds
	while(I2C_BUSY == (SSI->I2C_CNTL & I2C_BUSY))
	{
		wd_reset();
		delay(DELAY_US(20));
        timeout -= 20;
        if(0 >= timeout)
        {
            i2c_error |= 1;
            return i2c_error;
        }
	}
		
	return 0; 
}

///*****************************************************************************
//* Description:  
//*        Wait in while loop until ACK is received from EEPROM slave
//* Parameters:
//*   Input:
//*         None. 
//*   Output: 
//*         None.
//* Return Status: 
//*         None
//*****************************************************************************/
//int i2c_ack_wait(void)
//{
//    int timeout = 50000; // 50.000 milliseconds
//    if(i2c_error)
//        return i2c_error;
//	while(I2C_RX_ACK != (SSI->I2C_CNTL & I2C_RX_ACK))
//	{
//		wd_reset();
//		delay(DELAY_US(20));
//        timeout -= 20;
//        if(0 >= timeout)
//        {
//            i2c_error |= 2;
//            return i2c_error;
//        }
//	}
//	return i2c_error; 
//}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 

