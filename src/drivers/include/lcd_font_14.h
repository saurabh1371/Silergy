/**************************************************************************
   This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: A font for a 14-segment display.
 * Tells which segments to turn on and off in a given group of segments.
 *
 * Copyright 2014, Silergy Inc. All rights reserved.
 *
 * $Id: lcd_font_14.h 17071 2018-03-21 00:40:21Z rvandewa $
 ***************************************************************************/

#ifndef LCD_FONT_H
#define LCD_FONT_H

// Generic LCD character font for a 14-segment display.
//       -----a-----   a = 0001
//      |\    |    /|  b = 0002
//      | \   i   / |  c = 0004
//      f  h  |  j  b  d = 0008
//      |   \ | /   |  e = 0010
//      |    \|/    |  f = 0020
//       --g-- --k---  g = 0040
//      |    /|\    |  h = 0080
//      |   / | \   |  i = 0100
//      e  n  m  l  c  j = 0200
//      | /   |   \ |  k = 0400
//      |/    |    \|  l = 0800 // If on the left, see LEFT_L, below
//       -----d-----   m = 1000
//                     n = 2000 // If on the right, see LEFT_L, below
//                    dp = 4000 (decimal point)

// If segment l is on the left, not the right, and segment n
// is on the right, not the left, define LEFT_L as 1 in the glass .h
// file before including this font.
#ifndef LEFT_L
#define LEFT_L 0    // Default to diagram above.
#endif
// Define the first 128 characters of UTF-8.
enum ePUNCTUATION {
    BLANK           =   0x0000,
    EXCLAMATION_POINT = 0x4006,
    DOUBLE_QUOTE    =   0x0120,
    NUMBER_SIGN     =   0x1552,
    DOLLAR_SIGN     =   0x156D,
    PERCENT_SIGN    =   0x2EE4,
    AMPERSAND       =   0x290C,
    APOSTROPHE      =   0x0200,
#if LEFT_L
    LEFT_PARENTHESIS=   0x2200,
    RIGHT_PARENTHESIS = 0x0880,
#else
    LEFT_PARENTHESIS=   0x0A00,
    RIGHT_PARENTHESIS = 0x2080,
#endif
    ASTERISK        =   0x2EC0,
    PLUS_SIGN       =   0x1540,
#if LEFT_L
    COMMA           =   0x0800,
#else
    COMMA           =   0x2000,
#endif
    MINUS_SIGN      =   0x0440,
    //PERIOD          =   0x0010,
    PERIOD          =   0x4000,     // Uses the decimal point(s).
#if LEFT_L
    FRONT_SLASH     =   0x0A00,
#else
    FRONT_SLASH     =   0x2200,
#endif
    COLON           =   0x0110,
#if LEFT_L
    SEMICOLON       =   0x0820,
#else
    SEMICOLON       =   0x2020,
#endif
    LESS_THAN       =   0x0600,
    EQUAL           =   0x0441,
    GREATER_THAN    =   0x00C0,
    QUESTION_MARK   =   0x1403,
    AT_SIGN         =   0x053B,
    RIGHT_BRACKET   =   0x0039,
#if LEFT_L
    BACK_SLASH      =   0x2080,
#else
    BACK_SLASH      =   0x0880,
#endif
    YEN             =   0x16C0,     // For JIS-X-201.
    LEFT_BRACKET    =   0x000F,
    CARET           =   0x2800,
    UNDERLINE       =   0x0008,
    ACCENT          =   0x0080,
#if LEFT_L
    LEFT_CURLY_BRACKET  =   0x2240,
#else
    LEFT_CURLY_BRACKET  =   0x0A40,
#endif
    VERTICAL_LINE   =   0x1100,
#if LEFT_L
    RIGHT_CURLY_BRACKET =   0x0C80,
#else
    RIGHT_CURLY_BRACKET =   0x2480,
#endif
    TILDE           =   0x0444,     // coded as logical-not symbol.
    MACRON          =   0x0001,     // "overline" For JIS-X-201.
    DEL             =   0x7FFF,
    REPLACEMENT     =   0x0463,     // small square box, unicode replacement
    QUESTION_INV    =   0x045A,     // Inverted question mark
#if LEFT_L
    CENTS_SIGN      =   0x0E58
#else
    CENTS_SIGN      =   0x2C58
#endif
};
#define TEST DEL
#define OVERFLOW_LCD ASTERISK
#define SEG_HEARTBEAT DP_0 // DP[0]
#define END_LCD_STR -1

enum eFIGURES {
#if LEFT_L
    FIGURE_0 = 0x0A3F, 
#else
    FIGURE_0 = 0x223F, 
#endif
    FIGURE_1 = 0x0006, FIGURE_2 = 0x045B, FIGURE_3 = 0x044F,
    FIGURE_4 = 0x0466, FIGURE_5 = 0x046D, FIGURE_6 = 0x047D, FIGURE_7 = 0x0007,
    FIGURE_8 = 0x047F, FIGURE_9 = 0x046F, DP    = 0x4000 };

enum eALPHABET {
    ALPHA_A = 0x0477, ALPHA_B = 0x150F, ALPHA_C = 0x0039,
    ALPHA_D = 0x110F, ALPHA_E = 0x0479, ALPHA_F = 0x0471,
    ALPHA_G = 0x043D, ALPHA_H = 0x0476, ALPHA_I = 0x1109,
    ALPHA_J = 0x001E, 
#if LEFT_L
    ALPHA_K = 0x2270, 
#else
    ALPHA_K = 0x0A70, 
#endif
    ALPHA_L = 0x0038,
    ALPHA_M = 0x02B6, 
#if LEFT_L
    ALPHA_N = 0x20B6, 
#else
    ALPHA_N = 0x08B6, 
#endif
    ALPHA_O = 0x003F,
    ALPHA_P = 0x0473, 
#if LEFT_L
    ALPHA_Q = 0x203F, ALPHA_R = 0x2473,
#else
    ALPHA_Q = 0x083F, ALPHA_R = 0x0C73,
#endif
    ALPHA_S = 0x046D, ALPHA_T = 0x1101, ALPHA_U = 0x003E,
#if LEFT_L
    ALPHA_V = 0x0A30, 
#else
    ALPHA_V = 0x2230, 
#endif
    ALPHA_W = 0x2836, ALPHA_X = 0x2A80,
    ALPHA_Y = 0x1280, 
#if LEFT_L
    ALPHA_Z = 0x0A09, 
#else
    ALPHA_Z = 0x2209, 
#endif
    ALPHA_h = 0x0474,
    ALPHA_c = 0x0458, 
#if LEFT_L
    ALPHA_N_TILDE = 0x2055
#else
    ALPHA_N_TILDE = 0x0855
#endif
};

/***************************************************************************
 * Copyright (C) 2014 Silergy Inc. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Inc.
 ***************************************************************************/
#endif  /* lcd_font_14.h */

