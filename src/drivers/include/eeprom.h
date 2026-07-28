/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: EEPROM I2C device driver. EEPROM AT24C256C :
 *        http://www.atmel.com/images/Atmel-8568-SEEPROM-AT24C256C-Datasheet.pdf 
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: eeprom.h 17120 2018-03-27 20:54:54Z ptanwar $
 ****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Booleans.

#ifndef EEPROM_H
#define EEPROM_H

// EEPROM's version.
extern const char fw_ver_eeprom[];

// Initialize the EEPROM.
void eeprom_init(void);

// Turn off the EEPROM.
void eeprom_deinit(void);

// Write to the EEPROM. fail = nonzero return.
int eeprom_write(int addr, uint8_t *wr_buff, int size);

// Read the EEPROM. fail if nonzero retur.
int eeprom_read (int addr, uint8_t *rd_buff, int size);

// Erase the entire EEPROM.
int eeprom_erase(unsigned char);

// Erase the entire EEPROM.
bool EPClearPage(int);

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
#endif // eeprom_h 
 
