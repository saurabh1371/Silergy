/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 *
 *  DESCRIPTION: Automatic calibration of the meter.
 *  For equation 1, on split phase meters.
 *  The code sets the calibrations to nominal values, then measures
 *  power, then adjusts the calibration so that the actual measurements
 *  will be the same as the ideal measurements in v_cal and i_cal.
 *
 *  The typical error is a failure to connect the meter to a calibration
 *  signal.  In these cases the calibration value is set too large.
 *  The code flags this with an LCD message and the phase number.
 *
 *  This code is derived from successful code used in the 6510 and 6530 demos.
 *  
 *  The calibration is saved and restored using routines from main.c
 *  Billing registers are cleared by setting a configuration flag
 *  that is interpreted in teh main loop.
 *  Errors are detected by setting the error variable to the error index.
 * 
 *  $Id: autocal.h 16212 2017-12-08 00:31:28Z ptanwar $
 ***************************************************************************/
#ifndef AUTOCAL_H
#define AUTOCAL_H 1

// Firmware Device Driver version
extern const char fw_ver_autocal[];

void cal_disable (void); // Disable calibration modes.

void cal_begin (void);  // Begin calibration. Call by the CLI.

void cal_end (void);    // End calibration. Call by the CLI.

void calibrate (void);  // Perform calibration. Called by the main loop.

#endif // CAL_H
/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

