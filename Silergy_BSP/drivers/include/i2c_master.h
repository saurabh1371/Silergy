/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Serial function declarations
 * $Id: i2c_master.h 21833 2019-09-12 01:34:23Z rvandewa $  
 ***************************************************************************/
#ifndef _I2C_MASTER_
#define _I2C_MASTER_

extern const char fw_ver_i2c_master[];

// EECTRL command operation for 2 Wire (I2C) configuration  ////////////////////////
#define I2C_CMD_NOP 						0x00 // Nothing is sent to EEPROM 
#define I2C_CMD_RX_BYTE_TX_ACK  0x02 // Receive a byte from EEPROM and send ACK
#define I2C_CMD_TX_BYTE					0x03 // Transmit a byte to EEPROM
#define I2C_CMD_STOP						0x05 // Issue a ‘STOP’ sequence
#define I2C_CMD_RX_BYTE_TX_NACK	0x06 // Receive last byte from EEPROM and send NACK
#define I2C_CMD_START						0x09 // Issue a ‘START’ sequence
// EECTRL status for 2 Wire (I2C) configuration 
#define I2C_TX_ACK	0x10 // Indicates when an ACK bit has been sent out
#define I2C_RX_ACK	0x20 // Indicates when an ACK bit has been received
#define I2C_BUSY		0x40 // I2C (uWire) interface is busy
#define I2C_ERROR		0x80 // Illegal command given to UW_CTL
///////////////////////////////////////////////////////////////////////////////////

// Initialize I2C master interface
extern void i2c_master_init(void);
// De-initialize I2C master interface
extern void i2c_master_deinit(void);
// Sends start condition to I2C slave
// nonzero return is failure.
extern int i2c_master_start(void);
// Sends stop condition to I2C slave
// nonzero return is failure.
extern int i2c_master_start(void);
extern int i2c_master_stop(void);
// Transmit a byte (byte could be cmd, data, info)
// nonzero return is failure.
extern int i2c_master_tx_byte(uint8_t tx_byte);
// Receive a byte
extern uint8_t i2c_master_rx_byte(bool ack);
	
#endif //_I2C_MASTER_



/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/


