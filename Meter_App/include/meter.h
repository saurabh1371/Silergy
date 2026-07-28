/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Meter routines, to accumulate billing data,
 * called from the main loop.
 *
 * Copyright 2013, Silergy Corp.   All rights reserved.
 * $Id: meter.h 25648 2020-06-10 11:00:29Z phuddar $
 ***************************************************************************/
#ifndef METER_H
#define METER_H 1


// Library version
extern const char fw_ver_meter[]; 


extern uint32_t meter_state;     // State of creep, etc.
#define MINIA_MASK 0x01   // IA is below creep level
#define MINIB_MASK 0x02
#define MINIC_MASK 0x04
#define MINVA_MASK 0x08   // VA is below creep level
#define MINVB_MASK 0x10
#define MINVC_MASK 0x20
#define CREEPI_MASK 0x40  // Meter is in creep because of current.
#define CREEPV_MASK 0x80  // Meter is in creep because of voltage.
#define CREEP_MASK 0x100  // Meter is in creep.
#define SAG_MASK 0x200    // Meter is in sag
#define I_NO_V_MASK 0x400 // Meter has a phase with current but no voltage.
#define LINE_BAD_MASK 0x800 // Metering from neutral (antitamper code)

// time till the registers can be saved again, accumulation intervals.
extern int32_t meter_save_timer;
#define SAVE_DELAY_SECONDS 2

// Run meter code; returns nonzero when new data. 
extern int meter_run(void);

// Calculates demand, called from timed code.
extern void meter_demand(void);

// Starts and stops the meter.
void meter_start(const ce_t *ce_ptr);
void meter_stop(void);

// Saves billing data. Called for events that save billing data.
void meter_save_data(void);

// Reads temperature, compensates AFE for the temperature.
void meter_compensate_afe(void);

void meter_dlms_reset(void);
/***************************************************************************
 * Copyright (C) 2013-2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif
