/*****************************************************************************
 * DESCRIPTION: Tamper detection logic.
 * This code manages burglar alarms in a meter.
 * The idea is to use whatever tamper features are permitted by the PCB.
 * DIOs, interrupts or wake inputs are defined in the PCB's map file.
 * If a tamper GPIO is defined, it is polled.
 * If a tamper interrupt is defined, it is enabled.
 * If a tamper wake is defined, it is selected before sleep.
 * The tamper enables and detection are "smart", so that if a tamper 
 * already exists, the tamper interrupt or wake is not invoked.
 * This is so that the meter can seem to function normally, while still
 * recording a tamper.
 * The tamper recording sets a bit EV_TAMPER_MASK in state , 
 * a global variable.
 * The demo code, in main(), uses the EV_TAMPER_MASK & state
 * to count seconds of tamper.
 * The LCD display can poll this bit and produce a display if needed.
 * Be careful to display a tamper indication in a way that a meter-reader
 * can recognize, but not a utility customer.  If a tamper is clearly shown
 * on a meter, further vandalism often occurs.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2016, Silergy Corp. All rights reserved.
 *
 * $Id: tamper.h 18743 2018-09-11 22:13:29Z rvandewa $
 ****************************************************************************/
#ifndef TAMPER_H
#define TAMPER_H 1

// Version string.
extern const char fw_ver_tamper[];

// Tamper detection initialization.
void tamper_init(void);

// Tamper detection polling.
int tamper_run(void);

// Tamper detection interrupt(s) utilize the external interrupts.

// Tamper detection by waking from sleep: Set wake flags.
int tamper_wakes(int in);


/***************************************************************************
 * Copyright (C) 2016 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // TAMPER_H
