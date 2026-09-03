/*****************************************************************************
 * DESCRIPTION: Meter run logic, the main loop.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2012...2020, Silergy Corp. All rights reserved.
 *
 * $Id: meter_1p2w.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
#include <string.h>  // C strings and bulk data handling.
#include "options.h" // Define the IC, CE code and other options.
#include "afe.h"     // To read the AFE.
#include "delay.h"   // precision delay loop, for LPM pulsing.
#include "flash.h"   // flash_enable, in case reg_put uses flash.
#include "gpio.h"    // program pins.
#include "log.h"     // Log events.
#include "math.h"    // Meter math
#include "mmath.h"   // Meter math
#include "nvram.h"   // Read and write nonvolatile RAM.
#include "stm.h"     // Software timers.
#include "tmp.h"     // Temperature measurement.
#include "meter.h"   // Check function prototypes.
#include "Defines.h"
#include "asdaq_variables.h"
// #include "dlms_variables.h"
/*** Version ****/
const char fw_ver_meter[] =
    "meter" /* Library file name */
    " v"    /* ver string */
    "1.01"  /* Major_ver.Minor_ver */
    "\r\n"  /* CR + LF */
    ;

Local_ce_data Local_RAM_ce_data;

/* ---------------------------------------------------------------------------
 * KVA demand accumulator + cross-file demand snapshots (added).
 *
 * These mirror the existing KW demand path (global.misc.dmd_cnt/dmd_frac,
 * global.reg.dmd_max) so that KVA MD gets the same full-resolution,
 * floating-point-averaged treatment instead of the coarse load_val[]-pulse
 * calc previously done in asdaq_app.c:store_md_data().
 *
 * ASSUMPTION / TO VERIFY (meter.h was not available when this was written):
 *   - dmd_cnt/dmd_frac's real types, declared in meter.h inside global.misc,
 *     are not visible here. dmd_va_cnt/dmd_va_frac below are declared to
 *     match how they're used with add()/p2f() (same pattern as dmd_cnt/
 *     dmd_frac and vah_cnt/vah_frac) - double check against meter.h and
 *     adjust the types if they don't match, or (better) move these two
 *     fields into global.misc and dmd_va_max into global.reg alongside
 *     their KW counterparts once you can edit meter.h.
 *   - afe_wsum2w() is reused here for the VA sum -> physical-unit
 *     conversion. Confirm this scale factor is valid for VA as well as W;
 *     use an afe_vasum2va()-equivalent instead if the AFE driver has one.
 */
int32_t dmd_va_cnt = 0;
int32_t dmd_va_frac = 0;
int32_t g_dmd_va_max = 0;

/* Full-resolution snapshot of THIS period's average demand, refreshed every
 * meter_demand() call (every global.cal.interval minutes). asdaq_app.c's
 * store_md_data() reads these instead of re-deriving demand from load_val[]. */
int32_t g_dmd_period_kw = 0;
int32_t g_dmd_period_kva = 0;
/* --------------------------------------------------------------------- */

// This can handle equation 0 (Wh = VA * IA)
// and with a standard CE code, equation 1 (Wh = 0.5*V(IA-IB))
#if EQUATION != EQUATION0 && EQUATION != EQUATION1
#warning Symbol 'EQUATION' is not set correctly.
#endif

// Internal routines.
// The following routines have to be synchronized with the data cycle.
static void meter_demand_reset(void);    // Reset demand.
static void meter_registers_reset(void); // Reset billing data.
#if 0
static int meter_in_creep(void);         // Determines if the meter is in creep.
#endif
void meter_sum_data(void); // Adds up the billing data.

// Global state of the metering: creep, etc.
uint32_t meter_state; // The bits are in meter.h

int32_t meter_vsqsum_limit; // Overvoltage limit.
float meter_isqsum_limit_f; // Overcurrent limit.

// Flag indicating that new data is available. Used when saving data.
static bool new_data = false;

/***************************************************************************
 * Description:
 *       Start the meter
 *   Input:
 *       Calibration, etc.
 * Parameters:
 *       CE structure, telling which CE to start.
 *   Output:
 *       Start metering
 * Return Status:
 *         None
 ***************************************************************************/
void meter_start(const ce_t *ce_ptr)
{
    // Initialize the AFE: Move the calibration to the correct CE code.
    afe_init(ce_ptr);
    global.cal.wsum_per_cnt = afe_enable(ce_ptr, &global.cal);
    meter_vsqsum_limit = afe_v2vsqsum(0.1 * (float)global.cal.v_limit);
    meter_isqsum_limit_f = afe_a2isqsum(0.1 * (float)global.cal.i_limit);

    // Start demand logic.
    global.misc.dmd_cnt = 0; // Count of pulses starts at zero.
    global.misc.dmd_frac = 0;
    global.misc.dmd_acc_cnt = 0;
    global.misc.dmd_tm = global.reg.tm; // Store the starting time.

    // KVA demand accumulator - mirrors the KW init above (added).
    dmd_va_cnt = 0;
    dmd_va_frac = 0;
}

/***************************************************************************
 * Description:
 *       Stop the meter
 *   Input:
 *       Nothing.
 * Parameters:
 *   Output:
 *       AFE stops
 * Return Status:
 *         None
 ***************************************************************************/
void meter_stop(void)
{
    afe_disable();
}

/***************************************************************************
 * Description:
 *       Run the metering.
 *   Input:
 *       Data from the AFE, temperature
 * Parameters:
 *       None- it's set-up by meter_start
 *   Output:
 *       It updates the billing data if needed.
 *       It adjusts the AFE as needed.
 * Return Status:
 *       Returns nonzero if the AFE ran again.
 ***************************************************************************/
int meter_run(void)
{
    bool new_meter_data_flag;

    // Enable NVRAM writes, in case the data is updated.
    // The disable at the end of meter-run() is safety code to
    // prevent NVRAM corruption from EMI.
    nvram_enable();

    // clear information before next cycle of the meter.
    // Reset demand. Invoke: )1=4 (Note: This can change NVRAM.)
    meter_demand_reset();

    // clear reg_data?  Invoke: )1=2
    meter_registers_reset(); // (Note: This can change NVRAM.)

    // Is new AFE data available?
    new_meter_data_flag = afe_status(AFE_HAS_DATA);
    if (new_meter_data_flag)
    {
        meter_compensate_afe(); // Adjust the meter.

        if (false == sag_int_enabled)               // Is Sag int. enabled?
            ce_int_enable(YPLS_I, meter_save_data); // Enable the sag interrupt.

        // Demand must count time whether or not the meter is in creep.
        ++global.misc.dmd_acc_cnt; // count accumulation intervals of demand.

        meter_sum_data(); // Add up the billing data.

        // Data may have changed, so enable saving the data.
        new_data = true;

        // Flag the watchdog test that the meter cycle ran.
        global.misc.wd_set |= CE_WD;
    }

    // Disable NVRAM writes. The data does not need updating.
    // This is safety code to prevent NVRAM corruption from EMI.
    nvram_disable();

    return new_meter_data_flag;
} // meter_run

/****************************************************************************
 * Description:
 *       Runs timed demand logic.
 *       E.g. 2x/hour or 4x/hour (or 30x/hr- good for a demo)
 *       Called from a timing-task routine in main.c
 * Parameters:
 *   Input:
 *         Demand summation calculated by meter-run()
 *   Output:
 *         If demand is greater than previous demand,
 *         it notes the amount and time.
 * Return Status:
 *         None
 *****************************************************************************/
void meter_demand(void)
{
    float dmd, ftmp;
    int32_t dmd_tmp;
    float dmd_va;       // KVA demand (added)
    int32_t dmd_va_tmp; // KVA demand (added)

    // Figure accumulated demand.
    // Whether demand is from Wh
    // (utilities use this to manage fuel costs,
    // or because of regulations)
    // or from VAh
    // (utilities use this to manage capital costs,
    // which are determined by wire heating)
    // is determined by how dmd_cnt and dmd_frac
    // are added in the main loop.
    // The demo code uses Wh, which is a bit more
    // typical.

    // Figure the energy in wsum of CE
    dmd = p2f(&global.misc.dmd_cnt, &global.misc.dmd_frac);

    // Find the average energy per accumulation interval
    // for the demand period. (averaging is right, because
    // utilities typically use demand to size equipment for heating.)
    ftmp = (float)global.misc.dmd_acc_cnt;
    dmd /= ftmp;

    // Convert the average energy in wsum/CE units to Watts
    dmd_tmp = afe_wsum2w(lroundf(dmd));

    // Is the demand crazy? (e.g., from EMI)
    // MAX_DEMAND is in options.h
    if (dmd_tmp >= MAX_DEMAND)
    {
        dmd_tmp = MAX_DEMAND;
    }

    // Snapshot of THIS period's average KW demand, full resolution (added).
    // asdaq_app.c:store_md_data() reads this instead of recomputing demand
    // from load_val[] pulse deltas.
    g_dmd_period_kw = dmd_tmp;

    // Find maximum demand interval.
    if (global.reg.dmd_max <= dmd_tmp)
    {
        // Store the max value.
        global.reg.dmd_max = dmd_tmp;
        // Store the start of max interval.
        global.reg.dmd_mon = global.misc.dmd_tm.tm_mon;
        global.reg.dmd_mday = global.misc.dmd_tm.tm_mday;
        global.reg.dmd_hour = global.misc.dmd_tm.tm_hour;
        global.reg.dmd_min = global.misc.dmd_tm.tm_min;
    }

    // Restart the demand calculation for the next interval.
    global.misc.dmd_cnt = 0;
    global.misc.dmd_frac = 0;

    /* --- KVA demand (added) ---
     * Same technique as KW above: full-precision average over the period,
     * using the same accumulation-interval count (ftmp) since both W and
     * VA sums were accumulated over the identical set of AFE cycles. */
    dmd_va = p2f(&dmd_va_cnt, &dmd_va_frac);
    dmd_va /= ftmp;
    dmd_va_tmp = afe_wsum2w(lroundf(dmd_va));

    if (dmd_va_tmp >= MAX_DEMAND)
    {
        dmd_va_tmp = MAX_DEMAND;
    }

    g_dmd_period_kva = dmd_va_tmp;

    if (g_dmd_va_max <= dmd_va_tmp)
    {
        g_dmd_va_max = dmd_va_tmp;
    }

    dmd_va_cnt = 0;
    dmd_va_frac = 0;

    global.misc.dmd_acc_cnt = 0;

    // Store the starting time.
    global.misc.dmd_tm = global.reg.tm;
}

/***************************************************************************
 * Description:
 *        Resets the demand.
 *   Input:
 *        Config register has demand-reset bit set.
 * Parameters:
 *       None.
 *   Output:
 *         billing data in C structure reg_data
 * Return Status:
 *         None.
 ***************************************************************************/
static void meter_demand_reset(void)
{
    // clear information before next cycle of the meter.
    // Reset demand. Invoke: )1=4
    if (global.cal.cfg & RESET_DEMAND)
    {
        global.cal.cfg &= ~RESET_DEMAND;

        // Clears only the working copy.
        // This will propagate as needed.
        // It's not really safe to clear it in all register sets.
        global.reg.dmd_mon = 0;
        global.reg.dmd_mday = 1;
        global.reg.dmd_hour = 0;
        global.reg.dmd_min = 0;
        global.reg.dmd_max = 0;
        g_dmd_va_max = 0; // KVA demand reset (added) - mirrors dmd_max above
    }
}

/***************************************************************************
 * Description:
 *        Resets the Billing registers.
 *   Input:
 *        Config register has billing register-reset bit set.
 * Parameters:
 *         None.
 *   Output:
 *         billing data in C structure reg_data
 * Return Status:
 *         None.
 ***************************************************************************/
static void meter_registers_reset(void)
{
    // clear reg_data?  Invoke: )1=2
    if (global.cal.cfg & CLEAR_REG)
    {
        global.cal.cfg &= ~CLEAR_REG;

        // Clear register data.
        memset((uint8_t *)&global.misc, 0, sizeof(Misc_t));
        reg_reset();

// Clear pulse counters in CE data.
// (This is needed if they are used to count meter
// billing.)
#ifdef WSUM_ACCUM
        afe_set_wh_pulse_cnt(0);
#endif
#ifdef VSUM_ACCUM
        afe_set_varh_pulse_cnt(0);
#endif

        // Set the start of the demand interval.
        global.misc.dmd_tm = global.reg.tm;
    }
}

/***************************************************************************
 * Description:
 *        Metering code: Compensate the AFE's voltage reference for
 *        temperature and anything else.
 *        This works more consistently if synchronized with the CE's cycle.
 *        This would be the right place, for example, to adjust the phase
 *        if the meter has a current-dependent phase adjustment.
 *        Reading CE registers is very fast, so don't hesitate to get input.
 *   Input:
 *        It reads the temperature.
 *        The bit in cfg tells whether to simulate the temperature.
 *        Inside the afe_compensate_vref(), it uses coefficients from
 *           global. cal, ppmc [] and ppmc2 [].
 * Parameters:
 *        None.
 *   Output:
 *         It sets the ce variables gain_adj[].
 * Return Status:
 *         None.
 ***************************************************************************/
void meter_compensate_afe(void)
{
    int16_t stemp; // Hardware temperature
    float temp_f;

    // Read the temperature, or set it for a software test.
    if (global.cal.cfg & MANUAL_TEMP)
    { // cli enable: )1=20
        // Simulate temperature sensor: )33=270 = 27.0C
        float temp_f = ((float)global.misc.temp_c) / 10.0f;
        stemp = (int16_t)lroundf(COMPUTE_STEMP(temp_f));
    }
    else
    {                                                // Default: Use SOC's sensor, more accurate than remotes.
        temp_f = tmp();                              // Read temperature, also sets hardware temp. reg.
        global.misc.temp_c = lroundf(temp_f * 10.0); // for other users
        stemp = (int16_t)TEMP->BAT_TEMP_b.stemp;     // get the hardware temp. reg.
    }

    // All temperature is from the SoC.
    // The MPU compensates Vref.
    // To disable these set ce's PPMC and PPMC2 to zero.
    afe_compensate_vref((int32_t)stemp); // Compensate SoC's Vref adjustment
}

/***************************************************************************
 * Description:
 *        Metering code: Test for creep and errors.
 *   Input:
 *         It reads electrical data from the AFE.
 *         v is the voltage as read from the AFE.
 *         i_f is the current as read from the AFE, but in a float.
 *           (The AFE has 64-bits of current range.)
 * Parameters:
 *         None
 *   Output:
 *         It sets the global variable meter_state.
 * Return Status:
 *         Nonzero indicate not in creep.
 ***************************************************************************/
#if 0
 static int meter_in_creep(void)
{
    int vmask, imask, wmask, inovmask;

    meter_state = 0; // No items in creep.

    if (0 == global.cal.i_min)
    { // is creep disabled?
        // yes; (handy for an engineering meter.)
        log_state(EV_OVERVOLTAGE, false);
        log_state(EV_OVERCURRENT, false);
        log_state(EV_SAG, false);
        log_state(EV_I_NO_V, false);
        return 0;
    }

    // These limits are set by afe_start(), from
    // constants in the calibration structure (Demo\Include\cal.h),
    // which ultimately come from constants in the
    // .h file that describes the pcb (e.g. Demo\Include\mapeb.h)

    // Check for over-voltage and over-current.
    // Like the creep limits, the over limits are from the calibration data
    // The defaults are from the PCB file (map*.h).
    log_state(EV_OVERVOLTAGE, afe_vsqsum(ALL_PHASES) > meter_vsqsum_limit);
    log_state(EV_OVERCURRENT, afe_isqsum(ALL_PHASES) > meter_isqsum_limit_f);

    // Set a bit for each voltage in creep.
    if (afe_vsqsum(PHASE_A) < afe_vsqsum_min)
        meter_state |= MINVA_MASK;

    // Set a bit for every phase with voltage.
    // (shifts so the bits align with current)
    vmask = ((MINVA_MASK) & ~meter_state) >> 3;
    if (0 == vmask)
        meter_state |= CREEPV_MASK; // All voltages in creep.
    if (vmask != (MINVA_MASK >> 3))
        meter_state |= SAG_MASK;
    log_state(EV_SAG, vmask); // Any phases in sag.

    // Set a bit for each current in creep.
    if ((afe_isqsum(PHASE_A) < afe_isqsum_min_f) && (afe_isqsum(PHASE_NEUTRAL) < afe_isqsum_min_f))
        meter_state |= MINIA_MASK;

    // Set a bit for every phase with current.
    imask = (MINIA_MASK) & ~meter_state;
    if (0 == imask)
        meter_state |= CREEPI_MASK; // All currents in creep.

    // Detect if any phases have current but no voltage.
    inovmask = imask & ~vmask;
    if (inovmask)
        meter_state |= I_NO_V_MASK; // Possibly a tamper.
    log_state(EV_I_NO_V, inovmask);

    // Detect if all phases are in creep.
    // Use current and volts because it resists noise well.
    wmask = imask & vmask;
    if (0 == wmask)
        meter_state |= CREEP_MASK;

    return meter_state & CREEP_MASK; // Meter in creep?
} // meter in creep ()
#endif
/***************************************************************************
 * Description:
 *        Metering code: Adds up the billing data.
 *   Input:
 *         It reads energy and phase data from the AFE.
 * Parameters:
 *         None
 *   Output:
 *         It updates the billing data.
 * Return Status:
 *         None.
 ***************************************************************************/
extern void PulseGeneration(void);
int Sqrt_v0sqsum_240V = 9160; // 11450;
uint8_t THD_10V_40V_inject_flag = 0;
extern float afe_w_thd(int phase_idx);
extern float afe_arms_thd(int phase_idx);
extern float afe_vrms_thd(int phase_idx);
extern uint16_t THD_Power, THD_Current_P, THD_Voltage;

int32_t Power_Magnet_Tamper = 0x07CFD121;
#define NM_Power_Threshold 0x0063FDA8 // 240V*1.5A=360W
void meter_sum_data(void)
{
    int wsum, varsum, vasum;

    if (magnetic_stat == 1) // since add() function will make energy based on wsum
    {
        ce_data.w0sum_x = ce_data.w1sum_x = ce_data.wsum_x = Power_Magnet_Tamper;
        ce_data.var0sum_x = ce_data.var1sum_x = ce_data.varsum_x = 0;
    }

    //***********************Harmonics Analysis******************
    if (inst_kw_p > 12) // 12W
        THD_Power = afe_w_thd(PHASE_A);

    if (inst_phase_current > 100) // 100mA
        THD_Current_P = afe_arms_thd(PHASE_A);

    if (inst_voltage > 100) // 100V
        THD_Voltage = afe_vrms_thd(PHASE_A);

    if ((THD_Voltage > 8 && THD_Voltage < 12) && (THD_Current_P > 38 && THD_Current_P < 42))
    {
        ce_data.w0sum_x = ce_data.w1sum_x = 67709000; // Power for 3.72KW

        THD_10V_40V_inject_flag = 1;
    }
    else
        THD_10V_40V_inject_flag = 0;

    //*************************************************************

    //****************Bring ce data variables to local memory so that this can get modified*********
    Local_RAM_ce_data.wsum_x = ce_data.wsum_x;           //  0x84 ;       //  0x84
    Local_RAM_ce_data.w0sum_x = ce_data.w0sum_x;         //  0x85
    Local_RAM_ce_data.w1sum_x = ce_data.w1sum_x;         //  0x86
    Local_RAM_ce_data.varsum_x = ce_data.varsum_x;       //  0x87
    Local_RAM_ce_data.var0sum_x = ce_data.var0sum_x;     //  0x88
    Local_RAM_ce_data.var1sum_x = ce_data.var1sum_x;     //  0x89
    Local_RAM_ce_data.i0sqsum_x_h = ce_data.i0sqsum_x_h; //  0x8A
    Local_RAM_ce_data.i0sqsum_x_l = ce_data.i0sqsum_x_l; //  0x8B
    Local_RAM_ce_data.i1sqsum_x_h = ce_data.i1sqsum_x_h; //  0x8C
    Local_RAM_ce_data.i1sqsum_x_l = ce_data.i1sqsum_x_l; //  0x8D
    Local_RAM_ce_data.v0sqsum_x = ce_data.v0sqsum_x;     //  0x8E
    Local_RAM_ce_data.v1sqsum_x = ce_data.v1sqsum_x;     //  0x8F
                                                         //**********************************************************************

    if (NM_CT_Detected == 1) // RAJIV
    {
        //**************************NM Power Calculation****************************************************

        Local_RAM_ce_data.w0sum_x = Sqrt_v0sqsum_240V * sqrt(Local_RAM_ce_data.i0sqsum_x_h);

        if (Local_RAM_ce_data.w0sum_x < NM_Power_Threshold) // below 240V*1.5A=360W not required
        {
            Local_RAM_ce_data.i0sqsum_x_h = 0;
            Local_RAM_ce_data.i0sqsum_x_l = 0;
            Local_RAM_ce_data.w0sum_x = 0;

        } //*/

        Local_RAM_ce_data.w1sum_x = Sqrt_v0sqsum_240V * sqrt(Local_RAM_ce_data.i1sqsum_x_h);

        if (Local_RAM_ce_data.w1sum_x < NM_Power_Threshold) // below 240V*1.5A=360W not required
        {
            Local_RAM_ce_data.i1sqsum_x_h = 0;
            Local_RAM_ce_data.i1sqsum_x_l = 0;
            Local_RAM_ce_data.w1sum_x = 0;
        }
    }

    // Add up Watt-hours, and demand.
    if (channel == 0)
    {
        wsum = afe_wsum(PHASE_A); // Get Wh in CE units.
        varsum = afe_varsum(PHASE_A);
        vasum = afe_vasum(PHASE_A);
    }
    else
    {
        wsum = afe_wsum(PHASE_NEUTRAL); // Get Wh in CE units.
        varsum = afe_varsum(PHASE_NEUTRAL);
        vasum = afe_vasum(PHASE_NEUTRAL);
    }

    if (wsum > 0)
    {
        add(&global.reg.wh_cnt, &global.misc.wh_frac, wsum);
    }
    else
    { // Sum the MPU's export counters.
        add(&global.reg.wh_cnt_ex, &global.misc.wh_frac_ex, -wsum);
    }

    // Sum Wh for demand.
    add(&global.misc.dmd_cnt, &global.misc.dmd_frac, labs(wsum));

    // Add up VAR-hours
    if (varsum > 0)
    { // inductive
        add(&global.reg.varh_cnt, &global.misc.varh_frac, varsum);
    }
    else
    { // capacitive
        add(&global.reg.varh_cnt_ex, &global.misc.varh_frac_ex, -varsum);
    }

    // Sum the VAh.
    add(&global.misc.vah_cnt, &global.misc.vah_frac, vasum);

    // Sum VA for KVA demand (added) - mirrors "Sum Wh for demand" above,
    // but keeps its own accumulator (dmd_va_cnt/dmd_va_frac) since KVA
    // demand must be tracked independently of KW demand.
    add(&dmd_va_cnt, &dmd_va_frac, vasum);

    // Count total zero crossings. (to rate the RTC in US & EU)
    global.misc.zc_cnt += afe_zero_crossings();

    // Update the CRC on the billing data registers.
    reg_crc();

    // Write the changed data to NVRAM.
    nvram_write(PCB_NVRAM_ADR_NVM, (uint8_t *)&nvm, sizeof(Nvm_t));

    inst_param_func();
    PulseGeneration();
} // meter sum data ()

/***************************************************************************
 * Description:
 *       Runs when AC mains sag, invoked by the pulse interrupt from the CE
 *   Input:
 *       The meter data
 * Parameters:
 *       None.
 *   Output:
 *       Saves data to the storage area.
 * Return Status:
 *         None
 ***************************************************************************/
void meter_save_data(void)
{
    flash_enable(); // Just in case the flash verstion of reg.c

    if (new_data)
    {
        new_data = false;

        // Save the meter registers.
        reg_put();
    }

    flash_disable(); // Just in case the flash verstion of reg.c
} // meter_save_data()

/***************************************************************************
 * Description:
 *        Runs when meter is powered with PB pressed. Resets the Billing registers.
 *   Input:
 *        Config register has billing register-reset bit set.
 * Parameters:
 *         None.
 *   Output:
 *         billing data in C structure reg_data
 * Return Status:
 *         None.
 ***************************************************************************/
void meter_dlms_reset(void)
{
    global.cal.cfg = CLEAR_REG;
    meter_registers_reset();
}
/***************************************************************************
 * Copyright (C) 2011..2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
