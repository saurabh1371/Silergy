 /**************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: a font for 7-segment displays. 
 *
 * Copyright 2014, Silergy Inc. All rights reserved.
 *
 * $Id: lcd_font_7.h 15428 2017-09-28 22:05:38Z rvandewa $
 **************************************************************************/
#ifndef _LCD_SYMBOLS
#define _LCD_SYMBOLS

// Generic LCD Symbols include file.
// Generic displays have all these icons.
//
enum ePUNCTUATION {
    BLANK           =   0x0000,
    EXCLAMATION_POINT     =   0x0006,
    DOUBLE_QUOTE    =   0x0022,
    NUMBER_SIGN     =   0x005C,
    DOLLAR_SIGN     =   0x006C,
    PERCENT_SIGN    =   0x0024,
    AMPERSAND       =   0x0054,
    APOSTROPHE      =   0x0020,
    LEFT_PARENTHESIS=   0x0039,
    RIGHT_PARENTHESIS = 0x000F,
    ASTERISK        =   0x0049,
    PLUS_SIGN       =   0x0046,
    COMMA           =   0x0010,
    MINUS_SIGN      =   0x0040,
    //PERIOD          =   0x0080,
    PERIOD          =   0x0100,     // Uses the decimal point(s).
    FRONT_SLASH     =   0x0052,
    COLON           =   0x0180,
    SEMICOLON       =   0x0104,
    LESS_THAN       =   0x0061,
    EQUAL           =   0x0041,
    GREATER_THAN    =   0x0043,
    QUESTION_MARK   =   0x0053,
    AT_SIGN         =   0x0035,
    RIGHT_BRACKET   =   0x0039,
    BACK_SLASH      =   0x0064,
    YEN             =   0x006E,     // For JIS-X-201.
    LEFT_BRACKET    =   0x000F,
    CARET           =   0x0023,
    UNDERLINE       =   0x0008,
    ACCENT          =   0x0020,
    LEFT_CURLY_BRACKET  =   0x0039,
    VERTICAL_LINE   =   0x0006,
    RIGHT_CURLY_BRACKET =   0x000F,
    TILDE           =   0x0021,     // coded as logical-not symbol.
    MACRON          =   0x0001,     // "overline" For JIS-X-201.
    DEL             =   0x7FFF,
    REPLACEMENT     =   0x0063,     // small square box, unicode replacement
    QUESTION_INV    =   0x005B,     // Inverted question mark
    CENTS_SIGN      =   0x0058      // small c
};
#define TEST DEL
#define OVERFLOW_LCD ASTERISK
#define END_LCD_STR -1
enum eMISC {
    DP_4 = 0x08, DP_5 = 0x10, DP_6  = 0x20, DP_7 = 0x40, DP_ALL = 0x7F,
    L_0  = 0x00, L_2  = 0x01, L_4   = 0x02, L_6  = 0x04, L_ALL  = 0x07,
    L_TIME = 0x03
};

enum eFIGURES {
	FIGURE_0 = 0x3F, FIGURE_1 = 0x06, FIGURE_2 = 0x5B, FIGURE_3 = 0x4F,
    FIGURE_4 = 0x66, FIGURE_5 = 0x6D, FIGURE_6 = 0x7D, FIGURE_7 = 0x07,
    FIGURE_8 = 0x7F, FIGURE_9 = 0x67, DP = 0x0080, CL = 0x0100
};
#define SEG_HEARTBEAT DP_7

enum eALPHABET {
	ALPHA_A = 0x77, ALPHA_B = 0x7C, ALPHA_C = 0x58, ALPHA_D = 0x5E,
    ALPHA_E = 0x79, ALPHA_F = 0x71, ALPHA_G = 0x3D, ALPHA_H = 0x76,
    ALPHA_I = 0x06, ALPHA_J = 0x1E, ALPHA_K = 0x75, ALPHA_L = 0x38,
    ALPHA_M = 0x2B, ALPHA_N = 0x54, ALPHA_O = 0x3F, ALPHA_P = 0x73,
    ALPHA_Q = 0x67, ALPHA_R = 0x50, ALPHA_S = 0x6D, ALPHA_T = 0x78,
    ALPHA_V = 0x3E, ALPHA_U = 0x1C, ALPHA_W = 0x6A, ALPHA_X = 0x76,
    ALPHA_Y = 0x6E, ALPHA_Z = 0x5B, ALPHA_c = 0x58, ALPHA_h = 0x74,
    ALPHA_N_TILDE = 0x55 };


/***************************************************************************
 * Copyright (C) 2014 Silergy Inc. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of SIlergy Inc.
 ***************************************************************************/
#endif  /* lcd_symbols.h */

