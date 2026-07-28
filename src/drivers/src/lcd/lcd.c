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
 * 2. Add the UTF code point to font[] in this file.
 * 3. Use the character in UTF-8 strings passed to lcd_put_str().
 *
 * $Id: lcd.c 24144 2020-03-30 22:56:41Z ptanwar $
 **************************************************************************/
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
// Enable the private LCD glass data from the LCD glass .h file in options.h
#define LCD_PRIVATE_DATA   1
#include "options.h"
#include "lcd.h"

/*** Version ****/
const char fw_ver_lcd[] =
"lcd"      /* Device Driver name */
" v"       /* ver string */
"2.00"     /* Major_ver.Minor_ver */
"\r\n"     /* CR + LF */
;

#define LCD_MAP_CNT ((LCD_PIN_CNT+31)/32)
#ifndef LCD_FULL_ISO
#define LCD_FULL_ISO 0
#endif
/*** External variables used within this module ***/

/*** Public variables declared within this module ***/

/*** Private functions declared within this module ***/

/*** Private variables used within this module ***/
#define FONT_BEGIN (' ')
#if LCD_FULL_ISO
#define FONT_END (0x7f)
#define FONT_LENGTH (0x60)
#else 
#define FONT_END ('_')
#define FONT_LENGTH (0x40)
#endif

// Pointer to current LCD RAM image.
uint8_t *lcd_image_base;
uint8_t *lcd_image_ptr;
int8_t lcd_image_last;

#ifndef LCD_MODE_DEF // If no presets, calculate them.
int lcd_mode, lcd_mapl, lcd_maph;
#endif


#if FONT_LENGTH!=((FONT_END - FONT_BEGIN)+1)
#error font calculation error
#endif
/* A font is a size and style of type. */
static const uint16_t font [FONT_LENGTH] =
{
    /* Upper case latin of UTF-8. */
    /* 64 characters starting at UTF code point hex 00000020 */
    BLANK, EXCLAMATION_POINT, DOUBLE_QUOTE, NUMBER_SIGN,
    DOLLAR_SIGN, PERCENT_SIGN, AMPERSAND, APOSTROPHE,
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS, ASTERISK,  PLUS_SIGN,
    COMMA, MINUS_SIGN, PERIOD, FRONT_SLASH,
    FIGURE_0,   FIGURE_1,   FIGURE_2,   FIGURE_3,
    FIGURE_4,   FIGURE_5,   FIGURE_6,   FIGURE_7,
    FIGURE_8,   FIGURE_9, COLON, SEMICOLON,
    LESS_THAN, EQUAL, GREATER_THAN, QUESTION_MARK,
    AT_SIGN, ALPHA_A, ALPHA_B, ALPHA_C,
    ALPHA_D, ALPHA_E, ALPHA_F, ALPHA_G,
    ALPHA_H, ALPHA_I, ALPHA_J, ALPHA_K,
    ALPHA_L, ALPHA_M, ALPHA_N, ALPHA_O,
    ALPHA_P, ALPHA_Q, ALPHA_R, ALPHA_S,
    ALPHA_T, ALPHA_U, ALPHA_V, ALPHA_W,
    ALPHA_X, ALPHA_Y, ALPHA_Z, RIGHT_BRACKET,
    BACK_SLASH, LEFT_BRACKET, CARET, UNDERLINE,
#if LCD_FULL_ISO
    /* Latin lower case */
    /* 32 characters starting at UTF code point hex 00000060 */
    ACCENT, ALPHA_A, ALPHA_B, ALPHA_C,
    ALPHA_D, ALPHA_E, ALPHA_F, ALPHA_G,
    ALPHA_H, ALPHA_I, ALPHA_J, ALPHA_K,
    ALPHA_L, ALPHA_M, ALPHA_N, ALPHA_O,
    ALPHA_P, ALPHA_Q, ALPHA_R, ALPHA_S,
    ALPHA_T, ALPHA_U, ALPHA_V, ALPHA_W,
    ALPHA_X, ALPHA_Y, ALPHA_Z, LEFT_CURLY_BRACKET,
    VERTICAL_LINE, RIGHT_CURLY_BRACKET, TILDE, DEL,
#endif
};

#define CHR_0 ('!')
#define CHR_LIM (sizeof(font)/2 - 1)
#define SEGMENT_MASK  0xFF              /* Number of SEGMENT drivers. */
#define COM_MASK      ~SEGMENT_MASK     /* Number of COMMONs. */
#define COM_ALIGN     8


/***************************************************************************   
* Description:  
*       Set the LCD bit for a segment.  This is an output routine that
*       actually "lights" a segment.
*       This routine is additive: It only sets bits, never clears them.
*       This is easier to use than a true write, because it's like paper.
*       Use lcd_clear() to clear the display.
* Parameters:
*   Input:
*         An integer that describes an LCD segment.
*         A boolean that describes whether it is on or off.
*   Output: 
*         It writes the segment.
* Return Status: 
*         None
***************************************************************************/
void lcd_put_seg (int seg, bool on)
{
    int pin_idx;
    int pulse_train;
    int valid_pin;

    if (on) // If off, it remains clear. No need to do anything.
    {
        /* Which pin controls the LCD segment? */
        pin_idx = (uint8_t)seg;

        if ( 0 <= pin_idx && LCD_PIN_CNT > pin_idx ) // valid pin number?
        {
            // Is the pin an LCD pin?
            if (pin_idx < 32)
                valid_pin = LCD->MAPL & (1 << pin_idx);
            else
                valid_pin = LCD->MAPH & (1 << (pin_idx - 32));

            // If it's an LCD pin, write to the pin's pulse-train register.
            if (valid_pin)
            {
                pulse_train = 0xFF & (seg >> 8);

                /* Write the bit to the pin's pulse-train register */
                if(lcd_image_ptr) // Is DMA live?
                {   // If using RAM images, write to the current image.
                    if(LCD_IMAGE_SIZE > pin_idx) {
                        // The pin registers are sequential, so access them 
                        // as an array.
                        lcd_image_ptr[pin_idx] |= (uint8_t)pulse_train;
                    }
                } else {   // Write to the registers. No RAM.
                    LCD->DIO[pin_idx] |= pulse_train;
                } // if it's not DMA
            } // if it's an LCD
        } // if pin index is ok
    } // if on
} // lcd_put_seg ()


/***************************************************************************   
* Description:  
*       Set a segment that's an identified separate icon.
*       This is an output routine that actually lights a segment.  
* Parameters:
*   Input:
*       An icon index.
*   Output: 
*       A lit icon.
* Return Status: 
*       none.
***************************************************************************/
void lcd_put_icon (int32_t icon_idx)
{
    if(ICON_LAST < icon_idx)
        return;

    lcd_put_seg(icons[icon_idx],true);
}


/***************************************************************************   
* Description:  
*       Takes a 32-bit UTF-32 character, looks it up in the font,
*       and then displays it.
* Parameters:
*   Input:
*         a char
*   Output: 
*         a font entry.
* Return Status: 
*         None
***************************************************************************/
static int get_graphic(int chr)
{
    uint16_t chr_bits;

    #if !LCD_FULL_ISO
    chr = toupper(chr);
    #endif

    /* look up the shape of the character */
    if(chr < FONT_BEGIN || chr > FONT_END) {
        chr_bits = REPLACEMENT;    /* Default, if it can't be displayed. */
    } else {

        chr_bits = font[chr - FONT_BEGIN];
    }
    return chr_bits;
} // static int get_graphic(int32_t chr)


/***************************************************************************   
* Description:  
    Draws a character. LCD[chr_idx] = chr_bits from font.
    It draws a character in the obvious way, by drawing every segment.
    How does it know which segments?  It looks at the font table entry
    for the character.  If a bit is on, the segment is lit, if off, the
    segment is cleared.
* Parameters:
*   Input:
*         The index of the character in the display; 1 = left most
*         The character to write.
*   Output: 
*         None.
* Return Status: 
*         None
***************************************************************************/
void lcd_put_char (int32_t char_idx, char ch)
{                               // Handle up to 16 segments per icon.
    const uint16_t * seg_ptr;
    int seg_cnt;
    int char_bits;

    // If there's no character position, return.
    if (1 > char_idx || char_idx > DIGIT_CNT)
        return; 

    // This code uses the period encoded in the font and character tables.

    /* Get the character's bit pattern. bit 0 = seg A, 1 = B, etc. */
    char_bits = get_graphic((int)ch);

    /* Find the segments that make up a character. */
    seg_ptr = lcd_chrs[ char_idx ];

    /* For every possible segment, write the font's bits to it. */
    for (seg_cnt = 0; seg_cnt < lcd_chr_seg_cnts [ char_idx ]; ++seg_cnt)
    {
        int seg;

        seg = *seg_ptr++;   /* get segment data, go to the next segment */

        /* write the font bit */
        lcd_put_seg (seg, ((0 != (char_bits & 1))?true:false) );

        char_bits >>= 1;     /* select the next font bit */
    }
} // lcd_putchar ()


/***************************************************************************   
* Description:  
*       Write a string in flash to the LCD. (LCD[chr_idx] := "xyz")
* Parameters:
*   Input:
*         chr_idx is the position on LCD, of the first character.
*               Direction can be controlled by the LCD encoding.
*               It's left-to-right with 7 and 14 segments.
*               But Arabic or Hebrew LCDs could be right-to-left.
*               Ideographic displays could be top-to-bottom.
*         str is the pointer to a zero-terminated string.
*   Output: 
*         A string on the LCD.
* Return Status: 
*         None
***************************************************************************/
void lcd_put_str( int32_t char_idx , const char * str )
{
    char ch;                // The character.

    if (NULL == str)
        return;
    if (1 > char_idx)
        return;

    // Exit the loop at the end of the display.
    for(;DIGIT_CNT >= char_idx; ++char_idx)
    {
        ch = *str++;           // Get the next character. 

    // Exit the loop at the end of the string.
    if (0 == ch) break; 

        // For period, add a decimal point in the previous char.
        if ('.' == ch)
            --char_idx;

        lcd_put_char(char_idx, ch);
    }
} // lcd_put_str()


/***************************************************************************   
* Description:  
*       Display integer on the LCD. (LCD[chr_idx] := int32_t)
* Parameters:
*   Input:
*       chr_idx LCD location of first numeral.
*       digit_cnt Maximum count of digits.
*           If a number will not fit in this size, '*' is shown in all
*           digit positions.
*       zero_cnt Number of zeros to force on right.
*       decimal_cnt Number of digits after decimal point.
*           -1 means no decimal point is shown.
*           0 means decimal point is to right of least-significant digit.
*       num is the signed integer number to display.
*           If num < 0, '-' is in the first digit.
*   Output: 
*         A number placed on the LCD.
* Return Status: 
*         None.
***************************************************************************/
const char error_str[] = "Error";
void lcd_put_num(int32_t char_idx, int32_t digit_cnt, int32_t num)
{
    int32_t ch;
    int32_t i;

    if (num < 0)
    {
        num = -num;

        // '-' in the left digit.
        lcd_put_char(char_idx,'-');

        // Make room for '-'.
        ++char_idx;
        --digit_cnt;
    }

    // The digit is made starting from the right side to the left.
    for (i = char_idx + digit_cnt - 1; i >= char_idx; --i)
    {
        ch = num % 10;
        num /= 10;

        // Set the display's segments from commons data in the font arrays.
        lcd_put_char(i, ch + '0');

        // If leading zeros are not required, uncomment 2 lines below.
        //if (0 == num)
        //    break;
    }
    if (0 != num)
    {
        lcd_clear();
        lcd_put_str(LINE1, error_str);
    }
} // lcd_put_num()


/***************************************************************************   
* Description:  
*       Clear the LCD. (LCD[*] := ' ')
*       Note that this only turns off segments that are defined in the
*       LCD tables.  It cannot affect GPIOs that use LCD registers.
* Parameters:
*   Input:
*         None.
*   Output: 
*         All LCD segments are off. (bits cleared to zero)
* Return Status: 
*         None
***************************************************************************/
void lcd_clear (void)
{
    int i;
    /* Write the bit to the pin's pulse-train register */
    if(lcd_image_ptr) // Is DMA live?
    {   // If using RAM images, write to the current image.
        for(i = 0; i < LCD_IMAGE_SIZE; ++i)
        {
            lcd_image_ptr[i] = 0;
        }
    } else {
        LCD->MODE_b.rst = 1; // Toggle the reset to clear the segments.
        LCD->MODE_b.rst = 0;
    }
    LCD->MODE_b.blank = 0; // Enable the segments.
    LCD->MODE_b.on = 0; // Clear test as well
} // lcd_clear ()


/***************************************************************************   
* Description:  
*       Show all the segments of the LCD. (LCD[*] := CHR[*])
*       It is called "test" because it makes the classic LCD test pattern
*       that is shown after reset on many systems.
*       Note that this only turns on segments that are defined in the
*       LCD tables.  It cannot affect GPIOs that use LCD registers.
* Parameters:
*   Input:
*         None.
*   Output: 
*         All LCD segments are on. (bits set to one)
* Return Status: 
*         None
***************************************************************************/
void lcd_test (void)
{
    int i;
    /* Write the bit to the pin's pulse-train register */
    if(lcd_image_ptr) // Is DMA live?
    {   // If using RAM images, write to the current image.
        for(i = 0; i < LCD_IMAGE_SIZE; ++i)
        {
            lcd_image_ptr[0] = 0xff;
        }
    } else {
        LCD->MODE_b.on = 1;
    }
} // lcd_test ()


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

/***************************************************************************   
* Description:  
*       This initializes the LCD system.
*       It reads the LCD table, sets all MAP bits needed, and selects
*       the configuration based on the number of commons.  To save dynamic
*       power, it selects the slowest refresh that is faster than 24Hz.
* Parameters:
*   Input:
*         lcd_ptr_in: NULL, uses registers. RAM array for DMA LCD.
*         lcd_image_cnt: 0: Use registers. Nonzero: count of RAM LCD images.
*   Output: 
*         No parameters, but it sets-up the LCD hardware.
* Return Status: 
*         None
***************************************************************************/
#ifndef LCD_MODE_DEF // If presets, don't calculate them.
#define LCD_COM_CNT 8
static uint8_t com_pin_ary[LCD_COM_CNT] = {
    com0_seg, com1_seg, com2_seg, com3_seg, 
    com4_seg, com5_seg, com6_seg, com7_seg };
#endif
void lcd_init(int *lcd_ptr_in, int lcd_image_cnt)
{
    #ifdef LCD_MODE_DEF // If presets, don't calculate them.
    LCD->MAPL = LCD_MAPL_DEF;
    LCD->MAPH = LCD_MAPH_DEF;
    LCD->MODE_b.rst = 1;  // Clear after pins are defined.
    LCD->MODE = LCD_MODE_DEF;

    #else // If no presets, calculate them.
    int com = 0;    /* Records commons used. */
    int i, chr_idx, seg_idx;   /* character index, segment index in chr */

    LCD->MODE_b.rst = 1;
    LCD->MODE = 0;

    /* Force all pins to non-segment. */
    for(i = 0; i < LCD_PIN_CNT; ++i)
    {
        /* Map pins to SFRs. */
        lcd_map(i,0);
    }

    /* scan the LCD table, and configure every pin
     * used by every character position. */
    for (chr_idx = 1; chr_idx <= LCD_CHR_CNT; ++chr_idx)
    {
        const uint16_t *chr;
        uint8_t seg_cnt;

        chr =  lcd_chrs[chr_idx];
        seg_cnt = lcd_chr_seg_cnts[chr_idx];

        /* Clear every segment, and enable every pin that's used. */
        for (seg_idx = 0; seg_idx < seg_cnt; ++seg_idx)
        {
            int seg, pin;

            /* find the segment description */
            seg = chr[seg_idx];
            pin = seg & 0xFF;

            if (pin < LCD_PIN_CNT) /* the pin is valid */
            {
                /* note which commons are in use */
                com |= 0x0FF & (seg >> 8); /* the common is used */
                /* Map pins to SFRs. */
                lcd_map(pin,1);
            }
        }
    }

    LCD->MODE_b.rst = 1;
    LCD->MODE = 0;

    /* If this logic doesn't work for your app., please change it.
     * For refresh to be invisible it should refresh faster than 24Hz.
     * To save power, it should be as slow as possible. */
    /* com is a bit mask set in the loops just before this.
     * It has a bit set for each common that is used in the display. */
    if (0x00 == com) {
        /* No display segments.  Shut down the LCD (mode remains 0) */
        LCD->MODE = 0;
    } else if (0x01 == com) {
        /* 1 states, static, 1 common in use */
        LCD->MODE_b.mode = 4;
        /* Run at 64Hz > 24Hz flicker rate. */
        LCD->MODE_b.clk = 0;
        /* run from V3P3 with DAC */
        LCD->MODE_b.vmode = 3;
        /* enable the LCD */
        LCD->MODE_b.en = 1;
    } else if (com <= 0x03) {
        com = 0x03;
        /* 2 states, 2 commons in use, 1/2 bias */
        LCD->MODE_b.mode = 2;
        /* clock 01 = 64Hz / 2 = 32 Hz, which is 
         * > 24 hz (human-visible flicker rate) */
        LCD->MODE_b.clk = 0;
        /* run from V3P3 with DAC */
        LCD->MODE_b.vmode = 3;
        /* enable the LCD */
        LCD->MODE_b.en = 1;
    } else if (com <= 0x07) {
        com = 0x07;
        /* 3 states, 3 commons in use, 1/3 bias,
         * low-freq mode to save power. */
        LCD->MODE_b.mode = 1;
        LCD->MODE_b.lowf = 1;
        /* clock 01 = 128Hz / 3 = 42.7 Hz, which is 
         * > 24 hz (human-visible flicker rate) */
        LCD->MODE_b.clk = 1;
        /* run from V3P3 with DAC */
        LCD->MODE_b.vmode = 3;
        /* enable the LCD */
        LCD->MODE_b.en = 1;
    } else if (com <= 0x0F) {
        com = 0x0F;
        /* 4 states, 4 commons in use, 1/3 bias,
         * low-freq. mode to save power. */
        LCD->MODE_b.mode = 0;
        LCD->MODE_b.lowf = 1;
        /* clock 01 = 128Hz / 4 = 32 Hz, which is 
         * > 24 hz (human-visible flicker rate) */
        LCD->MODE_b.clk = 1;
        /* run from V3P3 with DAC */
        LCD->MODE_b.vmode = 3;
        /* enable the LCD */
        LCD->MODE_b.en = 1;
    } else if (com <= 0x1F) {
        com = 0x1F;
        /* 5 states, 5 commons in use, 1/3 bias,
         * low-freq mode to save power. */
        LCD->MODE_b.mode = 5;
        LCD->MODE_b.lowf = 1;
        /* clock 01 = 128Hz / 5 = 25.6 Hz, which is 
         * > 24 hz (human-visible flicker rate) */
        LCD->MODE_b.clk = 1;
        /* run from V3P3 with DAC */
        LCD->MODE_b.vmode = 3;
        /* enable the LCD */
        LCD->MODE_b.en = 1;
    } else if (com <= 0x3F) {
        com = 0x3F;
        /* 6 states, 6 commons in use, 1/3 bias, set
         * low-freq mode to save power. */
        LCD->MODE_b.mode = 6;
        LCD->MODE_b.lowf = 1;
        /* clock 10 = 256Hz / 6 = 43 Hz, which is 
         * > 24 hz (human-visible flicker rate) */
        LCD->MODE_b.clk = 2;
        /* run from V3P3 with DAC */
        LCD->MODE_b.vmode = 3;
        /* enable the LCD */
        LCD->MODE_b.en = 1;
    } else {
        com = 0xFF;
        /* 8 states, 8 commons in use, 1/3 bias,
         * low-freq mode to save power. */
        LCD->MODE_b.mode = 7;
        LCD->MODE_b.lowf = 1;
        /* clock 10 = 256Hz / 8 = 32 Hz, which is 
         * > 24 hz (human-visible flicker rate) */
        LCD->MODE_b.clk = 2;
        /* run from V3P3 with DAC */
        LCD->MODE_b.vmode = 3;
        /* set all the commons to be in use */
        LCD->MODE_b.all_com = 1;
        /* enable the LCD */
        LCD->MODE_b.en = 1;
    }

    /* Set up the commons in LCDMAP. */
    for(i = 0; i < LCD_COM_CNT; ++i)
    {
        if(com & (1UL << i)) // If a common is needed.
        {
            lcd_map(com_pin_ary[i],1); // map it.
        }
    }

    // Record the set-up for use in presets.
    lcd_mode = LCD->MODE;
    lcd_mapl = LCD->MAPL;
    lcd_maph = LCD->MAPH;
    #endif // Had to calculate the presets.

    LCD->CNTL_b.dac = 0x10; // DAC is half-range.

    // Enable page RAM if needed.
    if(NULL == lcd_ptr_in
       || 0 >= lcd_image_cnt
       || lcd_image_cnt > LCD_IMAGE_CNT_MAX)
    {
        lcd_ptr_in = NULL;
        lcd_image_cnt = 0;
    }
    lcd_image_ptr = lcd_image_base = (uint8_t *)lcd_ptr_in;
    lcd_image_last = lcd_image_cnt - 1;
    // Enable DMA LCD operation.
    LCD->CNTL_b.page_addr = (((uint32_t)lcd_image_base) >> 2);
    LCD->CNTL_b.num_pages = lcd_image_cnt;
    // DMA starts when the page timer is set.
} // void lcd_init(void)


/***************************************************************************   
* Description:  
*   Power down the LCD system; LCDs might be
*   damaged by the DC bias if lcd_E = 0.
*   It forces the LCD to blank and disables any clocks.
* Parameters:
*   Input:
*         None.
*   Output: 
*         Places the LCD in a low-power state.  Makes it blank, too.
* Return Status: 
*         None
***************************************************************************/
void lcd_deinit(void)
{
    /* Disable and clear to avoid DC bias */
    LCD->MODE_b.en = 0;
    LCD->CNTL_b.page_dma_en = 0;   // Disable the DMA.
    lcd_image_base = NULL;
    lcd_image_ptr = NULL;
    lcd_clear();
} // void lcd_deinit(void)


/***************************************************************************   
* Description:  
*   Select the LCD image to write.
*   Start the DMA on the first non-zero image index.
*   If RAM was not selected for initialization this is ignored.
*   Input:
*         The image index 0.. 31; 
*   Output: 
*         Selects the image used by the output routines.
* Return Status: 
*         None
***************************************************************************/
void lcd_image(int image_idx)
{
    // If DMA ram exists, and the image index is valid.
    if( NULL != lcd_image_base
        && 0 <= image_idx 
        && image_idx <= (int)lcd_image_last )
    {   // Set the image pointer.
        lcd_image_ptr = lcd_image_base + (image_idx * LCD_IMAGE_SIZE);
    } else {
        lcd_image_ptr = NULL; // prevent writes to the nonexistent image.
    }
}


/***************************************************************************   
* Description:  
*   Select the speed of the LCD image switching.
*   Input:
*         The time in seconds.
*   Output: 
*         
* Return Status: 
*         None
***************************************************************************/
void lcd_page_sec(int sec_cnt)
{
    LCD->DMA_PTIME_b.init = 0;
    LCD->DMA_PTIME_b.scroll = 4 * sec_cnt;
    if(lcd_image_ptr)   // If the RAM is set up.
    {
        LCD->CNTL_b.page_dma_en = 1;   // Enable the DMA.
    }
}


/****************************************************************************
 * Copyright (C) 2005-2017 Silergy Corp., All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
