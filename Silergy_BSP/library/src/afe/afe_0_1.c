/**************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Description : Metering for an AFE accessed via Shared RAM.
 * This implements Meter drop equation 0, i.e. Wh = VA * IA
 * It can also be used with equation 1, i.e. W = 0.5*V*(IA - IB)
 *
 * This uses a CE wired so Wh0 = V0 * I0, Wh1 = V0 * I1
 *
 * To make it more readable, this code intentionally combines
 * the metering and start-up logic is in one file.
 * This design makes code readable, but reuse and testing is more difficult.
 *
 * Copyright 2020, Silergy Corp. All rights reserved.
 *
 * $Id: afe_0_1.c 24340 2020-04-08 23:36:03Z rvandewa $
 ***************************************************************************/
#include <math.h>       // atan2()
#include "options.h"
#include "delay.h"      // Calibrated programmed delay
#include "gpio.h"       // To disable and enable pulses via the GPIO API.
#include "mmath.h"      // lroundf
#include "Defines.h"
#include "asdaq_variables.h"
// #include "dlms_variables.h"

#if REMOTES
#include "log.h"        // needs to report errors.
#include "rmt.h"        // remote interface driver
#endif
#include "afe.h"        // Check function prototypes
#include "info_block.h" // Read info-block to set defaults.

#define VA_FROM_W_VAR 1 // 1:va = sqrt(w^2 + var^2)

/*** Version ****/
const char fw_ver_afe[] =
"afe_0_1" /* Device Driver name */
" v"     /* ver string */
"3.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

#if EQUATION!=EQUATION0 && EQUATION!=EQUATION1
#error Meter EQUATION symbol is wrong!
#endif
#if 1!=PHASE_CNT
#error The count of phases, PHASE_CNT is misconfigured, and should be 1.
#endif

// This equation usually has different current sensors for the primary
// phase L1 and neutral. All known CE codes handle it, so there
// is no code to translate LSBs in the MPU.



// When nonzero, the data from the CE is invalid.
uint8_t afe_starting_cnt = CE_PLL_CNT;

// Calibration data (i.e. afe-specific data)
Cal_t *afe_cal_ptr;

// CE data; ce_ptr is in main.c

// Becomes zero when the CE data is ready.
int afe_busy = -1;

float afe_isqsum_min_f; // Starting current for one phase
int32_t afe_vsqsum_min; // Starting voltage

// I1 must be scaled by MPU software.
float afe_insq2ixsq_f;      // * neutral i^2 -> normal i^2
int64_t afe_wnsum2wsum;     // neutral w, to w, fixed-point 

static float vsum_nom_f;    // floating point sqrt(vsqsum_nom) nominal V

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
        ce_ptr = &default_ce;

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

    ce_put_cal( &(c_ptr->afe_cal_a[0]), sizeof(c_ptr->afe_cal_a)/4 );

    // Symbols beginning PCB_* are from the PCB's map.h file.
    return afe_wh2wsum(PCB_WH_PLCDCNT); // Wh/LCD count
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

    // Copy the calibration data to the calibration array.
    // It is already in the CE RAM, so leave it there.
    ce_get_cal( &(c_ptr->afe_cal_a[0]), sizeof(c_ptr->afe_cal_a)/4 );

    // Symbols beginning PCB_* are from the PCB's map.h file.
    return afe_wh2wsum(PCB_WH_PLCDCNT); // CE Wh/LCD
}


/****************************************************************************
* Description:
* Recalculates, defaults calculated from the i_max and v_max of the sensors.
* Does not affect calibration.
*
* Parameters:
* Input:
* Cal_t includes i_max, v_max, i_min (the starting current), v_min
* (the starting voltage), and a copy of the CE�s calibration array. 
* Cal_t should be saved and restored by higher-level code�s 
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
        ce_data.ib_scale = 1;

        while(true)
        {
            // Set software scaling factor for I1SCALE using IBSCALE.
            fT = sqrt((ce_ptr->cisqn)/(ce_ptr->cisq));
            fT *= ((float)c_ptr->i_max_neutral)/((float)c_ptr->i_max);
            fT *= (float)ce_ptr->unity_gain;
            fT /= (float)ce_data.ib_scale;
            if( ( 2.0*(float)UNITY_GAIN ) > fT)
                break;
            ce_data.ib_scale *= 2;
            if(ce_data.ib_scale > 16)
            {
                ce_data.ib_scale = 16;
                break;
            }
        }
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
#define FIRST_REMOTE PHASE_A     // If there is a remote, it is PHASE A
#define LAST_REMOTE PHASE_A

/****************************************************************************
* Description:
* Sets defaults in the AFE, including defaults calculated from
* the i_max and v_max of the sensors.
*
* Parameters:
* Input:
* Cal_t includes i_max, v_max, i_min (the starting current), v_min
* (the starting voltage), and a copy of the CE�s calibration array. 
* Cal_t should be saved and restored by higher-level code�s 
* save and restore of the calibration.
* Return Status:
*   returns counts per pulse, counts' LSBs as returned by afe_wh();
*   or afe_native_per_pulse(), suitable for initializing the meter math add().
****************************************************************************/
int32_t afe_get_cal_default(Cal_t *c_ptr)
{
    //int trimt;
    float trimt_f;
    int chan_idx, ppmc, ppmc2;
	  #ifdef INFO_BLOCK_H
    int32_t trimt;
	  #endif

    c_ptr = afe_valid_cal_ptr(c_ptr);
    ce_ptr = afe_valid_ce_ptr(ce_ptr);

    // Copy default CE calibration in flash to CE_RAM.
    ce_get_cal_default(CE_PARM_BASE, 
            ( (ce_ptr->data_ptr) + ( CE_PARM_BASE - CE_START_ADDR ) ), 
            sizeof(c_ptr->afe_cal_a)/4);

    // Permit external temperature compensation, external pulse.
    ce_data.ceconfig |= CE_EXT_TEMP | EXT_PULSE;
    // Disable zero-crossing interrupt, internal creep. 
    ce_data.ceconfig &= ~(EDGE_INT | INT_CREEP);

    // Set temperature compensation for the SoC's Vref.
    // Note that this can be the same for all CE codes.
    // This is for a regular part without high-precision trims
    // and no remotes. Remotes are calculated later, after the CE starts,
    // because their trims have to be read from the remotes after the 
    // interface starts.
    #ifdef INFO_BLOCK_H
    // Trim location is in device's driver include file, SY7T_Mxx.h
    trimt = info_block_read( TRIMT_INDEX );
    trimt = ~trimt;
    trimt <<= TRIMT_LEFT_SHIFT;
    trimt >>= TRIMT_RIGHT_SHIFT;
    trimt_f = (float)(trimt);
    #else
    trimt_f = 0;
    #endif
    // ppmc and ppmc2 are the coefficients of an integer polynomial
    // that describes the voltage of the ADC's voltage reference
    // over temperature. The output of the polynomial is multiplied 
    // with ADC results to cancel temperature error.
    // The integer polynomial for this SoC is described in CE reference 
    // manual(s).
    // Vrefnom is the nominal value at 22C from the SoC's data sheet.
    // The calculations for TC1 and TC2 are from the SoC's data sheet.
    // Calculations for customer-supplied components can be added.
    // The evaluation board user's manual has details.
    // "H-series" high-accuracy SoCs are possible with special factory
    // testing. Contact a representative for details.
    // TCx_TO_PPMC and VREVNOM are in device's driver include file SY7T_Mxx.h
    // For AM11C0:
    ppmc  = lroundf ( (TC1_TO_PPMC/VREFNOM) * // convert for integer polynomial
        // calculate TC1 from datasheet
        (5.9790e-5 + (-3.3866e-6 * trimt_f) + (4.5681e-8 * trimt_f * trimt_f)));
    ppmc2 = lroundf ( (TC2_TO_PPMC2/VREFNOM) * // convert for integer polynomial
        (-2.5406e-9) );   // TC2 from datasheet
    for (chan_idx = FIRST_VREF; chan_idx <= LAST_VREF; ++chan_idx)
    {   // Set all temperature compensation channels.
        c_ptr->ppmc[chan_idx]  = ppmc; // for current and voltage
        c_ptr->ppmc2[chan_idx] = ppmc2;
    }

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
//    else
//    {
//        int32_t w = afe_cal_ptr->pulse_src;

//        // Synchronously set pulse inputs from the CE's Wh & VARh values.
//        if(0 == w) { // default: sum of all Wh.
//            w = afe_vasum_creep(ALL_PHASES, afe_wsum(ALL_PHASES));
//        } else { // set from any CE variable. (e.g. for narrowband Wh)
//            w = *(afe_cal_ptr->pulse_src + (int32_t*)CE_RAM);
//        }
//        afe_set_wpulse(w);

//        afe_set_varpulse(afe_vasum_creep(ALL_PHASES, afe_varsum(ALL_PHASES)));
//    }
}

void PulseGeneration(void)
{
    //afe_busy = 0; // Data is ready.
    
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

    afe_starting_cnt = ce_ptr->pll_cnt;
    afe_busy = -1; // tell the system that no data is available.
    global.misc.acc_cnt = 0; /* restart the count of accumulation intervals */

    /* Set-up the CE, including pulse output defaults */
    (*(ce_ptr->ce_init_fn))();
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
int32_t afe_enable(const ce_t *ce_ptr_tmp, Cal_t *c_ptr)
{
    float f;
    #if REMOTES
    int t;
    #endif

    c_ptr = afe_valid_cal_ptr(c_ptr);
    ce_ptr = afe_valid_ce_ptr(ce_ptr_tmp);

    // Put the calibration data into the CE.
    afe_put_cal(c_ptr);

    // Recalculate internal minimums for voltage, current and volt-amps
    afe_vsqsum_min = afe_v2vsqsum(((float)c_ptr->v_min)/10.0);
    afe_isqsum_min_f = afe_a2isqsum(((float)c_ptr->i_min)/1000.0);

    // Calculate the nominal voltage of the mission-mode
    // CE so it can be multiplied into the sqrt of the 
    // current to get wsum values for the mission-mode CE.
    // In production meters, vsum_nom_f is a constant.
    f = (float)(c_ptr->v_nom)/10.0;
    vsum_nom_f = sqrtf((float)afe_v2vsqsum(f));

    // Set the scaling for neutral.
    if(0 == ce_ptr->i1_scaled_by_ce)
    {
        // Set software scaling factors for insqsum and wnsum.
        afe_insq2ixsq_f = ((float)c_ptr->i_max_neutral)/((float)c_ptr->i_max);
        afe_insq2ixsq_f *= afe_insq2ixsq_f;
        afe_wnsum2wsum = 
            ((((int64_t)c_ptr->i_max_neutral << 32) 
              + (int64_t)(c_ptr->i_max/2))/c_ptr->i_max)>>32;
    }

    /* Enable the CE */
    afe_starting_cnt = ce_ptr->pll_cnt;
    (*(ce_ptr->ce_enable_fn))();

    #if REMOTES
    // If needed, set the PPMCs for remotes when they start.
    if( is_read != c_ptr->cal_src // default calibration?
            && c_ptr->ppmc2[NATIVE_VREF] 
            == c_ptr->ppmc2[FIRST_REMOTE] // remote values?
            && 0 != c_ptr->ppmc2[NATIVE_VREF] // & not zeroed for testing?
      )
    {   // Make a default temperature compensation for each remote.
        auto float tc1, tc2;

        // Reads back the float linear temperature compensation TC1 as 1/C, 
        // and quadratic TC2 as 1/(C^2) from remotes.
        // The CE design sets the constants TC1_TO_PPMC and TC2_TO_PPMC2 
        // and the channel indexes of remotes.
        for (t = FIRST_REMOTE; t <= LAST_REMOTE; ++t)
        {   // rmt_get_ppmc() routines encapsulate the remote-specific
            // trim reading and temperature-coefficient calculations.
            if(rmt_get_ppmc(&tc1, &tc2, t))
                log_event(EV_HARDWARE);

            c_ptr->ppmc[t]  = lroundf(TC1_TO_PPMC * tc1); 
            c_ptr->ppmc2[t] = lroundf(TC2_TO_PPMC2 * tc2); 
        }
    }
    #endif

    // Set the xfer_busy interrupt to detect when data is ready.
    // afe-data-ready is a call-back fn, called when the interrupt runs.
    ce_int_enable(XFER_N_I, afe_data_ready);

    // Symbols beginning PCB_* are from the PCB's map.h file.
    return afe_wh2wsum(PCB_WH_PLCDCNT); // CE Wh/LCD
} // afe_enable


/****************************************************************************
* Description:
* Simulates the CE 
*
* Parameters:
* Input:
* Cal_t includes the CE�s calibration array. Cal_t should be saved and 
* restored by higher-level code�s save and restore of the calibration.
* Return Status:
* returns counts per pulse, counts' LSBs as returned by afe_wh();
* or afe_native_per_pulse(), suitable for initializing the meter math add().
****************************************************************************/
#define PI (3.14159265359)
#define RADIANS_PER_DEGREE (PI/180.0)
void afe_sim(void)
{
    int i;
    float f, va, radians, sine, cosine;

    afe_disable();

    // Calculate nominal values from the calibration expectations.
    // Simulated current.
    i = (int32_t)afe_a2isqsum(((float)afe_cal_ptr->i_cal)*0.1);
    Local_RAM_ce_data.i0sqsum_x_h = i;
    Local_RAM_ce_data.i1sqsum_x_h = i; 

    // Simulated voltage.
    f = afe_v2vsqsum(((float)afe_cal_ptr->v_cal)*0.1);
    i = (int32_t)f;
    Local_RAM_ce_data.v0sqsum_x = i;

    // Simulated volt-amps.
    // va = i * v, calculated directly from test signal description.
    va = afe_w2wsumf(
            ((float)afe_cal_ptr->v_cal)
            * ((float)afe_cal_ptr->i_cal)
            * 0.01
            );

    // Simulated Wh.
    radians = ((float)afe_cal_ptr->theta_cal)*RADIANS_PER_DEGREE;
    cosine = cosf(radians);
    i = lroundf(cosine * va);
    Local_RAM_ce_data.w0sum_x = i;
    Local_RAM_ce_data.w1sum_x = i;

    // Simulated VARh.
    sine = sinf(radians);
    i = lroundf(sine * va);
    Local_RAM_ce_data.var0sum_x = i;
    Local_RAM_ce_data.var1sum_x = i;

    // Simulated mains frequency.
    // PCB_* symbols are from the application PCB description file, e.g. map.h.
    ce_data.freq_x = afe_hz2freqx((float)PCB_HZ_NOM);
    // Set the simulated zero-crossing count.
    ce_data.mainedge_x = 2 * PCB_HZ_NOM;
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
    afe_starting_cnt = ce_ptr->pll_cnt;
    afe_busy = -1; // tell the system that no data is available.
    ce_deinit();
    ce_int_enable(XFER_N_I, NULL);
} // afe_disable


/****************************************************************************
* Description:
*   Gets the AFE�s status.
* Parameters:
*   Input:
*         a mask to select the status desired
* Return Status:
*         return: true if data is ready.
****************************************************************************/
int afe_status(int select_mask)
{
    int ret_val = 0; // no data

    if(select_mask & AFE_RUNNING)
    {
        if(CE->CNTL_b.ce_en)
            ret_val |= AFE_RUNNING;

        if(0 <= afe_busy) // simulated?
            ret_val |= AFE_RUNNING;
    }

    if(select_mask & AFE_HAS_DATA)
    {
        if (0 == afe_busy) // data is ready.
        {
            afe_busy = 1;  // afe is busy again.

            ++global.misc.acc_cnt; // count accumulation intervals.

            if (afe_starting_cnt)
            {
                --afe_starting_cnt;
                if(0 == afe_starting_cnt)
                {
                    // Enable wpulse and vpulse after glitches are done.
                    afe_pulse_enable();     // Enable WPULSE and VPULSE outputs.

                    ret_val |= AFE_HAS_DATA; // Data just became available.
									PowerOnSec=0;
                }
                // else Data is not valid yet.
            } else {
                ret_val |= AFE_HAS_DATA; // Data just became available.
            }

            // Things that need to be done each accumulation interval.
            // data is available, see if anything is needed.
        } // else if data not ready, do nothing
    } // else if data ready status not wanted
    return ret_val;   // No data is the default.
} // afe_status()


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
    const char *ch_ptr;
    ch_ptr = ce_name();
    return (const char *)ch_ptr;
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
//int32_t Power_Magnet_Tamper=0x07D383E0;
int32_t afe_set_wpulse(int32_t wh)
{
/*	
    if(ce_data.ceconfig & EXT_PULSE)
    {
        ce_data.apulsew = wh;
    } else { 
        // clear ce_data.apulsew
        ce_data.apulsew = 0; 
    }
*/
	
	if(magnetic_stat==1)
	{
		inst_voltage=2400;
		inst_phase_current=inst_neutral_current=30000;
		inst_kw=7200;
		inst_freq=500;
		inst_pf=100;
		//ce_data.apulsew=Power_Magnet_Tamper;
	}
	
	//else
		{
	
					if(labs(Local_RAM_ce_data.w0sum_x)>labs(Local_RAM_ce_data.w1sum_x))
						 ce_data.apulsew=Local_RAM_ce_data.w0sum_x;
					else
						 ce_data.apulsew=Local_RAM_ce_data.w1sum_x;
				}
		
//		  ce_data.apulsew =  9876425;

    // If forcing a creep value, prevent frequency generation.
    // even if the CE is operating from an internal pulse.
   // if(0 == wh && 0 != afe_cal_ptr->i_min )
      //  ce_data.wsum_accum = 0xA0000000;

    return wh;
} // afe_set_wpulse

/* Set the VARh pulse input. */
int32_t afe_set_varpulse(int32_t varh)
{
    if(ce_data.ceconfig & EXT_PULSE)
    {	
        ce_data.apulser = varh;
    } else {
        // clear ce_data.apulser
        ce_data.apulser = 0; 
    }

    // If forcing a creep value, prevent frequency generation
    // even if the CE is operating from an internal pulse.
    if(0 == varh && 0 != afe_cal_ptr->i_min )
        ce_data.vsum_accum = 0xA0000000;

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
    gpio_alt_off(wpulse_seg);

    if(VPULSE_OK)
    {
        gpio_alt_off(vpulse_seg);
    }
    else
    {
        gpio_alt_on(vpulse_seg);
    }

    #ifdef xpulse_seg
    gpio_alt_off(xpulse_seg);
    #endif

    #ifdef ypulse_seg
    gpio_alt_off(ypulse_seg);
    #endif
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
    // Enable the pulses.
    gpio_alt_on(wpulse_seg);

    if(VPULSE_OK)
    {
        gpio_alt_on(vpulse_seg);
    }

    #ifdef xpulse_seg
    gpio_alt_on(xpulse_seg);
    #endif

    #ifdef ypulse_seg
    gpio_alt_on(ypulse_seg);
    #endif
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
    int on = LCD->DIO_b[wpulse_seg].alt; // Save state of pulse enable.

    afe_pulse_disable();

    // Turn the active-low LEDs on.
    // A normal reason for pulses to fail is that they are LCD outputs.
    gpio_set_state(wpulse_seg, 0);      // Set WPULSE to low as a GPIO
    gpio_dir_out(wpulse_seg);           // Set WPULSE to low as a GPIO
    if(VPULSE_OK)
    {
        gpio_set_state(vpulse_seg, 0);  // Set VPULSE to low as a GPIO
        gpio_dir_out(vpulse_seg);       // Set VPULSE to low as a GPIO
    }
    #ifdef xpulse_seg
    gpio_set_state(xpulse_seg, 0);      // Set XPULSE to low as a GPIO
    gpio_dir_out(xpulse_seg);           // Set XPULSE to low as a GPIO
    #endif
    #ifdef ypulse_seg
    gpio_set_state(ypulse_seg, 0);      // Set YPULSE to low as a GPIO
    gpio_dir_out(ypulse_seg);           // Set YPULSE to low as a GPIO
    #endif

    delay(DELAY_MS(10));

    // Turn the active-low LEDs off.
    gpio_set_state(wpulse_seg, 1);      // Set WPULSE to high as a GPIO
    if(VPULSE_OK)
    {
        gpio_set_state(vpulse_seg, 1);  // Set VPULSE to high as a GPIO
    }
    #ifdef xpulse_seg
    gpio_set_state(xpulse_seg, 1);      // Set XPULSE to high as a GPIO
    #endif
    #ifdef ypulse_seg
    gpio_set_state(ypulse_seg, 1);      // Set YPULSE to high as a GPIO
    #endif

    delay(DELAY_MS(10));

    // Turn the LEDs off.
    gpio_dir_in(wpulse_seg);           // Set WPULSE to low as a GPIO
    if(VPULSE_OK)
    {
        gpio_dir_in(vpulse_seg);       // Set VPULSE to low as a GPIO
    }
    #ifdef xpulse_seg
    gpio_dir_in(xpulse_seg);           // Set XPULSE to low as a GPIO
    #endif
    #ifdef ypulse_seg
    gpio_dir_in(ypulse_seg);           // Set YPULSE to low as a GPIO
    #endif

    if(on)
        afe_pulse_enable();
    else
        afe_pulse_disable();

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
    ce_data.ceconfig &= ~EXT_PULSE;
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
    ce_data.ceconfig |= EXT_PULSE;
} // afe_pulse_external()


/****************************************************************************
* Description:
* Select voltage for frequency.
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
    int old = ce_data.ceconfig;
    int tmp;

    tmp = old & (~FREQ_SEL_MASK);

    switch(phase_idx)
    {
        case PHASE_A:
            // zeroed above.
            break;
        default: // do nothing
            tmp = old;
            break;
    }

    if(tmp != old)
        ce_data.ceconfig = tmp;
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
    switch(phase_idx)
    {
        case NATIVE_VREF:
            ce_data.gain_adj[ALL_PHASES] = temp_gain;
            break;
        case PHASE_A:
            ce_data.gain_adj[PHASE_A] = temp_gain;
            break;
        case PHASE_B:
            // Falls through here.
        case PHASE_NEUTRAL:
            ce_data.gain_adj[PHASE_B] = temp_gain;
            break;
        default:    // Do nothing.
            break;
    }
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
            // Falls through here.
						//return 0;
        case PHASE_A:
            return Local_RAM_ce_data.w0sum_x;
				    /*
					   if(abs(Local_RAM_ce_data.w0sum_x)>abs(Local_RAM_ce_data.w1sum_x))
							 return Local_RAM_ce_data.w0sum_x;
						 else
							 return Local_RAM_ce_data.w1sum_x;
						*/
				
        case PHASE_NEUTRAL:
            if(ce_ptr->i1_scaled_by_ce)
                return Local_RAM_ce_data.w1sum_x;
            else
                return (afe_wnsum2wsum*Local_RAM_ce_data.w1sum_x)>>32;
        case PHASE_B:
						return 0;		
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
    return lroundf(vsum_nom_f * sqrtf(afe_isqsum(phase_idx)));
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
            // Falls through here.
        case PHASE_A:
            return Local_RAM_ce_data.var0sum_x;
				    /*
						if(abs(Local_RAM_ce_data.w0sum_x)>abs(Local_RAM_ce_data.w1sum_x))
							 return Local_RAM_ce_data.var0sum_x;
						 else
							 return Local_RAM_ce_data.var1sum_x;
						*/ 
        case PHASE_NEUTRAL:
            // Falls through here.
						if(ce_ptr->var1_exists)
            {
                if(ce_ptr->i1_scaled_by_ce)
                {
                    return Local_RAM_ce_data.var1sum_x;
                }
                else
                {
                    return (afe_wnsum2wsum*Local_RAM_ce_data.var1sum_x)>>32;
                }
            }
            else
                return 0;
        case PHASE_B:
            return 0;
            
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
    float vah;

    switch(phase_idx)
    {
        case ALL_PHASES:
            // Falls through here.
        case PHASE_A:
            if(ce_ptr->var0_exists)
            {
                float wh, varh;
                wh = (float)afe_wsum(phase_idx);
                varh = (float)afe_varsum(phase_idx);
                vah = sqrtf(wh*wh + varh*varh);
            }
            else
            {
                float i, v;
                i = (float)afe_isqsum(phase_idx);
                v = (float)afe_vsqsum(phase_idx);
                vah = sqrtf(i * v);
            }
            return lroundf(vah);

        case PHASE_B:
            // Falls through here.
        case PHASE_NEUTRAL:
            if(ce_ptr->var1_exists)
            {
                float wh, varh;
                wh = (float)afe_wsum(phase_idx);
                varh = (float)afe_varsum(phase_idx);
                vah = sqrtf(wh*wh + varh*varh);
            }
            else
            {
                float i, v;
                i = (float)afe_isqsum(phase_idx);
                v = (float)afe_vsqsum(phase_idx);
                vah = sqrtf(i * v);
            }
            return lroundf(vah);
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
    int32_t i;

    switch(phase_idx)
    {
        case ALL_PHASES:
            // Falls through here.
        case PHASE_A:
            // Falls through here.
        case PHASE_B:
            // Falls through here.
        case PHASE_NEUTRAL:
            // Falls through here.
            i = Local_RAM_ce_data.v0sqsum_x;
            break;
        default:
            i = 0;
            break;
    }
    return i;
} // afe_vsqsum

/****************************************************************************
* Description:
*       Convert a high and low value of isqsum to a float.
*       in the AFE's native units.
*       This only works for sqsum units, because they are always positive.
*
* Parameters:
*   Input:
*         the high integer, signed
*         the low integer, unsigned.
*   Output:
*         A float in the correct units.
****************************************************************************/
float afe_hl2f(int32_t high, uint32_t low)
{
    float fh, fl;

    fh = (float)high;

    fl = (float)low;
    fl *=(ce_ptr->h_per_l); // Scale the low to the same units.

    fh += fl;
    return fh;
}


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
        case ALL_PHASES:
            // Falls through here.
        case PHASE_A:
            f = afe_hl2f(Local_RAM_ce_data.i0sqsum_x_h, Local_RAM_ce_data.i0sqsum_x_l);
            break;

        case PHASE_NEUTRAL:
            f = afe_hl2f(Local_RAM_ce_data.i1sqsum_x_h, Local_RAM_ce_data.i1sqsum_x_l);
            if(0 == (ce_ptr->i1_scaled_by_ce))
            {
                f *= afe_insq2ixsq_f;
            }
            break;
				case PHASE_B:
						f = 0.0;
            break;	
        default:
            f = 0.0;
            break;
    }
    return f;
} // afe_isqsum

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
    return ce_data.wpulse_ctr;
}
int32_t afe_varh_pulse_cnt(void)
{
    return ce_data.vpulse_ctr;
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
    ce_data.wpulse_ctr = cnt;
}
void afe_set_varh_pulse_cnt(int32_t cnt)
{
    ce_data.vpulse_ctr = cnt;
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
    if(0 == afe_cal_ptr->i_min)   // if creep is disabled
        return vasum;
    if(afe_starting_cnt)            // If data is invalid.
        return 0;
   // if(afe_vsqsum(phase_idx) < afe_vsqsum_min)	// if below threshold.
    //    return 0;
    if(afe_isqsum(phase_idx) < afe_isqsum_min_f)	// if below threshold.
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
    if(0 == afe_cal_ptr->i_min)   // if creep is disabled
        return isqsum_f;
    if(afe_starting_cnt)            // If data is invalid.
        return 0.0;
    switch(phase_idx)
    {
        case ALL_PHASES:
        case PHASE_A:
        case PHASE_B:
        case PHASE_NEUTRAL:
            if(isqsum_f <= afe_isqsum_min_f)	// if below threshold.
                return 0;
            return isqsum_f;

        default:
            return 0;
    }
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
//    if(vsqsum <= afe_vsqsum_min)    // if below threshold.
//        return 0;
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

    return ce_data.mainedge_x;
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
    return afe_freqx2hz(ce_data.freq_x);
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
    return 0.0; /* stubbed for single phase */
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
    return 0.0; /* stubbed */
} // afe_a_to_c

//***********************************************Harmonics Analysis***********

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
    switch(phase_idx)
    {
        case ALL_PHASES:
            // Falls through here.
        case PHASE_A:
            return ce_data.w0sum_h_x;

        case PHASE_NEUTRAL:
            // Falls through here.
        case PHASE_B:
            if(ce_ptr->i1_scaled_by_ce)
            {
                return ce_data.w1sum_h_x;
            }
            else
            {
                return (afe_wnsum2wsum*ce_data.w1sum_h_x)>>32;
            }

        default:
            return 0;
    }
} // afe_wsum_nb


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
    int i;

    switch(phase_idx)
    {
        case ALL_PHASES:
            // Falls through here.
        case PHASE_A:
            // Falls through here.
        case PHASE_NEUTRAL:
            // Falls through here.
        case PHASE_B:
            // Falls through here.
            i = ce_data.v0sqsum_h_x;
            break;
        default:
            i = 0;
            break;
    }
    return i;
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
    float f;

    switch(phase_idx)
    {
        case ALL_PHASES: // Sum of currents.
        case PHASE_A:
            f = afe_hl2f(ce_data.i0sqsum_h_x_h, ce_data.i0sqsum_h_x_l);
            break;

        case PHASE_NEUTRAL:
        case PHASE_B:
            f = afe_hl2f(ce_data.i1sqsum_h_x_h, ce_data.i1sqsum_h_x_l);
            if(0 == (ce_ptr->i1_scaled_by_ce))
            {
                f *= afe_insq2ixsq_f;
            }
            break;

        default:
            f = 0.0;
            break;
    }
    return f;
} // afe_isqsum_nb


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

// Generic THD calculation.
float afe_thd(float wideband, float narrowband)
{
    float thd;

    // Safety code to assure that negative power can't cause issues.
    wideband = fabs(wideband);
    narrowband = fabs(narrowband);

    if(ce_data.nfreq == -1) // Filter set to fundamental?
    {   // Calculate percentage of fundamental.
        float not_fundamental = wideband - narrowband;

        if(0.0 == not_fundamental) // Unit in creep?
        {
            return 0.0; // Probably in creep.
        }

        if(narrowband <= not_fundamental) // Prevent /0
        {
            return 0;//100.0; // Greater than 100%
        }

        thd = 100.0 * sqrt(not_fundamental / narrowband);
    }
    else
    {   // Calculate percentage of harmonic.
        int not_harmonic = wideband - narrowband;

        if(0.0 == narrowband) // Unit in creep?
        {
            return 0.0; // Probably in creep.
        }

        if(not_harmonic <= narrowband) // Prevent /0
        {
            return 0;//100.0; // greater than 100%
        }

        thd = 100.0 * sqrt(narrowband / not_harmonic);
    }

    return thd;
}

// Total Harmonic Distortion for Watts
float afe_w_thd(int phase_idx) /* T*/
{
    int wideband, narrowband;

    // Get data, apply creep, then force it to be positive.
    wideband = afe_vasum_creep(phase_idx, afe_wsum(phase_idx));
    narrowband = afe_vasum_creep(phase_idx, afe_wsum_nb(phase_idx));
    return afe_thd((float)wideband, (float)narrowband);
}

// Total Harmonic Distortion for Current.
float afe_arms_thd(int phase_idx)
{
    float wideband, narrowband;

    // Uses squares because power ~= (i^2)
    wideband = afe_isqsum_creep(phase_idx, afe_isqsum(phase_idx));
    narrowband = afe_isqsum_creep(phase_idx, afe_isqsum_nb(phase_idx));
    return afe_thd(wideband, narrowband);
}

// Total Harmonic Distortion for Voltage.
float afe_vrms_thd(int phase_idx)
{
    int wideband, narrowband;

    // Uses squares because power ~= (v^2)
    wideband = afe_vsqsum_creep(afe_vsqsum(phase_idx));
    narrowband = afe_vsqsum_creep(afe_vsqsum_nb(phase_idx));
    return afe_thd((float)wideband, (float)narrowband);
}

//****************************************************************************



/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

