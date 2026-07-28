
/* File: vim878.h 
 * This file is normally included in main\options.h
 * Defines icons, line sizes and positions, etc. for this display
 * Note, this is for a second revision, in which pin 5 of the LCD
 * should be attached to VPULSE.
 */

enum DipSeqAuto{
DISP_DATE,//1
DISP_TIME,//0
DISP_V,//6
DISP_I_S,//7
DISP_I_N,//8
DISP_FREQ,//9
DISP_PF,//10
DISP_KW_S,
DISP_KWH,//2
DISP_MD,//3
MD_TIME,//4
MD_DATE,//5
DISPLAY_AUTO_END=50,	


DISP_KW_N,
DISP_EE_TEST,

};


#define KWh_Icon 	lcd_put_icon(T20);\
									lcd_put_icon(T14);\
									lcd_put_icon(T17);

#define KW_Icon  	lcd_put_icon(T20);\
									lcd_put_icon(T14);

#define V_Icon 		lcd_put_icon(T15);

#define A_Icon 		lcd_put_icon(T16);
								

//UPPCL
const  char test_str[][15] =
{
    //0    1    2    3    4   5   6     7   8   9       10        11   12      13    14     15      16     17       18   19     20
    "PF","LG","LD","LAB","P","N","EE","V","SP","NP","SILERG5","COMM","CLr2","CLr3","CLr1","UnLoc","CLrc","rESEt","SLno","CAL","HZ"
};

// Set-up values for the LCD hardware on Maxim's India PCB.
#define LCD_MAPL_DEF 0x000783FF	//0x00078BFF
#define LCD_MAPH_DEF 0x000000F0
#define LCD_MODE_DEF 0xFF000048//0xfD000008

// Minus character.
#define LCD_MINUS (36)

// The following data define the BQ09193A glass, and need not change on
// different PCBs.

// Define the digits in the display.
#define DIGIT_CNT 8 // 8 digits, total.


/* Icon mapping, indexes arrays in the glass file for segment and common.  */
#define LCD_FIRST_PERIOD 1 // 1st character from left
#define LCD_LAST_PERIOD 6  // 6th character from left
enum icon
{
P4 ,
T6 ,
T1 ,
P2 ,
T5 ,
P1 ,
T4 ,
T3 ,
T2 ,
P3 ,
T10,
T11,
T12,
T13,
T15,
T16,
T17,
T7 ,
T8 ,
T9 ,
T20,
T14,
T18,
T19

,//T6
//EARTH,
//REV,
//NM,
//MAG,
//BILL,
//MD,
//T2,
//T3,
//_h,
//TOD,
//COPEN,
//TIME_Icon,
//ON,
//OK,
//P1,
//COL,
//T1,
//_v,
//_k,
//P4,
//P3,
//COMM,
//P2
//
};

// This driver has a debug routine to make debug output
// when the meter is in an extreme low power mode.
// It puts out two numbers that flash alternately and automatically.  
// "two" has a decimal point on the right.
void lcd_debug(int one_1, int two_2);
/* File: vim878.C */
/* Include your LCD file here as derived in xl sheet */

/* List of icon segment indexes, 2nd Column from Icon map */
const  unsigned char icon_seg_8COM[] =
{ 
0,
1,
2,
3,
4,
5,
6,
7,
8,
9,
0,
0,
0,
0,
0,
0,
0,
1,
1,
1,
1,
1,
1,
1

};

/* Commons connected to icons, Last Column from Icon map  */
const  unsigned char icon_common_8COM[] =
{
1  ,
1  ,
1  ,
1  ,
1  ,
1  ,
1  ,
1  ,
1  ,
1  ,
2  ,
4  ,
8  ,
16 ,
32 ,
64 ,
128,
2  ,
4  ,
8  ,
16 ,
32 ,
64 ,
128

};


/* find segments for characters 1 to 6. */
const unsigned char char_seg_8COM[]=
{
   /* find 0 segments (A,B,C,D) for characters 1 to 8. */
0,
7,
6,
5,
4,
3,
2,
9,
8,
0
};




// font 0..9A..Z-; Numbers are "commons", a bit
// pattern written to a segment register in LCDINDADDR to make
// part of a 14-segment character.
const unsigned char font_8COM[] =
{
    /* font[0][], commons for font for characters 0 to 9 and A to Z, '-'  */
238,
72,
186,
218,
92,
214,
246,
74,
254,
94,
126,
244,
166,
248,
182,
54,
230,
124,
36,
200,
180,
164,
110,
110,
238,
62,
94,
48,
214,
180,
236,
236,
236,
124,
220,
186,
18
    };

/* end of file */


/* end of file */

