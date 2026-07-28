 /***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: CE init
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: ceM11a02f.h 25263 2020-05-13 21:40:34Z rvandewa $
 ****************************************************************************/
#ifndef CEM11A02F_H
#define CEM11A02F_H 1

#define default_ce cem11a02f         // default CE
#include <ce.h>
#include <stdint.h>
extern const char fw_ver_ce_code[];

// Access all CE constants and controls with this structure.
extern const ce_t default_ce;
extern const ce_t vlct_ce;
#define CE_PLL_CNT 2                 // accmulation interval of first valid data

// This CE code has a standard SY7T_M11 CE data map.


// CE data in RAM
#define CE_RAM_LOCATOR 0x20000000

// CE calibration parameters
// Start of CE Parameters 
#define  CE_START_ADDR   ((int32_t *)&ce_data)
// Start of CE Parameters  in most CE codes
#define  CE_PARM_BASE    (&ce_data.cal_i0)
#define  CE_DEFAULT_CNT  ((0x31-0x10)+1) // should be 0x22 34

typedef struct
{
    int32_t ia_raw;      // 0x00 Remote:IA, Non-remote:IA
    int32_t va_raw;      // 0x01 Remote:VA, Non-remote:Not used
    int32_t slot0_raw;   // 0x02 Remote:VB, Non-remote:VA (Customary)
    int32_t slot1_raw;   // 0x04 Remote:IB, Non-remote:IB (Customary)
    int32_t reserve1[0x10-0x04];  // RESERVED REGISTER(S)

// CE calibration parameters
    int32_t cal_i0;      //  0x10 I0 gain
    int32_t cal_v0;      //  0x11 V0 gain
    int32_t phadj_0;     //  0x12 Phase adjust 0, Current delay
    int32_t cal_i1;      //  0x13 I1 gain
    int32_t cal_v1;      //  0x14 V1 gain
    int32_t phadj_1;     //  0x15 Phase adjust 1, Current delay
    int32_t i1_scale;    //  0x16 Neutral current sensor scale factor
    int32_t ib_scale;    //  0x17

    int32_t reserve2[0x20-0x18];  // RESERVED REGISTER(S)

    uint32_t ceconfig;   //  0x20
/*
#define     SINGLE_PH       BIT25     // When set single phase operation
#define     VSENSE          BIT24     // When set using remote devices on voltage
#define     CE_6K_TEMP      BIT23     // When set using remote devices on current
#define     CE_EXT_TEMP     BIT22     // When set external temperature compensation via MPU used
#define     EDGE_INT        BIT21     // When set, XPULSE, zero crossing interrupt enabled 
#define     SAG_INT         BIT20     // when set, YPULSE,  SAG interrupt enabled 
#define     SAG_CNT_MASK    0x00FFF00 // Delay before sag is asserted, in samples 
#define     FREQ_SEL1       BIT7
#define     FREQ_SEL0       BIT6
#define     FREQ_SEL_MASK   0x00000C0 //  Selects phase index to measure frequency 
#define     FREQ_SEL_SHIFT  6
#define     EXT_PULSE       BIT5      // When zero, pulse generators use internal data:
#define     IG8X            BIT4      // when set, applies digital gain 4x to voltage
#define     INT_CREEP       BIT3      // when set, CE performs creep
#define     VG4X            BIT2      // when set, applies digital gain 4x to voltage
#define     PULSE_FAST      BIT1      // when set,increases pulse speed by 16x
#define     PULSE_SLOW      BIT0      // when set,decreases pulse speed by 64x 
*/
    uint32_t wrate;      //  0x21  Kh/Pulse setting 
    uint32_t kvar;       //  0x22  adjust VARh/Wh 
    uint32_t ce_sumpre;  //  0x23  samples/accumulation
    uint32_t sag_thr;    //  0x24  sag threshold 
    uint32_t creep_thr;  //  0x25
    int32_t reserve2p5;  //  0x26
    int32_t quant_v0;    //  0x27
    int32_t quant_i0;    //  0x28
    int32_t quant_w0;    //  0x29
    int32_t quant_var0;  //  0x2a
    int32_t quant_v1;    //  0x2b
    int32_t quant_i1;    //  0x2c
    int32_t quant_w1;    //  0x2d
    int32_t quant_var1;  //  0x2e

    int32_t reserve3[0x54-0x2f];  // RESERVED REGISTER(S)

    int32_t nfreq;       //  0x54
    int32_t gain_adj[3]; //  0x55...0x57  Vx, I0..I1 temp. gain adjustment 

    int32_t apulsew;     //  0x58  input to watt pulse
    uint32_t wpulse_ctr; //  0x59  watt pulse counter 
    uint32_t wpulse_frac;//  0x5a
    int32_t wsum_accum;  //  0x5b  watt pulse fraction 
    int32_t apulser;     //  0x5c  input to var pulse 
    uint32_t vpulse_ctr; //  0x5d  var pulse counter 
    uint32_t vpulse_frac;//  0x5e
    int32_t vsum_accum;  //  0x5f  var pulse fraction 
   
    int32_t reserve4[0x80-0x60];      // RESERVED REGISTER(S)	
    // End of CE Data 

    // CE Output offsets
    uint32_t cestatus_x;  //  0x80        
    int32_t temp_x;       //  0x81 
    uint32_t freq_x;      //  0x82
    uint32_t mainedge_x;  //  0x83
    int32_t wsum_x;       //  0x84 
    int32_t w0sum_x;      //  0x85
    int32_t w1sum_x;      //  0x86
    int32_t varsum_x;     //  0x87
    int32_t var0sum_x;    //  0x88
    int32_t var1sum_x;    //  0x89
    int32_t i0sqsum_x_h;  //  0x8A
    uint32_t i0sqsum_x_l; //  0x8B
    int32_t i1sqsum_x_h;  //  0x8C
    uint32_t i1sqsum_x_l; //  0x8D
    int32_t v0sqsum_x;    //  0x8E
    int32_t v1sqsum_x;    //  0x8F

    int32_t reserve5[0x92-0x90];      // RESERVED REGISTER(S)	
	
    //  CE Output Harmonics
    int32_t wsum_h_x;      //  0x92
    int32_t w0sum_h_x;     //  0x93

    int32_t w1sum_h_x;     //  0x94

    int32_t i0sqsum_h_x_h;  // 0x95
    uint32_t i0sqsum_h_x_l; // 0x96
    int32_t i1sqsum_h_x_h;  // 0x97
    uint32_t i1sqsum_h_x_l; // 0x98
    int32_t v0sqsum_h_x;    // 0x99
    int32_t v1sqsum_h_x;    // 0x9A

    int32_t reserve7[0xA4-0x9B];      // RESERVED REGISTER(S)	

    int32_t varsum_f_x;    //  0xA4
    int32_t var0sum_f_x;   //  0xA5
    int32_t var1sum_f_x;   //  0xA6
    
    int32_t reserve8[0xFD-0xA7];    // RESERVED REGISTER(S)

    int32_t stemp;     //  0xFD
    int32_t scr;       //  0xFE
    int32_t s_len;     //  0xFF

} ce_data_t;

// ce_ram structure place in CEDATA section, with R/W type.
#if 1
extern ce_data_t ce_data; 
#else
#define ce_data (*(ce_data_t *)CE_RAM_LOCATOR) 
#endif

	typedef struct
{
		int32_t wsum_x;       //  0x84 
		int32_t w0sum_x;      //  0x85
		int32_t w1sum_x;      //  0x86
		int32_t varsum_x;     //  0x87
		int32_t var0sum_x;    //  0x88
		int32_t var1sum_x;    //  0x89
		int32_t i0sqsum_x_h;  //  0x8A
		uint32_t i0sqsum_x_l; //  0x8B
		int32_t i1sqsum_x_h;  //  0x8C
		uint32_t i1sqsum_x_l; //  0x8D
		int32_t v0sqsum_x;    //  0x8E
		int32_t v1sqsum_x;    //  0x8F
}Local_ce_data;
extern Local_ce_data Local_RAM_ce_data; 

#define CE_RAM ((uint32_t *)&ce_data)

// CE calibration parameters
#define CAL_I0     ce_data.cal_i0  
#define CAL_V0     ce_data.cal_v0  
#define PHADJ_0    ce_data.phld_0  
#define CAL_I1     ce_data.cal_i1  
#define CAL_V1     ce_data.cal_v1  
#define PHADJ_1    ce_data.phld_1  
#define CAL_I2     ce_data.cal_i2  
#define CAL_V2     ce_data.cal_v2  
#define PHADJ_2    ce_data.phld_2  
//#define CAL_I3     ce_data.cal_i3
//#define I3_SCALE   ce_data.i3_scale
//#define PHADJ_3    ce_data.phld_3
//#define CE_CONFIG  ce_data.ceconfig

// Constants for auto-calibration.
#define MAX_GAIN      0x00007FFFL
#define UNITY_GAIN    0x00004000L
#define MIN_GAIN      0x00002000L
#define ZERO_PHASE    0x0000L
#define MAX_PHASE     (32767)
#define MIN_PHASE     (-32767)
#define LIMIT_PHASE_ERROR 1
#define LIMIT_PHASE_CORRECTION 0
#define MAX_TAN_THETA (0.12278)  /* +7 degrees */
#define MIN_TAN_THETA (-0.12278) /* -7 degrees */

#define CE_FS 4096

// CE CONFIG bits and fields.
#define     CE_RFS          BIT29     // Set for very low speed (VLS) clock, to save power.
#define     SINGLE_PH       BIT25     // When set single phase operation on 3-phase CE code.
#define     VSENSE          BIT24     // When set using remote devices on voltage.
#define     CE_6K_TEMP      BIT23     // When set using remote devices on current.
#define     CE_EXT_TEMP     BIT22     // When set external temperature compensation via MPU used
#define     EDGE_INT        BIT21     // When set, XPULSE, zero crossing interrupt enabled 
#define     SAG_INT         BIT20     // when set, YPULSE,  SAG interrupt enabled 
#define     SAG_CNT_MASK    0x00FFF00 // Delay before sag is asserted, in samples 
#define     FREQ_SEL1       BIT7
#define     FREQ_SEL0       BIT6
#define     FREQ_SEL_MASK   0x00000C0 //  Selects phase index to measure frequency 
#define     FREQ_SEL_SHIFT  6
#define     EXT_PULSE       BIT5      // When zero, pulse generators use CE's internal data.
#define     IG8X            BIT4      // when set, applies digital gain 8x to current
#define     INT_CREEP       BIT3      // when set, clear pulse inputs if below CREEP_THR
#define     VG4X            BIT2      // when set, applies digital gain 4x to voltage
#define     PULSE_FAST      BIT1      // when set,increases pulse speed by 16x
#define     PULSE_SLOW      BIT0      // when set,decreases pulse speed by 64x 

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
#endif  // CEM11A02F_H
