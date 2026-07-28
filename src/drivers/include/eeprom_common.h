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
 * $Id: eeprom_common.h 25395 2020-05-19 17:04:01Z phuddar $
 ****************************************************************************/
#ifndef EEPROM_COMMON_H
#define EEPROM_COMMON_H

//ST EEPROMs require a nonconforming bus transition in a random read, and so
//require a bit-banging I2C driver.

enum EEPROM_RC { _OK, _PENDING, _ERR_OVERRUN, _ERR_NACK, _ERR_PENDING };

#define POWER_DELAY (DELAY_US(50))
#define PCB_24C1024 1
// EEPROM Selection defined in /Projects/xxx/include/map.h
#if PCB_24C1024
   #define EEPROM_SIZE  0x20000  // AT24C1024
   #define EEPROM_PAGE_SIZE 256  // for Atmel's AT24C1024
   #define EEPROM_PAGE_WR_DELAY 5 // Self-timed Write Cycle (5 ms Typical)
#elif PCB_24C512
   #define EEPROM_SIZE 0x10000   // AT24C512, ST's M24C512
   #define EEPROM_PAGE_SIZE 128  // for ST's M24512, or Atmel's AT24C512
   #define EEPROM_PAGE_WR_DELAY 5 // Self-timed Write Cycle (5 ms Typical)
#elif PCB_24C256
   #define EEPROM_SIZE 0x8000    // AT24C256, ST's M24C256
   #define EEPROM_PAGE_SIZE  64  // for ST's M24C256,128, Atmel's AT24C256,128
   #define EEPROM_PAGE_WR_DELAY 5 // Self-timed Write Cycle (5 ms Typical)
#elif PCB_24C128
   #define EEPROM_SIZE 0x4000    // AT24C128, ST's M24C128
   #define EEPROM_PAGE_SIZE  64  // for ST's M24C256,128, Atmel's AT24C256,128
   #define EEPROM_PAGE_WR_DELAY 5 // Self-timed Write Cycle (5 ms Typical)
#elif PCB_24C64
   #define EEPROM_SIZE 0x2000    // AT24C64, ST's M24C64
   #define EEPROM_PAGE_SIZE  32  // for ST's M24C64,32 Atmel's AT24C64,32
   #define EEPROM_PAGE_WR_DELAY 10 // Self-timed Write Cycle (10 ms max)
#elif PCB_24C32
   #define EEPROM_SIZE 0x1000    // AT24C32, ST's M24C32
   #define EEPROM_PAGE_SIZE  32  // for ST's M24C64,32 Atmel's AT24C64,32
   #define EEPROM_PAGE_WR_DELAY 10 // Self-timed Write Cycle (10 ms max)
#else
#error EEPROM is not selected
#endif


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // EEPROM_COMMON_H

