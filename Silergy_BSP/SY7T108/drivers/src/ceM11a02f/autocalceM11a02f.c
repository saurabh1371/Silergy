/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 *
 *  DESCRIPTION: Automatic calibration of the meter.
 *  For equations:
 *  0: Wh = VA * IA
 *  1: Wh = VA * (IA - IB) /2 
 *  2: Wh = VA * IA + VB * IB
 *  5: Wh = VA * IA + VB * IB + VC * IC
 *  The code sets the calibrations to nominal values, then measures
 *  power, then adjusts the calibration so that the actual measurements
 *  will be the same as the ideal measurements in v_cal and i_cal.
 *
 *  The typical error is a failure to connect the meter to a calibration
 *  signal.  In these cases the calibration value is set too large.
 *  The code flags this with an LCD message and the phase number.
 *
 *  This code is ported from successful code used in the 6510, 6530 and 6540
 *  demos.
 *  The calibration is saved and restored using routines from main.c
 *  Billing registers are cleared by setting a configuration flag
 *  that is interpreted in the main loop.
 *  Errors are detected by setting the error variable to the error index.
 *
 *  $Id: autocalceM11a02e.c 21396 2019-06-15 02:27:04Z rvandewa $
 ***************************************************************************/
#include <stdlib.h>
#include <ctype.h>      // C data types.
#include <stdint.h>     // Standard integer definitions.
#include <math.h>   /* for atan(), acos() */
#include "options.h"
#include "ce.h"
#include "afe.h"    /* To get the data from the front-end. */
//#include "lcd.h"    /* To report a failure. */
#include "mmath.h"  /* for sqrtf() */

/*** Version ****/
const char fw_ver_autocal[] =
"autocalcem11a02e" /* Device Driver name */
" v"              /* ver string */
"1.01"            /* Major_ver.Minor_ver */
"\r\n"            /* CR + LF */
;

#if EQUATION!=EQUATION1  /* Equation 1 requires different logic */

/*** Private functions referenced by this module ***/
static void  adjust(int idx); /* adjust phase 0..n */

/*** Private variables referenced by this module ***/
int16_t cnt_down;   /* Remaining accumulation intervals of calibration. */
static const char *cal_error;

typedef struct Cal_Data_s
{
    int32_t whi;    /* register math doesn't do signed addition... */
    int32_t whi_frac;
    int32_t whe;    /* so the imports and exports have to be added up. */
    int32_t whe_frac;
    int32_t varhi;  /* register math doesn't do signed addition... */
    int32_t varhi_frac;
    int32_t varhe;  /* so the imports and exports have to be added up. */
    int32_t varhe_frac;
    int32_t vah;    /* VAh added up. */
    int32_t vah_frac;
    int32_t v;      /* register math doesn't do signed addition... */
    int32_t v_frac;
} Cal_Data_t;

typedef struct Element_s
{
    int32_t *pcal_v;
    const  char *v_error_strz;
    int32_t *pcal_i;
    const  char *i_error_strz;
    int32_t *pphase_adj;
    const  char *p_error_strz;
} Element_t;

//static const char cal_str[] = "Cal.";
static const char va_str[] = "VA";
static const char ia_str[] = "IA";
static const char pa_str[] = "PA";
static const char vb_str[] = "VB";
static const char ib_str[] = "IB";
static const char pb_str[] = "PB";

/* Only 1-phase CE codes are defined in this code. */
#define ELEMENT_CNT 2

// The following table describes the type of meter that the
// CE code expects.
static const  Element_t element_array [ELEMENT_CNT] = {
{&ce_data.cal_v0, va_str, &ce_data.cal_i0, ia_str, &ce_data.phadj_0, pa_str},
{&ce_data.cal_v1, vb_str, &ce_data.cal_i1, ib_str, &ce_data.phadj_1, pb_str},
};

Cal_Data_t data_array[ELEMENT_CNT];

void clear_ce_data(int i)
{
    *(element_array[i].pcal_v) = UNITY_GAIN;
    *(element_array[i].pcal_i) = UNITY_GAIN;
    *(element_array[i].pphase_adj) = ZERO_PHASE;
}

void clear_cal_data(int i)
{
    global.misc.acc_cnt = 0;
    data_array[i].whi = 0;
    data_array[i].whi_frac = 0;
    data_array[i].whe = 0;
    data_array[i].whe_frac = 0;
    data_array[i].varhi = 0;
    data_array[i].varhi_frac = 0;
    data_array[i].varhe = 0;
    data_array[i].varhe_frac = 0;
    data_array[i].vah = 0;
    data_array[i].vah_frac = 0;
    data_array[i].v = 0;
    data_array[i].v_frac = 0;
}

void cal_begin (void)                   /* Begin calibration. */
{
//    int idx;
    cal_error = NULL;

//    Commented out to permit calibration one phase at a time.
//    /* Sets default calibration for CE. */
//    for (idx = ELEMENT_CNT - 1; idx >= 0; --idx)
//    {
//        clear_ce_data(idx);
//    }

    /* Start the calibration process, with 2 accumulation intervals
     * before measurement begins, to clear the data pipeline. */
    cnt_down = (int16_t)(2 + global.cal.s_cal);

    /* set the global calibration flag */
    global.cal.cfg |= AUTO_CAL;
}

/* This the calibration state machine.
 * It is called when xfer_busy is set, indicating that CE
 * data is available.
 * The states are:
 * 1. Let the CE settle, then
 * 2. Measure the current for calibration of the phases, then
 * 3 Calibrate the phases.
 * This calibrates gains, and phase for power elements. */
void calibrate (void)
{
    int i;

    if(0 == (global.cal.cfg & AUTO_CAL))
        return;

    /* Safety code. if calibration is somehow enabled (it's just one bit),
     * and it doesn't make sense, disable it again. */
    if (0 >= cnt_down || (2*global.cal.s_cal) < cnt_down)
    {
        cal_disable();             /* Disable calibration. */
        return;
    }

    /* Count accumulation intervals. */
    --cnt_down;

    /* Add up the data this time. */
    for (i = 0; i < ELEMENT_CNT; ++i)
    {
        int32_t wh, varh, vah, v;
        wh = afe_wsum(i+1);
        varh = afe_varsum(i+1);
        vah = afe_vasum_creep(i+1,afe_vasum(i+1));
        #if EQUATION==EQUATION0 // single-phase
        // Note fixed index "1"; needed for single-phase neutral
        v = afe_vsqsum(1);
        #else // not single-phase
        v = afe_vsqsum(i+1);
        #endif

        if (wh < 0) {
            add( &data_array[i].whe, &data_array[i].whe_frac, -wh);
        } else {
            add( &data_array[i].whi, &data_array[i].whi_frac, wh);
        }
        if (varh < 0) {
            add( &data_array[i].varhe, &data_array[i].varhe_frac, -varh);
        } else {
            add( &data_array[i].varhi, &data_array[i].varhi_frac, varh);
        }

        add( &data_array[i].vah, &data_array[i].vah_frac, vah);

        add( &data_array[i].v, &data_array[i].v_frac, v);
    }

    if (cnt_down == global.cal.s_cal)
    {
        if(0 == (global.cal.cfg & AUTO_CAL)) {
            cal_disable();
            return;
        }
        for (i = 0; i < ELEMENT_CNT; ++i)
        {
            clear_cal_data(i);
        }
    }

    if (0 == cnt_down)
    {   /* If the last accumulation interval, calibrate.
         * Don't save "calibration mode" in the configuration. */
        for (i = 0; i < ELEMENT_CNT; ++i)
        {
            adjust(i);
        }

      //  lcd_clear();

        /* Test the calibration */
        if(NULL != cal_error)
        {
            afe_disable();       /* After 10 seconds, watchdog reset. */

        } else {
            /* If the calibration is OK, save it. */
            cal_put();              /* Save the new calibration. */

            global.cal.cfg |= CLEAR_REG;       /* Clear the billing registers to 0. */
        }
        cal_disable();              /* Disable the calibration. */
    }
}

/* called during initialization, after the calibration is restored.
 * It disables calibration. */
void cal_disable (void)
{
    cnt_down = -1;          /* Make the count invalid. */
    global.cal.cfg &= ~AUTO_CAL;       /* Turn off calibration mode. */
}

/* end calibration mode gracefully */
void cal_end(void)
{
    cnt_down = 0;
}

/* limit gains */
static bool gain_limit(int32_t *val_ptr, double cal_val)
{
    int32_t out_val = lroundf((float)cal_val);
    bool error = false;

    if(NULL == val_ptr) // Can this value be set?
        return false;   // No. It is intentional, so no error.
    // Already set, so no error.
    if(*val_ptr != UNITY_GAIN)
        return false;   // gain is already set, no error.

    /* gains vary from 2..0.5, -0.5..-2 */
    error |= (bool)(out_val > MAX_GAIN);
    error |= (bool)(out_val < MIN_GAIN);

    /* if there's an error, revert to the default */
    if (error)
        out_val = UNITY_GAIN;
    *val_ptr = out_val;

    return error ? true:false;
}

/* limit phase */
static bool phase_limit(int32_t *val_ptr, double cal_val, int32_t def_phase)
{
    int32_t out_val = lroundf((float)cal_val);
    bool error = false;

    if(NULL == val_ptr) // Can this value be set?
        return false;   // No.
    // Already set, so no error.
    if(ZERO_PHASE != *val_ptr)
        return false;

    /* if there's an error, revert to the default */
    if (error)
        out_val = def_phase;
    /* The phase correction can sometimes be better characterized over series
     * production. So only set the phase if there is no default phase set */
    if (def_phase == *val_ptr)
        *val_ptr = out_val;

    return error ? true:false;
}

#define pi (3.14159265)
#define Fs (CE_FS)   // from ce<number>.h, a global from options.h->ce.h
#define alpha 0.01562500
#define beta  0.99804687500
// pphadj points at the phase adjustment floating point, a fraction.
// pphgn points at the phase adjustment gain floating point, a fraction.
// phi is the correction in radians.
// hz is the frequency of the signal in hz
// Reverse-engineered from the spreadsheet, unit test in /Sample_code/Metrology
static void phase_adj_ct(double *pphadj, double *pphgn, double phi, float hz)
{
	double tanphi, radians, a, b, c, _cos, _sin, out, Phgn;

	tanphi = tan(phi);
	radians = (2.0 * pi * (double)hz) / Fs;
	_cos = cos(radians);
	_sin = sin(radians);
	a = 1.0 + (beta * beta) - (2.0 * beta * _cos);
	b = (float)beta * (float)_sin;
	c = 1.0 - (beta * _cos);

	out = (a * tanphi) / (b - (c * tanphi)) / (alpha);
	Phgn = sqrt(1.0 + (alpha * out) *(2.0 + (alpha * out) - 2.0 * (beta*_cos)) / a);

	*pphadj = out;
	*pphgn = Phgn;
}


/* Adjust the gains and phase of each phase.
 * The assumed test signal is an AC signal with no phase offset between
 * voltage and current.
 * This calculates a rotating and scaling linear transform in polar
 * coordinates to adjust the CE.  The transform is an ideal solution,
 * with no approximations beyond those in the data sheet. */
static void  adjust (int i)
{
    auto double fT;   /* temporary float */
    auto double Phgn;    /* correction of gain for phase, fraction */
    const  Element_t *pel;    /* pointer to an element */
    const Cal_Data_t *pda0;      /* pointer to channels data */
    double VAI;   /* ideal volt-amps */
    double VI;    /* ideal volts */
    double WhM;   /* Watt-hours measured */
    double VARhM; /* VAR-hours measured */
    double VAhM;  /* Volt-amp-hours measured */
    double VM;    /* Volts measured */
    double Vgn;   /* Corrective gain of the voltage, 1=unity */
    double hz;    /* measured frequency */

    /* locate the Element's structure: */
    pel = &element_array[i];
    pda0 = &data_array[i];

    // To disable autocal for neutral, uncomment this.
    //#if EQUATION==EQUATION0
//    if(i > 0) {
//        *(pel->pcal_i) = 0;
//        *(pel->pcal_v) = 0;
//        *(pel->pphase_adj) = ZERO_PHASE;
//        return;
//    }
    //#endif

    /* get measured net Wh and VARh
     * They're calculated from imports and exports because the
     * meter math is unsigned.  The advantage of the meter math
     * is that unlike floating point, it has no integration underflow.
     * The floating point here is used for ratios, which is fine.
     * p2fd() converts 64-bit numbers to doubles that count CE counts */
    WhM    = p2d (&pda0->whi,   &pda0->whi_frac);
    WhM   -= p2d (&pda0->whe,   &pda0->whe_frac);   /* subtract exports */
    VARhM  = p2d (&pda0->varhi, &pda0->varhi_frac);
    VARhM -= p2d (&pda0->varhe, &pda0->varhe_frac); /* subtract exports */
    VAhM   = p2d (&pda0->vah, &pda0->vah_frac); /* VAh */
    VM     = p2d(&pda0->v, &pda0->v_frac); /* V */

    /* If the current or voltage was off, do not adjust this phase. */
    /* This permits calibration to occurr one phase at a time. */
    if(VAhM == 0.0)
        return;

    /* Calibration equations begin here */

    /* prevent divide by zero */
    if (WhM > -1.0e-17 && WhM < 1.0e-17)
        WhM = 1.0e-17;
    // y/x = tan(phi), phi = atan(y/x) = atan(var/wh)
    fT = atan2(VARhM, WhM);    // atan2(y, x) = atan(y/x)
    // fT is angle in radians.

    /* This permits the phase angle correction to occur at any 
     * voltage angle */
    if (0 != global.cal.theta_cal) /* at 0, no correction is needed */
    {
        // theta_cal is in degrees. Converts it to radians.
		fT =  ((pi * (double)global.cal.theta_cal)/180.0) - fT;
    }

    // Using the measured frequency for the phase adjustment
    // cancels out the effects of the IC's and calibrator's frequency errors.
    hz = (double)afe_frequency(); // Get the measured frequency.

    phase_adj_ct(&fT, &Phgn, fT, hz);

    /* Adjust to the CE's range. */
    fT = ((double)UNITY_GAIN) * fT;      /* convert to CE's LSBs for phase */
    if (phase_limit(pel->pphase_adj, fT, 0))
    {
        cal_error = pel->p_error_strz;
    }

    /* find the ideal voltage
     * The ideal Vrms is stored in Totals.Parms
     * Parameters_T is loaded from a table ri_Parms[]
     * in defaults.c at power-up. */
    VI  = ((double)(global.cal.v_cal)) / (10.0);    /* ideal volts */

    /* Find the measured voltage */
    VM /= (double)global.cal.s_cal;
    VM = (double)afe_vsqsum2v(lroundf((float)VM));

    /* The voltage of the test signal has no harmonics or delay. (Ideally...) */
    Vgn = VI/VM;    /* calculate voltage gain, 1 = unity */
    fT = ((double)UNITY_GAIN) * Vgn;     /* convert to CE's LSBs for gain */
    /* Adjust to the CE's range, and find errors. */
    if (gain_limit(pel->pcal_v, fT))
    {
        cal_error = pel->v_error_strz;
    }

    /* Convert measured VAh in DSP units to measured VA in 
     * engineering units.
     * Why?  It's easier to check the math. VAM should be ~= Vrms * Irms. */
    VAhM /= (double)global.cal.s_cal;
    VAhM = afe_wsumf2w((float)VAhM);

    /* Find the ideal VA;
     * The ideal Vrms and Irms are stored in Totals.Parms
     * Parameters_T is loaded from a table ri_Parms[] in defaults.c
     * at power-up. 100?  v_cal is ideal V * 10, i_cal is ideal I * 10. */
    VAI = ((double)global.cal.v_cal * (double)global.cal.i_cal) / 100.0;

    /* Ign, the current gain, needs scaling to eliminate power errors,
     * and calculating relative to VAh scales the same at every phase angle.
     * fT = Ign */
    fT = VAI/(VAhM * Vgn * Phgn);        /* phased current gain, 1 = unity */

    fT = ((float)UNITY_GAIN) * fT;      /* convert to CE's LSBs for gain */
    /* Adjust to the CE's range and detect errors. */
    if (gain_limit(pel->pcal_i, fT))
    {
        cal_error = pel->i_error_strz;
    }
}
#endif /* equation not equation 1 */


/***************************************************************************
 * Copyright (C) 2005...2016 Silergy Corp. 
 * All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. Inc.
 ***************************************************************************/

