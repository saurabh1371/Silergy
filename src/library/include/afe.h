/**************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Description : Metering for an AFE accessed via Shared RAM.
 * To make it more readable, this code intentionally combines
 * the metering and start-up logic is in one file.
 * This design makes code readable, but reuse and testing is more difficult.
 *
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * $Id: afe.h 23350 2020-02-22 00:23:05Z rvandewa $
 ***************************************************************************/
#ifndef AFE_H
#define AFE_H 1

// Firmware Device Driver version
extern const char fw_ver_afe[]; 

// When nonzero, the data from the CE is invalid.
extern uint8_t afe_starting_cnt;

extern float afe_isqsum_min_f; // Starting current
extern int32_t afe_vsqsum_min; // Starting voltage
extern int32_t afe_vasum_min;  // Starting volt-amps

// Standard AFE API.

#ifdef CAL_H // If calibration data is available.

// Calibration data (i.e. afe-specific data)
extern Cal_t *afe_cal_ptr;

// Put a calibration into the AFE.
extern int32_t afe_put_cal(Cal_t *c_ptr);

// Recalculates CE values that depend on i_max or v_max. E.g. WRATE.
// This does not affect the CE calibration, CE_CONFIG, or PPMCs
extern int32_t afe_recalc(Cal_t *c_ptr);

// Get a default calibration for the AFE and calibration struct.
// The default is from the CE's flash image, i_max and vmax,
// with recalculated values for WRATE, 
// starting and limit voltage and current.
// Also sets the default PPMC and PPMC2, I3_SCALE.
extern int32_t afe_get_cal_default(Cal_t *c_ptr);

// Get calibration from the AFE.
extern int32_t afe_get_cal(Cal_t *c_ptr);

// Initialize the AFE to defaults.
void afe_init(const ce_t *ce_ptr_tmp);

// Enable and calibrate the AFE.
int32_t afe_enable(const ce_t *ce_ptr_tmp, Cal_t *c_ptr);

/* Set hardware calibration defaults into an afe structure. 
   This sets Vref default temperature compensation
   and other hardware defaults.  Imax, Vmax and other application
   calibration values must be set explicitly before calling
   this function. 
   returns counts per pulse, counts' LSBs as returned by afe_wh(); */
int32_t afe_set_cal(Cal_t *c_ptr);

#endif // CAL_H calibration data available.

// Disables the AFE, with minimum power; After this, all data request
// routines return values that have been conditioned by the creep logic.
void afe_disable(void);

// Stops the CE, and puts simulated data into the CE output variables.
// Pulses intentionally do not run, but this permits testing other items.
void afe_sim(void);

// Gets the selected status from the AFE.
#define AFE_HAS_DATA 1
#define AFE_RUNNING 2
int afe_status(int);

// Tell afe code that data is ready.
// Normally called from the interrupt.
void afe_data_ready(void);

// identify the AFE version; returns pointer to the CE name.
const char * afe_version(void);

/* Set a pulse input. */
int32_t afe_set_wpulse(int32_t wh);         // For Wh pulse
int32_t afe_set_varpulse(int32_t varh);     // For VARh pulse
int32_t afe_set_xpulse(int32_t wh);         // Stubbed
int32_t afe_set_ypulse(int32_t wh);         // Stubbed.

/* Disable pulse outputs */
void afe_pulse_disable(void);

/* Enable pulse outputs */
void afe_pulse_enable(void);
 
/* Test pulse outputs: 10ms on, then off */
void afe_pulse_test(void);

/* AFE generates pulses without MPU setting wpulse or rpulse */
void afe_pulse_internal(void);

/* Set phase to sense frequency */
void afe_set_frequency_phase(int phase_idx);

/* MPU can set the input to the AFE’s pulse generators */
void afe_pulse_external(void);

// Get the pulse counts.
int32_t afe_wh_pulse_cnt(void);
int32_t afe_varh_pulse_cnt(void);

// Set the pulse counts.
void afe_set_wh_pulse_cnt(int32_t cnt);
void afe_set_varh_pulse_cnt(int32_t cnt);

// Computes Gain Adj for temp. compensation. Uses ppmc arrays.
void afe_compensate_vref (int32_t stemp);

/* Set temperature gain, using number from compensate_vref, above.
 * phase_idx: 0 (voltage), 1,2,3=current phases;4=3-phase neutral */
void afe_set_temp_gain(int phase_idx, int32_t temp_gain);

// Combine a high and low word for current.
float afe_hl2f(int32_t high, uint32_t low);

// Phase indexes
#define ALL_PHASES  0       // Whole meter, sum of Wh, VARh, A, max of V
#define PHASE_A     1
#define PHASE_B     2
#define PHASE_C     3
#define PHASE_NEUTRAL 4     //  only, and its wsum_sim
#define LAST_PHASE  PHASE_CNT

// Get wide-band values from AFE in native units.
int32_t afe_wsum(int phase_idx);        // Wh in CE units
int32_t afe_wsum_sim(int phase_idx);    // Simulate Wh from A, uses v_nom
int32_t afe_varsum(int phase_idx);      // VARh in CE units
int32_t afe_vasum(int phase_idx);       // VAh in CE units
int32_t afe_vsqsum(int phase_idx);      // Voltage, in CE units
float afe_isqsum(int phase_idx);        // Current, in CE units

/* Narrowband data from AFE in native units */
int32_t afe_wsum_nb(int phase_idx);     // Wh in CE units
int32_t afe_wsum_sim_nb(int phase_idx); // Simulate Wh from NB current
// afe_varsum_nb() is intentionally absent.
int32_t afe_vasum_nb(int phase_idx);    // VAh in CE units
int32_t afe_vsqsum_nb(int phase_idx);   // Voltage, in CE units
float afe_isqsum_nb(int phase_idx);     // Current, in CE units

/* Unit conversions */
float afe_wrate2wh(int32_t wrate);      // CE's WRATE to Wh/pulse. (i.e. 1/Kh)
int32_t afe_wh2wrate(float wh);         // Wh/Pulse (1/Kh) to CE's WRATE.
float afe_vsqsum2v(int32_t vsqsum);     // CE's vsqsum value to volts rms.
int32_t afe_v2vsqsum(float v);          // Volts RMS to CE's vsqsum value.
float afe_thr2v(int32_t thr);           // CE's sag or swell threshold to volts
int32_t afe_v2thr(float v);             // Volts to CE's sag or swell threshold
float afe_isqsum2a(float isqsum);       // CE's isqsum value to amps rms.
float afe_a2isqsum(float a);            // Amps RMS to CE's isqsum value.
float afe_insqsum2a(float isqsum);      // CE's neutral isqsum value to amps rms 
float afe_a2insqsum(float a);           // Amps RMS to CE's neutral isqsum value
float afe_wsum2wh(int32_t wsum);        // CE's wsum value to watt-hours
int32_t afe_wh2wsum(float wh);          // watt-hours to CE's wsum valud
float afe_wsum2w(int32_t wsum);         // CE's wsum value to watts
float afe_wsumf2w(float wsum);          // CE's wsum value to watts
int32_t afe_w2wsum(float w);            // watts to CE's wsum value
float afe_w2wsumf(float w);             // watts to CE's wsum value, as a float

int32_t afe_sumpre(void); /* samples per accumulation interval */

float afe_freqx2hz(int freqx); /* calculate mains freq. from CE value */
int afe_hz2freqx(float hz); /* calculate CE value of mains freq. */

/* Pass-through valid data that is above creep. */
int32_t afe_vasum_creep(int32_t phase_idx, int32_t wsum);      // VA-creep
float afe_isqsum_creep(int32_t phase_idx, float isqsum);       // Amps-creep
int32_t afe_vsqsum_creep(int32_t vsqsum);                      // Volt-creep

/* Data from AFE in engineering units. */
float afe_vrms(int phase_idx);          // Measured Vrms
float afe_arms(int phase_idx);          // Measured Arms
float afe_w(int phase_idx);             // Measured W
float afe_wh(int phase_idx);            // Measured Wh for this acc. interval.
float afe_var(int phase_idx);           // Measured VAR
float afe_varh(int phase_idx);          // Measured VARh
float afe_va(int phase_idx);            // Measured VA
float afe_power_factor(int phase_idx);  // Measured Power Factor, %

/* Narrowband data, not available in all AFEs */
float afe_vrms_nb(int phase_idx);       // Narrowband Vrms
float afe_arms_nb(int phase_idx);       // Narrowband Arms
float afe_w_nb(int phase_idx);          // Narrowband W
float afe_wh_nb(int phase_idx);         // NB Wh for this acc. interval.
float afe_var_nb(int phase_idx);        // Narrowband VAR
float afe_va_nb(int phase_idx);         // Narrowband VA
float afe_power_factor_nb(int phase_idx);  // Narrowband Power Factor, %
float afe_thd(float wideband, float narrowband); // Generic THD
float afe_w_thd(int phase_idx);         // Total harmonic distortion, watts
float afe_a_thd(int phase_idx);         // Total harmonic distortion, current
float afe_v_thd(int phase_idx);         // Total harmonic distortion, voltage

/* Misc. measurements */
int32_t afe_zero_crossings(void);   // zero crossings of accumulation interval
float afe_frequency(void);          // mains frequency, in Hz
float afe_a_to_b (void);            // Angle A-to-B, 3-phase only, degrees.
float afe_a_to_c (void);            // Angle A-to-C, 3-phase only, degrees.

/***************************************************************************
 * Copyright (C) 2016 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // AFE_H

