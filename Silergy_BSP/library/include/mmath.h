/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: This file has math functions that are not present in
 * the standard library.
 * 
 * $Id: mmath.h 18296 2018-07-13 20:31:18Z rvandewa $
 **************************************************************************/
#ifndef MMATH_H
#define MMATH_H 1


// Library version
extern const char fw_ver_mmath[]; 

// Add watts in ce units (frac) to pulse counts (cnt_ptr and frac_ptr).  
// Nearly identical logic will add 1 to the meter display, 
// by setting cnt per pulse to the unit that is displayed.
// Returns nonzero if *cnt_ptr rolls over.
int add(int32_t *cnt_ptr, int32_t *frac_ptr, int32_t frac);

// Like add(), but it expands the amount by the rate.
// Useful when metering on a duty cycle to save power.
// Returns nonzero if *cnt_ptr rolls over.
int add_prorated(int32_t* cnt_ptr, int32_t* frac_ptr, 
   int32_t frac, float rate);

// Convert pulse counts and fraction to a float or double.
// This is only accurate for ratios.  The float (or double)
// will eventually underflow, and can't be used for billing.
// However, it is very convenient in the autocalibration
// logic, which needs to take ratios of energy measurements.
float p2f(int32_t const *cnt_ptr, int32_t const *fract_ptr);
double p2d(int32_t const *cnt_ptr, int32_t const *fract_ptr);

/* watt-hours (good for a demo, bad for a meter) */
/* takes a pointer to a count of pulses */
int32_t wh (const int32_t* wh_ptr);

/* kilowatt-hours (good for a meter, bad for a demo) */
/* takes a pointer to a count of pulses */
int32_t kwh (const int32_t* wh_ptr);

/* utility functions, often unimplemented. */
#if __STDC_VERSION__ < 199400 // in C90
int32_t labs(int32_t lval);
#endif
int32_t lmax(int32_t a, int32_t b);
int32_t lmin(int32_t a, int32_t b);

// standard C99 library function not provided by most compilers.
// Converts a float to the nearest long integer.
extern long int lroundf (float f0);

// Standard square root.
float sqrtf(float);

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif
