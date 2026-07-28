/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2005-2017 Silergy Inc. All Rights Reserved.
 * DESCRIPTION: POWER METER - Describes an LCD, BQ14053
 * 
 *  HISTORY: See end of file.
 **************************************************************************/
#define LCD_PRIVATE_DATA 1
#include "options.h"

// List all segments of all LCD character. 
// Segments are described according to the LCD spec.  
// The software expects digit 1 to be far left.
// This part is portable to other PCBs, except for the 
// disconnected digits that are commented-out.

uint16_t const digit_1[] =                   // digit #1 on left.
    { _1A, _1B, _1C, _1D, _1E, _1F, _1G,
      _1H, _1I, _1J, _1K, _1L, _1M, _1N };

uint16_t const digit_2[] =                   // digit #2
    { _2A, _2B, _2C, _2D, _2E, _2F, _2G,
      _2H, _2I, _2J, _2K, _2L, _2M, _2N };

uint16_t const digit_3[] =                   // digit 3
    { _3A, _3B, _3C, _3D, _3E, _3F, _3G,
      _3H, _3I, _3J, _3K, _3L, _3M, _3N, _3DP };

uint16_t const digit_4[] =                   // digit 4
    { _4A, _4B, _4C, _4D, _4E, _4F, _4G,
      _4H, _4I, _4J, _4K, _4L, _4M, _4N };

uint16_t const digit_5[] =                   // digit 5
    { _5A, _5B, _5C, _5D, _5E, _5F, _5G,
      _5H, _5I, _5J, _5K, _5L, _5M, _5N, _5DP };

uint16_t const digit_6[] =                   // digit 6
    { _6A, _6B, _6C, _6D, _6E, _6F, _6G,
      _6H, _6I, _6J, _6K, _6L, _6M, _6N, _6DP };

uint16_t const digit_7[] =                   // digit 7
    { _7A, _7B, _7C, _7D, _7E, _7F, _7G,
      _7H, _7I, _7J, _7K, _7L, _7M, _7N, _7DP };

uint16_t const digit_8[] =                   // digit 8
    { _8A, _8B, _8C, _8D, _8E, _8F, _8G,
      _8H, _8I, _8J, _8K, _8L, _8M, _8N };

uint16_t const digit_x[] =                   // digit x
    { _xA, _xB, _xC, _xD, _xE, _xF, _xG }; 

uint16_t const digit_y[] =                   // digit y
    { _yA, _yB, _yC, _yD, _yE, _yF, _yG }; 

uint16_t const icons[] =                     // digit y
    { 
        // decimal points (some are nonexistent)
        _none,      //P1=0,   // nonexistent
        _none,      //P2,     // nonexistent
        _3DP,       //P3,
        _none,      //P4,     // nonexistent
        _5DP,       //P5,
        _6DP,       //P6,
        _7DP,       //P7,
        _none,      //P8,     // nonexistent
        _D1,        //D1,     // Dash 1, between digit 2 & 3
        _D2,        //D2,     // Dash 2, between digit 4 & 5
        _T,         //COL1,   // colons
        _ZONTM,     //ZONTM,  // ZON symbol (TM) Silergy Corp.
        _K,         //kk,     // K of KWh
        _W,         //w,      // first half of W
        _V,         //v,      // v of KVa, second half of W
        _A,         //A,      // A of VARh
        _R,         //R,      // capital R
        _H,         //h,      // h of Wh
        _CD,        //c,      // C of degrees C
        _M,         //LOGO,   // logo (TM) Silergy Corp.
        _S1,        //S1,     // left arrow
        _S2,        //S2,     // right arrow
        _PF,        //phi,    // greek letter phi (phase)
        _LBAT,      //LOWBAT, // Low battery text
        _L1,        //L1,     // Line 1
        _L2,        //L2,     // Line 2
        _L3,        //L3,     // Line 3
        _EXCLAIM,   //EXCLAIM,    // "!"
        _COMS,      //COMS,   // COMS up/down arrow
        _SW,        //SW,     // switch symbol
        _NM         //NM,     // Neutral metering
    };

// Characters in the LCD.
uint16_t const * const lcd_chrs[] =
{
    NULL,
    digit_1,    digit_2,    digit_3,    digit_4,
    digit_5,    digit_6,    digit_7,    digit_8,
    digit_x,    digit_y,
    icons
};

// Number of segments in each character.
#define LCD_CHR_SIZE sizeof (digit_1[0])
uint8_t const lcd_chr_seg_cnts[] =
{
    0,
    sizeof (digit_1) / LCD_CHR_SIZE, sizeof (digit_2) / LCD_CHR_SIZE,
    sizeof (digit_3) / LCD_CHR_SIZE, sizeof (digit_4) / LCD_CHR_SIZE, 
    sizeof (digit_5) / LCD_CHR_SIZE, sizeof (digit_6) / LCD_CHR_SIZE,
    sizeof (digit_7) / LCD_CHR_SIZE, sizeof (digit_8) / LCD_CHR_SIZE, 
    sizeof (digit_x) / LCD_CHR_SIZE, sizeof (digit_y) / LCD_CHR_SIZE
};  

/***************************************************************************
 * Copyright (C) 2006-2017 Silergy Inc. All Rights Reserved.
 * this program is fully protected by the United States copyright 
 * laws and is the property of Silergy Inc.
 ***************************************************************************/
/* lcd_BQ14053 */

