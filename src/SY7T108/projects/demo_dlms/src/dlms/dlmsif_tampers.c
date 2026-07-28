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
 * AUTHOR:  PBH
 * $Id: dlmsif_tampers.c 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/

/********************************************************************************
*
*     Description: 
*        Filename: dlmsif_tampers.c
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

//#include "zon_m1.h"
#include "options.h"
//#include "mapeb11_1.h"
#include "gpio.h"
//#include "afe.h"
#include "dlms_eeprom.h"
#include "dlms_silergy_stubs.h"
#include "dlms_variables.h"
#include "dlmsif_tampers.h"
#include "dlmsif_loadsurvey.h"
#include "dlmsif_dayprofile.h"
#include "dlmsif_tou.h"
#include "dlms_utils.h"
#include "rtc.h"
#include "afe.h"
#include "Defines.h"

/*
// Tamper Snap shot for each type of Tamper  
uint16_t TamperCode;
// voltages
//uint8_t tamper_RPhaseMissing;
//uint8_t tamper_YPhaseMissing;
//uint8_t tamper_BPhaseMissing;
uint8_t tamper_VoltageLow;
//uint8_t tamper_VoltageHigh;
//uint8_t tamper_VoltUnbalance;
// currents
//uint8_t tamper_RPhaseCTReverse;
//uint8_t tamper_YPhaseCTReverse;
//uint8_t tamper_BPhaseCTReverse;
//uint8_t tamper_RPhaseCTOpen;
//uint8_t tamper_YPhaseCTOpen;
//uint8_t tamper_BPhaseCTOpen;
//uint8_t tamper_CurrentUnbalance;
//uint8_t tamper_CTbypass;
//uint8_t tamper_CurrentHigh;
uint8_t tamper_earthloading;
// power
uint8_t tamper_PowerMissing;
// Transaction

// Magnet
uint8_t tamper_Magnet;
uint8_t tamper_Neutral_Unbalance;
//uint8_t tamper_VeryLowPF;
uint8_t tamper_neutral_missing;
// non-rollover
uint8_t tamper_TopCover;

// Tamper Log  
// Persistence/ Restoration Counters and flags  
// voltage events counters
//uint8_t tamper_RPhaseMissing_Counter,tamper_RPhaseMissing_Occured;
//uint8_t tamper_YPhaseMissing_Counter,tamper_YPhaseMissing_Occured;
//uint8_t tamper_BPhaseMissing_Counter,tamper_BPhaseMissing_Occured;
//uint8_t tamper_Highvoltage_Counter,tamper_Highvoltage_Occured;
uint8_t tamper_Lowvoltage_Counter,tamper_Lowvoltage_Occured;
//uint8_t tamper_Voltageunbalance_Counter,tamper_Voltageunbalance_Occured;
// current events counters
//uint8_t tamper_RCT_reverse_Counter,tamper_RCT_reverse_Occured;
//uint8_t tamper_YCT_reverse_Counter,tamper_YCT_reverse_Occured;
//uint8_t tamper_BCT_reverse_Counter,tamper_BCT_reverse_Occured;
//uint8_t tamper_RCT_open_Counter,tamper_RCT_open_Occured;
//uint8_t tamper_YCT_open_Counter,tamper_YCT_open_Occured;
//uint8_t tamper_BCT_open_Counter,tamper_BCT_open_Occured;
//uint8_t tamper_CurrentUnbalance_Counter,tamper_CurrentUnbalance_Occured;
//uint8_t tamper_CT_bypass_Counter,tamper_CT_bypass_Occured;
//uint8_t tamper_Overcurrent_Counter,tamper_Overcurrent_Occured;
uint8_t tamper_earthloading_Counter,tamper_earthloading_Occured;
// power failure events counters
uint8_t tamper_Powerfail_Counter,tamper_Powerfail_Occured;
// transaction related events counter (only occurrence events)
uint8_t tamper_Rtc_Occured,tamper_Rtc_Counter;
uint8_t tamper_Demand_integration_Occured,tamper_Demand_integration_Counter;
uint8_t tamper_Profile_capture_Occured,tamper_Profile_capture_Counter;
uint8_t tamper_Singleaction_billing_Occured,tamper_Singleaction_billing_Counter;
uint8_t tamper_Activitycalender_Occured,tamper_Activitycalender_Counter;
// magenetic events
uint8_t tamper_Mag_Counter,tamper_Mag_Occured;
uint8_t tamper_Neutral_unbalance_Counter,tamper_Neutral_unbalance_Occured;
//uint8_t tamper_Verylowpf_Counter,tamper_Verylowpf_Occured;
uint8_t tamper_neutral_missing_Counter,tamper_neutral_missing_Occured;
// non rollover events
uint8_t tamper_TopCover_Counter,tamper_TopCover_Occured;
// control events - unused events
uint8_t tamper_control_Occured;

void dlmsif_tampers_settamperflags(void);
void dlmsif_tampers_init_tampervariables(void);
*/


/******************************************************************************
*   Function Name : dlmsif_tampers_init_tampervariables
*     Description : initialize tamper variables
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tampers_init_tampervariables(void)
{
//	 tamper_RPhaseMissing=0;
//	 tamper_YPhaseMissing=0;
//	 tamper_BPhaseMissing=0;
	 tamper_VoltageLow=0;
//	 tamper_VoltageHigh=0;
//	 tamper_VoltUnbalance=0;
	// currents
//	 tamper_RPhaseCTReverse=0;
//	 tamper_YPhaseCTReverse=0;
//	 tamper_BPhaseCTReverse=0;
//	 tamper_RPhaseCTOpen=0;
//	 tamper_YPhaseCTOpen=0;
//	 tamper_BPhaseCTOpen=0;
//	 tamper_CurrentUnbalance=0;
//	 tamper_CTbypass = 0;
//	 tamper_CurrentHigh=0;
	 tamper_earthloading=0;
	// power
	 tamper_PowerMissing=0;
	// Transaction

	// Magnet
	 tamper_Magnet=0;
	 tamper_Neutral_Unbalance=0;
//	 tamper_VeryLowPF=0;
	 tamper_neutral_missing=0;
	// non-rollover
	 tamper_TopCover=0;

//	tamper_RPhaseMissing_Counter=0;tamper_RPhaseMissing_Occured=0;
//	tamper_YPhaseMissing_Counter=0;tamper_YPhaseMissing_Occured=0;
//	tamper_BPhaseMissing_Counter=0;tamper_BPhaseMissing_Occured=0;
//	tamper_Highvoltage_Counter=0;tamper_Highvoltage_Occured=0;
	tamper_Lowvoltage_Counter=0;tamper_Lowvoltage_Occured=0;
//	tamper_Voltageunbalance_Counter=0;tamper_Voltageunbalance_Occured=0;
	// current events counters
//	tamper_RCT_reverse_Counter=0;tamper_RCT_reverse_Occured=0;
//	tamper_YCT_reverse_Counter=0;tamper_YCT_reverse_Occured=0;
//	tamper_BCT_reverse_Counter=0;tamper_BCT_reverse_Occured=0;
//	tamper_RCT_open_Counter=0;tamper_RCT_open_Occured=0;
//	tamper_YCT_open_Counter=0;tamper_YCT_open_Occured=0;
//	tamper_BCT_open_Counter=0;tamper_BCT_open_Occured=0;
//	tamper_CurrentUnbalance_Counter=0;tamper_CurrentUnbalance_Occured=0;
//	tamper_CT_bypass_Counter=0;tamper_CT_bypass_Occured=0;
//	tamper_Overcurrent_Counter=0;tamper_Overcurrent_Occured=0;
	tamper_earthloading_Counter=0;tamper_earthloading_Occured=0;
	//power failure events counters
	tamper_PowerMissing = 0;
	tamper_Powerfail_Counter=0;tamper_Powerfail_Occured=0;
	//transaction related events counter(only occurrence events)
	tamper_Rtc_Occured=0;
	tamper_Demand_integration_Occured=0;
	tamper_Profile_capture_Occured=0;
	tamper_Singleaction_billing_Occured=0;
	tamper_Activitycalender_Occured=0;

	tamper_Rtc_Counter=0;
	tamper_Demand_integration_Counter=0;
	tamper_Profile_capture_Counter=0;
	tamper_Singleaction_billing_Counter=0;
	tamper_Activitycalender_Counter = 0;
	//magnetic events
	tamper_Mag_Counter=0;tamper_Mag_Occured=0;
	tamper_Neutral_unbalance_Counter=0;tamper_Neutral_unbalance_Occured=0;
//	tamper_Verylowpf_Counter=0;tamper_Verylowpf_Occured=0;
	tamper_neutral_missing_Counter=0;tamper_neutral_missing_Occured=0;
	// non rollover events
	tamper_TopCover_Counter=0;tamper_TopCover_Occured=0;
        // control events - unused
	tamper_control_Occured = 0;

	// clear tamper count
	TamperCount = 0;

}
*/

/******************************************************************************
*   Function Name : dlmsif_tampers_init
*     Description : clear ALL (MAXTAMPERCOUNT in numbers) event logs. 
*                   Called when meter is newly installed
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tampers_init(void)
{
//	uint8_t i;

//  profile_generic_maxevent_entries = MAXTAMPERCOUNT;

//	memset(Tamper,0x00,TAMPER_BUF_SIZE);
//	// clear EVENTID logs in EEPROM (200 x 2) bytes
//	Address1 = EVENTLOG_BASE_ADDRESS+12;
//	for(i=0;i<MAXTAMPERCOUNT;i++)
//	{
//		Write2eeprom(Tamper,Address1,2);	// init first 2 bytes (event id) of each entry
//		Address1  += EVEN_LOG_BUFSIZE;
//	}

//        memset(Tamper,0xFF,12);                         // initialize date/time = 0xFF
//        Address1 = EVENT_EXTENDEDTYPE_BASEADDRESS;      // Initialise capture_time[] to 0xFF....
//	for(i=0;i<6;i++)
//	{
//		Write2eeprom(Tamper,Address1,12);	// init first 2 bytes (event id) of each entry
//		Address1  += 12;
//	}

//	dlmsif_tampers_init_tampervariables();		// initialise tamper variables on POR.

//	Address1 = EVENTIDS_BASEADDRESS;
//        // Initialise tamper events in EEPROM
//        // 26 events' variables (1 byte each)+ TamperCount (1 bytes) + type of events (2 bytes * 7 types)
//	Write2eeprom(Tamper,Address1,MAX_TAMPER_EVENTS+1+14);		


}
*/

/******************************************************************************
*   Function Name : EventLogsGet
*     Description : Read tamper variables from EEPROM. 
*                   Get the event log history on POR. Reads 26+1 bytes from EEPROM and stores into variables
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tampers_get(void)
{
    uint8_t i = 0;
	dlmsif_tampers_init_tampervariables();								// initialise tamper variables on POR.

  profile_generic_maxevent_entries = MAXTAMPERCOUNT;

	// read from EEPROM
	Readeeprom(Tamper,EVENTIDS_BASEADDRESS,MAX_TAMPER_EVENTS+1);		// 13 events' variables (1 byte each)+ TamperCount (1 bytes)

//	tamper_RPhaseMissing_Occured = Tamper[0];
//	tamper_YPhaseMissing_Occured = Tamper[1];
//	tamper_BPhaseMissing_Occured = Tamper[2];
//	tamper_Highvoltage_Occured = Tamper[3];
	tamper_Lowvoltage_Occured = Tamper[i++];
//	tamper_Voltageunbalance_Occured = Tamper[5];
	// current events counters
//	tamper_RCT_reverse_Occured = Tamper[6];
//	tamper_YCT_reverse_Occured = Tamper[7];
//	tamper_BCT_reverse_Occured = Tamper[8];
//	tamper_RCT_open_Occured = Tamper[9];
//	tamper_YCT_open_Occured = Tamper[10];
//	tamper_BCT_open_Occured = Tamper[11];
//	tamper_CurrentUnbalance_Occured = Tamper[12];
//	tamper_CT_bypass_Occured = Tamper[13];
//	tamper_Overcurrent_Occured = Tamper[14];
    tamper_earthloading_Occured = Tamper[i++];
	// power failure events counters
  	tamper_Powerfail_Occured = Tamper[i++];
	// transaction related events counter (only occurrence). No need to restore.
//	tamper_Rtc_Occured = Tamper[i++];
//	tamper_Demand_integration_Occured = Tamper[i++];
//	tamper_Profile_capture_Occured = Tamper[i++];
//	tamper_Singleaction_billing_Occured = Tamper[i++];
//	tamper_Activitycalender_Occured = Tamper[i++];
	// magenetic events
        i = 7;
	  tamper_Mag_Occured = Tamper[i++];
	  tamper_Neutral_unbalance_Occured = Tamper[i++];
	//tamper_Verylowpf_Occured = Tamper[i++];
    i++;
    tamper_neutral_missing_Occured = Tamper[i++];
	// non rollover events
	  tamper_TopCover_Occured = Tamper[i++];
	// control events - unused
	  tamper_control_Occured = Tamper[i++];

	  TamperCount = Tamper[MAX_TAMPER_EVENTS];
	  TamperCount++;					// point to write pointer.

	Readeeprom((uint8_t *)&dlms_event_voltage,EVENTTYPES_BASEADDRESS,2);
	Readeeprom((uint8_t *)&dlms_event_current,EVENTTYPES_BASEADDRESS+2,2);
	Readeeprom((uint8_t *)&dlms_event_power,EVENTTYPES_BASEADDRESS+4,2);
	Readeeprom((uint8_t *)&dlms_event_transaction,EVENTTYPES_BASEADDRESS+6,2);
	Readeeprom((uint8_t *)&dlms_event_other,EVENTTYPES_BASEADDRESS+8,2);
	Readeeprom((uint8_t *)&dlms_event_non_rollover,EVENTTYPES_BASEADDRESS+10,2);
	Readeeprom((uint8_t *)&dlms_event_control,EVENTTYPES_BASEADDRESS+12,2);

        Address1 = EVENT_EXTENDEDTYPE_BASEADDRESS;      // Address of event id date/time 
	Readeeprom((uint8_t *)&dlms_voltage_event_date_time,Address1,12);              
	Address1  += 12;
	Readeeprom((uint8_t *)&dlms_current_event_date_time,Address1,12);
	Address1  += 12;
	Readeeprom((uint8_t *)&dlms_power_event_date_time,Address1,12);
	Address1  += 12;
	Readeeprom((uint8_t *)&dlms_transaction_event_date_time,Address1,12);
	Address1  += 12;
	Readeeprom((uint8_t *)&dlms_other_event_date_time,Address1,12);
	Address1  += 12;
	Readeeprom((uint8_t *)&dlms_nonrollover_event_date_time,Address1,12);
	Address1  += 12;
	Readeeprom((uint8_t *)&dlms_event_control_date_time,Address1,12);
}
*/

/******************************************************************************
*   Function Name : dlmsif_tampers_eventid_getaddress
*     Description : Get the EEPROM address offset where 'EventID state' and 'tampercounter' is stored
*        Input(s) : type - category of event: 0 = occurrence/restoration event, 
*                                             1 = tamper count
*                                             2 = one of V,I,P,transaction,magnetic,cover,remote events
*       Output(s) : EEPROM address offset
*           Notes : increment 'dlms_num_tampercount' for V,I and magnet events
*
******************************************************************************/
/*
uint16_t dlmsif_tampers_eventid_getaddress(uint8_t type)
{
	uint8_t id_offset;

	switch(type)
	{
	 	case 0:				// tamper events

			if((TamperCode >= MIN_VOLTAGE_EVENTID) && (TamperCode <= MAX_VOLTAGE_EVENTID))
			{	// voltage event ids
				id_offset = (TamperCode-1)/2;
                                if(TamperCode%2)            // BIS amendment 1 - incr count for occurrence
                                  dlms_num_tampercount++;
			}
			else if((TamperCode >= MIN_CURRENT_EVENTID) && (TamperCode <= MAX_CURRENT_EVENTID))
			{	// current event ids
				id_offset = ((TamperCode-MIN_CURRENT_EVENTID)/2) + NUM_VOLTAGE_EVENTS;
                                if((TamperCode-(MIN_CURRENT_EVENTID-1))%2)            // BIS amendment 1 - incr count for occurrence
                                  dlms_num_tampercount++;

			}
			else if((TamperCode >= MIN_POWER_EVENTID) && (TamperCode <= MAX_POWER_EVENTID))
			{	// power event ids
				id_offset = ((TamperCode-MIN_POWER_EVENTID)/2) + NUM_VOLTAGE_EVENTS + NUM_CURRENT_EVENTS;
				//dlms_num_tampercount++;					//tamper count is not to be incremented
			}
			else if((TamperCode >= MIN_TRANSACTION_EVENTID) && (TamperCode <= MAX_TRANSACTION_EVENTID))
			{	// transaction related ids (Only occurance events)
				id_offset = (TamperCode-MIN_TRANSACTION_EVENTID) + NUM_VOLTAGE_EVENTS + NUM_CURRENT_EVENTS +
                                                                                                         NUM_POWER_EVENTS;
				//dlms_num_tampercount++;					//tamper count is not to be incremented
			}
			else if((TamperCode >= MIN_OTHERS_EVENTID) && (TamperCode <= MAX_OTHERS_EVENTID))
			{	// magent event ids
				id_offset = ((TamperCode-MIN_OTHERS_EVENTID)/2) + NUM_VOLTAGE_EVENTS + NUM_CURRENT_EVENTS + 
                                                                                                       NUM_POWER_EVENTS + 
                                                                                                       NUM_TRANSACTION_EVENTS;
                                if((TamperCode-(MIN_OTHERS_EVENTID-1))%2)            // BIS amendment 1 - incr count for occurrence
                                  dlms_num_tampercount++;

			}
			else if(TamperCode == MIN_NONROLLOVER_EVENTID)
			{	// 	cover open (Only occurance)
				id_offset = (TamperCode-MIN_NONROLLOVER_EVENTID) + NUM_VOLTAGE_EVENTS + NUM_CURRENT_EVENTS + 
                                                                                                        NUM_POWER_EVENTS + 
                                                                                                        NUM_TRANSACTION_EVENTS + 
                                                                                                        NUM_OTHERS_EVENTS;
				//dlms_num_tampercount++;					//tamper count is not to be incremented
			}
			else
			{	// remote connect/disconnect (NOT USED)
			 	id_offset = NUM_VOLTAGE_EVENTS + NUM_CURRENT_EVENTS + NUM_POWER_EVENTS + NUM_TRANSACTION_EVENTS + 
                                            NUM_OTHERS_EVENTS + NUM_CONTROL_EVENTS;
				//dlms_num_tampercount++;					//tamper count is not to be incremented
			}
		break;

		case 1:						// tamper count
			id_offset = 1 + NUM_VOLTAGE_EVENTS + NUM_CURRENT_EVENTS + NUM_POWER_EVENTS + NUM_TRANSACTION_EVENTS + NUM_OTHERS_EVENTS + NUM_CONTROL_EVENTS;
			break;

		case 2:					// one of V,I,P,transaction,magnetic,cover,remote events
			if((TamperCode >= MIN_VOLTAGE_EVENTID) && (TamperCode <= MAX_VOLTAGE_EVENTID))
			{	// voltage event ids ; not used for single phase
				id_offset = 0;
				dlms_event_voltage = TamperCode;
                                memcpy(dlms_voltage_event_date_time,Tamper,12);
			}
			else if((TamperCode >= MIN_CURRENT_EVENTID) && (TamperCode <= MAX_CURRENT_EVENTID))
			{	// current event ids
				id_offset = 2;
				dlms_event_current = TamperCode;
                                memcpy(dlms_current_event_date_time,Tamper,12);
			}
			else if((TamperCode >= MIN_POWER_EVENTID) && (TamperCode <= MAX_POWER_EVENTID))
			{	// power event ids
				id_offset = 4;
				dlms_event_power = TamperCode;
                                memcpy(dlms_power_event_date_time,Tamper,12);
			}
			else if((TamperCode >= MIN_TRANSACTION_EVENTID) && (TamperCode <= MAX_TRANSACTION_EVENTID))
			{	// transaction related ids (Only occurance events)
				id_offset = 6;
				dlms_event_transaction = TamperCode;
                                memcpy(dlms_transaction_event_date_time,Tamper,12);
			}
			else if((TamperCode >= MIN_OTHERS_EVENTID) && (TamperCode <= MAX_OTHERS_EVENTID))
			{	// magent event ids
				id_offset = 8;
				dlms_event_other = TamperCode;
                                memcpy(dlms_other_event_date_time,Tamper,12);
			}
			else if(TamperCode == MIN_NONROLLOVER_EVENTID)
			{	// 	cover open (Only occurance)
				id_offset = 10;
				dlms_event_non_rollover = TamperCode;
                                memcpy(dlms_nonrollover_event_date_time,Tamper,12);
			}
			else
			{	// remote connect/disconnect (NOT USED)
			 	id_offset = 12;
				dlms_event_control = TamperCode;
                                memcpy(dlms_event_control_date_time,Tamper,12);
			}
                     break;

		default:	// (case 3) one of V,I,P,transaction,magnetic,cover,remote events for extended register status
			if((TamperCode >= MIN_VOLTAGE_EVENTID) && (TamperCode <= MAX_VOLTAGE_EVENTID))
			{	// voltage event ids ; not used for single phase
				id_offset = 0;		
			}
			else if((TamperCode >= MIN_CURRENT_EVENTID) && (TamperCode <= MAX_CURRENT_EVENTID))
			{	// current event ids
				id_offset = 12;
			}
			else if((TamperCode >= MIN_POWER_EVENTID) && (TamperCode <= MAX_POWER_EVENTID))
			{	// power event ids
				id_offset = 24;
			}
			else if((TamperCode >= MIN_TRANSACTION_EVENTID) && (TamperCode <= MAX_TRANSACTION_EVENTID))
			{	// transaction related ids (Only occurance events)
				id_offset = 36;
			}
			else if((TamperCode >= MIN_OTHERS_EVENTID) && (TamperCode <= MAX_OTHERS_EVENTID))
			{	// magent event ids
				id_offset = 48;
			}
			else if(TamperCode == MIN_NONROLLOVER_EVENTID)
			{	// 	cover open (Only occurance)
				id_offset = 60;
			}
			else
			{	// remote connect/disconnect (NOT USED)
			 	id_offset = 72;
			}
                     break;
	}

	return id_offset;

}
*/

/******************************************************************************
*   Function Name : dlmsif_tampers_eventlogsset
*     Description : log the present tamper event (54 bytes) in EEPROM
*        Input(s) : occurance = 0 for restoration event,   1 for occurrence event
*       Output(s) : None
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tampers_eventlogsset(uint8_t occurance)
{
//        uint16_t offset;
//	if(TamperCode == RTC_CLOCK_TRANSACTION)
//	{
//		// for RTC tamper, we need to log RTC older date/time
//		memcpy(Tamper,rtc_tamper_date,12);
//	}
//	else
//	{
//		Tamper[0] = ((YEAR_100 * 100) + YEAR) >> 8;
//		Tamper[1] = ((YEAR_100 * 100) + YEAR) & 0xFF;	
//		Tamper[2] = MONTH;
//		Tamper[3] = DATE;
//		Tamper[4] = (DAY == 1)? 7 : (DAY-1);	  // DLMS (1=monday), 6543 (1=sunday)

//		Tamper[5] = HOUR;
//		Tamper[6] = MIN;
//		Tamper[7] = SEC;
//		Tamper[8] = 0xFF;//operating_sec/360;

//		Tamper[9] = ((dlms_time_deviation &0xFF00) >> 8);
//		Tamper[10] = (int8_t)dlms_time_deviation;
//		Tamper[11] = 0;
//	}
//        copy_to_buf(12,(void *)&TamperCode,2);

//	//i = 14;
//        copy_to_buf(14,(void *)&dlms_measured_current,4);

//        copy_to_buf(18,(void *)&dlms_voltage,4);

//        copy_to_buf(22,(void *)&dlms_pf,4);

//        copy_to_buf(26,(void *)&dlms_cumulative_energy_kwh,4);

//	Address1 = EVENTLOG_BASE_ADDRESS + (TamperCount * EVEN_LOG_BUFSIZE);
//	Write2eeprom( (uint8_t *)&Tamper[0], Address1, EVEN_LOG_BUFSIZE);            // Write to eeprom (buffer,start address, no of bytes)

//	// write event status of an event in EEPROM for POR restoration
//        offset = dlmsif_tampers_eventid_getaddress(0);
//	Write2eeprom( (uint8_t *)&occurance,EVENTIDS_BASEADDRESS +  offset,1) ;
//	// write 'tampercount' of an event in EEPROM for POR restoration
//	Write2eeprom( (uint8_t *)&TamperCount,EVENTIDS_BASEADDRESS + dlmsif_tampers_eventid_getaddress(1),1) ;
//	// write tamper event type in EEPROM fpr POR restoration
//	Write2eeprom( (uint8_t *)&TamperCode,EVENTTYPES_BASEADDRESS + dlmsif_tampers_eventid_getaddress(2),2) ;	
//        // write tamper event status(occurrance/restoration) and date-time stamp
//        offset = dlmsif_tampers_eventid_getaddress(3);
//	Write2eeprom( (uint8_t *)&Tamper[0],EVENT_EXTENDEDTYPE_BASEADDRESS +  offset, 12) ;  // 12 byte date/time
}
*/

/******************************************************************************
*   Function Name : dlmsif_tampers_incr_tampercount
*     Description : increment the 'tampercount' by 1
*        Input(s) : None
*       Output(s) : None
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tampers_incr_tampercount(void)
{
  if(TamperCount >= MAXTAMPERCOUNT-1)
    TamperCount = 0;
  else
    TamperCount ++;
}
*/

/******************************************************************************
*   Function Name : dlmsif_tampers_checks
*     Description : check if tamper occurred. This is a bug function covering all tampers
*        Input(s) : None
*       Output(s) : None
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tampers_checks(void)
{
    //  Determine what tampers are on   
    dlmsif_tampers_settamperflags();

	// check Low voltage tamper
  if(tamper_VoltageLow)
  {
    if(!tamper_Lowvoltage_Occured)
    {
      if(tamper_Lowvoltage_Counter >= TAMPER_OCCURRANCE_COUNTER)
      {
        tamper_Lowvoltage_Occured = 1;
        tamper_Lowvoltage_Counter = 0;
        TamperCode = LOW_VOLTAGE_OCCURS;
        // Store in EEPROM, Tamper Type and Time   
        dlmsif_tampers_eventlogsset(tamper_Lowvoltage_Occured);
        dlmsif_tampers_incr_tampercount();
      }
      else
      {
        tamper_Lowvoltage_Counter++;
      }
    }
    else
        tamper_Lowvoltage_Counter = 0;
  }
  else
  {
    if ( tamper_Lowvoltage_Occured )
    {
      if ( tamper_Lowvoltage_Counter >= TAMPER_RESTORE_COUNTER )
      {
        tamper_Lowvoltage_Occured = 0;
        tamper_Lowvoltage_Counter = 0;
        TamperCode = LOW_VOLTAGE_RESTORE;
        dlmsif_tampers_eventlogsset(tamper_Lowvoltage_Occured);
		    dlmsif_tampers_incr_tampercount();
      }
      else
      {
        tamper_Lowvoltage_Counter++;
      }
    }
    else
    {
        tamper_Lowvoltage_Counter = 0;
    }
  }	
	if(tamper_earthloading)
	{
		if(!tamper_earthloading_Occured)
		{
			if(tamper_earthloading_Counter >= TAMPER_OCCURRANCE_COUNTER)
			{
				tamper_earthloading_Occured = 1;
				tamper_earthloading_Counter = 0;
				TamperCode = EARTHLOADING_OCCURS;
				//Address1 = EVENTLOG_BASE_ADDRESS + (TamperCount * TAMPER_BUF_SIZE);
                // Store in EEPROM, Tamper Type and Time   
        dlmsif_tampers_eventlogsset(tamper_earthloading_Occured);
				dlmsif_tampers_incr_tampercount();
			}
            else
            {
                tamper_earthloading_Counter++;
            }
		}
		else
			tamper_earthloading_Counter = 0;
	}
    else
    {
        if ( tamper_earthloading_Occured )
        {
            if ( tamper_earthloading_Counter >= TAMPER_RESTORE_COUNTER )
            {
                tamper_earthloading_Occured = 0;
                tamper_earthloading_Counter = 0;
                TamperCode = EARTHLOADING_RESTORE;
                //Address1 = EVENTLOG_BASE_ADDRESS + (TamperCount * TAMPER_BUF_SIZE);
                dlmsif_tampers_eventlogsset(tamper_earthloading_Occured);
				dlmsif_tampers_incr_tampercount();
            }
            else
            {
                tamper_earthloading_Counter++;
            }
        }
        else
        {
            tamper_earthloading_Counter = 0;
        }
    }

    if ( tamper_PowerMissing )
    {
        if ( tamper_Powerfail_Occured )
        {
            tamper_Powerfail_Counter = 0;
        }
        else
        {
            if ( tamper_Powerfail_Counter >= TAMPER_OCCURRANCE_COUNTER )
            {
                tamper_Powerfail_Occured = 1;
                tamper_Powerfail_Counter = 0;
                TamperCode = POWER_FAILURE_OCCURS;
                // Store in EEPROM, Tamper Type and Time   
                dlmsif_tampers_eventlogsset(tamper_Powerfail_Occured);		// no parameters are captured. BIS-G-1.3 - 7 ??
		dlmsif_tampers_incr_tampercount();
            }
            else
            {
                tamper_Powerfail_Counter++;
            }
        }
    }
    else
    {
        if ( tamper_Powerfail_Occured )
        {
            if ( tamper_Powerfail_Counter >= 1  ) //TAMPER_RESTORE_COUNTER
            {
                tamper_Powerfail_Occured = 0;
                tamper_Powerfail_Counter = 0;
                TamperCode = POWER_FAILURE_RESTORE;
                // Store in EEPROM, Tamper Type and Time   
                dlmsif_tampers_eventlogsset(tamper_Powerfail_Occured);			// no parameters are captured. BIS-G-1.3 - 7 ??
								dlmsif_tampers_incr_tampercount();
            }
            else
            {
                tamper_Powerfail_Counter++;
            }
        }
        else
        {
            tamper_Powerfail_Counter = 0;
        }
    }

	if(tamper_Rtc_Occured)
	{
            if(tamper_Rtc_Counter++ >= TAMPER_TRANSACTION_COUNTER)
            {
              TamperCode = RTC_CLOCK_TRANSACTION;
              dlmsif_tampers_eventlogsset(tamper_Rtc_Occured);
              dlmsif_tampers_incr_tampercount();
              // Initialise the loadsurvey data when the 'profile_capture_period' value changes 
              if(dlms_profile_capture_period == 1800)
                      LS_SlotsPerDay = 48;
              else
                      LS_SlotsPerDay = 96;

             // dlmsif_loadsurvey_init();
            //  dlmsif_dayloadsurvey_init();
              // Initialise current billing profile 
            //  dlmsif_tou_init_billing_profile();

              tamper_Rtc_Occured = 0;
              tamper_Rtc_Counter = 0;
              dlms_cumulative_pgm_count++;		// increment the programming count;
            }
	}
	if(tamper_Demand_integration_Occured)
	{
		if ( tamper_Demand_integration_Counter++ >= TAMPER_TRANSACTION_COUNTER )
		{
                  TamperCode = DEMAND_INTEGRATION_TRANSACTION;
                  // Store in EEPROM, Tamper Type and Time   
                  dlmsif_tampers_eventlogsset(tamper_Demand_integration_Occured);
                  dlmsif_tampers_incr_tampercount();
                  // Initialise current billing profile 
                  //dlmsif_tou_init_billing_profile();

                  tamper_Demand_integration_Occured = 0;
                  tamper_Demand_integration_Counter = 0;
                  dlms_cumulative_pgm_count++;		// increment the programming count;
		}
	}
	if(tamper_Profile_capture_Occured)
	{
		if ( tamper_Profile_capture_Counter++ >= TAMPER_TRANSACTION_COUNTER )
		{
                  TamperCode = PROFILE_CAPTURE_TRANSACTION;
                  // Store in EEPROM, Tamper Type and Time   
                  dlmsif_tampers_eventlogsset(tamper_Profile_capture_Occured);
                  dlmsif_tampers_incr_tampercount();

                  // Initialise the loadsurvey data when the 'profile_capture_period' value changes 
                  if(dlms_profile_capture_period == 1800)
                          LS_SlotsPerDay = 48;
                  else
                          LS_SlotsPerDay = 96;

                  dlmsif_loadsurvey_init();
                  dlmsif_dayloadsurvey_init();

                  tamper_Profile_capture_Occured = 0;
                  tamper_Profile_capture_Counter = 0;
                  dlms_cumulative_pgm_count++;		// increment the programming count;
		}
	}
	if(tamper_Singleaction_billing_Occured)
	{
		if ( tamper_Singleaction_billing_Counter++ >= TAMPER_TRANSACTION_COUNTER )
		{
                  TamperCode = SINGLE_ACTION_SCH_TRANSACTION;
                  // Store in EEPROM, Tamper Type and Time   
                  dlmsif_tampers_eventlogsset(tamper_Singleaction_billing_Occured);

                  dlmsif_tampers_incr_tampercount();

                  tamper_Singleaction_billing_Occured = 0;
                  tamper_Singleaction_billing_Counter = 0;
                  dlms_cumulative_pgm_count++;		// increment the programming count;
		}
	}
	if(tamper_Activitycalender_Occured)
	{

		if ( tamper_Activitycalender_Counter++ >= TAMPER_TRANSACTION_COUNTER )
		{
			// occurs when active calendar is updated
			TamperCode = ACTIVITY_CALENDAR_TRANSACTION;
			// Store in EEPROM, Tamper Type and Time   
			dlmsif_tampers_eventlogsset(tamper_Activitycalender_Occured);
			//	1. Generate new bill
			//dlmsif_tou_generate_bill(tamper_Activitycalender_Occured);
			//	2. Activate the calendar
			//dlmsif_tou_activate_calendar();
			// Initialise current billing profile 
			//dlmsif_tou_init_billing_profile();

			dlmsif_tampers_incr_tampercount();

			tamper_Activitycalender_Occured = 0;
			tamper_Activitycalender_Counter = 0;
			dlms_cumulative_pgm_count++;		// increment the programming count;
		}
	}

    //  Check for a magnetic  Tamper
    if ( tamper_Magnet )
    {
        if ( tamper_Mag_Occured )
        {
            tamper_Mag_Counter = 0;
        }
        else
        {
             if ( tamper_Mag_Counter >= TAMPER_OCCURRANCE_COUNTER )
            {
                 tamper_Mag_Occured = 1;
                 tamper_Mag_Counter = 0;
                 TamperCode = EVENT_MAG_OCCURS;
                // Store in EEPROM, Tamper Type and Time   
                dlmsif_tampers_eventlogsset(tamper_Mag_Occured);

		dlmsif_tampers_incr_tampercount();
            }
            else
            {
                tamper_Mag_Counter++;
            }
        }
    }
    else
    {
        if ( tamper_Mag_Occured )
        {
            if ( tamper_Mag_Counter >= TAMPER_RESTORE_COUNTER )
            {
                tamper_Mag_Occured = 0;
                tamper_Mag_Counter = 0;
                TamperCode = EVENT_MAG_RESTORE;
                // Store in EEPROM, Tamper Type and Time   
                dlmsif_tampers_eventlogsset(tamper_Mag_Occured);

		dlmsif_tampers_incr_tampercount();
            }
            else
            {
                tamper_Mag_Counter++;
            }
        }
        else
        {
            tamper_Mag_Counter = 0;
        }
    }

	// check Neutral Unbalance tamper
	if(tamper_Neutral_Unbalance)
	{
		if(!tamper_Neutral_unbalance_Occured)
		{
			if(tamper_Neutral_unbalance_Counter >= TAMPER_OCCURRANCE_COUNTER)
			{
				tamper_Neutral_unbalance_Occured = 1;
				tamper_Neutral_unbalance_Counter = 0;
				TamperCode = EVENT_MAG_NEUTRAL_D_OCCURS;
                                // Store in EEPROM, Tamper Type and Time   
                                dlmsif_tampers_eventlogsset(tamper_Neutral_unbalance_Occured);

				dlmsif_tampers_incr_tampercount();
			}
                        else
                        {
                            tamper_Neutral_unbalance_Counter++;
                        }
		}
		else
			tamper_Neutral_unbalance_Counter = 0;
	}
    else
    {
        if ( tamper_Neutral_unbalance_Occured )
        {
            if ( tamper_Neutral_unbalance_Counter >= TAMPER_RESTORE_COUNTER )
            {
                tamper_Neutral_unbalance_Occured = 0;
                tamper_Neutral_unbalance_Counter = 0;
                TamperCode = EVENT_MAG_NEUTRAL_D_RESTORE;
                dlmsif_tampers_eventlogsset(tamper_Neutral_unbalance_Occured);
		dlmsif_tampers_incr_tampercount();
            }
            else
            {
                tamper_Neutral_unbalance_Counter++;
            }
        }
        else
        {
            tamper_Neutral_unbalance_Counter = 0;
        }
    }

	if(tamper_neutral_missing)
	{
		if(!tamper_neutral_missing_Occured)
		{
			if(tamper_neutral_missing_Counter >= TAMPER_OCCURRANCE_COUNTER)
			{
				tamper_neutral_missing_Occured = 1;
				tamper_neutral_missing_Counter = 0;
				TamperCode = EVENT_MAG_NEUTRALMISIING_OCCURS;
				//Address1 = EVENTLOG_BASE_ADDRESS + (TamperCount * TAMPER_BUF_SIZE);
                // Store in EEPROM, Tamper Type and Time   
                                dlmsif_tampers_eventlogsset(tamper_neutral_missing_Occured);
		                dlmsif_tampers_incr_tampercount();
			}
                        else
                        {
                                tamper_neutral_missing_Counter++;
                        }
		}
		else
			tamper_neutral_missing_Counter = 0;
       }
       else
       {
        if ( tamper_neutral_missing_Occured )
        {
            if ( tamper_neutral_missing_Counter >= TAMPER_RESTORE_COUNTER )
            {
                tamper_neutral_missing_Occured = 0;
                tamper_neutral_missing_Counter = 0;
                TamperCode = EVENT_MAG_NEUTRALMISIING_RESTORE;
                //Address1 = EVENTLOG_BASE_ADDRESS + (TamperCount * TAMPER_BUF_SIZE);
                dlmsif_tampers_eventlogsset(tamper_neutral_missing_Occured);
		dlmsif_tampers_incr_tampercount();
            }
            else
            {
                tamper_neutral_missing_Counter++;
            }
        }
        else
        {
            tamper_neutral_missing_Counter = 0;
        }
       }
	// check very low PF tamper

	// Check meter cover open: ONLY occurrence is logged (refer to BIS)
    if ( tamper_TopCover )
    {
        if ( tamper_TopCover_Occured )
        {
            tamper_TopCover_Counter = 0;
        }
        else
        {
            if ( tamper_TopCover_Counter >= TAMPER_OCCURRANCE_COUNTER )
            {
                tamper_TopCover_Occured = 1;
                tamper_TopCover_Counter = 0;
                TamperCode = EVENT_COVER_OPEN_OCCURS;
                // Store in EEPROM, Tamper Type and Time   
                dlmsif_tampers_eventlogsset(tamper_TopCover_Occured);
		dlmsif_tampers_incr_tampercount();
            }
            else
            {
                tamper_TopCover_Counter++;
            }
        }
    }
    // Tamper cover Open restoration is not logged.
    else
    {
        if ( tamper_TopCover_Occured )
        {
            if ( tamper_TopCover_Counter >= TAMPER_RESTORE_COUNTER )
            {
                tamper_TopCover_Occured = 0;
                tamper_TopCover_Counter = 0;
            }
            else
            {
                tamper_TopCover_Counter++;
            }
        }
        else
        {
            tamper_TopCover_Counter = 0;
        }
    }


}
*/

/******************************************************************************
*   Function Name : dlmsif_tampers_settamperflags
*     Description : Detect tamper(s) 
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tampers_settamperflags(void)
{
  //uint32_t xx;
  //uint32_t Ivect;     // Vectorial Sum of Ir + Iy + Ib + In

  //Ivect = labs((long)(10.0 * afe_arms(PHASE_A))); //fabs(1000.0 * afe_get(AFE_IR));     // 

 //Voltage tampers - ONLY Low_VOLTAGE tamper
	// Low voltage
    if (((dlms_voltage/10) >= VOLTAGE_60VOLTS) && ((dlms_voltage/10) <= VOLTAGE_110VOLTS))
    {
        tamper_VoltageLow = 1;
    }
    else
    {
        tamper_VoltageLow = 0;
    }

 //Current tampers
	  if( ((float)dlms_phase_current > (1.1 * (float)dlms_neutral_current)) ||
			  ((float)dlms_neutral_current > (1.1 * (float)dlms_phase_current)) )
			{
					tamper_earthloading = 1;
			}
			else
			{
					tamper_earthloading = 0;
			}

// Power tampers
    if ( ((dlms_voltage/10) < VOLTAGE_60VOLTS) && 
         ((dlms_phase_current/1000) < VOLTAGE_60VOLTS) && ((dlms_neutral_current/1000) < VOLTAGE_60VOLTS) )
    {
        tamper_PowerMissing = 1;
    }
    else
    {
        tamper_PowerMissing = 0;
    }

// Others (Magnetic/Neutral disturbance/Low PF) tampers
    // Nuetral disturbance
    if(dlms_voltage/10 >= VOLTAGE_300VOLTS)
    {
        tamper_Neutral_Unbalance = 1;
    }
    else
    {
        tamper_Neutral_Unbalance = 0;
    }

    // neutral missing
    if ((dlms_voltage/10) < VOLTAGE_60VOLTS)
    {
        tamper_neutral_missing = 1;
    }
    else
    {
        tamper_neutral_missing = 0;
    }


// Cover Open tampers

    //  Top Cover open, power present 
    if(gpio_get_state(PCB_CASE_TAMPER_SEG))
		//if ( PI1  &   BIT1 )
    {
        tamper_TopCover = 0;
        //ClearIcon(ok);
    }
    else
    {
        tamper_TopCover = 1;
        //DisplayIcon(ok);
    }

}
*/

/******************************************************************************
*   Function Name : dlmsif_tampers_set
*     Description : Called on battery mode entry. Saves status in EEPROM 
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tampers_set(void)
{
    // log power fail tamper
    tamper_Powerfail_Occured = 1;
    tamper_Powerfail_Counter = 0;
    TamperCode = POWER_FAILURE_OCCURS;
    dlmsif_tampers_eventlogsset(tamper_Powerfail_Occured);
    TamperCount++;
}
*/
