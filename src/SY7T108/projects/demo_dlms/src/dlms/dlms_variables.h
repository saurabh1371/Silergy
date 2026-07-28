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
 * $Id: dlms_variables.h 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/

/********************************************************************************
*
*     Description: 
*        Filename: dlms_variables.h
*
*        Compiler: uVision ARMCC.exe
*
*        Version: uVision 5.27.1
*
*         Created: 				
*							
*   Modifications: 
*         
*******************************************************************************/
#ifndef __DLMS_VARIABLES_H__
#define __DLMS_VARIABLES_H__

#include <stdint.h>
#include "options.h"
#include "eeprom.h"
#include "dlmsif_tampers.h"
#include "Defines.h"

//#define REMOVE_ACTIVITY_CALENDAR_FROM_APP       // comment this line as the case may be

#define MAX_TZ                          5	//<number of TOU supported
#define EXECUTION_BILLING_NUMBERS       12      // number of single action schedules supported

#define MAX_SEASONS                     2
//#define MAX_CALENDAR_NAME_LEN           4

/*
// EEPROM addresses
#define EEPROM_UNUSED_ADDRESS           (0xA02)
#define ADDRESS_INITMETER               EEPROM_UNUSED_ADDRESS

#define EVENTIDS_BASEADDRESS            (ADDRESS_INITMETER + 2)          //0xA04
#define EVENTTYPES_BASEADDRESS          (EVENTIDS_BASEADDRESS + MAX_TAMPER_EVENTS + 1)  // 0xA13
// base address of EEPROM where event logs are stored
#define EVENTLOG_BASE_ADDRESS		         0xA21
#define EVENTLOG_BUF_SIZE		             30    // date/time(12 bytes)+ tamper code(2 bytes)+ 4 parameters(current, voltage, pf, kwh) (4 bytes each) 

#define LOADSURVEY_REF_ADDRESS		       0x1255
#define LOADSURVEY_REFBUF_SIZE           36    // date/time(12 bytes)+ 6 parameters (4 bytes each)

#define LOADSURVEY_BASE_ADDRESS		       (LOADSURVEY_REF_ADDRESS+10+LOADSURVEY_REFBUF_SIZE)
#define LOADSURVEY_BUF_SIZE		           42    // date/time(12 bytes)+ 4 parameters (4 bytes each) + max_dmd (14 bytes) 

#if (MAX_SEASONS==3)
#define ACTCAL_BASEADDRESS		           0xAFFE	// activity calendar base address
#define ACTCAL_WEEKPROFILE_ADDRESS       0xB03E // PASSIVE calendar week profile name address
#define ACTIVE_CAL_DAYID_OFFSET		       0xB04A
#define DAYX_TZ_BASE_ADDRESS		         0xB04C	// calendar day TZ base address

#define PASCAL_BASEADDRESS 		           0xB104	// PASSIVE calender base address
#define PASCAL_WEEKPROFILE_ADDRESS       0xB144 // PASSIVE calendar week profile name address
#define PASSIVE_CAL_DAYID_OFFSET	       0xB150	//  PASSIVE day_id  BASE ADDRESS
#define ACTCAL_ACTTIME_BASEADDRESS       0xB20A	// activate time address	

#define BILLING_PARAM_BASEADDRESS	       0xB216	// Table 29
#define BILLING_PARAM_SIZE		           96		  // size of Table 29
#define BILLING_PARAM_MAX_CYCLES	       12		  // maximum number of billing cycles

#define BILLING_PARAM_CURRENT_ADDRESS 	BILLING_PARAM_BASEADDRESS+((BILLING_PARAM_MAX_CYCLES+1) * BILLING_PARAM_SIZE)

#define BILLING_DATE_BASEADDRESS          0xB710	// 12 bytes (date = 5 bytes, time = 4 bytes, dummy = 3 bytes)

#define INSTANT_PARAM_BASEADDRESS	        0xB7A0	// base address of Instantaneous parameters
#define DLMS_MISC_BASEADDRESS		          0xB7D6	// misc parameters storage address

//#define PASSWORD_LLS_BASEADDRESS	        0xB7DE
//#define PASSWORD_HLS_BASEADDRESS	        0xB7E6

#define OTHER_ADDRESS			                0xB7F6

#define POWERON_DURATION_BASEADDRESS	    OTHER_ADDRESS
#define METERID_BASEADDRESS		            (OTHER_ADDRESS + 16)
#define AC_CHANGED_ADDRESS		            (METERID_BASEADDRESS + 4)
#define EVENT_EXTENDEDTYPE_BASEADDRESS    (AC_CHANGED_ADDRESS + 1)    // Date_time

#define DAILYLOADSURVEY_BASE_ADDRESS	     0xC000
#define DAILYLOADSURVEY_REF_ADDRESS	       0xC2BC


#define DLMS_EXTRA_ADDRESS                 0xC2D2   // time zone
#endif // #if (MAX_SEASONS==3)

#if (MAX_SEASONS==2)
#define ACTCAL_BASEADDRESS		           0x1F2E3 // activity calendar base address
#define ACTCAL_WEEKPROFILE_ADDRESS       0x1F30F // PASSIVE calendar week profile name address
#define ACTIVE_CAL_DAYID_OFFSET		       0x1F317
#define DAYX_TZ_BASE_ADDRESS		         0x1F319 // calendar day TZ base address

#define PASCAL_BASEADDRESS 		           0x1F393 // PASSIVE calender base address
#define PASCAL_WEEKPROFILE_ADDRESS       0x1F38F // PASSIVE calendar week profile name address
#define PASSIVE_CAL_DAYID_OFFSET	       0x1F3C7 //  PASSIVE day_id  BASE ADDRESS
#define ACTCAL_ACTTIME_BASEADDRESS       0x1F443 // activate time address	

#define BILLING_PARAM_BASEADDRESS	       KWMD_LOC	//0x1F44F // Table 29
#define BILLING_PARAM_SIZE		           96		  // size of Table 29
#define BILLING_PARAM_MAX_CYCLES	       12		  // maximum number of billing cycles

#define BILLING_PARAM_CURRENT_ADDRESS 	BILLING_PARAM_BASEADDRESS+((BILLING_PARAM_MAX_CYCLES+1) * BILLING_PARAM_SIZE)

#define BILLING_DATE_BASEADDRESS          0x1F949	// 12 bytes (date = 5 bytes, time = 4 bytes, dummy = 3 bytes)

#define INSTANT_PARAM_BASEADDRESS	        0x1F9D9	// base address of Instantaneous parameters
#define DLMS_MISC_BASEADDRESS		          0x1FA0F	// misc parameters storage address

#define PASSWORD_LLS_BASEADDRESS	        0x1FA17
#define PASSWORD_HLS_BASEADDRESS	        0x1FA1F

#define OTHER_ADDRESS			                0x1FA2F

#define POWERON_DURATION_BASEADDRESS	    OTHER_ADDRESS
#define METERID_BASEADDRESS		            (OTHER_ADDRESS + 16)
#define AC_CHANGED_ADDRESS		            (METERID_BASEADDRESS + 4)
#define EVENT_EXTENDEDTYPE_BASEADDRESS    (AC_CHANGED_ADDRESS + 1)    // Date_time

#define DAILYLOADSURVEY_BASE_ADDRESS	     0x1FB00
#define DAILYLOADSURVEY_REF_ADDRESS	       0x1FFB0


#define DLMS_EXTRA_ADDRESS                 0x1FDD2   // time zone
#else
#error "MAX_SEASONS not defined"
#endif  //#if (MAX_SEASONS==2)

#define DAILYLOADSURVEY_BUF_SIZE	         20       // date/time(12 bytes)+ 2 parameters (4 bytes each)

#define DLMS_DEMAND_INTEGRATION_PERIOD_15		(15*60)   // 15 minutes in seconds
#define DLMS_DEMAND_INTEGRATION_PERIOD_30		(30*60)   // 30 minutes in seconds
#define DLMS_DAYPROFILE_CAPTURE_PERIOD      (24*3600) // 24 hours in seconds

// Maximum Tamper Logs stored
#define MAXTAMPERCOUNT                       70
// number of bytes to store for each Tamper
#define TAMPER_BUF_SIZE                      54

#define MAX_SECRET_LLS_LEN	                 8        //<maximum secret size for HLS LLS layer	

#define MAX_SURVEY_DAYS                      35        //35 (35 for single phase 64 K EEPROM)
*/



#define SEC     ((uint8_t)global.reg.tm.tm_sec)
#define MIN     ((uint8_t)global.reg.tm.tm_min)
#define HOUR    ((uint8_t)global.reg.tm.tm_hour)
#define DAY     ((uint8_t)(global.reg.tm.tm_wday)+1)
#define DATE    ((uint8_t)global.reg.tm.tm_mday)
#define MONTH   ((uint8_t)(global.reg.tm.tm_mon)+1)
#define YEAR    ((uint8_t)global.reg.tm.tm_year)
#define YEAR_100 20

enum eDAY { DLMS_SUN = 1, DLMS_MON, DLMS_TUE, DLMS_WED, DLMS_THU, DLMS_FRI, DLMS_SAT };
enum eMONTH 
  { DLMS_JAN = 1, DLMS_FEB, DLMS_MAR, DLMS_APR, DLMS_MAY, DLMS_JUN, DLMS_JUL, 
  DLMS_AUG, DLMS_SEP, DLMS_OCT, DLMS_NOV, DLMS_DEC };
	
typedef struct {
uint8_t date[12];             //yr100,yr,month,dt,day,hr,mnt,sec,subsec,devh,devl,sts;
uint16_t kw;
}MD_KW;

typedef struct {
uint8_t date[12];             //yr100,yr,month,dt,day,hr,mnt,sec,subsec,devh,devl,sts;
uint16_t kva;
}MD_KVA;

typedef struct {
	uint32_t kwh;         // kwh of timezone
	uint32_t kvah;        // kvah of timezone
}MD_TZ;

typedef struct {
	MD_KW kw;             // Total MD_KW
	MD_TZ md_tz[MAX_TZ];  // MD_KW and MD_KVA of TZ1, TZ2 ... MAX_TZ
                              // kwh, kvah and kvarh of TZ1, TZ2 ...MAX_TZ
	MD_KVA kva;           // Total MD_KVA
}BILLING_PARAM;

extern uint32_t Address1;
extern uint8_t Tamper[];

extern uint8_t MD_SlotNumber;
extern uint8_t MD_SlotsPerDay;

extern unsigned char MonthArray[];

extern volatile uint32_t kwh_wh;
extern volatile uint32_t kVah_vah;

extern uint8_t dlms_lls_password[];	// = "ABCDEFGH";			// password for LLS association
extern uint8_t dlms_hls_password[];	// = "MXIMdlms654x_pwd";		// password for HLS association

extern BILLING_PARAM billing_param;

extern uint8_t TamperCount;	// tamper count of event logs

/* Variables in RAM */
extern int32_t LS_RefAddress;
extern uint8_t LS_SlotsPerDay;                  	/* Load survey 15/30 Minute intervals  */
extern uint8_t LS_Maxdays;

extern int32_t DLS_RefAddress;
extern uint8_t DLS_SlotsPerDay;                  	/* Load survey 15/30 Minute intervals  */
extern uint8_t DLS_Maxdays;
/* variables used by DLMS stack */

extern int16_t dlms_time_deviation;

// instantaneous parameters
extern uint32_t dlms_phase_current,dlms_neutral_current,dlms_measured_current;
extern uint32_t dlms_voltage;
extern int32_t dlms_pf;
extern int32_t dlms_avg_pf;						// average power factor (single phase)
extern uint32_t dlms_hz;
extern uint32_t dlms_apparent_power_kva;
extern int32_t dlms_active_power_kw;
extern int32_t dlms_reactive_power_kvar;
extern int32_t  inst_kw_p,inst_kw_n;

extern uint32_t inst_phase_current,inst_neutral_current,inst_measured_current;
extern uint32_t inst_voltage;
extern int32_t  inst_pf;
extern int32_t  avg_pf;						// average power factor (single phase)
extern uint32_t inst_freq;
extern uint32_t inst_kva;
extern int32_t  inst_kw;
extern int32_t  inst_kvar;

extern uint32_t dlms_num_powerfails;
extern uint32_t dlms_num_powerfail_dur;
extern uint32_t dlms_poweron_dur_minutes;              // total power on duration of meter in minutes 
extern uint32_t dlms_poweron_dur_billing_minutes;      // total power on duration of meter in minutes for billing
extern uint32_t dlms_num_tampercount;
extern uint32_t dlms_cumulative_billing_count;		// cumulative billing counter. Gets updated at billing period reset
extern uint32_t dlms_cumulative_pgm_count;			// cumulative programming count. Gets updated when Table 31 parameters are programmed

extern uint8_t dlms_billing_date[];				//{0,0,0,1,2,255}		// Billing date (3)
extern uint8_t billing_date_1[12];			// date of billing (single action schedule)
extern uint8_t execution_billing_date[][12];

extern uint32_t dlms_cumulative_energy_kwh;				//{1,0,1,8,0,255}		// cumulative energy : kwh (3)
extern uint32_t dlms_cumulative_energy_kvarh_lag;		//{1,0,5,8,0,255}		// cumulative energy : kvarh lag (3)
extern uint32_t dlms_cumulative_energy_kvarh_lead;		//{1,0,8,8,0,255}		// cumulative energy : kvarh lead (3)
extern uint32_t dlms_cumulative_energy_kvah;			//{1,0,9,8,0,255}		// cumulative energy : kVAh (3)

// billing parameters
extern uint8_t billing_count;
extern uint32_t kvarh_lag,kvarh_lead;


// name plate details
extern const uint8_t dlms_object_logical_device_name_value[];

extern const uint8_t dlms_manufacturer_name[];
extern const uint8_t dlms_firmware_version;
//extern int32_t dlms_meter_id;
extern const uint8_t dlms_meter_type;
extern const uint8_t dlms_meter_category[];
extern const uint8_t dlms_meter_current_rating[];
extern uint16_t dlms_manufacture_year;


extern const uint8_t SIZEOF_DLMS_OBJECT_LOGICAL_DEVICE_NAME_VALUE;
// Others
extern uint8_t  dlms_available_billing_period;		// 

// programmable parameters
extern uint8_t dlms_date_time[];
extern uint32_t profile_generic_default_entries;      // maximum default entries

/*
extern uint32_t dlms_demand_integration_period;
extern uint32_t dlms_profile_capture_period;		// in seconds (15 or 30 minutes)
extern uint32_t dlms_profile_dailyload_capture_period;         // in seconds (24 hours)
//extern uint8_t billing_date[];				// date of billing (single action schedule)

extern uint32_t dlms_noauto_capture;			// no auto capture

// events variables
extern uint16_t dlms_event_voltage,dlms_event_current,dlms_event_power;
extern uint16_t dlms_event_transaction,dlms_event_other,dlms_event_non_rollover;
extern uint16_t dlms_event_control;

extern uint8_t dlms_date_time_log[];
extern uint16_t dlms_event_id;
//extern uint32_t dlms_dummy_variable;
//extern uint8_t dlms_dummy_array[];

extern uint8_t dlms_day_id;
extern uint8_t dlms_no_tou;

extern uint8_t tamper_Rtc_Occured;
extern uint8_t tamper_Demand_integration_Occured;
extern uint8_t tamper_Profile_capture_Occured;
extern uint8_t tamper_Singleaction_billing_Occured;
extern uint8_t tamper_Activitycalender_Occured;
								   
// date/time of RTC tamper
extern uint8_t rtc_tamper_date[];

extern uint32_t dlms_userobjectvalue;

extern uint32_t profile_generic_default_entries;      // maximum default entries
extern uint32_t profile_generic_maxevent_entries;     // maximum event entries
extern uint32_t profile_generic_maxbilling_entries;   // maximum billing entries
extern uint32_t dlms_profile_max_bl_entries;          // maximum block load entries
extern uint32_t dlms_profile_max_dl_entries;          // maximum daily load entries

extern uint8_t ac_changed;	                      // flag to notify if activate_calendar_date/time is loaded with new value

extern uint8_t dlms_voltage_event_date_time[];
extern uint8_t dlms_current_event_date_time[];
extern uint8_t dlms_power_event_date_time[];
extern uint8_t dlms_transaction_event_date_time[];
extern uint8_t dlms_other_event_date_time[];
extern uint8_t dlms_nonrollover_event_date_time[];
extern uint8_t dlms_event_control_date_time[];             // connect/disconnect

extern uint8_t p_season_profile_name[][MAX_CALENDAR_NAME_LEN];
extern uint8_t p_season_start_time[][12];
extern uint8_t p_season_week_name[][MAX_CALENDAR_NAME_LEN];
extern uint8_t season_profile_name[][MAX_CALENDAR_NAME_LEN];
extern uint8_t season_start_time[][12];
extern uint8_t season_week_name[][MAX_CALENDAR_NAME_LEN];

extern uint8_t dlms_dummy_wk[][MAX_CALENDAR_NAME_LEN];

extern uint8_t dlms_activate_passive_calender_time[];

extern uint8_t season_id;                  // profile season number
*/



struct datetime_s
{
    uint16_t year;
    uint8_t month;
    uint8_t day_of_the_month;
    uint8_t day_of_the_week;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t centi_seconds;
    uint16_t deviation;
    uint8_t status;
};


typedef struct 
{
  unsigned char Bill_Date[12];
  unsigned long Sys_Power_Factor;
  unsigned long Power_On_Time;
  unsigned long long Cumm_Energy_KWh;
  unsigned long Cumm_Energy_KWh_TZ[8];
  unsigned long long Cumm_Energy_KVarh_Lag;  
  unsigned long long Cumm_Energy_KVarh_Lead;
  unsigned long long Cumm_Energy_KVAh;
  unsigned long Cumm_Energy_KVAh_TZ[8];
  unsigned int MD_KW;
  unsigned char MD_KW_DT[12];
  unsigned long MD_KW_TZ[8];  
  unsigned char MD_KW_TZ_DT[8][12];
  unsigned int MD_KVA;
  unsigned char MD_KVA_DT[12];
  unsigned long MD_KVA_TZ[8];  
  unsigned char MD_KVA_TZ_DT[8][12];
} sBilling_Profile;

typedef struct
{
    uint8_t Load_Date[12];  
    uint32_t Irms; 
    uint32_t IRrms; 
    uint32_t IYrms; 
    uint32_t IBrms; 
    uint32_t Vrms; 
    uint32_t VRrms; 
    uint32_t VYrms; 
    uint32_t VBrms; 
    uint16_t PF; 
    uint16_t PFR; 
    uint16_t PFY; 
    uint16_t PFB; 
    uint32_t E_Active; 
    uint32_t E_Reactive_Lag; 
    uint32_t E_Reactive_Lead; 
    uint32_t E_Apparent; 
    uint16_t tot_kw; 
} sLoad_Profile;


typedef struct
{
    unsigned char Tamper_Date[12];
    uint16_t Tamper_ID; 
    uint32_t Irms; 
    uint32_t IRrms; 
    uint32_t IYrms; 
    uint32_t IBrms; 
    uint32_t Vrms; 
    uint32_t VRrms; 
    uint32_t VYrms; 
    uint32_t VBrms; 
    uint32_t PF; 
    uint32_t PFR; 
    uint32_t PFY; 
    uint32_t PFB; 
    uint32_t E_Active; 
    uint32_t E_Reactive_Lag; 
    uint32_t E_Reactive_Lead; 
    uint32_t E_Apparent; 
    uint32_t tot_kw; 
} sTamper_Profile;


typedef struct
{
    uint8_t Season_Name[10];
    uint8_t Season_Date[12];
} sSeason_Profile;

typedef struct
{
    uint8_t Week_Name[5];
    //uint8_t Week_Id[7];
} sWeek_Profile;

typedef struct
{
    uint8_t day_id;
    uint8_t tod_time[8][4];
    uint16_t tariff_id[8];
} sDay_Profile;

extern sBilling_Profile stBilling_Profile;
extern sLoad_Profile stLoad_Profile;
extern sTamper_Profile stTamper_Profile;
extern sSeason_Profile stSeason_Profile[2]; 
extern sWeek_Profile stWeek_Profile[2]; 
extern sDay_Profile stDay_Profile[2]; 

typedef struct
{
   uint16_t Year;
   uint8_t Month;
   uint8_t Date;
   uint8_t Day;
   uint8_t Hr;
   uint8_t Min;  
} sSA_Range;

extern sSA_Range SA_Range[2];
extern uint32_t  range_num_entries, range_start_entry;


#endif //#ifndef  __DLMS_VARIABLES_H__

