/*******************************************************************************
 * Copyright (C) 2013 ..  2020 Silergy Corp, All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.** The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Id: dlmsif_tampers.h 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/

/********************************************************************************
*
*     Description: 
*        Filename: dlmsif_tampers.h
*
*        Compiler: uVision ARMCC.exe
*
*         Version: uVision 5.27.1
*
*         Created: 				
*							
*   Modifications: 
*         
*******************************************************************************/
#ifndef __DLMSIF_TAMPERS_H__
#define __DLMSIF_TAMPERS_H__

#include <stdint.h>

#define EVEN_LOG_BUFSIZE    30      // event log buffer = 30 bytes

/* Creep  */
//#define    WCREEP    27000  //240 * 10 mA for Zon 
#define    Varcreep  27000  //240 * 10 mA for Zon 

#define VOLTAGE_60VOLTS		60	// in volts
#define VOLTAGE_110VOLTS	110	// in volts
#define VOLTAGE_150VOLTS	150	// in volts
#define VOLTAGE_280VOLTS	280	// in volts
#define VOLTAGE_300VOLTS	300     // in volts
#define CURRENT_HIGH		60	// in Amps
#define MIN_CURRENT		50	// in mAmps

#define PF_THRESHOLD		950	// (pf/1000)for neutral disturbance
#define PF_THRESHOLD_LOW	20	// (pf/100) for very low pf

#define VARSUM_THRESHOLD	10000	//
#define CTBYPASS_THRESHOLD      200     // in mamps

//voltage event IDs
#define R_MISSING_OCCURS			1
#define R_MISSING_RESTORE			2
#define Y_MISSING_OCCURS			3
#define Y_MISSING_RESTORE			4
#define B_MISSING_OCCURS			5
#define B_MISSING_RESTORE			6
#define OVER_VOLTAGE_OCCURS			7
#define OVER_VOLTAGE_RESTORE			8
#define LOW_VOLTAGE_OCCURS			9
#define LOW_VOLTAGE_RESTORE			10
#define VOLTAGE_UNBALANCE_OCCURS		11
#define VOLTAGE_UNBALANCE_RESTORE		12
//current events IDs
#define R_CT_REVERSE_OCCURS			51
#define R_CT_REVERSE_RESTORE			52
#define Y_CT_REVERSE_OCCURS			53
#define Y_CT_REVERSE_RESTORE			54
#define B_CT_REVERSE_OCCURS			55
#define B_CT_REVERSE_RESTORE			56
#define R_CT_OPEN_OCCURS			57
#define R_CT_OPEN_RESTORE			58
#define Y_CT_OPEN_OCCURS			59
#define Y_CT_OPEN_RESTORE			60
#define B_CT_OPEN_OCCURS			61
#define B_CT_OPEN_RESTORE			62
#define CURRENT_UNBALANCE_OCCURS		63
#define CURRENT_UNBALANCE_RESTORE		64
#define CT_BYPASS_OCCURS			65
#define CT_BYPASS_RESTORE			66
#define OVER_CURRENT_OCCURS			67
#define OVER_CURRENT_RESTORE			68
#define EARTHLOADING_OCCURS			69
#define EARTHLOADING_RESTORE			70
//power events  IDs
#define POWER_FAILURE_OCCURS			101
#define POWER_FAILURE_RESTORE			102

// Transaction related event IDs
#define RTC_CLOCK_TRANSACTION                   151
#define DEMAND_INTEGRATION_TRANSACTION		152
#define PROFILE_CAPTURE_TRANSACTION     	153
#define SINGLE_ACTION_SCH_TRANSACTION  		154
#define ACTIVITY_CALENDAR_TRANSACTION		155

// magnetic events IDs
#define EVENT_MAG_OCCURS	 		201
#define EVENT_MAG_RESTORE			202
#define EVENT_MAG_NEUTRAL_D_OCCURS		203
#define EVENT_MAG_NEUTRAL_D_RESTORE		204
#define EVENT_MAG_VLF_OCCURS			205
#define EVENT_MAG_VLF_RESTORE			206
#define EVENT_MAG_NEUTRALMISIING_OCCURS		207
#define EVENT_MAG_NEUTRALMISIING_RESTORE	208

// non-rollover IDs
#define EVENT_COVER_OPEN_OCCURS			251

// remore connect/disconnect (NOT USED)
/***************************
NOT USED				 	301
						302
****************************/

#define MIN_VOLTAGE_EVENTID			LOW_VOLTAGE_OCCURS
#define MAX_VOLTAGE_EVENTID			LOW_VOLTAGE_RESTORE
#define NUM_VOLTAGE_EVENTS			((MAX_VOLTAGE_EVENTID-(MIN_VOLTAGE_EVENTID-1))/2)	// each event has occurrence/restore

#define MIN_CURRENT_EVENTID			EARTHLOADING_OCCURS
#define MAX_CURRENT_EVENTID			EARTHLOADING_RESTORE
#define NUM_CURRENT_EVENTS			((MAX_CURRENT_EVENTID-(MIN_CURRENT_EVENTID-1))/2)	// each event has occurrance/restore

#define MIN_POWER_EVENTID			POWER_FAILURE_OCCURS
#define MAX_POWER_EVENTID			POWER_FAILURE_RESTORE
#define NUM_POWER_EVENTS			((MAX_POWER_EVENTID-(MIN_POWER_EVENTID-1))/2)	// each event has occurrance/restore

#define MIN_TRANSACTION_EVENTID			RTC_CLOCK_TRANSACTION
#define MAX_TRANSACTION_EVENTID			ACTIVITY_CALENDAR_TRANSACTION
#define NUM_TRANSACTION_EVENTS			(MAX_TRANSACTION_EVENTID-(MIN_TRANSACTION_EVENTID-1))	//  occurrance is an event. Restoration is not an event

#define MIN_OTHERS_EVENTID			EVENT_MAG_OCCURS
#define MAX_OTHERS_EVENTID			EVENT_MAG_NEUTRALMISIING_RESTORE
#define NUM_OTHERS_EVENTS			((MAX_OTHERS_EVENTID-(MIN_OTHERS_EVENTID-1))/2)	// each event has occurrance/restore

#define MIN_NONROLLOVER_EVENTID			EVENT_COVER_OPEN_OCCURS
#define MAX_NONROLLOVER_EVENTID			EVENT_COVER_OPEN_OCCURS
#define NUM_NONROLLOVER_EVENTS			(MAX_NONROLLOVER_EVENTID-(MIN_NONROLLOVER_EVENTID-1))	//  occurrance is an event. Restoration is not an event

#define MIN_CONTROL_EVENTID			301
#define MAX_CONTROL_EVENTID			302
#define NUM_CONTROL_EVENTS			((MAX_CONTROL_EVENTID-(MIN_CONTROL_EVENTID-1))/2)	//  occurrance is an event. Restoration is not an event

// maximum number of events for single phase
// Voltage_events = 1, Current_events = 1, Power_fail = 1, Transaction_events = 5
// Mag_events = 4 (actually 3, LOW_PF not used), cover_open_event = 1, 
// remote_event = 1 (not used)
#define MAX_TAMPER_EVENTS               14

#define TAMPER_OCCURRANCE_COUNTER	60		// in seconds
#define TAMPER_RESTORE_COUNTER		60		// in seconds
#define TAMPER_TRANSACTION_COUNTER	10 		// in seconds


// exported functions
void dlmsif_tampers_init(void);
void dlmsif_tampers_checks(void);
void dlmsif_tampers_get(void);
void dlmsif_tampers_eventlogsset(uint8_t occurance);
void dlmsif_tampers_set(void);

#endif  //#ifndef __DLMSIF_TAMPERS_H__
