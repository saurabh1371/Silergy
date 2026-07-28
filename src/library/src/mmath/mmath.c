/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: This file has math functions that are not present in
 * the standard library or that frequently fail in a power meter.
 *
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * History and authorship is in the automated log at the end.
 ****************************************************************************/
#include "options.h"
#include "mmath.h"


/*** Version ****/
const char fw_ver_mmath[] =
"mmath"  /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


/****************************************************************************
* Description:
*        Add up meter data. This is very important code in a meter.
*        Adds watt-hours in DSP counts to Wh counts.  This same logic
*        can add 1 to the meter display, by setting per_cnt (in Cal_t in 
*        cal.h) to be the number of DSP counts that equal 1 unit to be 
*        displayed.
*        That is, this can directly produce the billing number in the meter 
*        display if the system is arranged to do so.
*        This code has a cumulative round-off error of half a DSP LSB per reset.
*        (That is, it is perfectly accurate, with no round-off error to the
*        limit of the available electrical data from the DSP interface.)
*        This code is easy to understand and modify, fast and compact compared
*        to multiprecision arithmetic, BCD counters and other schemes.
*        It detects rollover, which is a security feature of use to some meters.
*        Notice that the code copes with impossible conditions like values
*        out of range, etc.
*        These sometimes actually occur during ESD, EMC, replacement of
*        the EEPROM containing the billing register, or other weird cases.
*        (The meters literally have to cope with lightning strikes!)
*        Also, the code is loopless because in such weird cases, this code
*        is commonly executed in the main loop, and it must finish,
*        and have sane register values afterward.
*        An early version of this code did the division by a subtraction loop
*        and caused the meter to hang when an all-1s "ff" value got into a
*        register.
*        During ESD, register values may be -corrupted-, but it complains 
*        about that to the error recording system.
*        The error system has to report to the billing system, ultimately.
*        Restarting the billing registers should also clear the error system.
* Input:
*        DSP data for Wh, VARh, etc.
* Parameters:
*        cnt_ptr points to count of pulses or billable units.
*            The count and fraction use pointers because these may need to be
*            in different places in the system.  For example the count is
*            in the billing register structure in the demo code, and the
*            fraction is in the miscellaneous structure.  So, in the demo
*            code, the count is a preserved billing register item, and
*            the fraction is not preserved.  This saves space in the billing
*            data, permitting it to be saved to an EEPROM faster.
*        fract_ptr points to count of DSP units less than 1 billable unit.
*        frac is the input (e.g. Wh) in DSP units (e.g. from w0sum).
*            it will be added to the count and fraction selected by the 
*            pointers.
* Output:
*         The billing data and fraction of a billable unit are updated
*         wherever they are in the RAM.
* Return Status:
*         Returns nonzero if the major count (*cnt_ptr) rolls over.
*****************************************************************************/
int add(int32_t* cnt_ptr, int32_t* frac_ptr, int32_t frac)
{
    int32_t cnt;
    int rollover = false;
    
    frac += *frac_ptr;   // Add new DSP "fraction" to remainder from last time.
    // Big enough to make more display units or pulses?
    cnt = frac / global.cal.wsum_per_cnt;
    // per_cnt is the number of DSP units per LCD count, 
    // calculated from the calibration.
    // Subtract the display amount from the fraction.
    frac -= global.cal.wsum_per_cnt * cnt;
    cnt += *cnt_ptr;         // Add the new display units to billing register.
    // This logic assumes that the sums of two accumulation intervals
    // will not overflow a 32-bit "fraction" number.

    // When the units and fraction are different signs,
    // the units lies, because it counts a partial unit
    // (the sum of the last unit and an opposite-sign
    // fraction) as a whole unit.
    // Make the fraction positive, so it always adds
    // to a positive units count.  Units will always become positive.
    if (frac < 0)
    {
        --cnt; // can make units underflow. See next logic.
        frac += global.cal.wsum_per_cnt;
    }
    if (frac < 0) // fraction of wh_cnt should be >= 0 now
    {
        frac = 0; // the fraction was invalid, clear it
        log_event ( EV_REG_BAD );
	// printf("EV_REG_BAD");
    }
    *frac_ptr = frac;


    /* Billing register value is OK? */
    if (cnt < 0)
    {   /* This acts like a ratchet on the counter. */
        /* billing register is nuts.  Clear it, tell owner. */
        cnt = 0;
        log_event ( EV_REG_BAD );
	// printf("EV_REG_BAD");
    }
    /* roll the billing register over when it gets too big.
     * e.g. 1000000 -> 000000; 1000002 -> 000002
     * Symbols beginning PCB_* are from the PCB's map.h file. */
    else if (cnt > PCB_WHPMAX)
    {
        rollover = true;
        cnt -= PCB_WHPMAX;

        /* Value is possible? */
        if (cnt > 105L) /* 105 1.0 Wh pulses > 600Vx208Ax3phases in one sec.*/
        {
            /* Billing register is nuts.  Clear it, tell owner. */
            cnt = 0;
            log_event ( EV_REG_BAD );
	        // printf("EV_REG_BAD");
        }
    }
    *cnt_ptr = cnt; /* save the sum to the system's billing register location */

    return rollover; // Detects a legitimate rollover of the major count.
} // add()


/***************************************************************************
* Description:
*        Adds up prorated meter data without overflow.
*   Input:
*         DSP data for Wh, VARh, etc.
*         A rate, intended to prorate the measurement over unmetered time.
* Parameters:
*         Like add() in mmath.c, but with a multiplicand rate.
*         cnt_ptr points to count of pulses or billable units.
*            The count and fraction use pointers because these may need to be
*            in different places in the system.  For example the count is
*            in the billing register structure in the demo code, and the
*            fraction is in the miscellaneous structure.  So, in the demo
*            code, the count is a preserved billing register item, and
*            the fraction is not preserved.  This saves space in the billing
*            data, permitting it to be saved to an EEPROM faster.
*         fract_ptr points to count of DSP units less than 1 billable unit.
*         frac is the input (e.g. Wh) in DSP units (e.g. from w0sum).
*            it will be added to the count and fraction selected by the 
*            pointers.
*         rate is a floating point fraction to prorate the numbers.
*   Output:
*         billing data in C structure reg_data; CRC and power-failure
*         double-buffering is set by reg_crc()
* Return Status:
*         Returns nonzero if the major count (*cnt_ptr) rolls over.
***************************************************************************/
int add_prorated(int32_t* cnt_ptr, int32_t* frac_ptr, int32_t frac, float rate)
{
    int rollover = false;
    int32_t cnt;
    // Use double, because 52 bit mantissa exceeds 32-bit integer's precision
    // by 20 bits, i.e. > million seconds of accumulation time
    double frac_d, per_cnt_d;
    float tmp_f;

    frac_d = (double)frac;  // Prepare to prorate it.
    frac_d *= (double)rate; // prorate, i.e. multiply by a fraction
    // Add new DSP "fraction of pulse" to remainder from last time.
    // This should not underflow because of the large number of extra bits.
    frac_d += (double)*frac_ptr;
    // wrate_ mpu is the number of DSP units per pulse, in the calibration.
    per_cnt_d = (double)global.cal.wsum_per_cnt;
    // The float intermediate will handle 16 million units without underflow.
    // This is more than the turn-over for most unit-counters.
    // This is the step that actually avoids overflow, by converting
    // directly into units. Units have a much larger LSB, so they fit
    // better into 32-bits than the tiny DSP units from the CE.
    cnt = lroundf((float)(frac_d / per_cnt_d));
    // Subtract the display amount from the fraction. This is the step
    // that prevents underflow by calculating the remainder using the
    // 52-bit mantissa in the double-float.
    frac_d -= (per_cnt_d * ((double)cnt));

    cnt += *cnt_ptr;         // Add the new units to billing register.

    // When the units and fraction are different signs,
    // the units lies, because it counts a partial unit
    // (the sum of the last unit and an opposite-sign
    // fraction) as a whole unit.
    // Make the fraction positive, so it always adds
    // to a positive units count.  Units will always become positive.
    // Functionally, this converts any round-up from the lroundf() of cnt into
    // a truncation.
    if (frac_d < 0.0)
    {
        --cnt; // can make units underflow. See next logic.
        frac_d += per_cnt_d;
    }
    if (frac_d < 0.0) // fraction of wh_cnt should be >= 0 now
    {
        frac_d = 0.0; // the fraction was invalid, clear it
        log_event ( EV_REG_BAD );
	    //printf("EV_REG_BAD");
    }
    tmp_f = frac_d;
    *frac_ptr = lroundf(tmp_f);

    /* Billing register value is OK? */
    if (cnt == -1)   // If it's off by one, that's ok.
    {
        cnt += PCB_WHPMAX;  // wrap back around.
    }
    if (cnt < 0)    // off by a lot is not ok.
    {
        /* billing register is nuts.  Clear it, tell owner. */
        cnt = 0;
        log_event ( EV_REG_BAD );
	    //printf("EV_REG_BAD");
    }
    /* roll the billing register over when it gets too big.
     * e.g. 1000000 -> 000000; 1000002 -> 000002 */
    else if (cnt > PCB_WHPMAX) /* PCB_WHPMAX is in map.h */
    {
        rollover = true;
        cnt -= PCB_WHPMAX;
        if(cnt > (PCB_WHPMAX/16))    // 1 week or more of energy at once?
        {
            /* Billing register is nuts.  Clear it, tell owner. */
            cnt = 0;
            log_event ( EV_REG_BAD );
	        //printf("EV_REG_BAD");
        }
    }
    *cnt_ptr = cnt; /* save the sum to the system's billing register location */

    return rollover; // Detects a legitimate rollover of the major count.
} // add_prorated()


/****************************************************************************
* Description:
*        Convert a pulse count and fraction to a float in CE units.
*        This is only accurate for ratios.  It drops 7 bits of
*        precision due to underflow and can't be used for billing registers.
*        However, it is very convenient in the autocalibration
*        logic, which needs to take ratios of accurate energy measurements.
* Input:
* Parameters:
*        cnt_ptr points to count of pulses or billable units.
*        fract_ptr points to count of DSP units less than 1 billable unit.
* Output:
*         Returns wsum as a float.
* Return Status:
*         None
*****************************************************************************/
float p2f(int32_t const *cnt_ptr, int32_t const *fract_ptr)
{
    float f;
    f = (float)*cnt_ptr;
    f *= (float)global.cal.wsum_per_cnt;
    f += (float)*fract_ptr;
    return f;
} // p2f()


/****************************************************************************
* Description:
*        Convert a pulse count and fraction to a double in CE units.
*        This is only accurate for ratios.  In billing, it will eventually
*        lose precision due to underflow.
*        It is very convenient in the autocalibration
*        logic, which needs to take ratios of accurate energy measurements.
* Input:
* Parameters:
*        cnt_ptr points to count of pulses or billable units.
*        fract_ptr points to count of DSP units less than 1 billable unit.
* Output:
*         Returns wsum as a double
* Return Status:
*         None
*****************************************************************************/
double p2d(int32_t const *cnt_ptr, int32_t const *fract_ptr)
{
    double d;
    d = (double)*cnt_ptr;
    d *= (double)global.cal.wsum_per_cnt;
    d += (double)*fract_ptr;
    return d;
} // p2d()


/****************************************************************************
* Description:
*        Calculate watt-hours (good for a demo, bad for a real meter)
*        Why is this good for a demo?  The display will change.  It
*        doesn't looks broken.  Of course, this is perfect for a demo.
*        It is also not functional for a real meter.
* Input:
* Parameters:
*        cnt_ptr a pointer to a count of pulses or billable units.
* Output:
* Return Status:
*        an integer count of kilowatt-hours (or kilovar-hours, kilovah-etc.)
*****************************************************************************/
int32_t wh (const int32_t* cnt_ptr)
{
    int32_t display = *cnt_ptr;
    return display % PCB_WHPDISPMAX; // PCB_* is in the PCB's map.h file.
} // wh()


/****************************************************************************
* Description:
*        Calculate kilowatt-hours (good for a real meter, bad for a demo)
*        Why is this bad for a demo?  When displaying KWn a meter can run 
*        for hours at quite high currents and the display will not change.
*        It looks broken.  Of course, this is perfect for a real meter.
* Input:
* Parameters:
*        cnt_ptr a pointer to a count of pulses or billable units.
* Output:
* Return Status:
*        an integer count of kilowatt-hours (or kilovar-hours, kilovah-etc.)
*****************************************************************************/
int32_t kwh (const int32_t* cnt_ptr)
{
    int32_t display = (*cnt_ptr)/1000L;
    return display % PCB_WHPDISPMAX; // From the PCB's map.h file.

} // kwh()


#if __STDC_VERSION__ < 199400 // in C90
/****************************************************************************
* Description:
*        Take a long absolute value.
*        This is included just because many compiler environments lack it.
* Input:
* Parameters:
*        long signed integer.
* Output:
* Return Status:
*        long POSITIVE signed integer.
*****************************************************************************/
int32_t labs(int32_t lval)
{
    if (0 < lval)
        return lval;
    else
        return -lval;
} // labs()
#endif // in standard library for later versions.


/****************************************************************************
* Description:
*         Find the larger of two integers.
*         This is included just because many compiler environments lack it.
* Input:
* Parameters:
*         Two long integers.
* Output:
* Return Status:
*         The largest of the two.
*****************************************************************************/
int32_t lmax(int32_t a, int32_t b)
{
    if (a > b)
        return a;
    else
        return b;
} // lmax()


/****************************************************************************
* Description:
*         Find the smaller of two integers.
*         This is included just because many compiler environments lack it.
* Input:
* Parameters:
*         Two long integers.
* Output:
* Return Status:
*         The smallest of the two.
*****************************************************************************/
int32_t lmin(int32_t a, int32_t b)
{
    if (a < b)
        return a;
    else
        return b;
} // lmin()


/****************************************************************************
* Description:
*        Standard C99 library function API, except...
*        This converts floats the way people expect from an instrument,
*        rather than following the IEEE standard.
*        This converts a float, including negative numbers, to the nearest
*        long integer rather than rounding to the nearest -even- integer.
*        (This is contrary to the ANSI floating point standard.
*        The standard says that 0.5 rounds to 0.  This code rounds 0.5 to 1.)
*        Saturates negative and positive values that are out of range,
*        including infinities. (e.g. NAN-positive-infinity -> 0x7FFFFFFF
*        and NAN-negative-infinity->0x80000000)
*        Other NANs convert to 1. Signaling NANs report errors.
* Input:
* Parameters:
*        An ANSI 32-bit floating-point value.
* Output:
* Return Status:
*        A signed 32-bit 2s-complement integer.
*****************************************************************************/
long int lroundf (float f0)
{
    int32_t l; // l is an integer containing the bits
    int32_t m; // m is the mantissa, isolated
    int32_t e; // e is the exponent, isolated; later, the shift amount
    union
    {
        int32_t l;
        float f;
    } u;

    // The ANSI-standard way to get the bits of a float.
    u.f = f0;
    l = u.l;

    // get the exponent
    e = (l >> 23) & 0xff;  // shift the exponent
    e -= (127 + 23); // bits to make a long integer
    if (e < -24) // underflow, zero or denormal
    {
        return 0; // Closest integer approximation, sorry.
    }
    else if (e == 105) // 0xFF?
    {
        // NAN or infinity by the IEEE Standard
        m = l & 0x7FFFFFL;
        if (m != 0) // NaN?
        {
            if (0 == (m & 0x400000L)) // signaling NaN?
            {
                log_software();  // report an error
            }
            return 1; // return a maximally innocuous number.
        }
        else goto infinity; // Sorry about the goto.  It's simpler.
    }
    else if (e > 7) // overflow
    {
    infinity: 
        // Saturate overflow and infinities.
        if (l < 0)
            return 0x80000000L; // return minimum signed int
        else
            return 0x7FFFFFFFL; // return maximum signed int
    }
    else // A normal integer appears possible.
    {
        // get the mantissa
        m = (l & 0x7FFFFFL) | 0x800000L;

        // number of shifts to make a long
        if (e < 0)
        {
            e = (-e) - 1;
            m >>= e;
            // round to nearest value
            m +=1L;
            m >>= 1;
        }
        else if (e > 0)
        {
            m <<= e;
        }

        if (l < 0) // restore the sign
            return -m;
        else
            return m;
    }
} // lroundf()


/****************************************************************************
* Description:
*       A robust floating-point square root:
*       It converts negative numbers to positive and takes the square root.
*       A fast approximation,
*       followed by seven iterations of newton-raphson.
*       Doing 7 iterations in high-level code can take more time than many 
*       library implementations, but sometimes has increased accuracy.
*       The accuracy can be significant in autocalibration.
* Input:
* Parameters:
*       An ANSI-standard 32-bit float.
* Output:
* Return Status:
*       A square root.
*****************************************************************************/
float sqrtf(float f)
{
    int cnt;
    union
    {
        int tmp;
        float f;
    } u;

    /* Safety code to prevent errors. */
    if (f < 0)
        f = -f;

    u.f = f;

    /* To justify the following code, prove that
     * ((((val_int / 2^m) - b) / 2) + b) * 2^m
     *    = ((val_int - 2^m) / 2) + ((b + 1) / 2) * 2^m)
     * where: val_int = u.tmp
     * b = exponent bias
     * m = number of mantissa bits */

    u.tmp -= 1 << 23; /* Subtract 2^m. */
    u.tmp >>= 1; /* Divide by 2. */
    u.tmp += 1 << 29; /* Add ((b + 1) / 2) * 2^m. */

    /* u.f is now the starting approximation, usually within 3.5%.
     * Perform newton-raphson to refine the value. 
     * Do it 7 times because each time does about one digit
     * and there are about 7 digits in a float. */
    for(cnt = 7; cnt > 0; --cnt)
    {
        u.f = u.f - ((u.f * u.f) - f)/(2 * u.f);
    }

    return u.f;
} // sqrtf()


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

