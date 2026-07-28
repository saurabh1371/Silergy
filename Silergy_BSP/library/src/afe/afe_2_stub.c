/**************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Description : Stub to simulate 2-phase metering.
 * This implements equation 2, 3-phase delta, i.e. Wh = VA*IA + VB*IB
 *
 * Copyright 2020, Silergy Corp. All rights reserved.
 *
 * $Id: afe_1p_stub.c 21397 2019-06-15 02:52:26Z rvandewa $
 ***************************************************************************/
#include <string.h>
#include <math.h>       // atan2()
#include "options.h"
#include "ce.h"
#include "delay.h"      // Calibrated programmed delay
#include "mmath.h"      // lroundf
#include "afe.h"        // Check function prototypes

/*** Version ****/
const char fw_ver_afe[] =
"afe_2_stub" /* Device Driver name */
" v"     /* ver string */
"3.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

const char fw_ver_ce_code[] =
"no CE in this build (afe stubbed)"  /* Device Driver name */
"\r\n"       /* CR + LF */
;

#if EQUATION!=EQUATION0
#error Meter EQUATION symbol is wrong!
#endif

#if PHASE_CNT!=2
#error The phase count is wrong!
#endif

// This equation usually has identical current sensors for the phases, 
// but the neutral measurement may be a shunt, so translation of LSBs 
// is often used for the neutral. All known CE codes handle it, so there
// is no code to translate LSBs in the MPU.


// CE code and LSBs (i.e. the constants used to convert CE data to 
// useful units.), and other items needed by the MPU code.
void ce_stub_init(void) { }
void ce_stub_enable(void) { }
int32_t stub_array[2] = { 0, 0 };

const ce_t ce_stub =
{
    // Code
    ce_stub_init,
    ce_stub_enable, 
    // LSBs
    4096,               // CE_FS, from CE's Ref. Manual's Table
    4096,               // CE_SUM_SAMP, from table
    1.09435E-05,        // CE_CVSQ, from table
    1.05753E-05,        // CE_CISQ, from table
    1.07592E-05,        // CE_CP, from table
    6.03097E-09,        // CE_CV, from table
    6.56692E-10,        // CE_QUANT_W_LSB, from table
    3.12525E-13,        // CE_QUANT_I_LSB, from table
    6.67941E-10,        // CE_QUANT_V_LSB, from table
    // Other calculation quantities for the CE, from the reference manual.
    (1.0/2048.0),       // CE_H_PER_L, from insqres_x equation
    7.03687e13,         // CE_WRATE_POWER_OF_TWO, from WRATE equation
    64,                 // CE_PULSE_SLOW_FACTOR, 2^6, from WRATE equation
    16,                 // CE_PULSE_FAST_FACTOR, 2^4, from WRATE equ.
    4096,               // CE_NACC, Typical, assumed by CE ref. manual.
    1.0,                // CE_X, from WRATE equation
    2,                  // CE_PLL_CNT, accum. intervals for CE to stabilize
    1,                  // I1_SCALED_BY_CE, i.e. CE adjusts units for I1
    0x4000,             // UNITY_GAIN, needed by autocal to scale gains.
    1,                  // Does VAR0SUM exist?
    1,                  // Does VAR1SUM exist?
    (const int32_t *)stub_array, // Locate the CE's code and data.
    &stub_array[0],
    (const int32_t *)stub_array,
    &stub_array[0],
    (0x0)               // CE_NAME_IDX, index of the CE's name
};

// When nonzero, the data from the CE is invalid.
uint8_t afe_starting_cnt = CE_PLL_CNT;

// Calibration data (i.e. afe-specific data)
Cal_t *afe_cal_ptr;

float afe_isqsum_min_f; // Starting current for one phase
float afe_isqsum_min_total_f; // Starting current for whole meter
int32_t afe_vsqsum_min; // Starting voltage
int32_t afe_vasum_min;  // Starting volt-amps for one phase
int32_t afe_vasum_min_total;  // Starting volt-amps for whole meter

float isqsum_f; // current
int32_t vsqsum; // voltage
int32_t vasum;  // volt-amps, one phase
int32_t wsum;   // Wh, one phase
int32_t varsum; // VARh, one phase
int32_t afe_ppmc, afe_ppmc2;
int32_t apulsew, apulser;
int32_t afe_varh_cnt, afe_varh_frac, afe_wh_cnt, afe_wh_frac;
int32_t my_wsum_per_cnt;
int32_t my_wsum_per_pulse;
bool pulse_internal; // off by default (0 = false)
bool afe_on; // off by default (0 = false)

/* Start standard API */


/****************************************************************************
* Description:
* Validates the calibration pointer(s).
*
* Parameters:
*   Input:
*       A calibration pointer.
*   Output:
* Return Status:
*       A valid calibration pointer.
****************************************************************************/
static Cal_t *afe_valid_cal_ptr(Cal_t *c_ptr)
{
    if(NULL == c_ptr) {
        c_ptr = &global.cal;
    }
    afe_cal_ptr = c_ptr; 

    return c_ptr;
} // afe_valid_cal_ptr()


/****************************************************************************
* Description:
* Validates the CE pointer(s).
*
* Parameters:
*   Input:
*       A CE pointer.
*   Output:
* Return Status:
*       A valid CE pointer.
****************************************************************************/
const ce_t *afe_valid_ce_ptr(const ce_t *ptr)
{
    if(NULL == ce_ptr)
        ce_ptr = &ce_stub;

    if(NULL == ptr)
        ptr = ce_ptr;
    else
        ce_ptr = ptr; 

    return ptr;
} // afe_valid_cal_ptr()


/****************************************************************************
* Description:
* Put hardware calibration defaults from an external structure.
*
* Parameters:
*   Input:
*       None
*   Output:
*       returns CE counts per cnt, counts' LSBs as returned by afe_wh();
*       This number, wsum per pulse, is useful for a major/minor counter
*       scheme to could pulses or whatever the major count is.
* Return Status:
*       None. 
****************************************************************************/
int32_t afe_put_cal(Cal_t *c_ptr)
{
    c_ptr = afe_valid_cal_ptr(c_ptr);

    my_wsum_per_pulse = afe_wh2wsum(
            ((float)afe_cal_ptr->wh_whpp)
            /((float)afe_cal_ptr->p_whpp)
            ); 
    my_wsum_per_cnt = afe_wh2wsum(PCB_WH_PLCDCNT); 
    return my_wsum_per_cnt;
}


/****************************************************************************
* Description:
* Get hardware calibration into an external calibration structure.
*
* Parameters:
*   Input:
*       None
*   Output:
*       returns counts per pulse, counts' LSBs as returned by afe_wh();
*       This number, wsum per pulse, is useful for a major/minor counter
*       scheme to could pulses or whatever the major count is.
* Return Status:
*       None. 
****************************************************************************/
int32_t afe_get_cal(Cal_t *c_ptr)
{
    c_ptr = afe_valid_cal_ptr(c_ptr);

    my_wsum_per_pulse = afe_wh2wsum(
        ((float)c_ptr->wh_whpp)
        /((float)c_ptr->p_whpp)
        ); 

    my_wsum_per_cnt = afe_wh2wsum(PCB_WH_PLCDCNT); 
    return my_wsum_per_cnt; // Wh/LCD
}


/****************************************************************************
* Description:
* Recalculates, defaults calculated from the i_max and v_max of the sensors.
* Does not affect calibration.
*
* Parameters:
* Input:
* Cal_t includes i_max, v_max, i_min (the starting current), v_min
* (the starting voltage), and a copy of the CE’s calibration array. 
* Cal_t should be saved and restored by higher-level code’s 
* save and restore of the calibration.
* Return Status:
*   returns counts per pulse, counts' LSBs as returned by afe_wh();
*   or afe_native_per_pulse(), suitable for initializing the meter math add().
****************************************************************************/
int32_t afe_recalc(Cal_t *c_ptr)
{
    c_ptr = afe_valid_cal_ptr(c_ptr);
    ce_ptr = afe_valid_ce_ptr(ce_ptr);

    // Recalculate ce_data.wrate, the CE's Kh
    ce_data.wrate = afe_wh2wrate(((float)(c_ptr->wh_whpp))
            /((float)c_ptr->p_whpp));

    // Meter saves data at this voltage.
    ce_data.sag_thr = afe_v2thr(((float)c_ptr->v_min)/10.0);

    // Set CE's creep threshold for each phase.
    ce_data.creep_thr = lroundf // convert float to properly rounded 32-bit int.
        (
            (
                (((float)c_ptr->i_min)/1000.0) // minimum current, amps.
                *   // Minimum watts/phase, usually about 5w
                (((float)c_ptr->v_min)/10.0) // minimum voltage, volts.
            )
            / // float value for creep_thr
            (
                (ce_ptr->creep_thr_lsb)
                *
                (((float)c_ptr->v_max)/10.0) // v_max, volts
                *
                (((float)c_ptr->i_max)/10.0) // i_max, amps
            )
        );

    // Set the CE's scaling for neutral.
    if(1 == ce_ptr->i1_scaled_by_ce)
    {
        float fT;
        // Set software scaling factor for I1SCALE using IBSCALE.
        fT = sqrt((ce_ptr->cisqn)/(ce_ptr->cisq));
        fT *= ((float)c_ptr->i_max_neutral)/((float)c_ptr->i_max);
        fT *= (float)ce_ptr->unity_gain;
        fT /= (float)ce_data.ib_scale;
        ce_data.i1_scale = lroundf(fT);
    }

    // Return value for add. Also saves the default to the cal array.
    return afe_get_cal(c_ptr);
}


// Temperature compensation channels depend on gain_adj indexes in CE code.
#define FIRST_VREF 0
#define NATIVE_VREF (FIRST_VREF) // SoC's Vref
#define LAST_VREF (PHASE_CNT+1)  // last Vref is for neutral CT
#define NEUTRAL_VREF (LAST_VREF)
#define FIRST_REMOTE PHASE_A
#define LAST_REMOTE PHASE_B

/****************************************************************************
* Description:
* Sets defaults in the AFE, including defaults calculated from
* the i_max and v_max of the sensors.
*
* Parameters:
* Input:
* Cal_t includes i_max, v_max, i_min (the starting current), v_min
* (the starting voltage), and a copy of the CE’s calibration array. 
* Cal_t should be saved and restored by higher-level code’s 
* save and restore of the calibration.
* Return Status:
*   returns counts per pulse, counts' LSBs as returned by afe_wh();
*   or afe_native_per_pulse(), suitable for initializing the meter math add().
****************************************************************************/
int32_t afe_get_cal_default(Cal_t *c_ptr)
{
    int32_t *ce_ptr;

    // Set these to defaults so that the autocal works well.
    ce_ptr = (int32_t *)(ce_data.cal_i0);
    *ce_ptr++ = UNITY_GAIN; // phase A
    *ce_ptr++ = UNITY_GAIN;
    *ce_ptr++ = ZERO_PHASE;
    *ce_ptr++ = UNITY_GAIN;  // phase B
    *ce_ptr++ = UNITY_GAIN;
    *ce_ptr++ = ZERO_PHASE;

    afe_cal_ptr = c_ptr;

    // Permit external temperature compensation, external pulse.
    ce_data.ceconfig |= CE_EXT_TEMP | EXT_PULSE;
    // Disable zero-crossing interrupt, internal creep. 
    ce_data.ceconfig &= ~(EDGE_INT | INT_CREEP);

    // CE_PPMC and CE_PPMC2 are the coefficients of an integer polynomial
    // that describes the behavior of the ADC's voltage reference as
    // temperature varies. No afe means no compensation.
    afe_ppmc  = 0;
    afe_ppmc2 = 0;

    // Return value for add. Also saves the default to the cal array.
    return afe_recalc(c_ptr);
}


/****************************************************************************
* Description:
* Runs when the CE's xfer-busy interrupt indicates that data is done.
*
* Parameters:
* Input:
*       None
****************************************************************************/
void afe_data_ready(void)
{
    afe_busy = 0; // Data is ready.
    
    if(0 == (ce_data.ceconfig & EXT_PULSE)) // pulse writes disabled?
    {
        return;
    }
    else
    {
        int32_t w = afe_cal_ptr->pulse_src;

        // Synchronously set pulse inputs from the CE's Wh & VARh values.
        if(0 == w) { // default: sum of all Wh.
            w = afe_vasum_creep(ALL_PHASES, afe_wsum(ALL_PHASES));
        } else { // set from any CE variable. (e.g. for narrowband Wh)
            w = *(afe_cal_ptr->pulse_src + (int32_t*)CE_RAM);
        }
        afe_set_wpulse(w);

        afe_set_varpulse(afe_vasum_creep(ALL_PHASES, afe_varsum(ALL_PHASES)));
    }
}


/****************************************************************************
* Description:
* Initialize the AFE, with assigned code, and defaults.
* Calibration must be added with afe_enable(), below
*
* Parameters:
* Input:
* Cal_t includes the CE's calibration array. Cal_t should be saved and 
* restored by higher-level code's save and restore of the calibration.
* Return Status:
* returns counts per pulse, counts' LSBs as returned by afe_wh();
* or afe_native_per_pulse(), suitable for initializing the meter math add().
****************************************************************************/
void afe_init(const ce_t *ce_ptr_tmp)
{
    ce_ptr = afe_valid_ce_ptr(ce_ptr_tmp);
} // afe_init


/****************************************************************************
* Description:
* Add calibration to an initialized AFE, and enable the AFE.
*
* Parameters:
* Input:
* Cal_t includes the CE's calibration array. Cal_t should be saved and 
* restored by higher-level code's save and restore of the calibration.
* Return Status:
* returns counts per pulse, counts' LSBs as returned by afe_wh();
* or afe_native_per_pulse(), suitable for initializing the meter math add().
****************************************************************************/
#define RADIANS_PER_DEGREE 0.017453
int32_t afe_enable(const ce_t *ce_ptr_tmp, Cal_t *c_ptr)
{
    float sine, cosine, radians, vasum_f;
    int t;

    c_ptr = afe_valid_cal_ptr(c_ptr);
    ce_ptr = afe_valid_ce_ptr(ce_ptr_tmp);

    global.misc.acc_cnt = 0;    /* restart the count of accumulation intervals */

    // Recalculate internal minimums for voltage, current and volt-amps
    afe_vsqsum_min = afe_v2vsqsum(((float)afe_cal_ptr->v_min)*0.1);
    afe_isqsum_min_f = afe_a2isqsum(((float)afe_cal_ptr->i_min)*0.001);
    afe_isqsum_min_total_f 
        = afe_a2isqsum(((float)(PHASE_CNT * afe_cal_ptr->i_min)*0.001));
    // va = i * v
    t = afe_w2wsum((((float)afe_cal_ptr->v_min)*0.1)
        * (((float)afe_cal_ptr->i_min)*0.001));
    afe_vasum_min = t;
    afe_vasum_min_total = PHASE_CNT * t;

    // Calculate nominal values from the calibration expectations.
    isqsum_f = afe_a2isqsum(((float)afe_cal_ptr->i_cal)*0.1);
    vsqsum = afe_v2vsqsum(((float)afe_cal_ptr->v_cal)*0.1);

    // va = i * v, calculated directly from test signal description.
    vasum_f = afe_w2wsumf(
            ((float)afe_cal_ptr->v_cal)
            * ((float)afe_cal_ptr->i_cal)
            * 0.01
            );
    vasum = lroundf(vasum_f);
    radians = ((float)afe_cal_ptr->theta_cal)*RADIANS_PER_DEGREE;
    cosine = cosf(radians);
    wsum = lroundf(cosine * vasum_f);
    sine = sinf(radians);
    varsum = lroundf(sine * vasum_f);

    /* start the CE */
    afe_starting_cnt = 2;
    afe_on = true;

    my_wsum_per_pulse = afe_wh2wsum(
            ((float)afe_cal_ptr->wh_whpp)
            /((float)afe_cal_ptr->p_whpp)
            ); 
    my_wsum_per_cnt = afe_wh2wsum(PCB_WH_PLCDCNT); 
    return my_wsum_per_cnt;
} // afe_enable


/* Set hardware calibration defaults into an afe structure. 
   This sets Vref default temperature compensation
   and other hardware defaults.  Imax, Vmax and other application
   calibration values must be set explicitly before calling
   this function. 
   returns counts per pulse, counts' LSBs as returned by afe_wh(); */
int32_t afe_set_cal(Cal_t *c_ptr) { return afe_wh2wsum(PCB_WH_PLCDCNT); }



/****************************************************************************
* Description:
* Simulates the CE 
*
* Parameters:
* Input:
* Cal_t includes the CE’s calibration array. Cal_t should be saved and 
* restored by higher-level code’s save and restore of the calibration.
* Return Status:
* returns counts per pulse, counts' LSBs as returned by afe_wh();
* or afe_native_per_pulse(), suitable for initializing the meter math add().
****************************************************************************/
void afe_sim(void)
{
    afe_enable(NULL, afe_cal_ptr);
}


/****************************************************************************
* Description:
*   Disables the AFE, with minimum power; After this, all data request
*   routines return values that have been conditioned by the creep logic.
*
* Parameters:
*   Input:
*         None.
* Return Status:
*         None
****************************************************************************/
void afe_disable(void)
{
    afe_starting_cnt = 2;
    afe_on = false;
} // afe_disable


/****************************************************************************
* Description:
*    Accumulate the pulses.
* Parameters: 
*       none.
*   Input:
*       apulsew, apulser.
* Return Status:
*         Returns a count of accumulation intervals since reset.
****************************************************************************/
void accum_wpulse(void)
{
    if(0 >= my_wsum_per_pulse)
        return;
    afe_wh_frac += apulsew;
    while(afe_wh_frac > my_wsum_per_pulse)
    {
        afe_wh_frac -= my_wsum_per_pulse;
        ++afe_wh_cnt;
    }
}
void accum_rpulse(void)
{
    if(0 >= my_wsum_per_pulse)
        return;
    afe_varh_frac += apulser;
    while(afe_varh_frac > my_wsum_per_pulse)
    {
        afe_varh_frac -= my_wsum_per_pulse;
        ++afe_varh_cnt;
    }
}


/****************************************************************************
* Description:
*   Gets the AFE’s status.
* Parameters:
*   Input:
*         a mask to select the status desired
* Return Status:
*         return: true if data is ready.
****************************************************************************/
int afe_status(int mask) 
{
    static int my_last_sec;
    int ret_val = 0;

    if(afe_on)
    {
        if (mask & AFE_RUNNING)
            ret_val |= AFE_RUNNING;

        if (mask & AFE_HAS_DATA)
        {   /* simulated detect new data. simulated with RTC */
            if(my_last_sec != RTC->TIME)
            {
                my_last_sec = RTC->TIME;

                ++global.misc.acc_cnt;

                if(afe_starting_cnt)
                {
                    --afe_starting_cnt;
                }
                if (0 == afe_starting_cnt)
                {
                    ret_val |= AFE_HAS_DATA; // Data just became available.
                    if(pulse_internal)
                    {
                        apulsew = wsum;
                        accum_wpulse();
                        apulser = varsum;
                        accum_rpulse();
                    } // end if pulsing is internal
                } // if afe is valid
            } // end if an accumulation interval passed
        } // end if check for afe has data
    } // else afe is off, ret_val remains 0, nothing updated, etc.
    return ret_val;
}


// Tell afe code that data is ready.
// Normally called from the interrupt.
void afe_data_ready(void) { };


/****************************************************************************
* Description:
* identify the AFE version; returns pointer to the CE name.
*
* Parameters:
*   Input:
*       None.
*   Output:
*       None.
* Return Status:
*       Returns the AFE's CE's file name string. 
****************************************************************************/
const char * afe_version(void)
{
    return " AFE stubbed ";
} // afe_version()


/****************************************************************************
* Description:
* Set the CE's pulse input variables.
*
* Parameters:
*   Input:
*       Wh or VARh, in the units of CE's WSUM_X
*   Output:
*       Wh or VARh, in the units of CE's WSUM_X
* Return Status:
*       Returns the Wh or VARh. 
****************************************************************************/
int32_t afe_set_wpulse(int32_t wh)
{
    apulsew = wh;
    if(!pulse_internal)
    {
        accum_wpulse();
    }

    return wh;
} // afe_set_wpulse

/* Set the VARh pulse input. */
int32_t afe_set_varpulse(int32_t varh)
{
    apulser = varh;
    if(!pulse_internal)
    {
        accum_rpulse();
    }

    return varh;
} // afe_set_varpulse()

/* Set the pulse X input. (not on all configurations.) */
int32_t afe_set_xpulse(int32_t wh) { return wh; } // Stubbed.

/* Set the pulse Y input. (not on all configurations.) */
int32_t afe_set_ypulse(int32_t wh) { return wh; } // Stubbed.


/****************************************************************************
* Description:
* Disable pulse outputs.
*
* Parameters:
*   Input:
*       None
*   Output:
*       None
* Return Status:
*       None. 
****************************************************************************/
void afe_pulse_disable(void)
{
    my_wsum_per_pulse = 0;
} // afe_pulse_disable()


/****************************************************************************
* Description:
* Enable pulse outputs.
*
* Parameters:
*   Input:
*       None
*   Output:
*       None
* Return Status:
*       None. 
****************************************************************************/
void afe_pulse_enable(void)
{
    if(NULL != afe_cal_ptr)
    {
        my_wsum_per_pulse = afe_wh2wsum(
            ((float)afe_cal_ptr->wh_whpp)
            /((float)afe_cal_ptr->p_whpp)
            );
    }
} // afe_pulse_enable()


/****************************************************************************
* Description:
* Test pulse outputs: 10ms on, then off
*
* Parameters:
*   Input:
*       None
*   Output:
*       None
* Return Status:
*       None. 
****************************************************************************/
void afe_pulse_test(void)
{
} // afe_pulse_test()


/****************************************************************************
* Description:
* Pulse outputs from internal CE data.
* CE generates pulses without MPU setting wpulse or rpulse
*
* Parameters:
*   Input:
*       None
*   Output:
*       None
* Return Status:
*       None. 
****************************************************************************/
void afe_pulse_internal(void)
{
    pulse_internal = true;
} // afe_pulse_internal()


/****************************************************************************
* Description:
* Pulse outputs from external MPU data.
* CE generates pulses from MPU's setting of wpulse or rpulse
*
* Parameters:
*   Input:
*       None
*   Output:
*       None
* Return Status:
*       None. 
****************************************************************************/
void afe_pulse_external(void)
{
    pulse_internal = false;
} // afe_pulse_external()


/****************************************************************************
* Description:
* Sets the nonexistent frequency sense channel. It's a stub.
*
* Parameters:
*   Input:
*       phase index
*   Output:
*       None
* Return Status:
*       None. 
****************************************************************************/
void afe_set_frequency_phase(int phase_idx)
{
} // afe_set_frequency_phase()


/****************************************************************************
* Description:
*       Computes Gain Adj for temp. compensation.
*       This depends on the exact calculation of CE_PPMC and CE_PPMC2
*       These are calibration parameters in the CE RAM, but they  depend
*       on reading a trim, so the MPU sets defaults in cal\cal.c cal_reset()
*
* Parameters:
*   Input:
*         The temperature in degrees C, in floating point.
*         The CE_PPMC and CE_PPMC2 from the CE code.
*   Output:
*         gain_adj for the IC's band-gap. Which current sensors is in main.c
****************************************************************************/
#if 0x4000==UNITY_GAIN // 48-bit intermediate product in CE hardware.
// Unity gain = 2^14
void afe_compensate_vref (int32_t stemp)
{
    int idx;
    int64_t gain_adj;

    if(0 == (ce_data.ceconfig & CE_EXT_TEMP)) // External compensation disabled?
        return;

    for(idx = FIRST_VREF; idx <= LAST_VREF; ++idx)
    {
        /* Perform temperature compensation of CE. The equations are:
         * s = STEMP, from the hardware
         * GAIN_ADJ = 2^14 + (PPMC * s)/(2^14) 
         * + (PPMC2 * (s^2))/(2^20)
         * Factoring: 2^14 + ( s * (PPMC + (PPMC2 * s)/(2^6)))/2^14
         * to minimize multiplcations and rounding error.
         * Add rounding: 2^14 + ( s * (PPMC + (PPMC2 * s + 2^6/2)/(2^6)) 
         * + 2^14/2)/2^14 
         * How does rounding work? e.g. round(x/2^n) = int(0.5 + x/2^n)
         * = (0.5*2^n + x)/2^n */
        gain_adj = ((((int64_t)afe_cal_ptr->ppmc2[idx]) * stemp) + 32LL) >> 6;
        gain_adj = (((((int64_t)afe_cal_ptr->ppmc[idx]) 
                        + gain_adj) * stemp) + 8192LL) >> 14; // >>14 = /2^14
        gain_adj += (int64_t)ce_ptr->unity_gain;
        afe_set_temp_gain(idx, (int)gain_adj);
    }
} /* afe_compensate_vref() */

#elif 0x40000000==UNITY_GAIN // 64-bit intermediate product in CE hardware.
// Unity gain = 2^30
void afe_compensate_vref (int32_t stemp)
{
    int idx;
    int64_t gain_adj;

    if(0 == (ce_data.ceconfig & CE_EXT_TEMP)) // External compensation disabled?
        return;

    for(idx = FIRST_VREF; idx <= LAST_VREF; ++idx)
    {
        /* Perform temperature compensation of CE. The equations are:
         * s = STEMP, from the hardware
         * gain_adj = 2^30 + (s * PPMC)/(2^15) + (s^2 * PPMC2)/(2^26)
         * Factor PPMC and PPMC2 terms, to minimize multiplications and
         * also minimize rounding error.
         * gain_adj = 2^30 + (s * (PPMC + (s * PPMC2)/(2^11))/(2^15) 
         * Add fixed-point round-to-nearest in both terms:
         * gain_adj = 2^30 + 
         *    (2^15/2 + s * (PPMC + (2^11/2 + s * PPMC2)/(2^11))/(2^15)
         * =  (2^14 + s * (PPMC + (2^10 + s * PPMC2)/(2^11))/(2^15)
         * How does rounding work? e.g. round(x/2^n) = int(0.5 + x/2^n)
         * = (0.5*2^n + x)/2^n 
         */
        // 0x400LL is 2^11/2, so the term rounds to nearest. >> 11 = /2^11
        gain_adj = (0x400LL + stemp*(int64_t)afe_cal_ptr->ppmc2[idx]) >> 11;
        // 0x4000LL is 2^15/2, so the term rounds to nearest.
        gain_adj = (0x4000LL +  stemp * (gain_adj 
                    + (int64_t)afe_cal_ptr->ppmc[idx]) ) >> 15; //>>15 = /2^15
        gain_adj += (int64_t)UNITY_GAIN;
        afe_set_temp_gain(idx, (int)gain_adj);
    }
} /* afe_compensate_vref() */

#else
#error Nonstandard unity gain
#endif


/***************************************************************************   
* Description:  
* Set temperature gain in CE, using number from compensate_vref, above.
*   Input:
*       phase_idx: 0 (voltage), 1,2,3=current phases;4=3-phase neutral
* Output:
* Return Status:
*       None.
***************************************************************************/
void afe_set_temp_gain(int phase_idx, int32_t temp_gain)
{
    ++phase_idx;
    ++temp_gain;
}


/****************************************************************************
* Description:
*       return the current accumulation interval's
*       wh in the AFE's native units.
*
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The watt-hour in CE units. The LSB (least significant bit value)
*         is as described for the CE code.
****************************************************************************/
int32_t afe_wsum(int phase_idx)
{
   switch(phase_idx)
   {
       case ALL_PHASES: 
           return PHASE_CNT*wsum;
       case PHASE_A:
       case PHASE_B:
            return wsum;
       default:
            return 0;
    }
} // afe_wsum


/****************************************************************************
* Description:
*       return the current accumulation interval's
*       simulated wh in the AFE's native units.
*       Use if the voltage is tampered, uses nominal V.
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The watt-hour in CE units. The LSB (least significant bit value)
*         is as described for the CE code.
****************************************************************************/
int32_t afe_wsum_sim(int phase_idx)
{
    return afe_wsum(phase_idx);
} // afe_wh_sim


/****************************************************************************
* Description:
*       return the current accumulation interval's
*       varh in the AFE's native units.
*
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The var-hour in CE units. The LSB (least significant bit value)
*         is as described for the CE code.
****************************************************************************/
int32_t afe_varsum(int phase_idx)
{
    switch(phase_idx)
    {
        case ALL_PHASES: 
            return PHASE_CNT*varsum;
        case PHASE_A:
        case PHASE_B:
            return varsum;
        default:
            return 0;
    }
} // afe_varsum


/****************************************************************************
* Description:
*       return the current accumulation interval's
*       vah (volt-amp hours) in the AFE's units for w0sum.
*
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The va-hour in CE units. The LSB (least significant bit value)
*         is as described for the CE code for w0sum.
****************************************************************************/
int32_t afe_vasum(int phase_idx)    // Both inputs filtered by CE's PLL.
{
    switch(phase_idx)
    {
        case ALL_PHASES: 
            return PHASE_CNT*vasum;
        case PHASE_A:
        case PHASE_B:
             return vasum;
        default:
             return 0;
    }
} // afe_vasum()



/****************************************************************************
* Description:
*       return the current accumulation interval's
*       V (voltage) in the AFE's native units.
*
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The voltage in CE units. The LSB (least significant bit value)
*         is as described for the CE code.
****************************************************************************/
int32_t afe_vsqsum(int phase_idx)    // Current, in CE units
{
   return vsqsum;
} // afe_vsqsum


/****************************************************************************
* Description:
*       return the current accumulation interval's
*       I (current) in the AFE's native units.
*
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The watt-hour in CE units. The LSB (least significant bit value)
*         is as described for the CE code.
****************************************************************************/
float afe_isqsum(int phase_idx)    // Current, in CE units
{
    float f;

    switch(phase_idx)
    {
       case ALL_PHASES: // Sum of currents.
           f = sqrtf(isqsum_f);
           f *= (float)PHASE_CNT;
           return f * f;
       default:
           return isqsum_f;
   }
} // afe_isqsum


#if NARROWBAND
/* Narrowband data */


/****************************************************************************
* Description:
*       return the current accumulation interval's
*       narrowband wh in the AFE's native units.
*
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The watt-hour in CE units. The LSB (least significant bit value)
*         is as described for the CE code.
****************************************************************************/
int32_t afe_wsum_nb(int phase_idx)
{
    return afe_wsum(phase_idx);
} // afe_wsum_nb


/****************************************************************************
* Description:
*       return the current accumulation interval's
*       simulated narrowband wh in the AFE's native units.
*       Use if the voltage is tampered, uses nominal V.
*
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The watt-hour in CE units. The LSB (least significant bit value)
*         is as described for the CE code.
****************************************************************************/
int32_t afe_wsum_nb_sim(int phase_idx) // Simulated Wh, From nominal V.
{
    return afe_wsum(phase_idx);
} // afe_wh_nb_sim

// int32_t afe_varsum_nb(int phase_idx) is intentionally omitted.

/****************************************************************************
* Description:
*       return the current accumulation interval's
*       narrowband vah (volt-amp hours) in the AFE's units for w0sum.
*
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The va-hour in CE units. The LSB (least significant bit value)
*         is as described for the CE code for w0sum.
****************************************************************************/
int32_t afe_vasum_nb(int phase_idx)    // Both inputs filtered by CE's PLL.
{
    return afe_vasum(phase_idx);
} // afe_vasum_nb()


/****************************************************************************
* Description:
*       return the current accumulation interval's
*       narrowband V (voltage) in the AFE's native units.
*
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The voltage in CE units. The LSB (least significant bit value)
*         is as described for the CE code.
****************************************************************************/
int32_t afe_vsqsum_nb(int phase_idx)    // Voltage, in CE units
{
    return afe_vsqsum(phase_idx);
} // afe_vsqsum_nb


/****************************************************************************
* Description:
*       return the current accumulation interval's
*       narrowband I (current) in the AFE's native units.
*       It's returned as a float so it can include the low-range value.
* Parameters:
*   Input:
*         the index of the phase. Note that a phase index of 0 indicates
*         the billable total reported by the meter.
*   Output:
*         The current in CE units. The LSB (least significant bit value)
*         is as described for the CE code. The low-scale current is added-in
****************************************************************************/
float afe_isqsum_nb(int phase_idx)    // Current, in CE units
{
    return afe_isqsum(phase_idx);
} // afe_isqsum_nb

// End narrowband access
#endif // ifdef ce_data.w0sum_h_x


/****************************************************************************
* Description:
*       return the CE's pulse counts for Wh and VARh
* Parameters:
*   Input:
*       none
*   Output:
*       Pulse count
****************************************************************************/
int32_t afe_wh_pulse_cnt(void)
{
    return afe_wh_cnt;
}
int32_t afe_varh_pulse_cnt(void)
{
    return afe_varh_cnt;
}


/****************************************************************************
* Description:
*       Set the CE's pulse counts for Wh and VARh
* Parameters:
*   Input:
*       Pulse count
*   Output:
*       none.
****************************************************************************/
void afe_set_wh_pulse_cnt(int32_t cnt)
{
    afe_wh_cnt = cnt;
}
void afe_set_varh_pulse_cnt(int32_t cnt)
{
    afe_varh_cnt = cnt;
}

/* Unit conversions */

/****************************************************************************
* Description:
*       Calculate Wh per pulse, starting from value with LSB of ce_data.wrate
* Parameters:
*   Input:
*       Value in LSB of CE's ce_data.wrate
*   Output:
*       Wh per pulse
****************************************************************************/
float afe_wrate2wh(int32_t wrate)
{
    float num, den;
    num = (float)afe_cal_ptr->v_max;      // in 0.1v units
    num *= (float)afe_cal_ptr->i_max;     // in 0.1a units
    num *= (ce_ptr->cp);   // Combine vmax and imax constants
    if(ce_data.ceconfig & PULSE_SLOW)
        num *= (ce_ptr->pulse_slow_factor);
    num *= (ce_ptr->wrate_power_of_two);

    den = (float)wrate;
    // Combine constants; 3600 seconds/hr, LSB of vmax & imax
    den *= (ce_ptr->fs) * (ce_ptr->nacc) * (ce_ptr->x) * 3600.0 * 10.0 * 10.0; 
    if(ce_data.ceconfig & PULSE_FAST)
        den *= (ce_ptr->pulse_fast_factor);

    return num/den;
} // afe_wrate2wh()


/****************************************************************************
* Description:
*       Calculate value with LSB of CE's ce_data.wrate, from Wh per pulse.
* Parameters:
*   Input:
*       Wh per pulse
*   Output:
*       Value in LSB of CE's ce_data.wrate
****************************************************************************/
int32_t afe_wh2wrate(float wh)
{
    int wrate;
    float num, den;
    num = (float)afe_cal_ptr->v_max;      // in 0.1v units
    num *= (float)afe_cal_ptr->i_max;     // in 0.1a units
    num *= (ce_ptr->cp);   // Combine vmax and imax constants
    if(ce_data.ceconfig & PULSE_SLOW)
        num *= (ce_ptr->pulse_slow_factor);
    num *= (ce_ptr->wrate_power_of_two);

    den = (float)wh;
    // Combine constants; 3600 seconds/hr, LSB of vmax & imax
    den *= (ce_ptr->fs) * (ce_ptr->nacc) * (ce_ptr->x) * 3600.0 * 10.0 * 10.0; 
    if(ce_data.ceconfig & PULSE_FAST)
        den *= (ce_ptr->pulse_fast_factor);

    // Adding 1 forces the rounding up, reducing
    // the Kh below nominal, therefore making the calibration's
    // roundoff favor the customer.
    wrate = 1 + lroundf(num/den);
    return wrate;
} // afe_wh2wrate()


/****************************************************************************
* Description:
*       Calculate floating-point voltage from value in CE's vsqsum LSB
* Parameters:
*   Input:
*       vsqsum value
*   Output:
*       volts in floating point
****************************************************************************/
float afe_vsqsum2v(int32_t vsqsum)
{
    float num, den;
    num = (float)afe_cal_ptr->v_max;
    num *= num;
    num *= (float)vsqsum;
    num *= (ce_ptr->cvsq);
    den = (ce_ptr->sum_samp) * 100.0; // vmax_lsb
    return sqrtf(num/den);
} // afe_vsqsum2v()


/****************************************************************************
* Description:
*       Take floating-point volts and calculate value in CE's vsqsum LSB
* Parameters:
*   Input:
*       volts in floating point
*   Output:
*       vsqsum value
****************************************************************************/
int32_t afe_v2vsqsum(float v)
{
    float num, den;
    // Calculates VxSQSUM_X, given a voltage
    num = v * v;
    num *= (ce_ptr->sum_samp) * 100.0; // 100.0 = 10x10 = vmax^2 LSB
    den = (float)afe_cal_ptr->v_max;
    den *= den;
    den *= (ce_ptr->cvsq);
    return lroundf(num/den);
}   // afe_v2vsqsum()


/****************************************************************************
* Description:
*       Calculate floating-point voltage from sag or swell threshold
* Parameters:
*   Input:
*       sag or swell threshold
*   Output:
*       volts in floating point
****************************************************************************/
#define SQRT_2 (1.414213562)
float afe_thr2v(int32_t thr)
{
    float num;
    num = (ce_ptr->cv)/SQRT_2;
    num *= ((float)afe_cal_ptr->v_max)/10.0;
    num *= (float)thr;
    return num;
} // afe_thr2v()


/****************************************************************************
* Description:
*       Calculate sag or swell threshold from volts.
* Parameters:
*   Input:
*       volts in floating point
*   Output:
*       sag or swell threshold
****************************************************************************/
int32_t afe_v2thr(float v)
{
    float num, den;
    // Calculates VxSQSUM_X, given a voltage
    num = SQRT_2; // sqrt(2)
    num *= v;
    den = (ce_ptr->cv);
    den *= ((float)afe_cal_ptr->v_max)/10.0;
    return lroundf(num/den);
}   // afe_v2thr()


/****************************************************************************
* Description:
*       Calculate floating-point amps from value in CE's isqsum LSB
* Parameters:
*   Input:
*       isqsum value
*   Output:
*       amps in floating point
****************************************************************************/
float afe_isqsum2a(float isqsum)
{
    float num, den;
    num = (float)afe_cal_ptr->i_max;
    num *= num;
    num *= isqsum;
    num *= (ce_ptr->cisq);
    den = (ce_ptr->sum_samp) * 100.0; // imax_lsb
    return sqrtf(num/den);
} // afe_isqsum2a()


/****************************************************************************
* Description:
*       Take floating-point amps and calculate value in CE's isqsum LSB
* Parameters:
*   Input:
*       amps in floating point
*   Output:
*       isqsum value
****************************************************************************/
float afe_a2isqsum(float v)
{
    float num, den;
    // Calculates IxSQSUM_X, given an amperage
    num = v * v;
    num *= (ce_ptr->sum_samp) * 100.0; // 100.0 = 10x10 = vmax^2 LSB
    den = (float)afe_cal_ptr->i_max;
    den *= den;
    den *= (ce_ptr->cisq);  // imax lsb
    return num/den;
} // afe_a2isqsum()


/****************************************************************************
* Description:
*       Calculate floating-point watt-hours from value in CE's wsum LSB
* Parameters:
*   Input:
*       wsum value
*   Output:
*       watt-hours in floating point
****************************************************************************/
float afe_wsum2wh(int32_t wsum)
{
    float num;
    num = ((float)afe_cal_ptr->v_max);
    num *= ((float)afe_cal_ptr->i_max);
    // vmax & imax LSB
    num *= (ce_ptr->cp) / ((ce_ptr->fs) * 3600.0 * 10.0 * 10.0);
    num *= wsum;
    return num;
} // afe_wsum2wh()


/****************************************************************************
* Description:
*       Calculate value in CE's wsum LSB from floating-point watt-hours 
* Parameters:
*   Input:
*       wsum value
*   Output:
*       watt-hours in floating point
****************************************************************************/
int32_t afe_wh2wsum(float wh)
{
    float den;

    wh *= (10.0 * 10.0 * 3600.0 * (ce_ptr->fs));    // vmax & imax lsb, 3600s/hr
    den = ((float)afe_cal_ptr->v_max);
    den *= ((float)afe_cal_ptr->i_max);
    den *= (ce_ptr->cp);
    return lroundf(wh/den);
} // afe_wh2wsum()


/****************************************************************************
* Description:
*       Calculate floating-point watts from value in CE's wsum LSB
* Parameters:
*   Input:
*       wsum value
*   Output:
*       watts in floating point
****************************************************************************/
float afe_wsum2w(int32_t wsum)
{
    float num;
    num = ((float)afe_cal_ptr->v_max);
    num *= ((float)afe_cal_ptr->i_max);
    num *= (ce_ptr->cp) / ((ce_ptr->sum_samp) * 10.0 * 10.0);// vmax & imax LSB
    num *= wsum;
    return num;
} // afe_wsum2w()
float afe_wsumf2w(float wsum)
{
    float num;
    num = ((float)afe_cal_ptr->v_max);
    num *= ((float)afe_cal_ptr->i_max);
    num *= (ce_ptr->cp) / ((ce_ptr->sum_samp) * 10.0 * 10.0);// vmax & imax LSB
    num *= wsum;
    return num;
} // afe_wsum2w()


/****************************************************************************
* Description:
*       Calculate value in CE's wsum LSB from floating-point watts
* Parameters:
*   Input:
*       watts in floating point
*   Output:
*       wsum value
****************************************************************************/
int32_t afe_w2wsum(float w)
{
    float den;

    w *= (10.0 * 10.0 * (ce_ptr->sum_samp));    // vmax & i_max lsb
    den = ((float)afe_cal_ptr->v_max);
    den *= ((float)afe_cal_ptr->i_max);
    den *= (ce_ptr->cp);
    return lroundf(w/den);
} // afe_w2wsum()
float afe_w2wsumf(float w)
{
    float den;

    w *= (10.0 * 10.0 * (ce_ptr->sum_samp));    // vmax & i_max lsb
    den = ((float)afe_cal_ptr->v_max);
    den *= ((float)afe_cal_ptr->i_max);
    den *= (ce_ptr->cp);
    return w/den;
} // afe_w2wsumf()


/****************************************************************************
* Description:
*       Find the frequency value produced by the CE.
*
* Parameters:
*   Input:
*         Value produced by the CE.
*   Output:
*         Frequency of ac mains.
****************************************************************************/
#define TWOTO32F 4.294967296e9 // = 2.0^32
float afe_freqx2hz(int freq_x) /* calculate mains freq. */
{
    /* The LSB of afe_freq is FS/2^32, so one Hertz is
     * 2^32/FS; FS=sample frequency. */
    return ((float)(freq_x)) * ((ce_ptr->fs)/TWOTO32F);
} // afe_frequency


/****************************************************************************
* Description:
*       Find the frequency value produced by the CE.
*
* Parameters:
*   Input:
*         Frequency of ac mains.
*   Output:
*         Value produced by the CE.
****************************************************************************/
int afe_hz2freqx(float hz) /* calculate mains freq. */
{
    /* The LSB of afe_freq is FS/2^32, so one Hertz is
     * 2^32/FS; FS=sample frequency. */
    return lroundf((hz*TWOTO32F)/ce_ptr->fs);
} // afe_frequency


/****************************************************************************
* Description:
*       Get the samples per accumulation interval.
* Parameters:
*   Input:
*       none
*   Output:
*       samples in accumulation interval.
****************************************************************************/
int32_t afe_sumpre(void) /* samples per accumulation interval */
{
    return (ce_ptr->sum_samp);
} // afe_sumpre()


/****************************************************************************
* Description:
*       vasum creep logic. i.e. it performs creep on the CE's Wh data.
* Parameters:
*   Input:
*       phase_index, used to select the all-phases or individual creep
*       Wh as CE wsum
*   Output:
*       Wh as CE wsum, or zero if the data is invalid.
****************************************************************************/
int32_t afe_vasum_creep(int32_t phase_idx, int32_t vasum)
{
    int t;
    if(0 == afe_cal_ptr->i_min)   // if creep is disabled
        return vasum;

    if(afe_starting_cnt)            // If data is invalid.
        return 0;
    if(ALL_PHASES == phase_idx)     // select the creep threshold
        t = afe_vasum_min_total;
    else
        t = afe_vasum_min;
    if(labs(vasum) <= t)            // if below threshold.
        return 0;
    return vasum;
}


/****************************************************************************
* Description:
*       isqsum creep logic. i.e. it performs creep on the CE's Amps data.
* Parameters:
*   Input:
*       A as floating-point CE isqsum
*   Output:
*       A as floating-point CE isqsum, or zero if the data is invalid.
****************************************************************************/
float afe_isqsum_creep(int32_t phase_idx, float isqsum_f)
{
    float f;
    if(0 == afe_cal_ptr->i_min)   // if creep is disabled
        return isqsum_f;
    if(afe_starting_cnt)            // If data is invalid.
        return 0.0;
    if(ALL_PHASES == phase_idx)     // select the creep threshold
        f = afe_isqsum_min_total_f;
    else
        f = afe_isqsum_min_f;
    if(isqsum_f <= f)				// if below threshold.
        return 0.0;
    return isqsum_f;
}


/****************************************************************************
* Description:
*       vsqsum creep logic. i.e. it performs creep on the CE's V data.
* Parameters:
*   Input:
*       V as CE vsqsum
*   Output:
*       V as CE vsqsum, or zero if the data is invalid.
****************************************************************************/
int32_t afe_vsqsum_creep(int32_t vsqsum)
{
    if(0 == afe_cal_ptr->i_min)   // if creep is disabled
        return vsqsum;
    if(afe_starting_cnt)            // If data is invalid.
        return 0;
    if(vsqsum <= afe_vsqsum_min)    // if below threshold.
        return 0;
    return vsqsum;
}


/****************************************************************************
* Description:
*       Return wideband (i.e. normal metering) data from AFE in 
*       engineering units.
*       Vrms
*       Arms
*       W
*       VAR
*       VA
*       power factor
* Parameters:
*   Input:
*       phase index (all of meter = 0, 1, 2, 3 = phase ABC, 4 = neutrl
*   Output:
*       engineering units
****************************************************************************/
float afe_vrms(int phase_idx)
{
    return afe_vsqsum2v(afe_vsqsum_creep(afe_vsqsum(phase_idx)));
}

// Measured Arms
float afe_arms(int phase_idx)
{
    return afe_isqsum2a(afe_isqsum_creep(phase_idx, afe_isqsum(phase_idx)));
}

// Measured W
float afe_w(int phase_idx) /* watts of current acc. interval */
{
    return afe_wsum2w(afe_vasum_creep(phase_idx, afe_wsum(phase_idx)));
}

// Measured Wh
float afe_wh(int phase_idx) /* watts of current acc. interval */
{
    return afe_wsum2wh(afe_vasum_creep(phase_idx, afe_wsum(phase_idx)));
}

// Measured VAR
float afe_var(int phase_idx) /* volt-amps reactive of curr.interval */
{
    return afe_wsum2w(afe_vasum_creep(phase_idx, afe_varsum(phase_idx)));
}

// Measured VARh
float afe_varh(int phase_idx) /* volt-amps reactive of curr.interval */
{
    return afe_wsum2wh(afe_vasum_creep(phase_idx, afe_varsum(phase_idx)));
}

// Measured VA
float afe_va(int phase_idx) /* VAh of current acc. interval */
{
    return afe_wsum2w(afe_vasum_creep(phase_idx, afe_vasum(phase_idx)));
}

// Measured Power Factor, percent
float afe_power_factor(int phase_idx) /* power factor */
{
    int32_t w,va;

    va = afe_vasum_creep(phase_idx, afe_vasum(phase_idx));

    // Prevent division by zero. Also check if data is valid.
    if(0 == va)
        return 1.0f;

    w = afe_vasum_creep(phase_idx, afe_wsum(phase_idx));

    return ((float)w)/((float)va);
}


#if NARROWBAND
/****************************************************************************
* Description:
*       Return narrowband data from AFE in engineering units.
*       Vrms
*       Arms
*       W
*       VAR
*       VA
*       power factor
* Parameters:
*   Input:
*       phase index (all of meter = 0, 1, 2, 3 = phase ABC, 4 = neutrl
*   Output:
*       engineering units
****************************************************************************/
float afe_vrms_nb(int phase_idx)
{
    return afe_vsqsum2v(afe_vsqsum_creep(afe_vsqsum_nb(phase_idx)));
}

// Measured Arms
float afe_arms_nb(int phase_idx)
{
    return afe_isqsum2a(afe_isqsum_creep(phase_idx, afe_isqsum_nb(phase_idx)));
}

// Measured W
float afe_w_nb(int phase_idx) /* watts of current acc. interval */
{
    return afe_wsum2w(afe_vasum_creep(phase_idx, afe_wsum_nb(phase_idx)));
}

// Narrowband VAR is intentionally absent

// Measured VA
float afe_va_nb(int phase_idx) /* VAh of current acc. interval */
{
    return afe_wsum2w(afe_vasum_creep(phase_idx, afe_vasum_nb(phase_idx)));
}

// Measured Power Factor, percent
float afe_power_factor_nb(int phase_idx) /* power factor */
{
    int32_t w,va;

    va = afe_vasum_creep(phase_idx, afe_vasum_nb(phase_idx));

    // Prevent division by zero. Also check if data is valid.
    if(0 == va)
        return 1.0f;

    w = afe_vasum_creep(phase_idx, afe_wsum_nb(phase_idx));

    return ((float)w)/((float)va);
}

// Total Harmonic Distortion for Watts
float afe_w_thd(int phase_idx) /* T*/
{
    return 0;
}

// Total Harmonic Distortion for Current.
float afe_arms_thd(int phase_idx)
{
    return 0;
}

// Total Harmonic Distortion for Voltage.
float afe_vrms_thd(int phase_idx)
{
    return 0;
}
#endif // narrowband


/****************************************************************************
* Description:
*       Read the count of zero-crossings of AC mains
*       In some regions, this count, over 24 hours, is as accurate as an
*       atomic clock. In these regions it can be used to rate the crystal.
* Parameters:
*   Input:
*         None.
*   Output:
*         zero crossings of ac mains in the most recent accmumultion interval.
****************************************************************************/
int32_t afe_zero_crossings(void)
{   // If in voltage creep or data is invalid.
    if(0 == afe_vsqsum_creep(afe_vsqsum(ALL_PHASES)))
        return 0;

    return 100;
}


/****************************************************************************
* Description:
*       Read the frequency of AC mains.
*       The frequency is read at a particular phase.  On multiphase
*       systems, the phase to read can be set.  It's also automatically
*       switched to phase with voltage. the switch only occurs if a phase
*       measuring it does not have voltage.  Once switched, it stays.
*
* Parameters:
*   Input:
*         None.
*   Output:
*         Frequency of ac mains at a particular phase.
****************************************************************************/
float afe_frequency(void) /* mains frequency, in Hz */
{   // If in voltage creep, or data is invalid.
    if(0 == afe_vsqsum_creep(afe_vsqsum(ALL_PHASES)))
        return 0.0;

    /* The LSB of afe_freq is FS/2^32, so one Hertz is
     * 2^32/FS; FS=sample frequency. */
    return 50.0;
} // afe_frequency


/****************************************************************************
* Description:
*       voltage angle from phase a to b;
*       This is an installation test so an electrician
*       can verify that 3-phase motors will work after
*       a 3-phase customer drop is installed.
*       Extreme accuracy is not usually required.
* Parameters:
*   Input:
*         None.
*   Output:
*         Voltage angle from phase A to phase B
****************************************************************************/
float afe_a_to_b (void) /* 3-phase only, degrees */
{
    return 120.0; /* stubbed */
} // afe_a_to_b


/****************************************************************************
* Description:
*       voltage angle from phase a to c;
*       This is an installation test so an electrician
*       can verify that 3-phase motors will work after
*       a 3-phase customer drop is installed.
*       Extreme accuracy is not usually required.
* Parameters:
*   Input:
*         None.
*   Output:
*         Voltage angle from phase A to phase C
****************************************************************************/
float afe_a_to_c (void) /* 3 phase only, degrees */
{
    return 0; /* stubbed */
} // afe_a_to_c

/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

