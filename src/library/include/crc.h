/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp.  All Rights Reserved.
 * DESCRIPTION: POWER METER - check a data area for validity.
 * This prototype is for use with lrc_crc.c or lrc_parity.c
 *
 * $Id: crc.h 16026 2017-11-23 02:49:23Z rvandewa $
 ***************************************************************************/
//
#ifndef CRC_H
#define CRC_H 1


// Firmware Device Driver version
extern const char fw_ver_crc[]; 


#define POLY_CRC32  0x04C11DB7 

// Data OK?
// If set == false, return true  == data ok.
// If set == true, *(uint16_t*)(ptr + (len - 2)) = data ok test number.
extern bool crc16 (uint8_t *ptr, uint16_t len, bool set);

// CRC-32 value computed over the byte array
extern uint32_t crc32(uint8_t* byteArray, int16_t arrayLen);

typedef uint16_t Crc16_t;

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved. *
 * this program is fully protected by the United States copyright          *
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
#endif /* valid.h */
