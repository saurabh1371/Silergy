/***************************************************************************
 * Copyright (c) 2005-2017 Silergy Corp.  All rights Reserved. 
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * The LCD routines let any ISO character map to any LCD character image.
 * Upper and lower case latin, numerals, ISO punctuation and the
 * replacement character are already included.
 * The output routines are lcd_put_flash_str(...), lcd_put_num(...)
 * lcd_put_chr() (a character) and lcd_put_icon() (puts encoded segments)
 * The character index is the position in the display.  1 is the left-most,
 * first, most significant numeral of the main display. 
 * Multiple line displays are coded as if they are a single line, 
 * with "LINE1", "LINE2", etc, constants in "LCD_<glass name>.h" files.
 * Little number fields (e.g. for DLMS OBIS codes, Japanese fractional Wh) 
 * are usually LINE2.
 * The numeric output routine put_num must have the length of the field.
 * A '-' sign is placed to the left if needed.
 *
 * To add an icon:
 * Encode the segment number with a common in the most-significant byte,
 * and a segment number in the least significant byte.
 * Add the icon's number to the icon array, and add the name to the enum
 * in the .h file.  Note that decimal points need to be first in the 
 * icon map in order to put periods into strings.
 *
 * To add a character:
 * 1. Add its bit map to lcd_font_7.h for 7-segment displays, or lcd_font_14.h
 * for 14-segment displays.  A character in a font is a 16-bit bit map.  Bit 0
 * is segment A, 1 is segment B, bit 2 is segment C, etc.
 * 2. Add the UTF code point to font[] in the font file.
 * 3. Use the character in UTF-8 strings passed to lcd_putstr().
 *
 * $Id: lcd.h 16417 2018-01-12 20:00:24Z rvandewa $
 **************************************************************************/
#ifndef LCD_H
#define LCD_H 1

extern const char fw_ver_lcd[];

// Light an LCD segment.  
//  seg: An integer from lcd_<glassname>.h that describes an LCD segment.
//  on: A boolean that describes whether to turn the segment on or off.
void lcd_put_seg (int seg, bool on);

// Display a segment that's a named, numbered icon in lcd_<glass name>.h
//  icon_idx: An icon's index.
void lcd_put_icon (int32_t icon_idx);

// Display a character. LCD[chr_idx] = chr_bits from font.
//  char_idx: The index of the character in the display; 1 = left most
//  ch: The character to display.
void lcd_put_char (int32_t char_idx, char ch);

// Display a C string on the LCD. (LCD[chr_idx] := "xyz")
//  char_idx: The index of the character in the display; 1 = left most
//  str: The C string to display.
void lcd_put_str( int32_t char_idx, const char * str );

// Display a long integer on the LCD. (LCD[chr_idx] := int32_t)
//  char_idx: The index of the character in the display; 1 = left most
//  digit_cnt: Maximum count of digits.
//  num: the signed integer number to display.
void lcd_put_num( int32_t char_idx, int32_t digit_cnt, int32_t num);

// Clear the LCD. (LCD[*] := ' ')
void lcd_clear (void);

// Show all the segments of the LCD. (LCD[*] := CHR[*])
void lcd_test (void);

// Sets the page timing for the LCD's automatic page selection.
void lcd_page_sec(int seconds);

// This writes to the correct map registers.
void lcd_map(int pin_idx, int val);

// Initialize the LCD system.
//  lcd_ptr_in: Pointer to RAM array for DMA LCD. NULL = use registers.
//  lcd_image_cnt: Nonzero: count of RAM LCD images. 0: Use registers. 
void lcd_init(int *lcd_ptr_in, int lcd_image_cnt);

// Power down the LCD system.
void lcd_deinit(void);

// Select the LCD image to write.
void lcd_image(int image_idx);

// Select the speed of the LCD image switching.
// Enables DMA if the lcd_init() had RAM.
void lcd_page_sec(int sec_cnt);


#endif // LCD_H

/****************************************************************************
 * Copyright (C) 2005-2017 Silergy Corp., All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

