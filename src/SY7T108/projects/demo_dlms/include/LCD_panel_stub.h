/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2019 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Stub for an LCD panel.
 *
 * $Id: LCD_panel_stub.h 25648 2020-06-10 11:00:29Z phuddar $
 **************************************************************************/
#ifndef LCD_STUB_H
#define LCD_STUB_H 1

// describe the lcd_chrs[] array for the display code.
#define DIGIT_CNT 0 // 10 digits, total; 8 14-seg, 2 7-seg
#define LINE1 1     // 8 digits, full sized, left digit starts at 1
#define LINE1_LEN 0
#define LINE2 1     // 2 digits, starts at 9th character
#define LINE2_LEN 0
#define LCD_CHR_CNT DIGIT_CNT+1

/* Icon mapping, indexes arrays in the glass file for segment and common.  */
#define LCD_FIRST_PERIOD 0 // 1st character from left
#define LCD_LAST_PERIOD 0  // 6th character from left
#define ICON_LAST 0

extern const uint16_t icons[ICON_LAST+1];
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
#undef LEFT_L
#define LEFT_L 0 // segments LMN, L is on the right.
#include "lcd_font_14_7.h"


#define _none (ncPin) // an invalid segment number

#endif // LCD_PRIVATE_DATA

/***************************************************************************
 * Copyright (C) 2019 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif /* lcd_stub.h */
