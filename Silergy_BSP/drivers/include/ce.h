/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: These are the parts of CE initialization and start-up
 * that are portable.
 *
 * Copyright 2017, Silergy Corp.   All rights reserved.
 * $Id: ce.h 24632 2020-04-27 18:48:58Z ptanwar $
 ****************************************************************************/
#ifndef CE_H
#define CE_H 1

/*** Version ****/
extern const char fw_ver_ce[];
// The metering equations for a utility drop.
#define EQUATION0 0 // W = VA * IA (1p2w, single-phase, The CE's B outputs measure neutral)
#define EQUATION1 1 // W = (VA/2)*(IA - IB) (1p3w, split-phase CE's B outputs are nondifferential)
#define EQUATION2 2 // W = VA*IA + VB*IB (3p3w, Three-phase delta)
// Equation 3, 4 are differential 3-phase equations.
#define EQUATION5 5 // W = VA*IA + VB*IB + VC*IC (3p4w, Three-phase wye)

// Structure giving access to the CE.
typedef struct ce_s {
    void (*ce_init_fn)(void);
    void (*ce_enable_fn)(void);
    float fs; // sample frequency, Hz in floating point.
    int sum_samp; // Samples per second.
    float cvsq; // volts-squared constant 
    float cisq; // amps-squared constant
    float cisqn; // amps-squared constant for neutral
    float cp; // power constant
    float cv; // sag threshold constant
    // At small values, the front end tends to produce a fixed
    // small offset of noise, the noise-floor.  Subtracting the
    // noise-floor makes small measurements more precise.
    // The quant LSBs are used to calculate noise-floor offsets.
    float quant_w; // quantization for watt-hours
    float quant_i; // quantization for current
    float quant_v; // quantization for current
    float h_per_l; // least significant current units per most-significant units
    float wrate_power_of_two;
    int pulse_slow_factor; // The rate of the CE's slow-pulse bit
    int pulse_fast_factor; // The rate of the CE's fast-pulse bit
    int nacc; // samples per accumulation interval
    float x; // Used to calculate pulse speed. (Usually 1, sometimes 6)
    int pll_cnt; // Accumulation interval at which data is reliable (usually 2)
    int i1_scaled_by_ce; // Nonzero means it's true.
    int unity_gain; // fixed-point CE value for unity gain.
    int var0_exists;
    int var1_exists;
    const int32_t *code_ptr;
    const int * code_len_ptr;
    const int32_t *data_ptr;
    const int * data_len_ptr;
    int name_idx; // Where the CE name string starts.
    float creep_thr_lsb;
} ce_t;

// Global default CE in ce.c
extern const ce_t *ce_ptr;

extern volatile bool ce_xfer_done;                // true means data is available.

extern bool sag_int_enabled; // true means YPLS interrupt is enabled 

// Stop the CE, force it into a low-power state. (init is in the ce code.)
void ce_deinit(void);

// Stop the CE. It remains able to start again. (use ce_enable in the ce code)
void ce_disable(void);

// Load the CE's code.
void ce_load_code(const unsigned int *ce_code_ptr);

// Load the CE's RAM.
void ce_load_ram(const unsigned int * ce_data_ptr, int len_in);

// Copy from a constant array to the CE RAM.
void ce_get_cal_default(int32_t *dest_ptr, const int32_t *src_ptr, 
        int32_t length);

// Get the calibration data into Cal_t from CE ram.
void ce_get_cal(int32_t *dest_ptr, int32_t length);

// Put the calibration data into CE ram.
void ce_put_cal(int32_t *src_ptr, int32_t length);


// Enables ISRs for the CE.
// Indicate which interrupt with an ISR enable mask
// A function pointer, for a function to be executed. A null does nothing.
// Returns nonzero for failure.
int ce_int_enable(int mask, void (*fn_ptr)(void));
#define CE_BUSY_I 	BIT0 // Interrupt flags from bit 0..7
#define XFER_N_I 		BIT1
#define VPLS_I 			BIT2
#define WPLS_I 			BIT3
#define XPLS_I 			BIT4
#define YPLS_I 			BIT5
#define FIR_DONE_I 	BIT6
#define MUX_SYNC_I 	BIT7
#define CE_BUSY_IE 	BIT8 // Interrupt enable from bit 8..15
#define XFER_N_IE 	BIT9
#define VPLS_IE     BIT10 
#define WPLS_IE     BIT11 
#define XPLS_IE     BIT12 
#define YPLS_IE     BIT13 
#define FIR_DONE_IE BIT14 
#define MUX_SYNC_IE BIT15 


// Disables ISRs for the CE.
// Indicate which interrupt with an ISR enable mask.
void ce_int_disable(int mask);

// CE Interrupt Service Routine.
void ce_isr(void);

// Returns string pointer to the CE name.
const char *ce_name(void);
#define CE_NAME_SZ 4 // CE name array size

/***************************************************************************   
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
 #endif // CE_H
