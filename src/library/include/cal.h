/*****************************************************************************
 * DESCRIPTION: Meter run logic, the main loop.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2012...2013, Silergy Corp. All rights reserved.
 *
 * $Id: cal.h 21393 2019-06-14 01:39:30Z rvandewa $
 ****************************************************************************/
#ifndef CAL_H
#define CAL_H 1

// Firmware Device Driver version
extern const char fw_ver_cal[]; 

/**** Definitions ********/

// Install a default calibration.
void cal_reset(void);

// Get the stored calibration.  If the stored calibration is not valid,
// install a default calibration.  After this runs, a valid calibration
// will be present.
int cal_get(void);

// Save the current calibration.  It saves the CE calibration gains and phases
// and most MPU values that can be set with ")".
int cal_put (void);

// Calibration data.
enum eCALSRC {is_default = 0, is_read, is_user};
typedef struct
{
    int32_t afe_cal_a[CE_DEFAULT_CNT]; // Calibration data for AFE
    int16_t i_min;      // )0, Min. permitted current (LSB = 0.001A)
    uint8_t cfg;        // )1, Configuration bits.
    int16_t v_min;      // )2, Min. permitted voltage (LSB, 0.1V 800 = 80.0V)
    int32_t i_max;      // )3, maximum current. (2080 = 208.0A)
    int32_t v_max;      // )4, maximum voltage. (6000 = 600.0V)
    int16_t wh_whpp;    // )5, Wh, of Wh/pulse(s).
    int16_t p_whpp;     // )6, Pulses, of Wh/pulse(s).
    int16_t i_limit;    // )7, max. permitted current (0.1A)
    int16_t v_limit;    // )8, Max permitted voltage (0.1V)
    int16_t i_max_neutral; // )9, Maximum current for neutral current sensor.
    int16_t s_cal;      // )A, accumulation intervals of autocalibration measurement.
    int16_t v_cal;      // )B, 0.1 Volts rms of autocalibration.
    int16_t i_cal;      // )C, 0.1 Amp rms of autocalibration.
    int16_t theta_cal;  // )D, angle of autocalibration signal, degrees.
    uint16_t lcd_idx;   // )E, Selects LCD display.
    uint32_t lcd_bit;   // )F, Each bit selects an LCD display item.
    uint32_t mfr_id;    // )10, 3-character manufacturer ID string.
    int32_t meter_id;   // )11, Meter ID number
    int16_t gmt_offset; // )12, Minutes of offset from GMT
    int32_t tcab;       // )13, register image of TCAB
    int32_t tccd;       // )14, register image of TCCD
    uint16_t pulse_src; // )1a, pulse source
    uint8_t interval;   // )1b, Minutes of the demand interval.
    uint8_t pre_en;     // )15, preamp enable (only in single-phase meters.)
    int16_t v_nom;      // )1e, nominal voltage in a voltage tamper, 0.1vrms 
    int32_t wsum_per_cnt; // )1d, (w0sum counts)/LCD count
    // on ppmcs, 0 is the SoC, 1..last_phase are remote, last_phase+1 is N.
    int32_t ppmc[PHASE_CNT+2]; // )35...39, if phase count is 3
    int32_t ppmc2[PHASE_CNT+2]; // )3a...3e
    enum eCALSRC cal_src; // )16 Calibration source
    uint8_t cal_cnt;    // )17, Count of calibrations, >255=255
    uint8_t ver_hash;   // )18, hash of firmware version.
    uint8_t data_ok_cal[2];
} Cal_t;

/***************************************************************************
 * Copyright (C) 2011..2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif

