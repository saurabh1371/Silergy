/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Flash test declarations
 * $Id: flash.h 16155 2017-12-05 01:47:43Z rvandewa $  
 ***************************************************************************/
#ifndef FLASH_H     
#define FLASH_H

extern const char fw_ver_flash[];

//flash address in LONGWORDS, mpu address will be 4x this value
#define FLASH_ADDR_LONG_WORD(addr) (addr>>2) 

// Flash commands
#define FLASH_UNLOCK        0x464C4F00 //FLO. (FLash Operation)
#define FLASH_RD            0x00000000
#define FLASH_INFBLK_RD     0x00000001
#define FLASH_IBBLK_WR      0x00000002
#define FLASH_WR            0x00000003
#define FLASH_IBPAGE_ERASE  0x00000004
#define FLASH_PAGE_ERASE    0x00000005
#define FLASH_MASS_ERASE    0x00000006
#define FLASH_STANDBY       0x00000007
#define FLASH_FUSE_REFRESH  0x00000008


// Flash Registers
typedef struct {
    uint32_t FLASH_CN;      //FLASH_BASE base at 0x4000E000, also Flash Control Register
    uint32_t FLASH_ADDR;    //Address of flash where operation is to take place
    uint32_t FLASH_DATA0;   //Address of data
    uint32_t FLASH_PROT;    //Flash Protection Register
} FLASH_TypeDef;

#define FLASH_PAGE_SZ       (0x200)
/* flash size of 64K is out of bound for a 32k Flash */
#define ADD_OUTOFBOUND      (0x10000/FLASH_PAGE_SZ)        

// Flash commands are defined in SY7_device.h for the model of SoC.

#define FLASH_BUSY_BAR      0x80

#define WORD_SZ 4 // sizeof(uint32_t)

// Flash reads are just memory reads.

// Enable and disable flash writes and erases.
extern void flash_enable(void);
extern void flash_disable(void);

// Program Flash Memory for given buffer and address; 32-bit words only.
extern int flash_write(uint32_t address, uint32_t *data, int len);
// Erase Page in Flash Memory    
extern int flash_page_erase (uint32_t adr);
// Perform Mass erase of the Flash Memory
extern int flash_chip_erase (void);

#endif //FLASH_H

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 
