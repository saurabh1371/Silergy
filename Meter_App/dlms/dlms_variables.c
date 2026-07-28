/*******************************************************************************
 * Copyright (C) 2013 ..  2020 Silergy Corp., All rights Reserved.
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
 * $Id: dlms_variables.c 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/

/********************************************************************************
*
*     Description: 
*        Filename: dlms_variables.c
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
#include "dlms_variables.h"
#include "silergy_dlms_lib.h"
#include "Defines.h"

// instantaneous parameters
uint32_t dlms_phase_current,dlms_neutral_current,dlms_measured_current;
uint32_t dlms_voltage;
int32_t dlms_pf;
int32_t dlms_avg_pf;						// average power factor (single phase)
uint32_t dlms_hz;
uint32_t dlms_apparent_power_kva;
int32_t dlms_active_power_kw;
int32_t dlms_reactive_power_kvar;

uint32_t inst_phase_current,inst_neutral_current,inst_measured_current;
uint32_t inst_voltage;
int32_t  inst_pf;
int32_t  avg_pf;						// average power factor (single phase)
uint32_t inst_freq;
uint32_t inst_kva;
int32_t  inst_kw;
int32_t  inst_kvar;

uint32_t dlms_num_powerfails;
uint32_t dlms_num_powerfail_dur;
uint32_t dlms_poweron_dur_minutes;              // total power on duration of meter in minutes 
uint32_t dlms_poweron_dur_billing_minutes;      // total power on duration of meter in minutes for billing
uint32_t dlms_num_tampercount;
uint32_t dlms_cumulative_billing_count;		// cumulative billing counter. Gets updated at billing period reset
uint32_t dlms_cumulative_pgm_count;			// cumulative programming count. Gets updated when Table 31 parameters are programmed

uint8_t dlms_billing_date[12];		//{0,0,0,1,2,255}		// Billing date (3)

uint32_t dlms_cumulative_energy_kwh;			//{1,0,1,8,0,255}		// cumulative energy : kwh (3)
uint32_t dlms_cumulative_energy_kvarh_lag;		//{1,0,5,8,0,255}		// cumulative energy : kvarh lag (3)
uint32_t dlms_cumulative_energy_kvarh_lead;		//{1,0,8,8,0,255}		// cumulative energy : kvarh lead (3)
uint32_t dlms_cumulative_energy_kvah;			//{1,0,9,8,0,255}		// cumulative energy : kVAh (3)

// billing parameters
//uint8_t billing_count;
//uint32_t kvarhlag,kvarhlead;

uint32_t Address1;
//uint8_t Tamper[TAMPER_BUF_SIZE];

uint8_t MD_SlotNumber;
uint8_t MD_SlotsPerDay;

unsigned char MonthArray[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31};

uint8_t dlms_lls_password[MAX_SECRET_LLS_LEN];	// = "ABCDEFGH";			// password for LLS association
uint8_t dlms_hls_password[MAX_SECRET_HLS_LEN];	// = "MXIMdlms654x_pwd";		// password for HLS association

BILLING_PARAM billing_param;

uint8_t TamperCount;	// tamper count of event logs

int32_t LS_RefAddress;
uint8_t LS_SlotsPerDay;                  	/* Load survey 15/30 Minute intervals  */
uint8_t LS_Maxdays;

int32_t DLS_RefAddress;
uint8_t DLS_SlotsPerDay;                  	/* Daily Load survey 15/30 Minute intervals  */
uint8_t DLS_Maxdays;

/* variables used by DLMS stack */

uint8_t billing_count=5;
uint32_t kvarh_lag,kvarh_lead;


// name plate details
const uint8_t dlms_object_logical_device_name_value[] = "SLG_SY7T108G";//MXMZONM1_71314";
const uint8_t dlms_manufacturer_name[] = "SILERGY TECHNOLOGY";
const uint8_t dlms_firmware_version = 1;
//int32_t dlms_meter_id = 7108;
const uint8_t dlms_meter_type = 5;                      // 1P-2W
const uint8_t dlms_meter_category[] = "C3";             // Single Phase Whole Current meter
const uint8_t dlms_meter_current_rating[] = "10-40";	  // Ib-Imax	
uint16_t dlms_manufacture_year = 2023;                                     

// programmable parameters
uint8_t dlms_date_time[12];
uint32_t profile_generic_default_entries=1;      // maximum default entries

uint8_t execution_billing_date[EXECUTION_BILLING_NUMBERS][12];

/*
uint32_t dlms_demand_integration_period;
uint32_t dlms_profile_capture_period=CLASS07_BLOCKLOAD_INTERVAL;		// in seconds (15 or 30 minutes)
uint32_t dlms_profile_dailyload_capture_period;	// in seconds (24 hours)
// date/time the bill to be generated (user configured)
//uint8_t billing_date[12];			// date of billing (single action schedule)



uint32_t profile_generic_default_entries;      // maximum default entries
uint32_t profile_generic_maxevent_entries;     // maximum event entries
uint32_t profile_generic_maxbilling_entries;   // maximum billing entries
uint32_t dlms_profile_max_bl_entries;          // maximum block load entries
uint32_t dlms_profile_max_dl_entries;          // maximum daily load entries

uint8_t ac_changed;	                       // flag to notify if activate_calendar_date/time is loaded with new value

uint8_t dlms_voltage_event_date_time[12];
uint8_t dlms_current_event_date_time[12];
uint8_t dlms_power_event_date_time[12];
uint8_t dlms_transaction_event_date_time[12];
uint8_t dlms_other_event_date_time[12];
uint8_t dlms_nonrollover_event_date_time[12];
uint8_t dlms_event_control_date_time[12];          // connect/disconnect

uint8_t p_season_profile_name[MAX_SEASONS][MAX_CALENDAR_NAME_LEN];
uint8_t p_season_start_time[MAX_SEASONS][12];
uint8_t p_season_week_name[MAX_SEASONS][MAX_CALENDAR_NAME_LEN];
uint8_t season_profile_name[MAX_SEASONS][MAX_CALENDAR_NAME_LEN];
uint8_t season_start_time[MAX_SEASONS][12];
uint8_t season_week_name[MAX_SEASONS][MAX_CALENDAR_NAME_LEN];
uint8_t dlms_dummy_wk[MAX_SEASONS][MAX_CALENDAR_NAME_LEN];
uint8_t dlms_activate_passive_calender_time[12];

uint8_t season_id;                  // profile season number
*/


sBilling_Profile stBilling_Profile;
sLoad_Profile stLoad_Profile;
sTamper_Profile stTamper_Profile;
sSeason_Profile stSeason_Profile[2]; 
sWeek_Profile stWeek_Profile[2]; 
sDay_Profile stDay_Profile[2]; 


sSA_Range SA_Range[2];
uint32_t  range_num_entries, range_start_entry;
