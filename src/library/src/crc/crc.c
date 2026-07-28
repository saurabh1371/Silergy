/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: This code checks data for errors.
 * It uses a standard CRC function, which is good, but slow.
 *
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * $Id: crc.c 16215 2017-12-08 23:01:14Z rvandewa $
 ****************************************************************************/
#include "stdint.h"
#include "stdbool.h"
#include "crc.h"

/*** Version ****/
const char fw_ver_crc[] =
"crc" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


/***************************************************************************
* Description:
*         This routine validates a data structure with a CRC at the end.
*         The standard 16-bit CRC polynomial specified in ISO/IEC 3309 is used.
*                    16   12   5
*         Which is: x  + x  + x + 1
*         This is the most secure error detection in common use.  It detects all
*         1 and 2 bit errors, and most multiple bit errors. 
*         It detects misordered bytes.  It fails to detect leading zeros.  
*         It is slower than a longitudinal parity, and the practical difference
*         is hard to measure. It is much more secure than a checksum because
*         all bits are of equal significance and misordering is detected.
* Parameters:
*   Input:
*         ptr is a pointer to data in RAM
*         len is the length in bytes
*         set should be true to set the CRC in the last two bytes, or false
*           to test against a preexisting CRC in the last two bytes.
*   Output:
* Return Status:
*         true means that the data is ok, i.e. the CRC is valid.
*         false means that the data is not ok, i.e. the CRC is not valid.
*         The CRC is checked when set as well as when tested.
***************************************************************************/
bool crc16 (uint8_t *ptr, uint16_t len, bool set)
{
    uint8_t d;
    uint16_t CRC;

    if (set)
       len -= 2;                        // Do not include CRC in calculation.

    CRC = 0xFFFF;
    do
    {
       d = *ptr++ ^ (CRC & 0xFF);       // Compute combined value.
       d ^= d << 4;
       CRC  = (d << 3) ^ (d << 8) ^ (CRC >> 8) ^ (d >> 4);
    } while (--len);

    if (set)
    {                                   // Store complement of CRC.
       *ptr++ = (CRC & 0xFF) ^ 0xFF;
       *ptr   = (CRC >> 8)   ^ 0xFF;    // Verify.

       return ((*(ptr    ) == ((CRC >> 8)   ^ 0xFF))
            && (*(ptr - 1) == ((CRC & 0xFF) ^ 0xFF)));
    }
    else
       // From CCITT, CRC-16 always calculates to a fixed constant
       // if the CRC is part of the calculation.
       return (CRC == 0xF0B8);
}

/*****************************************************************************
* Description:  
*   For reference, CRC-32 algorithm 
* (x^32 + x^26 + x^23 + x^22+ x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 
* + x^4 +x^2 + x + 1)
* CRC polynomial as bit sequence : 100000100110000010001110110110111.
* Parameters:
*   Input:
*         uint8_t *byteArray : Pointer to the byte array over which CRC-32 
*               is computed.
*         int16_t arrayLen   : Number of bytes in byte array.
*   Output: 
*         None.
* Return: 
*         uint32_t crc32     : CRC-32 value computed over the byte array.
*****************************************************************************/
uint32_t crc32(uint8_t* byteArray, int16_t arrayLen)
{
    uint32_t crc_tmp,i,j;
    uint32_t crc_32 = 0;
    uint8_t bit_cnt,bit_chk;
    
        
    for(crc_tmp=0, bit_cnt=0; arrayLen > 0; )
    {
        // Check for MSB of 32-bit word (crc_tmp). 
        bit_chk = crc_tmp >> 31;
        // Shift left, make room for LSB of byte array (message).  
        crc_tmp <<= 1;
        // Starting from LSB, move byte array bits into 32-bit word.
        crc_tmp |= (*byteArray >> bit_cnt) & 0x001;

        // increment bit count    
        bit_cnt++;
        // After shifting 8 bits, increment to next data byte of array.
        if(bit_cnt > 7)
        {
            bit_cnt = 0;
            byteArray++;
            arrayLen--;
        }

        // skip XOR operation for leading zero
        if(bit_chk)
           // Modulo-2 division using XOR.
           crc_tmp ^= POLY_CRC32;

    }

    /* CRC-32 polynomial length is 32, hence 32-bits are appended at the end 
     * of byte array; work on these last 32-bits */
    for (i = 0; i < 32; ++i)  
    {
        bit_chk = crc_tmp >> 31;
        crc_tmp <<= 1;
        
        if(bit_chk)
            crc_tmp ^= POLY_CRC32;
    }

    /* reverse the bits of 32-bit of crc_tmp to generate CRC-32 */
    for(i = 0x80000000,j=0x0001; i != 0; i >>=1, j <<= 1) 
    {
        if (i & crc_tmp) 
           crc_32 |= j;
    }

    return crc_32;
}

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/


