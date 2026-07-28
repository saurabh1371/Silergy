/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2005..2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: LCD integration file. Useful to debug I/O with low voltages.
 *
 * $Id: lcd_stub.c 23558 2020-03-09 21:04:44Z rvandewa $
 ****************************************************************************/
#include "options.h"
#include "lcd.h"

/*** Version ****/
const char fw_ver_lcd[] =
"lcd_stub" /* Device Driver name */
" v"       /* ver string */
"1.00"     /* Major_ver.Minor_ver */
"\r\n"     /* CR + LF */
;

#if 1   /* needed for BLINK in lcd.h if lcd_*.c is not in the project */
const uint16_t Digit_01[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif

/* display */
void lcd_update(void) { }
void lcd_update_brownout(int t) { ++t; }
void lcd_put_sw_id(void) { }

/* Write a character. LCD[chr_idx] := chr */
void lcd_put_seg (int seg, bool on) {
    ++seg, ++on; }

/* Write a character. LCD[chr_idx] := chr */
void lcd_put_char (int chr_idx, char chr) {
    ++chr_idx, ++chr; }

/* Write an icon */
void lcd_put_icon(int icon_idx) { ++icon_idx; }

/* Display UTF-8 str at chr_idx.  LCD[chr_idx..] := str_ptr */
void lcd_put_str (int chr_idx, const char *str) {
    ++chr_idx, ++str; }

/* Display num on the LCD. LCD[chr_idx] := int32_t */
void lcd_put_num (
    int32_t chr_idx,     /* "-012.3" == &'-' */
    int32_t digit_cnt,   /* "-012.3" == 5 == strcnt("-0123") */
    int32_t num)         /* -123 */
{ ++chr_idx; ++digit_cnt; ++num; }

/* Blank the LCD. LCD[*] := ' ' */
void lcd_clear (void) { }

/* Show all the segments of the LCD. LCD[*] := CHR[*] */
void lcd_test (void) { }


/***************************************************************************   
* Description:  
*   Write a bit to the LCD map.
* Parameters:
*   Input:
*         pin_idx is an LCD pin number, 0.. max.
*         val is a value to write to a map register bit.
*   Output: 
*         LCD pins are enabled or disabled.
* Return Status: 
*         None
***************************************************************************/
void lcd_map(int pin_idx, int val)
{
    int bit_idx, sfr_idx, tmp, mask;

    if (LCD_PIN_CNT <= pin_idx) /* the pin is invalid */
        return;

    bit_idx = 0x1F & pin_idx;
    sfr_idx = pin_idx >> 5; 
    switch(sfr_idx) {
        case 0: tmp = LCD->MAPL; break; // Controls SEG0..31
        case 1: tmp = LCD->MAPH; break; // Controls SEG32..35
        default: tmp = 0; break;
    }
    mask = 1UL << bit_idx;
    if(0 == val)
    {
        tmp &= ~mask;
    } else {
        tmp |= mask;
    }
    switch(sfr_idx) {
        case 0: LCD->MAPL = tmp; break; // Controls SEG0..31
        case 1: LCD->MAPH = tmp; break; // Controls SEG32..35
        default: break;
    }
} // lcd_map()


/* Initialize the LCD. */
void lcd_init (int *i0, int i1) { i0++; i1++; }

/* Reduce the power and blank the LCD without inducing a harmful
 * DC bias */
void lcd_deinit (void) { }

/* Select the LCD image. */
void lcd_image(int image_idx) { ++image_idx; }

/* Start page operation, with a timer */
void lcd_page_sec(int sec_cnt) { ++sec_cnt; }

/***************************************************************************
 * Copyright (C) 2005..17 Silergy Corp.  All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/


