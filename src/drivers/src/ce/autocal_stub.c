/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 *  DESCRIPTION: Stub for automatic calibration of the meter.
 *
 *  $Id: autocal_stub.c 17380 2018-04-17 19:59:01Z ptanwar $
 ***************************************************************************/
#include "options.h"
#include "autocal.h"

const char fw_ver_autocal[] =
"autocal_stub" /* Device Driver name */
" v"         /* ver string */
"1.00"       /* Major_ver.Minor_ver */
"\r\n"       /* CR + LF */
;


/*** Private functions referenced by this module ***/
/*** Private variables referenced by this module ***/

/*** Public functions referenced by this module ***/

void cal_begin (void)                   /* Begin calibration. */
{
    cal_disable();
}

/* This is the calibration state machine.
 * In real code this calibrates gains, and phase for power elements. */
void calibrate (void)
{
}

/* called during initialization, after the calibration is restored.
 * It disables calibration. */
void cal_disable (void)
{
    global.cal.cfg &= ~AUTO_CAL;       /* Turn off calibration mode. */
}

/* end calibration mode gracefully */
void cal_end(void)
{
    cal_disable();
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/

