/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: SoC POWER METER - Help Messages. 
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * $Id: help.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
 #include <stdlib.h>
 #include "options.h"
 #include "help.h"  // Check definitions.

#if HELP 
//***************************************************************************
// How to use help
const char Usage_1[] = {"?        : Command Line Interpreter On-line help\r\n"};
const char Usage_2[] = {"Usage: ?<char> or ? to get this help page\r\n"};
const char Usage_3[] = {"Where <char> is an uppercase letter of the command. \r\n"};
const char Usage_4[] = {"The following commands/<char> are available: \r\n"};
const char Usage_5[] = {"  ,  - Repeat last command    : /  - Ignore rest of line\r\n"};
const char Usage_6[] = {"  ]  - Access CE Data RAM     : )  - Access MPU Data RAM\r\n"};
const char Usage_7[] = {"  B  - Battery mode commands  : C  - CE Controls\r\n"};
const char Usage_8[] = {"  D  - Read and write DIOs    : EE - EEPROM Controls\r\n"};
const char Usage_9[] = {"  ER - Error Recording        : F  - Flash Controls\r\n"};
const char Usage_10[]= {"  I  - Identification message : M  - Meter Display Control\r\n"};
const char Usage_11[]= {"  P  - Pulse source for WPULSE: PWM- PWM Control\r\n"};
const char Usage_12[]= {"  RA - Data RAM R/W access    : RM - SFR R/W access\r\n"};            
const char Usage_13[]= {"  RT - RTC Control            : T  - Trim Controls\r\n"};            
const char Usage_14[]= {"  V  - Device Driver Versions : Wx - WFI power and clock management\r\n"};
#if REMOTES
const char Usage_15[]= {"  X  - Read remote registers  : Zx - Reset commands.\r\n"};
#else
const char Usage_15[]= {"  Zx - Reset commands.\r\n"};
#endif
const char Usage_16[]= {"  <ctrl>x .. <ctrl>y  - Start autoreport, stop it.\r\n"};
const char Usage_17[]= {"\r\n"};
const char Usage_18[]= {"For Example: ?C to get help on Compute Engine Control.\r\n"};

const char * const Usage[] = {
    Usage_1,    Usage_2,    Usage_3,    Usage_4,    Usage_5,
    Usage_6,    Usage_7,    Usage_8,    Usage_9,    Usage_10,
    Usage_11,   Usage_12,   Usage_13,   Usage_14,   Usage_15,
    Usage_16,   Usage_17,   Usage_18, 
    NULL
};


// CE Data Memory Access.
const char Help_DA_1[] = {"]           : CE Data Access\r\n"};
const char Help_DA_2[] = {"]x..        : Starting CE data address\r\n"}; 
const char Help_DA_3[] = {" ..???..    : Read consecutive 32-bit words in Decimal\r\n"};
const char Help_DA_4[] = {" ..$$$..    : Read consecutive 32-bit words in Hex\r\n"};
const char Help_DA_5[] = {" ..=+n=+n.. : Write consecutive decimal memory values\r\n"};
const char Help_DA_6[] = {" ..=n=n..   : Write consecutive hex memory values\r\n"};
const char Help_DA_7[] = {"]U          ; Update default version of CE Data\r\n"};
const char Help_DA_8[] = {"]80..       : = CESTATUS, TEMP_X, FREQ_X, MAINEDGE_X\r\n"};
const char Help_DA_9[] = {"]84.. ]87.. : = WSUM,W0SUM,W1SUM; VARSUM, etc.\r\n"};
const char Help_DA_10[]= {"]8A.. ]8e.. : = I0SQSUMH/L,I1SQSUMH/L; V0SQSUM,V1SQSUM\r\n"};
const char Help_DA_11[]= {"]10.. ]13.. : = CAL_I0, CAL_V0, PH_ADJ0; for IB\r\n"};
const char Help_DA_12[]= {"]16..       : = I1 SCALE, IB SCALE\r\n"};
const char Help_DA_13[]= {"]20..       : = CE Config, WRATE (i.e kH/pulse adjustment)\r\n"};
const char Help_DA_14[]= {"]23..       : = SUMPRE, SAG_THR, CREEP_THR\r\n"};
const char Help_DA_15[]= {"]27.. ]2b.. : = QUANTS: V0,I0,W0,VAR0, V1,I1,W1,VAR1.\r\n"};
const char Help_DA_16[]= {"]54..       : = NFREQ\r\n"};
const char Help_DA_17[]= {"]55..       : = GAIN_ADJ0-2 (temp. compensation gains)\r\n"};
const char Help_DA_18[]= {"]58.. ]5e.. : = APULSEW,WPULSE_CTR,WPULSE_FRAC,WSUM_ACCUM; APULSER, etc.\r\n"};
const char Help_DA_19[]= {"\r\n"};
const char Help_DA_20[]= {"For example :]85?? - Read W0SUM, W1SUM in decimal\r\n"};
const char Help_DA_21[]= {"             ]10=4000=4000 - Set CAL_I0 and CAL_V0 to unity gain in hex.\r\n"};

const char * const  DAHelp[] = {
    Help_DA_1,  Help_DA_2,  Help_DA_3,  Help_DA_4,  Help_DA_5,         
    Help_DA_6,  Help_DA_7,  Help_DA_8,  Help_DA_9,  Help_DA_10,
    Help_DA_11, Help_DA_12, Help_DA_13, Help_DA_14, Help_DA_15,
    Help_DA_16, Help_DA_17, Help_DA_18, Help_DA_19, Help_DA_20,
    Help_DA_21,
    NULL
};


// MPU Data Memory Access.
const char Help_MA_1[]  = {")           : MPU Data Access\r\n"};
const char Help_MA_2[]  = {")x..        : Starting MPU data address\r\n"}; 
const char Help_MA_3[]  = {" ..???..    : Read consecutive 32-bit words in Decimal\r\n"};
const char Help_MA_4[]  = {" ..$$$..    : Read consecutive 32-bit words in Hex\r\n"};
const char Help_MA_5[]  = {" ..=n=n..   : Write consecutive memory' values\r\n"};
const char Help_MA_6[]  = {"\r\n"};
const char Help_MA_7[]  = {"For example :)08$$$ - Read MPU data words 0x08, 0x09 and 0x0A.\r\n"};
const char Help_MA_8[]  = {"             )04=12345678=9876ABCD - Write two words starting @ 0x04\r\n"};
const char Help_MA_9[]  = {"0:i_min    C:i_cal      18:cal ver  26-29:dmd mon,mday,h,m\r\n"};
const char Help_MA_10[] = {"1:cfg      D:theta_cal  1A:puls_adr 2A:tamper_sec\r\n"};
const char Help_MA_11[] = {"2:v_min    E:lcd_idx    1B:interval 2B:sag_sec\r\n"};
const char Help_MA_12[] = {"3:i_max    F:lcd_bit    1D:wsum/cnt 2C:operating_sec\r\n"};
const char Help_MA_13[] = {"4:v_max    10:mfr.ID    1E:v_nom    2D:reg_cnt\r\n"};
const char Help_MA_14[] = {"5:wh, wh/p 11:mtr no.   1F:acc_cnt  2E:ver_hash\r\n"};
const char Help_MA_15[] = {"6:p,  wh/p 12:gmt_offset 20:bit_log\r\n"};
const char Help_MA_16[] = {"7:i_limit  13:TC_AB     21:wh_im    30:wake_src\r\n"};
const char Help_MA_17[] = {"8:v_limit  14:TC_CD     22:wh_ex    31:wake\r\n"};
const char Help_MA_18[] = {"9:i_neutral 15:preamp   23:varh_im  32:temp_c\r\n"};
const char Help_MA_19[] = {"A:s_cal    16:cal_src   24:varh_ex  33-35:ppmc\r\n"};
const char Help_MA_20[] = {"B:v_cal    17:cal_cnt   25:dmd_max  36-38:ppmc2\r\n"};
const char Help_MA_21[] = {"\r\n"};
const char Help_MA_22[] = {")1=2 Clear reg. )1=4 Reset demand, )1=1 do KWh.\r\n"};

const char * const  MAHelp[] = {
    Help_MA_1,  Help_MA_2,  Help_MA_3,  Help_MA_4,  Help_MA_5,         
    Help_MA_6,  Help_MA_7,  Help_MA_8,  Help_MA_9,  Help_MA_10,
    Help_MA_11, Help_MA_12, Help_MA_13, Help_MA_14, Help_MA_15,
    Help_MA_16, Help_MA_17, Help_MA_18, Help_MA_19, Help_MA_20,
    Help_MA_21, Help_MA_22, 
    NULL
};


// Battery mode commands
const char Help_B_1[] = {"B    : Battery mode commands\r\n"};
const char Help_B_2[] = {"BL   : LCD only battery mode\r\n"};
const char Help_B_3[] = {"BRn  : Set RTCWAKE to hex n\r\n"};
const char Help_B_4[] = {"BS   : Sleep mode\r\n"};
const char Help_B_5[] = {"BT   : Command a battery test- only in mission mode\r\n"};
const char Help_B_6[] = {"BWSn : Wake on nth second in hex, after LCD or sleep\r\n"};
const char Help_B_7[] = {"\r\n"};
const char Help_B_8[] = {"get B> prompt indicating brownout mode, type:\r\n"};
const char Help_B_9[] = {"BWS10\r\n"};
const char Help_B_10[]= {"BS\r\n"};
const char Help_B_11[]= {"Will sleep for 16 (0x10) seconds and then wake.\r\n"};
const char Help_B_12[]= {"NOTE: TEMP_PER value is the Wake Timer's resolution in secs.\r\n"};
const char Help_B_13[]= {"      Demo Code sets TEMP_PER value to 8 secs, 2^(8-5)=8secs.\r\n"};
const char Help_B_14[]= {"      Therefore, Wake Timer works in multiple of 8 secs.\r\n"};
const char * const  BHelp[] = {
    Help_B_1,   Help_B_2,   Help_B_3,   Help_B_4,   Help_B_5,   
    Help_B_6,   Help_B_7,   Help_B_8,   Help_B_9,   Help_B_10,  
    Help_B_11,  Help_B_12,  Help_B_13,  Help_B_14,  
    NULL
};


// Compute Engine Settings Control.
const char Help_C_1[]= {"C         : Compute Engine Controls\r\n"};
const char Help_C_2[]= {"CEn       : Compute Engine Enable (1 -> Enable)\r\n"};
const char Help_C_3[]= {"CL...     : CaLibration methods.\r\n"};
const char Help_C_5[]= {"CR...     : Select RTM outputs and enable/disable.\r\n"};
const char Help_C_6[]= {"CS        : CE outputs simulate autocal signal. (CE is off.)\r\n"};
const char Help_C_7[]= {"CTn.m     : Select TMUX input m on TMUXn (n=1,2).\r\n"};
const char Help_C_8[]= {"\r\n"};
const char Help_C_9[]={"?CL or ?CR for further help.\r\n"};

const char * const CHelp[] = {
    Help_C_1, Help_C_2,  Help_C_3,  
    Help_C_5, Help_C_6,  Help_C_7,
    Help_C_8, Help_C_9,
    NULL
};


const char Help_CL_1[]= {"CL   : Calibration controls; )B=Vcal, )C=Ical\r\n"};
const char Help_CL_2[]= {"CLB  : Begin calibration for )A accum. intervals.\r\n"};
const char Help_CL_3[]= {"CLC  : Recalculate after change of i_max, etc.\r\n"};
const char Help_CL_4[]= {"CLS  : Save calibration to EEPROM.\r\n"};
const char Help_CL_5[]= {"CLR  : Restore calibration from EEPROM.\r\n"};
const char Help_CL_6[]= {"CLD  : Restore calibration to defaults.\r\n"};

const char * const  CLHelp[] = {
    Help_CL_1,  Help_CL_2,  Help_CL_3,  
	Help_CL_4,  Help_CL_5,  Help_CL_6,
    NULL };


const char Help_CR_1[]= {"CR         : CE RTM controls\r\n"};
const char Help_CR_2[]= {"CREn       : Enable/Disable RTM output.\r\n"};
const char Help_CR_3[]= {"CRSn.n.n.n : Select four RTM outputs.\r\n"};

const char * const  CRHelp[] = {
    Help_CR_1,  Help_CR_2,  Help_CR_3,  NULL
};


// DIO Access.
const char Help_DIO_1[] = {"D           : DIO register access\r\n"};
const char Help_DIO_2[] = {"Dx..        : x = Starting DIO index in decimal\r\n"}; 
const char Help_DIO_3[] = {" ..$$$..    : Read consecutive 8-bit DIO registers in Hex\r\n"};
const char Help_DIO_4[] = {" ..???..    : Read consecutive 8-bit DIO registers in Decimal\r\n"};
const char Help_DIO_5[] = {" ..=n=n..   : Write consecutive DIO values\r\n"};
const char Help_DIO_6[] = {"\r\n"};
const char Help_DIO_7[] = {"For example :D42$$ - Read WPULSE, RPULSE in hex\r\n"};
const char Help_DIO_8[] = {"             D42=2 - Turn on WPULSE LED (make it low).\r\n"};

const char * const  DIOHelp[] = {
    Help_DIO_1,  Help_DIO_2,  Help_DIO_3,  Help_DIO_4,  Help_DIO_5,         
    Help_DIO_6,  Help_DIO_7,  Help_DIO_8,  
    NULL
};


// (EE)Prom controls.
const char Help_EE_1[]= {"EE         :EEPROM control\r\n"};
const char Help_EE_3[]= {"EEE        :Erase the EEprom.\r\n"};
const char Help_EE_4[]= {"EERa.b     :Read EEprom at address 'a' for 'b' bytes.\r\n"};
const char Help_EE_5[]= {"EESc...cc; :Write characters to buffer (sets Write length)\r\n"};
const char Help_EE_6[]= {"EETa       :Transmit buffer to EEprom at address 'a'.\r\n"};
const char Help_EE_7[]= {"EEWn.n...n :Write values to buffer\r\n"};
const char Help_EE_8[]= {"For example: eeshello\r\n"};
const char Help_EE_9[]= {"             eet0210\r\n"};
const char Help_EE_10[]={"             Write 'hello' starting at EEProm address 0x210.\r\n"};

const char * const  EEHelp[] = {
    Help_EE_1,  Help_EE_3,  Help_EE_4,         
    Help_EE_5,  Help_EE_6,  Help_EE_7,  Help_EE_8,        
    Help_EE_9,  Help_EE_10,
    NULL
};

// Error Recording.
const char Help_ER_1[]= {"ER         :Error Recording\r\n"};
const char Help_ER_2[]= {"ERC        :Clear all errors, resense.\r\n"};
const char Help_ER_3[]= {"ERD        :Display errors.\r\n"};
const char Help_ER_4[]= {"ERSn       :Set error n.\r\n"};
const char Help_ER_5[]= {"ERUn       :Unset error n.\r\n"};
const char Help_ER_6[]= {"For example:\r\n"};
const char Help_ER_7[]= {" ers10\r\n"};
const char Help_ER_8[]= {" erd\r\n"};
const char Help_ER_9[]= {"           Set error 10, display errors.\r\n"};

const char * const  ERHelp[] = {
    Help_ER_1,  Help_ER_2,  Help_ER_3,  Help_ER_4,         
    Help_ER_5,  Help_ER_6, Help_ER_7,  Help_ER_8, 
    Help_ER_9, 
    NULL
};


// Flash control
const char Help_F_1[] =  {"F       : (F)lash controls\r\n"};
const char Help_F_2[] =  {"FEa     : Erase Flash page at address 'a'. \r\n"};
const char Help_F_3[]=   {"          Where 'a' is a 32-bit aligned byte address between 0x0000 to 0xFE00 for 64KB.\r\n"};
const char Help_F_4[] =  {"FRa.b   : Read Flash long-word starting at 'a' for 'b' long-words\r\n"};
const char Help_F_4A[] = {"          where 'a' is a 32-bit aligned byte address.\r\n"};
const char Help_F_5[] =  {"FTa     : Write Flash long-word at addr 'a'.\r\n"};
const char Help_F_6[] =  {"FWn.n.n : Array of long-words, sets write length (max is 8).\r\n"};
const char Help_F_7[] =  {"\r\n"};
const char Help_F_8[] =  {"For example: FWabcd1234.5678abcd\r\n"};
const char Help_F_9[] =  {"             FTf000\r\n"};
const char Help_F_10[] = {"             Write 0xabcd1234 to flash at addr 0xf000 and \r\n"};
const char Help_F_11[] = {"             0x5678abcd to flash at addr 0xf004.\r\n"};

const char * const  FHelp[] = {
    Help_F_1,   Help_F_2,   Help_F_3,   Help_F_4,    Help_F_4A, Help_F_5,   
    Help_F_6,   Help_F_7,   Help_F_8,   Help_F_9,    Help_F_10, Help_F_11,
    NULL
};


// (I)nformation messages
const char Help_I_1[] = {"I           : Information messages\r\n"};
const char Help_I_2[] = {"For Example : I - returns version data\r\n"};

const char * const  IHelp[] = {
    Help_I_1,  Help_I_2,  
    NULL
};

// Invoke bootloader
//const char Help_BL_1[]= {"IB  - Invoke Bootloader (requires Bootloader build)\r\n"};
//const char Help_BL_2[]= {"      Unit remains in Bootloader until the code upgrade.\r\n"};

//const char * const  BLHelp[] = {
//    Help_BL_1, Help_BL_2, NULL
//};


// Meter Controls.
const char Help_M[]   = {"M       - Meter Totals Display\r\n"};
const char Help_M0[]  = {"M0      - Meter ID\r\n"};
const char Help_M1[]  = {"M1      - Temperature (degrees C)\r\n"};
const char Help_M2[]  = {"M2      - Frequency (Hz)\r\n"};
const char Help_M3[]  = {"M3      - Wh Total Imported Energy\r\n"};
const char Help_M4[]  = {"M4      - Wh Total Exported Energy\r\n"};
const char Help_M5[]  = {"M5      - VARh Imported\r\n"};
const char Help_M6[]  = {"M6      - VARh Exported\r\n"};
const char Help_M7[]  = {"M7      - VAh\r\n"};
const char Help_M8[]  = {"M8      - Operating Hours\r\n"};
const char Help_M9[]  = {"M9      - Real Time Clock (must be set)\r\n"};
const char Help_M10[] = {"M10     - Calendar Date (must be set)\r\n"};
const char Help_M11[] = {"M11     - Power factor at element\r\n"};
#if EQUATION==EQUATION5
const char Help_M12[] = {"M12     - V/V (degrees) A/B, A/C\r\n"};
#endif
#if EQUATION==EQUATION2
const char Help_M12[] = {"M12     - V/V (degrees) A/B\r\n"};
#endif
const char Help_M13[] = {"M13     - Edge count last accumulation, cumulative\r\n"};
#if EQUATION==EQUATION5
const char Help_M14[] = {"M14     - Seconds of sag, neutral. 14.1=Sec. of Tamper\r\n"};
#else
const char Help_M14[] = {"M14     - Seconds of sag, 14.1=Sec. of Tamper\r\n"};
#endif
#if EQUATION==EQUATION5
const char Help_M15[] = {"M15     - Irms, current. M15.4 - neutral current\r\n"};
#else
const char Help_M15[] = {"M15     - Irms, current\r\n"};
#endif
const char Help_M16[] = {"M16     - Vrms\r\n"};
const char Help_M17[] = {"M17     - Battery V .0=Vbat, .1=Vbat RTC\r\n"};
const char Help_M18[] = {"M18     - Watts at element\r\n"};
const char Help_M19[] = {"M19     - VARs at element\r\n"};
const char Help_M20[] = {"M20     - VAs at element\r\n"};
const char Help_M21[] = {"M21     - Demand, date, time\r\n"};
const char Help_M22[] = {"M22     - LCD Test\r\n"};
const char Help_M23[] = {"\r\n"};
const char Help_M24[] = {"Press button to scroll LCD, or set )F.\r\n"};
const char Help_M25[] = {"element:select Total:0 A:1 B:2\r\n"};

const char * const  MHelp[] = {
    Help_M, Help_M0, Help_M1, Help_M2, Help_M3, Help_M4,
    Help_M5, Help_M6, Help_M7, Help_M8, Help_M9, Help_M10,
    Help_M11,
#if EQUATION==EQUATION5
    Help_M12,
#endif
#if EQUATION==EQUATION2
    Help_M12,
#endif
    Help_M13, Help_M14, Help_M15, Help_M16, Help_M17,
    Help_M18, Help_M19, Help_M20, Help_M21, Help_M22, 
    Help_M23, Help_M24, Help_M25, 
    NULL
};


// Pulse index.
const char Help_P0[]  = {"P      - Pulse source selection for WPULSE\r\n"};
const char Help_P1[]=   {"Px     - Pulse: x= hex CE address of source. 0 reverts to Wh.\r\n"};
const char Help_P2[]=   {"Value is placed in APULSEW, read it with ']5A$'\r\n"};
const char Help_P3[]=   {"Check pulse index value with ')1a$'\r\n"};
const char * const  PHelp[] = { 
    Help_P0, Help_P1, Help_P2, Help_P3, 
    NULL };


// PWM control
const char Help_PWM_1[] = {"PWM     : PWM0/1 control\r\n"};
const char Help_PWM_2[] = {"PWMx.a.b: Starts PWM'0/1' at frequency 'a'Hz and duty cyle 'b' \r\n"};
const char Help_PWM_3[] = {"PWMx.0.0: Stops  PWM'0/1' \r\n"};
const char Help_PWM_4[] = {"\r\n"};
const char Help_PWM_5[] = {"For example: PWM0.1000.50 Starts PWM0 at 1000Hz and 50% duty cycle \r\n"};
const char Help_PWM_6[] = {"For example: PWM1.0.0     Stops  PWM1 \r\n"};


const char * const  PWMHelp[] = {
    Help_PWM_1,   Help_PWM_2,   Help_PWM_3,   Help_PWM_4,    Help_PWM_5,   
    Help_PWM_6,   NULL
};


// Special Function (R)egister and I/O controls
const char Help_RM_1[] = {"RMx        : Select SFRs at address hex x\r\n"};
const char Help_RM_2[] = {" ..???..   : Read consecutive registers in Decimal\r\n"};
const char Help_RM_3[] = {" ..$$$..   : Read consecutive registers in Hex\r\n"};
const char Help_RM_4[] = {" ..=n=n..  : Set consecutive registers' values\r\n"};
const char Help_RM_5[] = {"\r\n"};
const char Help_RM_6[] = {"For example: RM4008$$ or RM40004008$$ - Read 40004008 & C in hex\r\n"};

const char * const  RMHelp[] = {
    Help_RM_1,   Help_RM_2,   Help_RM_3,   Help_RM_4,   Help_RM_5,         
    Help_RM_6,   NULL
};


// Special Function (RA)m access 
const char Help_RA_1[] = {"RAx....   : Select RAM at byte address x\r\n"};
const char Help_RA_2[] = {" ..???..  : Read consecutive RAM data bytes in Decimal\r\n"};
const char Help_RA_3[] = {" ..$$$..  : Read consecutive RAM data bytes in Hex\r\n"};
const char Help_RA_4[] = {" ..=n=n.. : Set consecutive RAM data bytes' values\r\n"};
const char Help_RA_5[] = {"\r\n"};
const char Help_RA_6[] = {"For example: RA20000004$$ - Read RAM data bytes at address 0x20000004 in hex\r\n"};

const char * const  RAHelp[] = {
    Help_RA_1,   Help_RA_2,   Help_RA_3,   Help_RA_4,   Help_RA_5,         
    Help_RA_6,   NULL
};


// (R)eal (T)ime Clock controls
const char Help_RT_1[] = {"RT        : Real Time Clock controls\r\n"};
const char Help_RT_2[] = {"RTDy.m.d  : Day of year: (year, month, day) Weekday is automatic.\r\n"};
const char Help_RT_3[] = {"RTMh.m    : Set the alarm. Alarm sets bit 8 of )20.\r\n"};
const char Help_RT_4[] = {"RTR       : Read Real Time Clock in local time.\r\n"};
const char Help_RT_5[] = {"RTU       : Unlock real time clock. RTU1 locks.\r\n"};
const char Help_RT_6[] = {"RTZ       : Read Real Time Clock in Universal time(GMT).\r\n"};
const char Help_RT_7[] = {"RTTh.m.s.d: Time of day: (hr, min, sec, 1=daylight savings time).\r\n"};

const char * const  RTHelp[] = {
    Help_RT_1,  Help_RT_2,  Help_RT_3,  Help_RT_4, Help_RT_5, 
    Help_RT_6,  Help_RT_7, 
    NULL
};


// Trim Control 
const char Help_T_1[]= {"T       : Trim Controls\r\n"};
const char Help_T_2[]= {"TRn??   : Read consecutive trim words in decimal.\r\n"};
const char Help_T_3[]= {"TRn$$   : Read consecutive trim words in hex.\r\n"};
const char Help_T_4[]= {"TWn=x   : Write to a trim word.\r\n"};
const char Help_T_5[]= {"TFR     : Refresh trim / flash info block.\r\n"};
const char Help_T_6[] = {"\r\n"};
const char Help_T_7[] = {"For example: TRE$ - Read trim word at address 0x0E in hex\r\n"};


const char * const  THelp[] = {
    Help_T_1,  Help_T_2,  Help_T_3,  Help_T_4,  Help_T_5,   Help_T_6,  Help_T_7,  NULL
};


// Display driver versions
const char Help_V_1[]= {"v, v1 - Displays common device driver versions.\r\n"};
const char Help_V_2[]= {"v2    - Displays model-specific device driver versions.\r\n"};
const char Help_V_3[]= {"v3    - Displays SDK library versions.\r\n"};

const char * const  VHelp[] = {
    Help_V_1, Help_V_2, Help_V_3,
    NULL
};


// Wait for software watchdog
const char Help_W_1[]= {"W  - W0: WFI off, so CPU clock always on, using more power.\r\n"};
const char Help_W_2[]= {"     W1: WFI on, so CPU clock turns off to save power.\r\n"};
const char * const  WHelp[] = {
    Help_W_1,   Help_W_2,   NULL
};

#if REMOTES  // defined in mapX.h, from options.h
// remote Access. Remote build only
const char Help_X_1[]  = {"xA,r       : Read remote 0, rhex register r.\r\n"};
const char Help_X_2[] =  {"\r\n"};
const char Help_X_3[] =  {"For example: X0.0A - read remote 0 (for VA, IA), register hex 0A\r\n"};

const char * const  RMTHelp[] = {
    Help_X_1,  Help_X_2,  Help_X_3,
    NULL
};
#endif


// Reset Part
const char Help_Z_0[]= {"Z    - Best effort reset. (ARM NVIC reset)\r\n"};
const char Help_Z_1[]= {"Z1   - SYS Hard reset (SYS->CNTL_b.reset=1)\r\n"};
const char Help_Z_2[]= {"Z2   - Loop to force watchdog reset (TEST_E must be low.)\r\n"};
const char Help_Z_3[]= {"Z3   - Soft reset (run reset vector; Compatible with emulator.)\r\n"};
const char Help_Z_4[]= {"Z4   - Hard fault\r\n"};
const char * const  ZHelp[] = {
    Help_Z_0,   Help_Z_1,   Help_Z_2,   Help_Z_3,   
    Help_Z_4,
    NULL
};


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // If help is disabled to save space


