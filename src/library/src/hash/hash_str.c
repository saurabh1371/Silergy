/***************************************************************************
 * This code and information is provided "as is" without warranty of any   *
 * kind, either expressed or implied, including but not limited to the     *
 * implied warranties of merchantability and/or fitness for a particular   *
 * purpose.                                                                *
 *
 * DESCRIPTION: This code checks data for errors.
 * It uses a longitudinal parity, which is fast, but less accurate.
 * There is a more accurate version of this code that uses CRCs.
 *
 * Copyright 2013, Silergy Corp.   All rights reserved.
 *
 * $Id: hash_str.c 16083 2017-12-04 05:26:15Z rvandewa $
 ****************************************************************************/
#include "options.h"
#include "hash_str.h"   // check the interface.

/*** Version ****/
const char fw_ver_hash_str[] =
"hash_str" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

/***************************************************************************   
* Description:  
*   Uses longitudinal parity to generate a unique value from a string.
* Parameters:
*   Input:
*         None.
*   Output: 
*         None.
* Return Status: 
*         None
***************************************************************************/
uint8_t hash_code_str (const char *ptr)
{
    int lrc;
    char ch;

    lrc = 0x55;
    do
    {
        ch = *ptr++;
        lrc ^= ch;
    } while ('\0' != ch);

    return (uint8_t)(lrc & 0xff);
} // int hash_code_str()

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

