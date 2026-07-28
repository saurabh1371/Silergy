/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2005-2017 Silergy Inc. All Rights Reserved.
 * DESCRIPTION: Describes an LCD, BQ14053
 * This file should be placed in options.h, and describes the LCD
 * to the other software.
 * This file is used with a table in a .c file with the same name.
 * 
 *  AUTHOR:  RGV
 **************************************************************************/
#ifndef LCD_BQ14053_H
#define LCD_BQ14053_H 1

// The following data define the glass, and need not change on
// different PCBs.

// Note the definition of LCD_GLASS, below, 
// near the end, which is very important.

// Define the digits in the display.
#define DIGIT_CNT 10 // 10 digits, total; 8 14-seg, 2 7-seg
#define LINE1 3     // 8 digits, full sized, left digit starts at 3
#define LINE1_LEN 6 // line1 length is 6 ( digit-3 to digit-8)
#define LINE2 9     // 2 digits, starts at 9th character
#define LINE2_LEN 2
#define LCD_CHR_CNT DIGIT_CNT+1

/* Icon mapping, indexes arrays in the glass file for segment and common.  */
#define LCD_FIRST_PERIOD 1 // 1st character from left
#define LCD_LAST_PERIOD 8  // 8th character from left
#define ICON_LAST 30
extern const uint16_t icons[];
enum icon
{
// decimal points (some are nonexistent)
P1=0,   // nonexistent
P2,     // nonexistent
P3,
P4,     // nonexistent
P5,
P6,
P7,
P8,     // nonexistent
D1,     // Dash 1, between digit 2 & 3
D2,     // Dash 2, between digit 4 & 5
COL1,   // colons
ZONTM,   // ZON(tm) symbol
kk,     // K of KWh
w,      // first half of W
v,      // v of KVa, second half of W
A,      // A of VARh
R,      // capital R
h,      // h of Wh
c,      // C of degrees C
LOGO,   // Maxim logo
S1,     // right-to-left arrow (export)
S2,     // left-to-right arrow (import)
phi,    // greek letter phi (phase)
LOWBAT, // Low battery text
L1,     // Line 1
L2,     // Line 2
L3,     // Line 3
EXCLAIM,    // "!"
COMS,   // COMS up/down arrow
SW,     // switch symbol
NM     // Neutral metering
};

// Characters in the LCD.
extern uint16_t const * const lcd_chrs[];

// Number of segments in each character.
extern const uint8_t lcd_chr_seg_cnts[];

#ifdef LCD_PRIVATE_DATA     // Defined only in lcd.c

// Font
#define LCD_FULL_ISO 0      // Full ISO character set (this is a test...)
// This glass uses the 14-segment font for the alphabet, but the
// 7-segment font for figures; It has one 7-segment character position,
// a "-" and a "1" sign.  These are all coded (in the character table)
// to work with the font.
#undef LEFT_L
#define LEFT_L 0 // segments LMN, L is on the right.
#include "lcd_font_14_7.h"

// List all segments of display in order by the pin
// of the LCD glass.
// This part is portable to other PCBs.
// Character 3
#define _3E  (COM0|PIN01)
#define _3F  (COM1|PIN01)
#define _3G  (COM2|PIN01)
#define _3K  (COM3|PIN01)
#define _3J  (COM4|PIN01)
#define _3N  (COM5|PIN01)

// Pins 2 and 3 are all NC

#define _3A  (COM0|PIN04)
#define _3B  (COM1|PIN04)
#define _3C  (COM2|PIN04)
#define _3D  (COM3|PIN04)
#define _3H  (COM4|PIN04)
#define _3I  (COM5|PIN04)

#define _3DP (COM0|PIN05)
#define _5DP (COM1|PIN05)
#define _6DP (COM2|PIN05)
#define _7DP (COM3|PIN05)
#define _4M  (COM4|PIN05)
#define _4L  (COM5|PIN05)

#define _4E  (COM0|PIN06)
#define _4F  (COM1|PIN06)
#define _4G  (COM2|PIN06)
#define _4K  (COM3|PIN06)
#define _4J  (COM4|PIN06)
#define _4N  (COM5|PIN06)

#define _4A  (COM0|PIN07)
#define _4B  (COM1|PIN07)
#define _4C  (COM2|PIN07)
#define _4D  (COM3|PIN07)
#define _4H  (COM4|PIN07)
#define _4I  (COM5|PIN07)

#define _5E  (COM0|PIN08)
#define _5F  (COM1|PIN08)
#define _5G  (COM2|PIN08)
#define _5K  (COM3|PIN08)
#define _5J  (COM4|PIN08)
#define _5N  (COM5|PIN08)

#define _5A  (COM0|PIN09)
#define _5B  (COM1|PIN09)
#define _5C  (COM2|PIN09)
#define _5D  (COM3|PIN09)
#define _5H  (COM4|PIN09)
#define _5I  (COM5|PIN09)

#define _6E  (COM0|PIN10)
#define _6F  (COM1|PIN10)
#define _6G  (COM2|PIN10)
#define _6K  (COM3|PIN10)
#define _6J  (COM4|PIN10)
#define _6N  (COM5|PIN10)

#define _6A  (COM0|PIN11)
#define _6B  (COM1|PIN11)
#define _6C  (COM2|PIN11)
#define _6D  (COM3|PIN11)
#define _6H  (COM4|PIN11)
#define _6I  (COM5|PIN11)

#define _7E  (COM0|PIN12)
#define _7F  (COM1|PIN12)
#define _7G  (COM2|PIN12)
#define _7K  (COM3|PIN12)
#define _7J  (COM4|PIN12)
#define _7N  (COM5|PIN12)

#define _7A  (COM0|PIN13)
#define _7B  (COM1|PIN13)
#define _7C  (COM2|PIN13)
#define _7D  (COM3|PIN13)
#define _7H  (COM4|PIN13)
#define _7I  (COM5|PIN13)

#define _8E  (COM0|PIN14)
#define _8F  (COM1|PIN14)
#define _8G  (COM2|PIN14)
#define _8K  (COM3|PIN14)
#define _8J  (COM4|PIN14)
#define _8N  (COM5|PIN14)

#define _8A  (COM0|PIN15)
#define _8B  (COM1|PIN15)
#define _8C  (COM2|PIN15)
#define _8D  (COM3|PIN15)
#define _8H  (COM4|PIN15)
#define _8I  (COM5|PIN15)

#define _yA  (COM0|PIN22)
#define _yB  (COM1|PIN22)
#define _yC  (COM2|PIN22)
#define _yD  (COM3|PIN22)
#define _8M  (COM4|PIN22)
#define _8L  (COM5|PIN22)

#define _yE  (COM0|PIN23)
#define _yF  (COM1|PIN23)
#define _yG  (COM2|PIN23)
#define _NM  (COM4|PIN23)

#define _xA  (COM0|PIN24)
#define _xB  (COM1|PIN24)
#define _xC  (COM2|PIN24)
#define _xD  (COM3|PIN24)
#define _7M  (COM4|PIN24)
#define _7L  (COM5|PIN24)

#define _xE  (COM0|PIN25)
#define _xF  (COM1|PIN25)
#define _xG  (COM2|PIN25)
#define _COMS (COM3|PIN25)
#define _SW  (COM5|PIN25)

#define _R   (COM0|PIN26)
#define _H   (COM1|PIN26)
#define _CD  (COM2|PIN26)
#define _M   (COM3|PIN26)
#define _3M  (COM4|PIN26)
#define _3L  (COM5|PIN26)

#define _K   (COM0|PIN27)
#define _W   (COM1|PIN27)
#define _V   (COM2|PIN27)
#define _A   (COM3|PIN27)
#define _2M  (COM4|PIN27)
#define _2L  (COM5|PIN27)

#define _S1  (COM0|PIN28)
#define _S2  (COM1|PIN28)
#define _PF  (COM2|PIN28)
#define _LBAT (COM3|PIN28)
#define _6M  (COM4|PIN28)
#define _6L  (COM5|PIN28)

#define _L1  (COM0|PIN29)
#define _L2  (COM1|PIN29)
#define _L3  (COM2|PIN29)
#define _EXCLAIM (COM3|PIN29)
#define _5M  (COM4|PIN29)
#define _5L  (COM5|PIN29)

#define _D1  (COM0|PIN30)
#define _D2  (COM1|PIN30)
#define _T   (COM2|PIN30)
#define _ZONTM (COM3|PIN30)
#define _1M  (COM4|PIN30)
#define _1L  (COM5|PIN30)

#define _1E  (COM0|PIN31)
#define _1F  (COM1|PIN31)
#define _1G  (COM2|PIN31)
#define _1K  (COM3|PIN31)
#define _1J  (COM4|PIN31)
#define _1N  (COM5|PIN31)

#define _1A  (COM0|PIN32)
#define _1B  (COM1|PIN32)
#define _1C  (COM2|PIN32)
#define _1D  (COM3|PIN32)
#define _1H  (COM4|PIN32)
#define _1I  (COM5|PIN32)

#define _2E  (COM0|PIN33)
#define _2F  (COM1|PIN33)
#define _2G  (COM2|PIN33)
#define _2K  (COM3|PIN33)
#define _2J  (COM4|PIN33)
#define _2N  (COM5|PIN33)

#define _2A  (COM0|PIN36)
#define _2B  (COM1|PIN36)
#define _2C  (COM2|PIN36)
#define _2D  (COM3|PIN36)
#define _2H  (COM4|PIN36)
#define _2I  (COM5|PIN36)

#define _none (ncPin) // an invalid segment number

#endif // LCD_PRIVATE_DATA


#endif // LCD_BQ14053_H
/***************************************************************************
 * Copyright (C) 2006-2017 Silergy Inc. All Rights Reserved.
 * this program is fully protected by the United States copyright 
 * laws and is the property of Silergy Inc.
 ***************************************************************************/
/* lcd_bq14053 */

