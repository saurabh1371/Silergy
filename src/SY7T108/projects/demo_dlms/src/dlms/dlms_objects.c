/******************************************************************************* 
 * Copyright (C) 2018 Silergy Corp, All rights Reserved.
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
 ********************************************************************************/ 

/********************************************************************************
*
*     Description: 
*        Filename: dlms_objects.c
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


#include "dlms_silergy_stubs.h"
#include "dlms_objects.h"
#include "stddef.h"
#include "dlms_variables.h"

#include "Defines.h"
#include "asdaq_variables.h"

#define CLASS_VERSION_0       0
#define CLASS_VERSION_1       1

#define CURRENT_SCALAR		    -2
#define VOLT_SCALAR			      -1
#define PF_IND_SCALAR		      -2
#define PF_TOTAL_SCALAR		    -2
#define FREQ_SCALAR           -1
#define KVA_SCALAR			      0
#define KVAH_SCALAR			      0
#define KW_SCALAR			        0
#define KVAR_SCALAR			      0
#define KWH_SCALAR			      0
#define KVARH_LAG_SCALAR	    0
#define KVARH_LEAD_SCALAR	    0
#define CLOCK_SCALAR		      0

#define POWERFAIL_DURATION_SCALAR		0
#define POWERON_DURATION_SCALAR			0
#define BILLING_DATE_SCALAR					0

// data class objects (1)
#define DLMS_OBJECT_LOGICAL_DEVICE_NAME_STR		  {0,0,42,0,0,255}	// holds logical device name
#define DLMS_OBJECT_DEVICE_ID				            {0,0,96,1,0,255}	// holds Device ID/meter serial number

// instantaneous params

#define DLMS_OBJECT_PHASE_CURRENT			          {1,0,11,7,0,255}	// phase current (single phase)
#define DLMS_OBJECT_NEUTRAL_CURRENT			        {1,0,91,7,0,255}	// Neutral current (single phase)
#define DLMS_OBJECT_MEASURED_CURRENT			      {1,0,94,91,14,255}	// Measured current (single phase)

#define DLMS_OBJECT_VOLTAGE				              {1,0,12,7,0,255}	// phase voltage (single phase)

//#define DLMS_OBJECT_PFR				{1,0,33,7,0,255}	// Power Factor R (3)
//#define DLMS_OBJECT_PFY				{1,0,53,7,0,255}	// Power Factor Y (3)
//#define DLMS_OBJECT_PFB				{1,0,73,7,0,255}	// Power Factor B (3)
#define DLMS_OBJECT_PF					                {1,0,13,7,0,255}	// Power Factor (3)

#define DLMS_OBJECT_FREQUENCY				            {1,0,14,7,0,255}	// frequency (3)

#define DLMS_OBJECT_APPARENT_POWER_KVA			    {1,0,9,7,0,255}		// apparent power - KVA (3)
#define DLMS_OBJECT_SIGNED_ACTIVE_POWER_KW		  {1,0,1,7,0,255}		// signed active power - KW (3)
#define DLMS_OBJECT_SIGNED_REACTIVE_POWER_KVAR  {1,0,3,7,0,255}		// signed re-active power - KVAR (3)

#define DLMS_OBJECT_NUM_POWERFAILS			        {0,0,96,7,0,255}	// number of power failures (1)
#define DLMS_OBJECT_NUM_POWERFAIL_DUR			      {0,0,94,91,8,255}	// cumulative power failure duration (3)
#define DLMS_OBJECT_TOTAL_POWERON_DUR			      {0,0,94,91,14,255}	// Total power on duration (3)
#define DLMS_OBJECT_TOTAL_POWERON_DUR_BILLING		{0,0,94,91,13,255}	// Total power on duration for billing (3)


#define DLMS_OBJECT_CUMULATIVE_TAMPERCOUNT		   {0,0,94,91,0,255}	// cumulative tamper count (1)
#define DLMS_OBJECT_CUMULATIVE_BILLING_COUNT     {0,0,0,1,0,255}		// billing period counter (1)
#define DLMS_OBJECT_CUMULATIVE_PROGRAMMINGCOUNT  {0,0,96,2,0,255}	// cumulative programming count (1)

#define DLMS_OBJECT_BILLING_DATE			           {0,0,0,1,2,255}		// Billing date (3)
#define DLMS_OBJECT_CUMULATIVE_ENERGY_KWH		     {1,0,1,8,0,255}		// cumulative energy : kwh (3)
#define DLMS_OBJECT_CUMULATIVE_ENERGY_KVARH_LAG	 {1,0,5,8,0,255}		// cumulative energy : kvarh lag (3)
#define DLMS_OBJECT_CUMULATIVE_ENERGY_KVARH_LEAD {1,0,8,8,0,255}		// cumulative energy : kvarh lead (3)
#define DLMS_OBJECT_CUMULATIVE_ENERGY_KVAH		   {1,0,9,8,0,255}		// cumulative energy : kVAh (3)
// block load profile params

//#define DLMS_OBJECT_CLOCK				               {0,0,1,0,0,255}		// clock object (8)
#define DLMS_OBJECT_LOADSRVY_VOLTAGE			       {1,0,12,27,0,255}	// LOADSRVY Average voltage (single phase)
#define DLMS_OBJECT_LOADSRVY_ENERGY_KWH			     {1,0,1,29,0,255}	// LOADSRVY kwh (3)
#define DLMS_OBJECT_LOADSRVY_ENERGY_KVAH		     {1,0,9,29,0,255}	// LOADSRVY kvah (3)
#define DLMS_OBJECT_LOADSRVY_CURRENT_IR			     {1,0,11,27,0,255}	// LOADSRVY Average voltage (single phase)

// SAP assignment object (17)
#define DLMS_OBJECT_SAP_ASSIGNMENT			         {0,0,41,0,0,255}	// holds SAP ASSIGNMENT


// HDLC SETUP OBJECT
#define DLMS_OBJECT_IEC_HDLC_SETUP			        {0,0,22,0,0,255}

// Profile generic
#define DLMS_OBJECT_PROFILE_INSTANT_SNAPSHOT		{1,0,94,91,0,255}	//profile generic snap shot of instantaneous params
#define DLMS_OBJECT_PROFILE_INSTANT_SCALAR		  {1,0,94,91,3,255}	//profile generic scalar

#define DLMS_OBJECT_PROFILE_DAILYLOAD			      {1,0,99,2,0,255}	// daily load profile
#define DLMS_OBJECT_PROFILE_DAILYLOAD_SCALAR		{1,0,94,91,5,255}	// daily load profile scalar
		

#define DLMS_OBJECT_PROFILE_BLOCKLOAD			      {1,0,99,1,0,255}	//profile block load
#define DLMS_OBJECT_PROFILE_BLOCKLOAD_SCALAR		{1,0,94,91,4,255}	//profile block load scalar

#define DLMS_OBJECT_PROFILE_VOLTAGE_EVENTS		  {0,0,99,98,0,255}	// (7)
#define DLMS_OBJECT_PROFILE_CURRENT_EVENTS		  {0,0,99,98,1,255}	// (7)
#define DLMS_OBJECT_PROFILE_POWER_EVENTS		    {0,0,99,98,2,255}	// (7)
#define DLMS_OBJECT_PROFILE_TRANSACTION_EVENTS	{0,0,99,98,3,255}	// (7)
#define DLMS_OBJECT_PROFILE_OTHER_EVENTS		    {0,0,99,98,4,255}	// (7)
#define DLMS_OBJECT_PROFILE_NON_ROLLOVER_EVENTS	{0,0,99,98,5,255}	// (7)

//#define DLMS_OBJECT_PROFILE_CONTROL_EVENTS		{0,0,99,98,6,255}	// (7) NOT SUPPORTED

#define DLMS_OBJECT_PROFILE_EVENTS_SCALAR		    {1,0,94,91,7,255}	// (7)

#define DLMS_OBJECT_PROFILE_BILLING_PARAMS		  {1,0,98,1,0,255}	// (7)
#define DLMS_OBJECT_PROFILE_BILLING_SCALAR		  {1,0,94,91,6,255}	// (7)

#define DLMS_OBJECT_PROFILE_NAMEPLATE			      {0,0,94,91,10,255}	// (7)


// associations
#define DLMS_OBJECT_CURRENT_ASSOCIATION_LN		  {0,0,40,0,0,255}	// CURRENT ASSOCIATION LN
#define DLMS_OBJECT_PC_ASSOCIATION_LN			      {0,0,40,0,1,255}	// Public Client ASSOCIATION LN
#define DLMS_OBJECT_MR_ASSOCIATION_LN			      {0,0,40,0,2,255}	// Meter Reader ASSOCIATION LN
#define DLMS_OBJECT_US_ASSOCIATION_LN			      {0,0,40,0,3,255}	// Utility Station ASSOCIATION LN
// events
#define DLMS_OBJECT_VOLTAGE_EVENT			          {0,0,96,11,0,255}	// (1)
#define DLMS_OBJECT_CURRENT_EVENT			          {0,0,96,11,1,255}	// (1)
#define DLMS_OBJECT_POWER_EVENT				          {0,0,96,11,2,255}	// (1)
#define DLMS_OBJECT_TRANSACTION_EVENT			      {0,0,96,11,3,255}	// (1)
#define DLMS_OBJECT_OTHER_EVENT				          {0,0,96,11,4,255}	// (1)
#define DLMS_OBJECT_NON_ROLLOVER_EVENT			    {0,0,96,11,5,255}	// (1)

//#define DLMS_OBJECT_CONTROL_EVENT			        {0,0,96,11,6,255}	// (1)	UN SUPPORTED OBJECT



// Name plate details (Table 30)
#define DLMS_OBJECT_METER_SERIAL_NUM			      DLMS_OBJECT_DEVICE_ID	// (1) same as "device id"
#define DLMS_OBJECT_MANUFACTURER_NAME			      {0,0,96,1,1,255}	// (1)
#define DLMS_OBJECT_FIRMWARE_VERSION			      {1,0,0,2,0,255}		// (1)
#define DLMS_OBJECT_METER_TYPE				          {0,0,94,91,9,255}	// (1)
// CT and PT ratios Not for single phase meters
//#define DLMS_OBJECT_CT_RATIO				          {1,0,0,4,2,255}		// (1)
//#define DLMS_OBJECT_PT_RATIO				          {1,0,0,4,3,255}		// (1)
#define DLMS_OBJECT_CATEGORY				            {0,0,94,91,11,255}	// (7)
#define DLMS_OBJECT_CURRENT_RATING			        {0,0,94,91,12,255}	// (7)
#define DLMS_OBJECT_MANUFACTURE_YEAR			      {0,0,96,1,4,255}	// (1)


// Programmable parameters
#define DLMS_OBJECT_CLOCK				                {0,0,1,0,0,255}		// clock object(8)
#define DLMS_OBJECT_DEMAND_INTEGRATION_PERIOD   {1,0,0,8,0,255}		// (1)
#define DLMS_OBJECT_PROFILE_CAPTUREPERIOD		    {1,0,0,8,4,255}		//Profile Capture Period (1) 
#define DLMS_OBJECT_DAYPROFILE_CAPTUREPERIOD    {1,0,0,8,5,255}		// profile capture period for daily load
#define DLMS_OBJECT_USEROBJECT				          {1,0,0,8,6,255}
#define DLMS_OBJECT_DIAGNOSTICS 			          {1,0,96,5,0,255}		// test user object
#define DLMS_OBJECT_ENDOF_BILLING_PERIOD		    {0,0,15,0,0,255}	// (22)
#define DLMS_OBJECT_ACTIVITY_CALENDAR			      {0,0,13,0,0,255}	// (20)

#define DLMS_OBJECT_AVAILABLE_BILLING_PERIOD	  {0,0,0,1,1,255}		// (1)
#define DLMS_OBJECT_ENDOF_BILLING_SCRIPT_TABLE	{0,0,10,0,1,255}	// (9)


//------------------------------------------------------------------------------- 
//billing period profile parameters
#define DLMS_OBJECT_BILLING_PF					  {1,0,13,0,0,255}	// (3)
#define DLMS_OBJECT_BILLING_KWH					  DLMS_OBJECT_CUMULATIVE_ENERGY_KWH	// (3)
#define DLMS_OBJECT_BILLING_KWH_TZ1				{1,0,1,8,1,255}	// (3)
#define DLMS_OBJECT_BILLING_KWH_TZ2				{1,0,1,8,2,255}	// (3)
#define DLMS_OBJECT_BILLING_KWH_TZ3				{1,0,1,8,3,255}	// (3)
#define DLMS_OBJECT_BILLING_KWH_TZ4				{1,0,1,8,4,255}	// (3)
#define DLMS_OBJECT_BILLING_KWH_TZ5				{1,0,1,8,5,255}	// (3)
#define DLMS_OBJECT_BILLING_KWH_TZ6				{1,0,1,8,6,255}	// (3)
#define DLMS_OBJECT_BILLING_KWH_TZ7				{1,0,1,8,7,255}	// (3)
#define DLMS_OBJECT_BILLING_KWH_TZ8				{1,0,1,8,8,255}	// (3)
#define DLMS_OBJECT_BILLING_KVARH_LAG			DLMS_OBJECT_CUMULATIVE_ENERGY_KVARH_LAG	// (3)
#define DLMS_OBJECT_BILLING_KVARH_LEAD		DLMS_OBJECT_CUMULATIVE_ENERGY_KVARH_LEAD	// (3)

#define DLMS_OBJECT_BILLING_KVAH				  DLMS_OBJECT_CUMULATIVE_ENERGY_KVAH	// (3)
#define DLMS_OBJECT_BILLING_KVAH_TZ1			{1,0,9,8,1,255}	// (3)
#define DLMS_OBJECT_BILLING_KVAH_TZ2			{1,0,9,8,2,255}	// (3)
#define DLMS_OBJECT_BILLING_KVAH_TZ3			{1,0,9,8,3,255}	// (3)
#define DLMS_OBJECT_BILLING_KVAH_TZ4			{1,0,9,8,4,255}	// (3)
#define DLMS_OBJECT_BILLING_KVAH_TZ5			{1,0,9,8,5,255}	// (3)
#define DLMS_OBJECT_BILLING_KVAH_TZ6			{1,0,9,8,6,255}	// (3)
#define DLMS_OBJECT_BILLING_KVAH_TZ7			{1,0,9,8,7,255}	// (3)
#define DLMS_OBJECT_BILLING_KVAH_TZ8			{1,0,9,8,8,255}	// (3)

#define DLMS_OBJECT_BILLING_MD_KW				  {1,0,1,6,0,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KW_TZ1			{1,0,1,6,1,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KW_TZ2			{1,0,1,6,2,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KW_TZ3			{1,0,1,6,3,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KW_TZ4			{1,0,1,6,4,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KW_TZ5			{1,0,1,6,5,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KW_TZ6			{1,0,1,6,6,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KW_TZ7			{1,0,1,6,7,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KW_TZ8			{1,0,1,6,8,255}		// (4)

#define DLMS_OBJECT_BILLING_MD_KVA				{1,0,9,6,0,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KVA_TZ1		{1,0,9,6,1,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KVA_TZ2		{1,0,9,6,2,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KVA_TZ3		{1,0,9,6,3,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KVA_TZ4		{1,0,9,6,4,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KVA_TZ5		{1,0,9,6,5,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KVA_TZ6		{1,0,9,6,6,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KVA_TZ7		{1,0,9,6,7,255}		// (4)
#define DLMS_OBJECT_BILLING_MD_KVA_TZ8		{1,0,9,6,8,255}		// (4)
//-----------------------------------------------------------------------------

#define DAYLIGHT_SAVING_BEGIN 		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x80,0x00,0x00} // no daylight savings
#define DAYLIGHT_SAVING_END				{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x80,0x00,0x00}	// no daylight savings
#define DAYLIGHT_DEVIATION 				0
#define TIME_DEVIATION 					  330		//time zone: deviation to UTC in minutes (5 h, 30 minutes)			


uint8_t hls_ctos[16];						          // ctos random number
uint8_t hls_stoc[] = "cpriplain____txt";	// stoc random number
const uint8_t sizeof_hls_stoc = sizeof(hls_stoc)/sizeof(hls_stoc[0]);

int16_t dlms_time_deviation = TIME_DEVIATION;

// Others
uint8_t  dlms_available_billing_period;

// programmable parameters
//uint8_t dlms_date_time[12];
//uint32_t dlms_demand_integration_period;
//uint32_t dlms_profile_capture_period;						// in seconds (15 or 30 minutes)
//uint32_t dlms_profile_dailyload_capture_period = 3600*24;	// in seconds (24 hour static)
//uint8_t billing_date[12];									// date of billing (single action schedule)

uint32_t dlms_noauto_capture = 0;							// no auto capture

// events variables
uint16_t dlms_event_voltage=0,dlms_event_current=0,dlms_event_power=0;
uint16_t dlms_event_transaction=0,dlms_event_other=0,dlms_event_non_rollover=0;
uint16_t dlms_event_control=0;

uint8_t dlms_date_time_log[DATE_TIME_LEN];
uint16_t dlms_event_id;

uint8_t dlms_dummy_byte;
uint16_t dlms_dummy_int;
uint32_t dlms_dummy_long;
uint8_t dlms_dummy_array[12];

//uint8_t dlms_day_id=1;
//uint8_t dlms_no_tou=2;

uint8_t rtc_tamper_date[DATE_TIME_LEN];

uint32_t dlms_userobjectvalue;
uint32_t dlms_userobject_1value;

uint8_t ext_register_sts = 1;

int8_t by_entry = 2;
int8_t by_range = 1;

const  DATA_CLASS data_class[] = {
	{	// logical device name	// id = 0
		DLMS_OBJECT_LOGICAL_DEVICE_NAME_STR,//{0,0,42,0,0,255},
		{CHOICE_VSTR,12,(void *)&dlms_object_logical_device_name_value[0]}
	},
	{	// Device ID		// id = 1
		DLMS_OBJECT_DEVICE_ID,				//{0,0,96,1,0,255},
		{CHOICE_UDL,4,(void *)&meter_no}//dlms_meter_id
	},
// 'dlms_num_tampercount' incremented for every captured event except for tables 34 (power), 35 (transaction), 37(non-rollover) and 38(control)
	{	// cumulative tamper count
		DLMS_OBJECT_CUMULATIVE_TAMPERCOUNT,	// id = 2
		{CHOICE_UDL,4,(void *)&all_tamper_cnt} //dlms_num_tampercount	
	},
	{	// cumulative billing period count	// id = 3
		DLMS_OBJECT_CUMULATIVE_BILLING_COUNT,
		{CHOICE_UDL,4,(void *)&md_reset_cnt} //dlms_cumulative_billing_count
	},
	{	// cumulative programming count		// id = 4
		DLMS_OBJECT_CUMULATIVE_PROGRAMMINGCOUNT,
		{CHOICE_UDL,4,(void *)&Cum_Prog_Count} //dlms_cumulative_pgm_count
	},
	{	// Profile capture period
		DLMS_OBJECT_PROFILE_CAPTUREPERIOD,	// id = 5
		{CHOICE_UDL,4,(void *)&Load_Profile_Capture_Period}
	},
	{	// Daily load Profile capture period
		DLMS_OBJECT_DAYPROFILE_CAPTUREPERIOD,	// id = 6
		{CHOICE_UDL,4,(void *)&Daily_Load_Profile_Capture_Period}
	},
		// NAME PLATE DETAILS
	{						// id = 7
		DLMS_OBJECT_MANUFACTURER_NAME,
		{CHOICE_VSTR,18,(void *)&dlms_manufacturer_name[0]}
	},
	{	
		DLMS_OBJECT_FIRMWARE_VERSION,		// id = 8
		{CHOICE_UINT,1,(void *)&dlms_firmware_version}
	},
	{	
		DLMS_OBJECT_METER_TYPE,			// id = 9
		{CHOICE_UINT,1,(void *)&dlms_meter_type}
	},
	{						// id = 10
		DLMS_OBJECT_CATEGORY,
		{CHOICE_VSTR,2,(void *)&dlms_meter_category}
	},
	{						// id = 11
		DLMS_OBJECT_CURRENT_RATING,
		{CHOICE_VSTR,5,(void *)&dlms_meter_current_rating}					   //
	},
	{						// id = 12
		DLMS_OBJECT_MANUFACTURE_YEAR,
		{CHOICE_UL,2,(void *)&dlms_manufacture_year}
	},
	{	// available billing period count	// id = 13
		DLMS_OBJECT_AVAILABLE_BILLING_PERIOD,
		{CHOICE_UINT,1,(void *)&Billing_Profile_Entries_In_Use} //dlms_available_billing_period
	},
	{						// id = 14
	 	DLMS_OBJECT_DEMAND_INTEGRATION_PERIOD,
		{CHOICE_UDL,4,(void *)&Max_Demand_Capture_Period} //dlms_demand_integration_period
	},
	// Add user objects here....
	{						// id = 15
	 	DLMS_OBJECT_USEROBJECT,
		{CHOICE_UDL,4,(void *)&dlms_userobjectvalue}
	},
	{						// id = 16
	 	DLMS_OBJECT_DIAGNOSTICS,
		{CHOICE_UDL,4,(void *)&dlms_userobject_1value}
	}
};
const   uint16_t SIZEOF_DATA_CLASS = sizeof(data_class)/sizeof(data_class[0]);

const   REGISTER_CLASS register_class[] = {
	{
		DLMS_OBJECT_PHASE_CURRENT,		// Nominal current 0
		{CHOICE_UDL,4,(void *)&inst_phase_current},
		{CURRENT_SCALAR,UNIT_AMPERE}						// -2, AMP
	},
	{
		DLMS_OBJECT_NEUTRAL_CURRENT,		// Nominal current 0
		{CHOICE_UDL,4,(void *)&inst_neutral_current},
		{CURRENT_SCALAR,UNIT_AMPERE}						// -2, AMP
	},
	{
		DLMS_OBJECT_VOLTAGE,			// Nominal voltage 0
		{CHOICE_UDL,4,(void *)&inst_voltage},
		{VOLT_SCALAR,UNIT_VOLT}						// -2, volt
	},
	{
		DLMS_OBJECT_PF,		// PF
		{CHOICE_DL,4,(void *)&inst_pf},
		{PF_TOTAL_SCALAR,UNIT_COUNT}					// scale, no units
	},
	{
		DLMS_OBJECT_FREQUENCY,				// frequency
		{CHOICE_UDL,4,(void *)&inst_freq},
		{FREQ_SCALAR,UNIT_HERTZ}						// 10^-1, HZ
	},
	{
		DLMS_OBJECT_APPARENT_POWER_KVA,			// Apparent power, kVA
		{CHOICE_UDL,4,(void *)&inst_kva},
		{KVA_SCALAR,UNIT_VOLT_AMP}					// scale, va
	},
	{
		DLMS_OBJECT_SIGNED_ACTIVE_POWER_KW,		// signed active power, kW
		{CHOICE_DL,4,(void *)&inst_kw},
		{KW_SCALAR,UNIT_WATT}					// scale, w
	},
/*	{
		DLMS_OBJECT_SIGNED_REACTIVE_POWER_KVAR,		// signed reactive power, kvar
		{CHOICE_DL,4,(void *)&dlms_reactive_power_kvar},
		{KVAR_SCALAR,UNIT_VAR}					// scale, var
	},
*/	{
		DLMS_OBJECT_TOTAL_POWERON_DUR,		// cumulative power failure duration
		{CHOICE_UDL,4,(void *)&Cum_Power_On_Dur}, ////dlms_poweron_dur_minutes
		{POWERFAIL_DURATION_SCALAR,UNIT_MINUTE}					// scale, sec
	},	
	{
		DLMS_OBJECT_BILLING_DATE,		// 
		{CHOICE_OCTSTR,12,(void *)&dlms_billing_date[0]},
		{BILLING_DATE_SCALAR,UNIT_COUNT}					// scale, count
	},
	{
		DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,		// apparent energy
		{CHOICE_UDL,4,(void *)&load_val[0]},
		{KWH_SCALAR,UNIT_WATT_HOUR}					// scale, wh
	},
/*	{
		DLMS_OBJECT_CUMULATIVE_ENERGY_KVARH_LAG,		// apparent energy
		{CHOICE_UDL,4,(void *)&dlms_cumulative_energy_kvarh_lag},
		{KVARH_LAG_SCALAR,UNIT_VAR_HOUR}					// scale, varh
	},
	{
		DLMS_OBJECT_CUMULATIVE_ENERGY_KVARH_LEAD,		// apparent energy
		{CHOICE_UDL,4,(void *)&dlms_cumulative_energy_kvarh_lead},
		{KVARH_LEAD_SCALAR,UNIT_VAR_HOUR}					// scale, varh
	},
*/	{
		DLMS_OBJECT_CUMULATIVE_ENERGY_KVAH,		// apparent energy
		{CHOICE_UDL,4,(void *)&load_val[1]},
		{KVAH_SCALAR,UNIT_VOLT_AMP_HOUR}					// scale, varh
	},

/*
	{
		DLMS_OBJECT_TOTAL_POWERON_DUR_BILLING,		// cumulative power failure duration billing
		{CHOICE_UDL,4,(void *)&dlms_poweron_dur_billing_minutes},
		{POWERFAIL_DURATION_SCALAR,UNIT_MINUTE}					// scale, sec
	},
*/
	
};

const   uint16_t SIZEOF_REGISTER_CLASS = sizeof(register_class)/sizeof(register_class[0]);

// Extended Register class
const   EXTENDED_REGISTER extended_register_class[] = {
	{
		DLMS_OBJECT_BILLING_MD_KW,								// MD KW
		{CHOICE_UL,2,(void *)&kwmd_val},				// MD,kw, value
		{KW_SCALAR,UNIT_WATT},									// scalar for MD KW
		{CHOICE_UINT, 1, (void *)&ext_register_sts},				// status
		(uint8_t *)&KWMD_DT[0]				// MD,kw, date
	},
	{
		DLMS_OBJECT_BILLING_MD_KVA,								// MD KVA
		{CHOICE_UL,2,(void *)&kvamd_val},			// MD,kva, value
		{KVA_SCALAR,UNIT_VOLT_AMP},								// scalar for KD KVA
		{CHOICE_UINT, 1, (void *)&ext_register_sts},				// status
		(uint8_t *)&KVAMD_DT[0]					// MD,kva, date
	},
	{			
		DLMS_OBJECT_CURRENT_EVENT,
		{CHOICE_UL,2,(void *)&Last_Tamper_Id[AMP_EVENT]},
		{CLOCK_SCALAR,UNIT_COUNT},			
		{CHOICE_UINT, 1, (void *)&ext_register_sts},	
		(uint8_t *)&Last_Tamper_Date_Time[AMP_EVENT][0]
	},
	{			
		DLMS_OBJECT_POWER_EVENT,
		{CHOICE_UL,2,(void *)&Last_Tamper_Id[PFAIL_EVENT]},
		{CLOCK_SCALAR,UNIT_COUNT},			
		{CHOICE_UINT, 1, (void *)&ext_register_sts},	
		(uint8_t *)&Last_Tamper_Date_Time[PFAIL_EVENT][0]
	},
	{			
		DLMS_OBJECT_TRANSACTION_EVENT,
		{CHOICE_UL,2,(void *)&Last_Tamper_Id[TRANSACT_EVENT]},
		{CLOCK_SCALAR,UNIT_COUNT},			
		{CHOICE_UINT, 1, (void *)&ext_register_sts},	
		(uint8_t *)&Last_Tamper_Date_Time[TRANSACT_EVENT][0]
	},
	{			
		DLMS_OBJECT_OTHER_EVENT,
		{CHOICE_UL,2,(void *)&Last_Tamper_Id[OTHER_EVENT]},
		{CLOCK_SCALAR,UNIT_COUNT},			
		{CHOICE_UINT, 1, (void *)&ext_register_sts},	
		(uint8_t *)&Last_Tamper_Date_Time[OTHER_EVENT][0]
	},
	{			
		DLMS_OBJECT_NON_ROLLOVER_EVENT,
		{CHOICE_UL,2,(void *)&Last_Tamper_Id[NOROLL_EVENT]},
		{CLOCK_SCALAR,UNIT_COUNT},			
		{CHOICE_UINT, 1, (void *)&ext_register_sts},	
		(uint8_t *)&Last_Tamper_Date_Time[NOROLL_EVENT][0]
	},
};

const   uint16_t SIZEOF_EXTENDED_REGISTER_CLASS = sizeof(extended_register_class)/sizeof(extended_register_class[0]);


const   CLOCK_CLASS clock_class[] = {
	{
		DLMS_OBJECT_CLOCK,				// CLOCK OBJECT
		(uint8_t *)&time_string[0], //dlms_date_time[0],				// date_time
		(uint16_t *)&dlms_time_deviation,			// time zone: deviation to UTC in minutes (5 h, 30 minutes)
		0,								// status : no daylight savings
		DAYLIGHT_SAVING_BEGIN,			// daylight savings begins
		DAYLIGHT_SAVING_END,			// daylight savings begins
		DAYLIGHT_DEVIATION,				// deviation fromdaylight in minutes
		0,								// daylight saving disabled
		1								// time base from 'internal crystal'
	}
};
const   uint16_t SIZEOF_CLOCK_CLASS = sizeof(clock_class)/sizeof(clock_class[0]);

const ASSOCIATED_PARTNERS_TYPE associated_partners_type[] = {
	{
		0x10,						// client_SAP
		0x01						// server_SAP;
	},
	{
		0x20,						// client_SAP
		0x01						// server_SAP;
	},
	{
		0x30,						// client_SAP
		0x01						// server_SAP;
	}
};

const XDLMS_CONTEXT_TYPE xdlms_context_type = {
	&conformance[0],
	DLMS_INFOFIELD_RX_SIZE,
	DLMS_INFOFIELD_TX_SIZE,
	DLMS_VERSION_NUMBER,
	0x0,						// not used
	{CHOICE_OCTSTR,0,(void *)&dedicated_key[0]}	// dedicated key not present when NO_CIPHER
}; 

const   IEC_HDLC_SETUP iec_hdlc_setup_class[] = {
	{
			DLMS_OBJECT_IEC_HDLC_SETUP,	
			BAUD_9600,							// baud
			DLMS_WINDOWS_TX_SIZE,				// windows_tx_size
			DLMS_WINDOWS_RX_SIZE,  				// windows_rx_size
			DLMS_INFOFIELD_TX_SIZE,				// max_info_field_len_tx
			DLMS_INFOFIELD_RX_SIZE,				// max_info_field_len_rx
			DLMS_INTEROCTET_TIME,				// inter_octet_timeout (in milliseconds)
			DLMS_INACTIVITY_TIME,				// inactivity_timeout (in seconds)
			DLMS_PHYDEV_ADDR					// dev_addr
	}
};
const   uint16_t SIZEOF_HDLCSETUP_CLASS = sizeof(iec_hdlc_setup_class)/sizeof(iec_hdlc_setup_class[0]);

const   ASSLIST_ELEMENT asslist_element = {
0x01,
{CHOICE_OCTSTR,12,(void *)&dlms_object_logical_device_name_value[0]}
};

// SAP ASSIGNMENT CLASS
const   SAP_ASSIGNMENT sap_assignment_class[] = {
	{
			DLMS_OBJECT_SAP_ASSIGNMENT,
			1,
			(ASSLIST_ELEMENT *)&asslist_element
	}
};
const   uint16_t SIZEOF_SAP_ASSIGNMENT = sizeof(sap_assignment_class)/sizeof(sap_assignment_class[0]);

const   CHOICE_VAL profile_generic_instant_params_buffer[] = {
	{	CHOICE_OCTSTR,12,(void *)&time_string[0]}, //dlms_date_time[0]	
	{	CHOICE_UDL,4,(void *)&inst_voltage	},
	{	CHOICE_UDL,4,(void *)&inst_phase_current	},
	{	CHOICE_UDL,4,(void *)&inst_neutral_current	},
	{	CHOICE_DL,4,(void *)&inst_pf	},
	{	CHOICE_UDL,4,(void *)&inst_freq	},
	{	CHOICE_UDL,4,(void *)&inst_kva},
	{	CHOICE_DL,4,(void *)&inst_kw	},
	{	CHOICE_UDL,4,(void *)&load_val[0]	},
	{	CHOICE_UDL,4,(void *)&load_val[1]	},
	{	CHOICE_UL,2,(void *)&kwmd_val	},			   //MD,kw, value
	{	CHOICE_OCTSTR,12,(void *)&KWMD_DT[0]},	   //MD,kw, date
	{	CHOICE_UL,2,(void *)&kvamd_val	},			 //MD,kVA
	{	CHOICE_OCTSTR,12,(void *)&KVAMD_DT[0]},	 //MD,kVA, date
	{	CHOICE_UDL,4,(void *)&Cum_Power_On_Dur}, //dlms_poweron_dur_minutes
	{	CHOICE_UDL,4,(void *)&all_tamper_cnt},   //dlms_num_tampercount	
	{	CHOICE_UDL,4,(void *)&md_reset_cnt},     //dlms_cumulative_billing_count	
	{	CHOICE_UDL,4,(void *)&Cum_Prog_Count}    //dlms_cumulative_pgm_count	
	
};
#define SIZEOF_PROFILE_GENERIC_INSTANT_PARAMS_BUFFER  (sizeof(profile_generic_instant_params_buffer)/sizeof(profile_generic_instant_params_buffer[0]))

const   CAPTURE_OBJECT profile_generic_instant_params_objs[] = {
	{	E_CLASS_CLOCK,DLMS_OBJECT_CLOCK,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_VOLTAGE,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PHASE_CURRENT,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_NEUTRAL_CURRENT,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PF,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_FREQUENCY,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_APPARENT_POWER_KVA,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_SIGNED_ACTIVE_POWER_KW,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KVAH,2,0	},
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,2,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,5,0	},		// attribute 5
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA,2,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA,5,0	},		// attribute 5
	{	E_CLASS_REGISTER,DLMS_OBJECT_TOTAL_POWERON_DUR,2,0	},
	{	E_CLASS_DATA,DLMS_OBJECT_CUMULATIVE_TAMPERCOUNT,2,0	},
	{	E_CLASS_DATA,DLMS_OBJECT_CUMULATIVE_BILLING_COUNT,2,0	},
	{	E_CLASS_DATA,DLMS_OBJECT_CUMULATIVE_PROGRAMMINGCOUNT,2,0	}

};

#define SIZEOF_PROFILE_GENERIC_INSTANT_PARAMS_OBJS  (sizeof(profile_generic_instant_params_objs)/sizeof(profile_generic_instant_params_objs[0]))

/*
typedef struct
{
    unsigned char Tamper_Date[12];
    uint16_t Tamper_ID; 
    uint32_t Irms; 
    uint32_t IRrms; 
    uint32_t IYrms; 
    uint32_t IBrms; 
    uint16_t Vrms; 
    uint16_t VRrms; 
    uint16_t VYrms; 
    uint16_t VBrms; 
    uint16_t PF; 
    uint16_t PFR; 
    uint16_t PFY; 
    uint16_t PFB; 
    uint32_t E_Active; 
    uint32_t E_Reactive_Lag; 
    uint32_t E_Reactive_Lead; 
    uint32_t E_Apparent; 
    uint32_t tot_kw; 
} sTamper_Profile;
*/

// This structure is only a place holder (for all event logs). This buffer is actually present in user area viz EEPROM/FLASH
// EEPROM @eeprom_base_address_voltage_event_log.
// The server fetches the data from EEPROM and uses this data structure template 
// to encode the values
// NOTE: DO NOT USE THE EMBEDDED ADDRESSES OF THIS STRUCTURE (They are DUMMYs)
const   CHOICE_VAL profile_generic_voltage_event_buffer[] = {
	{	CHOICE_OCTSTR,12,(void *)&stTamper_Profile.Tamper_Date[0]},
	{	CHOICE_UL, 2,(void *)&stTamper_Profile.Tamper_ID},
	{	CHOICE_UDL,4,(void *)&stTamper_Profile.Irms	},	//im (measured current) log
	{	CHOICE_UDL,4,(void *)&stTamper_Profile.Vrms	},	//vr log
	{	CHOICE_DL,4,(void *)&stTamper_Profile.PF	},	//pf log
	{	CHOICE_UDL,4,(void *)&stTamper_Profile.E_Active	}	//kwh log
};
// This structure is only a place holder (for all event logs). This buffer is actually present in user area viz EEPROM/FLASH
// EEPROM @eeprom_base_address_voltage_event_log.
// The server fetches the data from EEPROM and uses this data structure template 
// to encode the values
// NOTE: DO NOT USE THE EMBEDDED ADDRESSES OF THIS STRUCTURE (They are DUMMYs)
const   CHOICE_VAL profile_generic_power_event_buffer[] = {
	{	CHOICE_OCTSTR,12,(void *)&stTamper_Profile.Tamper_Date[0]},
	{	CHOICE_UL, 2,(void *)&stTamper_Profile.Tamper_ID}
/*	{	CHOICE_UDL,4,(void *)&dlms_dummy_variable	},	//im (measured current) log
	{	CHOICE_UDL,4,(void *)&dlms_dummy_variable	},	//vr log
	{	CHOICE_UDL,4,(void *)&dlms_dummy_variable	},	//pf log
	{	CHOICE_UDL,4,(void *)&dlms_dummy_variable	}	//kwh log
*/
};

/*

typedef struct
{
    unsigned char Load_Date[12];  
    uint32_t Irms; 
    uint32_t IRrms; 
    uint32_t IYrms; 
    uint32_t IBrms; 
    uint16_t Vrms; 
    uint16_t VRrms; 
    uint16_t VYrms; 
    uint16_t VBrms; 
    uint16_t PF; 
    uint16_t PFR; 
    uint16_t PFY; 
    uint16_t PFB; 
    uint32_t E_Active; 
    uint32_t E_Reactive_Lag; 
    uint32_t E_Reactive_Lead; 
    uint32_t E_Apparent; 
    uint32_t tot_kw; 
} sLoad_Profile;

*/
// This structure is only a place holder (for all event logs). This buffer is actually present in user area viz EEPROM/FLASH
// EEPROM @eeprom_base_address_voltage_event_log.
// The server fetches the data from EEPROM and uses this data structure template 
// to encode the values
// NOTE: DO NOT USE THE EMBEDDED ADDRESSES OF THIS STRUCTURE (They are DUMMYs)
const   CHOICE_VAL profile_generic_blockload_buffer[] = {
	{	CHOICE_OCTSTR,12,(void *)&stLoad_Profile.Load_Date[0]},
	{	CHOICE_UDL,4,(void *)&stLoad_Profile.Vrms	},	//average voltage (single phase)
	{	CHOICE_UDL,4,(void *)&stLoad_Profile.E_Active	},		//kwh log
	{	CHOICE_UDL,4,(void *)&stLoad_Profile.E_Apparent	},		//KVAh log
	{	CHOICE_UDL,4,(void *)&stLoad_Profile.Irms	},	//average current (single phase)	
	{	CHOICE_UL,2,(void *)&stLoad_Profile.tot_kw },	//MD,kw, value
	{	CHOICE_OCTSTR,12,(void *)&stLoad_Profile.Load_Date[0] }	//MD,kw, date	
	
};
// This structure is only a place holder (for all event logs). This buffer is actually present in user area viz EEPROM/FLASH
// EEPROM @eeprom_base_address_voltage_event_log.
// The server fetches the data from EEPROM and uses this data structure template 
// to encode the values
// NOTE: DO NOT USE THE EMBEDDED ADDRESSES OF THIS STRUCTURE (They are DUMMYs)

/*
typedef struct 
{
  unsigned char Bill_Date[12];
  unsigned long Sys_Power_Factor;
  unsigned int Power_On_Time;
  unsigned long long Cumm_Energy_KWh;
  unsigned long Cumm_Energy_KWh_TZ[8];
  unsigned long long Cumm_Energy_KVarh_Lag;  
  unsigned long long Cumm_Energy_KVarh_Lead;
  unsigned long long Cumm_Energy_KVAh;
  unsigned long Cumm_Energy_KVAh_TZ[8];
  unsigned long MD_KW;
  unsigned char MD_KW_DT[12];
  unsigned long MD_KW_TZ[8];  
  unsigned char MD_KW_TZ_DT[8][12];
  unsigned long MD_KVA;
  unsigned char MD_KVA_DT[12];
  unsigned long MD_KVA_TZ[8];  
  unsigned char MD_KVA_TZ_DT[8][12];
} sBilling_Profile;
*/

const CHOICE_VAL profile_generic_billing_buffer[] = {
	{	CHOICE_OCTSTR,12,(void *)&stBilling_Profile.Bill_Date[0]},		// billing date //dlms_date_time[0]
	{	CHOICE_DL,4,(void *)&stBilling_Profile.Sys_Power_Factor	},				// average PF for single phase
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KWh	},	//cumulative energy, kwh
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KWh_TZ[0]	},	//cumulative energy, kwhtz1
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KWh_TZ[1]	},	//cumulative energy, kwhtz2
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KWh_TZ[2]	},	//cumulative energy, kwhtz3
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KWh_TZ[3]	},	//cumulative energy, kwhtz4
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KWh_TZ[4]	},	//cumulative energy, kwhtz5
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[5].kwh	},	//cumulative energy, kwhtz6
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[6].kwh	},	//cumulative energy, kwhtz7
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[7].kwh	},	//cumulative energy, kwhtz8
//	{	CHOICE_UDL,4,(void *)&kvarhlag	},	//cumulative energy, kvarh (lag)
//	{	CHOICE_UDL,4,(void *)&kvarhlead	},	//cumulative energy, kvarh (lead)
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KVAh	},	//cumulative energy, kvah
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KVAh_TZ[0] },	//cumulative energy, kvahtz0
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KVAh_TZ[1] },	//cumulative energy, kvahtz1
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KVAh_TZ[2] },	//cumulative energy, kvahtz2
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KVAh_TZ[3] },	//cumulative energy, kvahtz3
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Cumm_Energy_KVAh_TZ[4] },	//cumulative energy, kvahtz4
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[5].kvah	},	//cumulative energy, kvahtz5
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[6].kvah	},	//cumulative energy, kvahtz6
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[7].kvah	},	//cumulative energy, kvahtz7
	{	CHOICE_UL,2,(void *)&stBilling_Profile.MD_KW	},	//MD,kw, value
	{	CHOICE_OCTSTR,12,(void *)&stBilling_Profile.MD_KW_DT[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[0].md_kw.kw	},	//MD,kwtz1
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[0].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[1].md_kw.kw	},	//MD,kwtz2
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[1].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[2].md_kw.kw	},	//MD,kwtz3
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[2].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[3].md_kw.kw	},	//MD,kwtz4
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[3].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[4].md_kw.kw	},	//MD,kwtz5
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[4].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[5].md_kw.kw	},	//MD,kwtz6
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[5].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[6].md_kw.kw	},	//MD,kwtz7
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[6].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[7].md_kw.kw	},	//MD,kwtz8
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[7].md_kw.date[0]	},	//MD,kw, date
	{	CHOICE_UL,2,(void *)&stBilling_Profile.MD_KVA	},	//MD,kVA
	{	CHOICE_OCTSTR,12,(void *)&stBilling_Profile.MD_KVA_DT[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[0].md_kva.kva	},	//MD,kVAtz1
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[0].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[1].md_kva.kva	},	//MD,kVAtz2
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[1].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[2].md_kva.kva	},	//MD,kVAtz3
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[2].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[3].md_kva.kva	},	//MD,kVAtz4
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[3].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[4].md_kva.kva	},	//MD,kVAtz5
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[4].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[5].md_kva.kva	},	//MD,kVAtz6
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[5].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[6].md_kva.kva	},	//MD,kVAtz7
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[6].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[7].md_kva.kva	},	//MD,kVAtz8
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[7].md_kva.date[0]	},	//MD,kva, date
	{	CHOICE_UDL,4,(void *)&stBilling_Profile.Power_On_Time	}				// total power on duration for billing
};

/*
const   CHOICE_VAL profile_generic_billing_buffer[] = {
	{	CHOICE_OCTSTR,12,(void *)&time_string[0]},		// billing date //dlms_date_time[0]
	{	CHOICE_DL,4,(void *)&dlms_avg_pf	},				// average PF for single phase
	{	CHOICE_UDL,4,(void *)&dlms_cumulative_energy_kwh	},	//cumulative energy, kwh
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[0].kwh	},	//cumulative energy, kwhtz1
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[1].kwh	},	//cumulative energy, kwhtz2
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[2].kwh	},	//cumulative energy, kwhtz3
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[3].kwh	},	//cumulative energy, kwhtz4
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[4].kwh	},	//cumulative energy, kwhtz5
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[5].kwh	},	//cumulative energy, kwhtz6
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[6].kwh	},	//cumulative energy, kwhtz7
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[7].kwh	},	//cumulative energy, kwhtz8
//	{	CHOICE_UDL,4,(void *)&kvarhlag	},	//cumulative energy, kvarh (lag)
//	{	CHOICE_UDL,4,(void *)&kvarhlead	},	//cumulative energy, kvarh (lead)
	{	CHOICE_UDL,4,(void *)&dlms_cumulative_energy_kvah	},	//cumulative energy, kvah
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[0].kvah	},	//cumulative energy, kvahtz0
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[1].kvah	},	//cumulative energy, kvahtz1
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[2].kvah	},	//cumulative energy, kvahtz2
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[3].kvah	},	//cumulative energy, kvahtz3
	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[4].kvah	},	//cumulative energy, kvahtz4
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[5].kvah	},	//cumulative energy, kvahtz5
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[6].kvah	},	//cumulative energy, kvahtz6
//	{	CHOICE_UDL,4,(void *)&billing_param.md_tz[7].kvah	},	//cumulative energy, kvahtz7
	{	CHOICE_UL,2,(void *)&billing_param.kw.kw	},	//MD,kw, value
	{	CHOICE_OCTSTR,12,(void *)&billing_param.kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[0].md_kw.kw	},	//MD,kwtz1
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[0].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[1].md_kw.kw	},	//MD,kwtz2
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[1].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[2].md_kw.kw	},	//MD,kwtz3
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[2].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[3].md_kw.kw	},	//MD,kwtz4
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[3].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[4].md_kw.kw	},	//MD,kwtz5
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[4].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[5].md_kw.kw	},	//MD,kwtz6
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[5].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[6].md_kw.kw	},	//MD,kwtz7
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[6].md_kw.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[7].md_kw.kw	},	//MD,kwtz8
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[7].md_kw.date[0]	},	//MD,kw, date
	{	CHOICE_UL,2,(void *)&billing_param.kva.kva	},	//MD,kVA
	{	CHOICE_OCTSTR,12,(void *)&billing_param.kva.date[0]	},	//MD,kw, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[0].md_kva.kva	},	//MD,kVAtz1
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[0].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[1].md_kva.kva	},	//MD,kVAtz2
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[1].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[2].md_kva.kva	},	//MD,kVAtz3
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[2].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[3].md_kva.kva	},	//MD,kVAtz4
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[3].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[4].md_kva.kva	},	//MD,kVAtz5
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[4].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[5].md_kva.kva	},	//MD,kVAtz6
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[5].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[6].md_kva.kva	},	//MD,kVAtz7
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[6].md_kva.date[0]	},	//MD,kva, date
//	{	CHOICE_UL,2,(void *)&billing_param.md_tz[7].md_kva.kva	},	//MD,kVAtz8
//	{	CHOICE_OCTSTR,12,(void *)&billing_param.md_tz[7].md_kva.date[0]	},	//MD,kva, date
	{	CHOICE_UDL,4,(void *)&dlms_poweron_dur_billing_minutes		}				// total power on duration for billing
};
*/

// This structure is only a place holder (for all event logs). This buffer is actually present in user area viz EEPROM/FLASH
// EEPROM @eeprom_base_address_voltage_event_log.
// The server fetches the data from EEPROM and uses this data structure template 
// to encode the values
// NOTE: DO NOT USE THE EMBEDDED ADDRESSES OF THIS STRUCTURE (They are DUMMYs)
const   CHOICE_VAL profile_generic_dailyload_buffer[] = {
	{	CHOICE_OCTSTR,12,(void *)&stLoad_Profile.Load_Date[0]},
	{	CHOICE_UDL,4,(void *)&stLoad_Profile.E_Active	},		//kwh log
	{	CHOICE_UDL,4,(void *)&stLoad_Profile.E_Apparent	},		//KVAh log	
};

const   CHOICE_VAL profile_generic_nameplate_buffer[] = {

	{	CHOICE_UDL,4,(void *)&meter_no }, //dlms_meter_id	
	{	CHOICE_VSTR,18,(void *)&dlms_manufacturer_name[0]	},
	{	CHOICE_UINT,1,(void *)&dlms_firmware_version	},
	{	CHOICE_UINT,1,(void *)&dlms_meter_type	},
	{	CHOICE_VSTR,2,(void *)&dlms_meter_category	},
	{	CHOICE_VSTR,5,(void *)&dlms_meter_current_rating	},
	{	CHOICE_UL,2,(void *)&dlms_manufacture_year	}
};

#define SIZEOF_PROFILE_GENERIC_NAMEPLATE_BUFFER  (sizeof(profile_generic_nameplate_buffer)/sizeof(profile_generic_nameplate_buffer[0]))

// capture objects for voltage event logs
const   CAPTURE_OBJECT profile_generic_voltage_event_objs[] = {
	{	E_CLASS_CLOCK,DLMS_OBJECT_CLOCK,2,0	},
	{   E_CLASS_DATA,DLMS_OBJECT_VOLTAGE_EVENT,2,0},
	{	E_CLASS_REGISTER,DLMS_OBJECT_MEASURED_CURRENT,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_VOLTAGE,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PF,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,2,0	}
};
#define SIZEOF_PROFILE_GENERIC_VOLTAGE_EVENT_OBJS  (sizeof(profile_generic_voltage_event_objs)/sizeof(profile_generic_voltage_event_objs[0]))
// capture objects for current event logs
const   CAPTURE_OBJECT profile_generic_current_event_objs[] = {
	{	E_CLASS_CLOCK,DLMS_OBJECT_CLOCK,2,0	},
	{   E_CLASS_DATA,DLMS_OBJECT_CURRENT_EVENT,2,0},
	{	E_CLASS_REGISTER,DLMS_OBJECT_MEASURED_CURRENT,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_VOLTAGE,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PF,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,2,0	}
};
#define SIZEOF_PROFILE_GENERIC_CURRENT_EVENT_OBJS  (sizeof(profile_generic_current_event_objs)/sizeof(profile_generic_current_event_objs[0]))

// capture objects for power event logs
const   CAPTURE_OBJECT profile_generic_power_event_objs[] = {
	{	E_CLASS_CLOCK,DLMS_OBJECT_CLOCK,2,0	},
	{   E_CLASS_DATA,DLMS_OBJECT_POWER_EVENT,2,0}
/*	{	E_CLASS_REGISTER,DLMS_OBJECT_MEASURED_CURRENT,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_VOLTAGE,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PF,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,2,0	}
*/
};
#define SIZEOF_PROFILE_GENERIC_POWER_EVENT_OBJS  (sizeof(profile_generic_power_event_objs)/sizeof(profile_generic_power_event_objs[0]))

// capture objects for transaction event logs
const   CAPTURE_OBJECT profile_generic_transaction_event_objs[] = {
	{	E_CLASS_CLOCK,DLMS_OBJECT_CLOCK,2,0	},
	{   E_CLASS_DATA,DLMS_OBJECT_TRANSACTION_EVENT,2,0},
	{	E_CLASS_REGISTER,DLMS_OBJECT_MEASURED_CURRENT,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_VOLTAGE,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PF,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,2,0	}
};
#define SIZEOF_PROFILE_GENERIC_TRANSACTION_EVENT_OBJS  (sizeof(profile_generic_transaction_event_objs)/sizeof(profile_generic_transaction_event_objs[0]))

// capture objects for other event logs
const   CAPTURE_OBJECT profile_generic_other_event_objs[] = {
	{	E_CLASS_CLOCK,DLMS_OBJECT_CLOCK,2,0	},
	{   E_CLASS_DATA,DLMS_OBJECT_OTHER_EVENT,2,0},
	{	E_CLASS_REGISTER,DLMS_OBJECT_MEASURED_CURRENT,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_VOLTAGE,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PF,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,2,0	}
};
#define SIZEOF_PROFILE_GENERIC_OTHER_EVENT_OBJS  (sizeof(profile_generic_other_event_objs)/sizeof(profile_generic_other_event_objs[0]))

// capture objects for non rollover event logs
const   CAPTURE_OBJECT profile_generic_nonrollover_event_objs[] = {
	{	E_CLASS_CLOCK,DLMS_OBJECT_CLOCK,2,0	},
	{   E_CLASS_DATA,DLMS_OBJECT_NON_ROLLOVER_EVENT,2,0},
	{	E_CLASS_REGISTER,DLMS_OBJECT_MEASURED_CURRENT,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_VOLTAGE,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PF,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,2,0	}
};
#define SIZEOF_PROFILE_GENERIC_NON_ROLLOVER_EVENT_OBJS  (sizeof(profile_generic_nonrollover_event_objs)/sizeof(profile_generic_nonrollover_event_objs[0]))

const   CAPTURE_OBJECT profile_generic_blockload_objs[] = {
	{	E_CLASS_CLOCK,DLMS_OBJECT_CLOCK,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_LOADSRVY_VOLTAGE,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_LOADSRVY_ENERGY_KWH,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_LOADSRVY_ENERGY_KVAH,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_LOADSRVY_CURRENT_IR,2,0	},
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,2,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,5,0	}		// attribute 5	
	
};
#define SIZEOF_PROFILE_GENERIC_BLOCKLOAD_OBJS  (sizeof(profile_generic_blockload_objs)/sizeof(profile_generic_blockload_objs[0]))

const   CAPTURE_OBJECT profile_generic_billing_objs[] = {
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_DATE,2,0	},									// index = 0
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_PF,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ1,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ2,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ3,2,0	},								// index = 5
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ4,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ5,2,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ6,2,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ7,2,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ8,2,0	},								// index = 10
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVARH_LAG,2,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVARH_LEAD,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ1,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ2,2,0	},								// index = 15-2
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ3,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ4,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ5,2,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ6,2,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ7,2,0	},								// index = 20-2
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ8,2,0	},
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,2,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,5,0	},		// attribute 5			// index = 23-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ1,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ1,5,0	},		// attribute 5		// index = 25-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ2,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ2,5,0	},		// attribute 5		// index = 27-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ3,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ3,5,0	},		// attribute 5		// index = 29-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ4,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ4,5,0	},		// attribute 5		// index = 31-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ5,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ5,5,0	},		// attribute 5		// index = 33-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ6,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ6,5,0	},		// attribute 5		// index = 35-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ7,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ7,5,0	},		// attribute 5		// index = 37-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ8,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ8,5,0	},		// attribute 5		// index = 39-2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA,2,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA,5,0	},		// attribute 5			// index = 41-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ1,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ1,5,0	},		// attribute 5		// index = 43-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ2,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ2,5,0	},		// attribute 5		// index = 45-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ3,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ3,5,0	},		// attribute 5		// index = 47-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ4,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ4,5,0	},		// attribute 5		// index = 49-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ5,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ5,5,0	},		// attribute 5		// index = 51-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ6,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ6,5,0	},		// attribute 5		// index = 53-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ7,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ7,5,0	},		// attribute 5		// index = 55-2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ8,2,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ8,5,0	},		// attribute 5		// index = 57-2
	{	E_CLASS_REGISTER,DLMS_OBJECT_TOTAL_POWERON_DUR_BILLING,2,0	}	// total power on duration for billing
};
#define SIZEOF_PROFILE_GENERIC_BILLING_OBJS  (sizeof(profile_generic_billing_objs)/sizeof(profile_generic_billing_objs[0]))

const   CAPTURE_OBJECT profile_generic_dailyload_objs[] = {
	{	E_CLASS_CLOCK,DLMS_OBJECT_CLOCK,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,2,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KVAH,2,0	}

};
#define SIZEOF_PROFILE_GENERIC_DAILYLOAD_OBJS  (sizeof(profile_generic_dailyload_objs)/sizeof(profile_generic_dailyload_objs[0]))

const   CAPTURE_OBJECT profile_generic_nameplate_objs[] = {
	{	E_CLASS_DATA,DLMS_OBJECT_METER_SERIAL_NUM,2,0	},
	{	E_CLASS_DATA,DLMS_OBJECT_MANUFACTURER_NAME,2,0	},
	{	E_CLASS_DATA,DLMS_OBJECT_FIRMWARE_VERSION,2,0	},
	{	E_CLASS_DATA,DLMS_OBJECT_METER_TYPE,2,0	},
	{	E_CLASS_DATA,DLMS_OBJECT_CATEGORY,2,0	},
	{	E_CLASS_DATA,DLMS_OBJECT_CURRENT_RATING,2,0	},
	{	E_CLASS_DATA,DLMS_OBJECT_MANUFACTURE_YEAR,2,0	}
};

#define SIZEOF_PROFILE_GENERIC_NAMEPLATE_OBJS  (sizeof(profile_generic_nameplate_objs)/sizeof(profile_generic_nameplate_objs[0]))
// This object gets filled during access selction process

ACCESS_BY_ENTRY access_by_entry;
ACCESS_BY_RANGE access_by_range;
uint8_t access_selection;
const ACCESS_SELECTOR access_selector = {
	(uint8_t *)&access_selection,
	(ACCESS_BY_ENTRY *)&access_by_entry,
	(ACCESS_BY_RANGE *)&access_by_range
};

const  int8_t dlms_scalar_date_scalar = CLOCK_SCALAR;
const  uint8_t dlms_scalar_date_unit = UNIT_COUNT;

const  int8_t dlms_scalar_current_scalar = CURRENT_SCALAR;
const  int8_t dlms_scalar_current_unit = UNIT_AMPERE;

const  int8_t dlms_scalar_voltage_scalar = VOLT_SCALAR;
const  int8_t dlms_scalar_voltage_unit = UNIT_VOLT;

const  int8_t dlms_scalar_pf_scalar = PF_IND_SCALAR;
const  uint8_t dlms_scalar_pf_unit = UNIT_COUNT;

const  int8_t dlms_scalar_totalpf_scalar = PF_TOTAL_SCALAR;

const  int8_t dlms_scalar_frequency_scalar = FREQ_SCALAR;
const  int8_t dlms_scalar_frequency_unit = UNIT_HERTZ;

const  int8_t dlms_scalar_kva_scalar = KVA_SCALAR;
const  int8_t dlms_scalar_kva_unit = UNIT_VOLT_AMP;

const  int8_t dlms_scalar_kw_scalar = KW_SCALAR;
const  int8_t dlms_scalar_kw_unit = UNIT_WATT;

const  int8_t dlms_scalar_kvar_scalar = KVAR_SCALAR;
const  int8_t dlms_scalar_kvar_unit = UNIT_VAR;

const  int8_t dlms_scalar_pfails_scalar = POWERFAIL_DURATION_SCALAR;
const  int8_t dlms_scalar_pfails_unit = UNIT_SECOND;

const  int8_t dlms_scalar_pon_scalar = POWERON_DURATION_SCALAR;
const  int8_t dlms_scalar_pon_unit = UNIT_MINUTE;

const  int8_t dlms_scalar_kwh_scalar = KWH_SCALAR;
const  int8_t dlms_scalar_kwh_unit = UNIT_WATT_HOUR;

const  int8_t dlms_scalar_kvarh_scalar = KVARH_LAG_SCALAR;
const  int8_t dlms_scalar_kvarh_unit = UNIT_VAR_HOUR;

const  int8_t dlms_scalar_kvah_scalar = KVAH_SCALAR;
const  int8_t dlms_scalar_kvah_unit = UNIT_VOLT_AMP_HOUR;

const   CHOICE_VAL scalar_date[] = 
{	
	{ CHOICE_INT,1,(void *)&dlms_scalar_date_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_date_unit}
};

const   CHOICE_VAL scalar_current[] = 
{	
	{ CHOICE_INT,1,(void *)&dlms_scalar_current_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_current_unit}
};
const   CHOICE_VAL scalar_voltage[] = 
{	
	{ CHOICE_INT,1,(void *)&dlms_scalar_voltage_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_voltage_unit}
};
const   CHOICE_VAL scalar_pf[] = 
{	
	{ CHOICE_INT,1,(void *)&dlms_scalar_pf_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_pf_unit}
};
const   CHOICE_VAL scalar_totalpf[] = 
{	
	{ CHOICE_INT,1,(void *)&dlms_scalar_totalpf_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_pf_unit}
};
const   CHOICE_VAL scalar_frequency[] = 
{	
	{ CHOICE_INT,1,(void *)&dlms_scalar_frequency_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_frequency_unit}
};
const   CHOICE_VAL scalar_kva[] = 
{	
	{ CHOICE_INT,1,(void *)&dlms_scalar_kva_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_kva_unit}
};
const   CHOICE_VAL scalar_kw[] = 
{
	{ CHOICE_INT,1,(void *)&dlms_scalar_kw_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_kw_unit}
};
const   CHOICE_VAL scalar_kvar[] = 
{
	{ CHOICE_INT,1,(void *)&dlms_scalar_kvar_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_kvar_unit}
};
const   CHOICE_VAL scalar_powerfails_dur[] = 
{
	{ CHOICE_INT,1,(void *)&dlms_scalar_pfails_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_pfails_unit}
};
const   CHOICE_VAL scalar_poweron_dur[] = 
{
	{ CHOICE_INT,1,(void *)&dlms_scalar_pon_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_pon_unit}
};
const   CHOICE_VAL scalar_poweron_dur_billing[] = 
{
	{ CHOICE_INT,1,(void *)&dlms_scalar_pon_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_pon_unit}
};
const   CHOICE_VAL scalar_kwh[] = 
{
	{ CHOICE_INT,1,(void *)&dlms_scalar_kwh_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_kwh_unit}
};
const   CHOICE_VAL scalar_kvarh[] = 
{
	{ CHOICE_INT,1,(void *)&dlms_scalar_kvarh_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_kvarh_unit}
};
const   CHOICE_VAL scalar_kvah[] = 
{
	{ CHOICE_INT,1,(void *)&dlms_scalar_kvah_scalar },
	{ CHOICE_ENUM, 1, (void *)&dlms_scalar_kvah_unit}
};


const   CHOICE_VAL profile_generic_instant_scalar_buffer[] = {

	// All voltages will have same scalar units
	{		CHOICE_STRUCTURE,2,(void *)&scalar_voltage[0]	},	// voltage (single phase)
	// All current will have same scalar units
	{		CHOICE_STRUCTURE,2,(void *)&scalar_current[0]	},	// phase current (single phase)
	{		CHOICE_STRUCTURE,2,(void *)&scalar_current[0]	},	// neutral current (single phase)
	// All individual PFs will have same scalar units
	{		CHOICE_STRUCTURE,2,(void *)&scalar_totalpf[0]	},
	// frequency
	{		CHOICE_STRUCTURE,2,(void *)&scalar_frequency[0]	},
	// kva
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]	},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]	},

	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]	},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]	},

	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},

	// parameters that do not have scalar units SHALL not be included
	{		CHOICE_STRUCTURE,2,(void *)&scalar_poweron_dur[0]	}	// 

};
#define SIZEOF_PROFILE_GENERIC_INSTANT_SCALAR_BUFFER  (sizeof(profile_generic_instant_scalar_buffer)/sizeof(profile_generic_instant_scalar_buffer[0]))

const   CAPTURE_OBJECT profile_generic_instant_scalar_objs[] = {

	{	E_CLASS_REGISTER,DLMS_OBJECT_VOLTAGE,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PHASE_CURRENT,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_NEUTRAL_CURRENT,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PF,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_FREQUENCY,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_APPARENT_POWER_KVA,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_SIGNED_ACTIVE_POWER_KW,3,0	},

	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KVAH,3,0	},

	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,3,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,5,0	},		// attribute 5
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA,3,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA,5,0	},		// attribute 5

	{	E_CLASS_REGISTER,DLMS_OBJECT_TOTAL_POWERON_DUR,3,0	}		

};

#define SIZEOF_PROFILE_GENERIC_INSTANT_SCALAR_OBJS  (sizeof(profile_generic_instant_scalar_objs)/sizeof(profile_generic_instant_scalar_objs[0]))

// event scalar buffer is common for all types of events (voltage, current, power etc)
const   CHOICE_VAL profile_generic_events_scalar_buffer[] = {
	// CLOCK  NOT INCLUDED
	// EVENT  NOT INCLUDED
	// All current will have same scalar units
	{		CHOICE_STRUCTURE,2,(void *)&scalar_current[0]	},
	// All voltages will have same scalar units
	{		CHOICE_STRUCTURE,2,(void *)&scalar_voltage[0]	},
	// All individual PFs will have same scalar units
	{		CHOICE_STRUCTURE,2,(void *)&scalar_totalpf[0]	},
	// kwh
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]	}
};
#define SIZEOF_PROFILE_GENERIC_EVENTS_SCALAR_BUFFER  (sizeof(profile_generic_events_scalar_buffer)/sizeof(profile_generic_events_scalar_buffer[0]))
// event scalar objects buffer is common for all types of events
const   CAPTURE_OBJECT profile_generic_events_scalar_objs[] = {
	// CLOCK CALSS NOT INCLUDED
	// DATA CLASS NOT INCLUDED
	{	E_CLASS_REGISTER,DLMS_OBJECT_MEASURED_CURRENT,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_VOLTAGE,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_PF,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,3,0	}
};
#define SIZEOF_PROFILE_GENERIC_EVENTS_SCALAR_OBJS  (sizeof(profile_generic_events_scalar_objs)/sizeof(profile_generic_events_scalar_objs[0]))
// block load scalar objects
const   CHOICE_VAL profile_generic_blockload_scalar_buffer[] = {
	// Clock not included
	// All voltages will have same scalar units
	{		CHOICE_STRUCTURE,2,(void *)&scalar_voltage[0]	},
	// energy 
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_current[0]	},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},	
	
};
#define SIZEOF_PROFILE_GENERIC_BLOCKLOAD_SCALAR_BUFFER  (sizeof(profile_generic_blockload_scalar_buffer)/sizeof(profile_generic_blockload_scalar_buffer[0]))
const   CAPTURE_OBJECT profile_generic_blockload_scalar_objs[] = {
	// CLOCK CALSS NOT INCLUDED
	{	E_CLASS_REGISTER,DLMS_OBJECT_LOADSRVY_VOLTAGE,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_LOADSRVY_ENERGY_KWH,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_LOADSRVY_ENERGY_KVAH,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_LOADSRVY_CURRENT_IR,3,0	},
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,3,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,5,0	}		  // attribute 5	
	
};
#define SIZEOF_PROFILE_GENERIC_BLOCKLOAD_SCALAR_OBJS  (sizeof(profile_generic_blockload_scalar_objs)/sizeof(profile_generic_blockload_scalar_objs[0]))

// block load scalar objects
const   CHOICE_VAL profile_generic_dailyload_scalar_buffer[] = {
	// Clock not included
	// energy 
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		}
};
#define SIZEOF_PROFILE_GENERIC_DAILYLOAD_SCALAR_BUFFER  (sizeof(profile_generic_dailyload_scalar_buffer)/sizeof(profile_generic_dailyload_scalar_buffer[0]))

const   CAPTURE_OBJECT profile_generic_dailyload_scalar_objs[] = {
	// CLOCK CALSS NOT INCLUDED
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_CUMULATIVE_ENERGY_KVAH,3,0	}
};
#define SIZEOF_PROFILE_GENERIC_DAILYLOAD_SCALAR_OBJS  (sizeof(profile_generic_dailyload_scalar_objs)/sizeof(profile_generic_dailyload_scalar_objs[0]))

const   CHOICE_VAL profile_generic_billing_scalar_buffer[] = {
	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},		// index=0
	{		CHOICE_STRUCTURE,2,(void *)&scalar_totalpf[0]	},
	// All kwh will have same scalr units
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},		// index=5
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kwh[0]		},		// index=10
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvarh[0]		}, 	// kvarh not present for 1-phase
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvarh[0]		}, 	// kvarh not present for 1-phase
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},		// index=15
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kvah[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},		// index=20
	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},		// index=25
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},		// index=30
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},		// index=35
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kw[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},
	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},		// index=40
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},		// index=45
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},		// index=50
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_kva[0]		},
//	{		CHOICE_STRUCTURE,2,(void *)&scalar_date[0]		},		// index=55
	{		CHOICE_STRUCTURE,2,(void *)&scalar_poweron_dur_billing[0]	}	//		// index=56	   

};
#define SIZEOF_PROFILE_GENERIC_BILLING_SCALAR_BUFFER  (sizeof(profile_generic_billing_scalar_buffer)/sizeof(profile_generic_billing_scalar_buffer[0]))

const   CAPTURE_OBJECT profile_generic_billing_scalar_objs[] = {
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_DATE,3,0	},					// No unit
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_PF,3,0	},						// No unit
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ1,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ2,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ3,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ4,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ5,3,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ6,3,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ7,3,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KWH_TZ8,3,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVARH_LAG,3,0	}, 	// kvarh not present for 1-phase
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVARH_LEAD,3,0	}, 	// kvarh not present for 1-phase
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ1,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ2,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ3,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ4,3,0	},
	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ5,3,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ6,3,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ7,3,0	},
//	{	E_CLASS_REGISTER,DLMS_OBJECT_BILLING_KVAH_TZ8,3,0	},
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,3,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ1,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ1,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ2,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ2,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ3,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ3,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ4,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ4,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ5,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ5,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ6,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ6,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ7,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ7,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ8,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KW_TZ8,5,0	},		// attribute 5
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA,3,0	},		// attribute 2
	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ1,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ1,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ2,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ2,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ3,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ3,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ4,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ4,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ5,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ5,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ6,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ6,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ7,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ7,5,0	},		// attribute 5
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ8,3,0	},		// attribute 2
//	{	E_CLASS_EXTENDED_REG,DLMS_OBJECT_BILLING_MD_KVA_TZ8,5,0	},		// attribute 5
	{	E_CLASS_REGISTER,DLMS_OBJECT_TOTAL_POWERON_DUR_BILLING,3,0	}	// total power on duration for billing
};
#define SIZEOF_PROFILE_GENERIC_BILLING_SCALAR_OBJS  (sizeof(profile_generic_billing_scalar_objs)/sizeof(profile_generic_billing_scalar_objs[0]))


// DO NOT MODIFY THESE DATA STRUCTURES
const   PROFILE_GENERIC profile_generic_class[] = {
	{
		DLMS_OBJECT_PROFILE_INSTANT_SNAPSHOT,
		SIZEOF_PROFILE_GENERIC_INSTANT_PARAMS_BUFFER,							// num_of_buffer entries
		(CHOICE_VAL *)&profile_generic_instant_params_buffer[0],				//
		SIZEOF_PROFILE_GENERIC_INSTANT_PARAMS_OBJS,								// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_instant_params_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&profile_generic_default_entries,			// entries in use	
		(uint32_t *)&profile_generic_default_entries                    // max profile entries
	},
	{	// index = 1
		DLMS_OBJECT_PROFILE_INSTANT_SCALAR,
		SIZEOF_PROFILE_GENERIC_INSTANT_SCALAR_BUFFER,										// num_of_buffer entries
		(CHOICE_VAL *)&profile_generic_instant_scalar_buffer[0],							//
		SIZEOF_PROFILE_GENERIC_INSTANT_SCALAR_OBJS,											// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_instant_scalar_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&profile_generic_default_entries,			// entries in use	
		(uint32_t *)&profile_generic_default_entries                    // max profile entries
	},
	{	// index = 2
		DLMS_OBJECT_PROFILE_NAMEPLATE,
		SIZEOF_PROFILE_GENERIC_NAMEPLATE_BUFFER,							// num_of_buffer entries
		(CHOICE_VAL *)&profile_generic_nameplate_buffer[0],				//
		SIZEOF_PROFILE_GENERIC_NAMEPLATE_OBJS,								// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_nameplate_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&profile_generic_default_entries,			// entries in use
		(uint32_t *)&profile_generic_default_entries                    // max profile entries
	},
	{	// index = 3
		DLMS_OBJECT_PROFILE_VOLTAGE_EVENTS,
		1,		// num_of_buffer entries (DUMMY)
		(CHOICE_VAL *)&profile_generic_voltage_event_buffer[0],	// same DUMMY buffer for all types of events
		SIZEOF_PROFILE_GENERIC_VOLTAGE_EVENT_OBJS,							// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_voltage_event_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&Tamper_Profile_Entries_In_Use[0], //profile_generic_default_entries,			// entries in use (actual value is got thru' dlms_obj_getpg_value(..))
		(uint32_t *)&Tamper_Profile_Entries[0]         //profile_generic_maxevent_entries                   // max profile entries
	},	
	{	// index = 4
		DLMS_OBJECT_PROFILE_CURRENT_EVENTS,
		1,		// num_of_buffer entries (DUMMY)
		(CHOICE_VAL *)&profile_generic_voltage_event_buffer[0],	// same DUMMY buffer for all types of events
		SIZEOF_PROFILE_GENERIC_CURRENT_EVENT_OBJS,							// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_current_event_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&Tamper_Profile_Entries_In_Use[1], //profile_generic_default_entries,			// entries in use (actual value is got thru' dlms_obj_getpg_value(..))
		(uint32_t *)&Tamper_Profile_Entries[1]         //profile_generic_maxevent_entries                   // max profile entries
	},
	{	// index = 5
		DLMS_OBJECT_PROFILE_POWER_EVENTS,
		1,		// num_of_buffer entries (DUMMY)
		(CHOICE_VAL *)&profile_generic_power_event_buffer[0],	// same DUMMY buffer for all types of events
		SIZEOF_PROFILE_GENERIC_POWER_EVENT_OBJS,							// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_power_event_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&Tamper_Profile_Entries_In_Use[2], //profile_generic_default_entries,			// entries in use (actual value is got thru' dlms_obj_getpg_value(..))
		(uint32_t *)&Tamper_Profile_Entries[2]         //profile_generic_maxevent_entries                   // max profile entries
	},
	{	// index = 6
		DLMS_OBJECT_PROFILE_TRANSACTION_EVENTS,
		1,		// num_of_buffer entries (DUMMY)
		(CHOICE_VAL *)&profile_generic_voltage_event_buffer[0],	// same DUMMY buffer for all types of events
		SIZEOF_PROFILE_GENERIC_TRANSACTION_EVENT_OBJS,							// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_transaction_event_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&Tamper_Profile_Entries_In_Use[3], //profile_generic_default_entries,			// entries in use (actual value is got thru' dlms_obj_getpg_value(..))
		(uint32_t *)&Tamper_Profile_Entries[3]         //profile_generic_maxevent_entries                   // max profile entries
	},
	{	// index = 7
		DLMS_OBJECT_PROFILE_OTHER_EVENTS,
		1,		// num_of_buffer entries (DUMMY)
		(CHOICE_VAL *)&profile_generic_voltage_event_buffer[0],	// same DUMMY buffer for all types of events
		SIZEOF_PROFILE_GENERIC_OTHER_EVENT_OBJS,							// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_other_event_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&Tamper_Profile_Entries_In_Use[4], //profile_generic_default_entries,			// entries in use (actual value is got thru' dlms_obj_getpg_value(..))
		(uint32_t *)&Tamper_Profile_Entries[4]         //profile_generic_maxevent_entries                   // max profile entries
	},
	{	// index = 8
		DLMS_OBJECT_PROFILE_NON_ROLLOVER_EVENTS,
		1,		// num_of_buffer entries (DUMMY)
		(CHOICE_VAL *)&profile_generic_voltage_event_buffer[0],	// same DUMMY buffer for all types of events
		SIZEOF_PROFILE_GENERIC_NON_ROLLOVER_EVENT_OBJS,							// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_nonrollover_event_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&Tamper_Profile_Entries_In_Use[5], //profile_generic_default_entries,			// entries in use (actual value is got thru' dlms_obj_getpg_value(..))
		(uint32_t *)&Tamper_Profile_Entries[5]         //profile_generic_maxevent_entries                   // max profile entries
	},

	{	// index = 9
		DLMS_OBJECT_PROFILE_EVENTS_SCALAR,
		SIZEOF_PROFILE_GENERIC_EVENTS_SCALAR_BUFFER,										// num_of_buffer entries
		(CHOICE_VAL *)&profile_generic_events_scalar_buffer[0],							//
		SIZEOF_PROFILE_GENERIC_EVENTS_SCALAR_OBJS,											// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_events_scalar_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&profile_generic_default_entries,			// entries in use
		(uint32_t *)&profile_generic_default_entries                    // max profile entries
	},
	{ // index = 10
		DLMS_OBJECT_PROFILE_BLOCKLOAD,
		1,		// num_of_buffer entries (DUMMY)
		(CHOICE_VAL *)&profile_generic_blockload_buffer[0],							//
		SIZEOF_PROFILE_GENERIC_BLOCKLOAD_OBJS,											// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_blockload_objs[0],
		(uint32_t *)&Load_Profile_Capture_Period,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&Load_Profile_Entries_In_Use, //profile_generic_default_entries,			// entries in use (actual value is got thru' dlms_obj_getpg_value(..))
		(uint32_t *)&Load_Profile_Entries			            // profile entries
	},
	{ // index = 11
		DLMS_OBJECT_PROFILE_BLOCKLOAD_SCALAR,
		SIZEOF_PROFILE_GENERIC_BLOCKLOAD_SCALAR_BUFFER,		// num_of_buffer entries
		(CHOICE_VAL *)&profile_generic_blockload_scalar_buffer[0],							//
		SIZEOF_PROFILE_GENERIC_BLOCKLOAD_SCALAR_OBJS,				// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_blockload_scalar_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&profile_generic_default_entries,			// entries in use
		(uint32_t *)&profile_generic_default_entries                    // max profile entries
	},
	{ // index = 12
		DLMS_OBJECT_PROFILE_BILLING_PARAMS,
		1,												// num_of_buffer entries (DUMMY)
		(CHOICE_VAL *)&profile_generic_billing_buffer[0],							//
		SIZEOF_PROFILE_GENERIC_BILLING_OBJS,				// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_billing_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&Billing_Profile_Entries_In_Use, //billing_count,                                     // entries in use
		(uint32_t *)&Billing_Profile_Entries //profile_generic_maxbilling_entries                 // max profile entries
	},
	{ // index = 13
		DLMS_OBJECT_PROFILE_BILLING_SCALAR,
		SIZEOF_PROFILE_GENERIC_BILLING_SCALAR_BUFFER,		// num_of_buffer entries
		(CHOICE_VAL *)&profile_generic_billing_scalar_buffer[0],							//
		SIZEOF_PROFILE_GENERIC_BILLING_SCALAR_OBJS,				// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_billing_scalar_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&profile_generic_default_entries,			// entries in use
		(uint32_t *)&profile_generic_default_entries                    // max profile entries
	},
	{ // index = 14
		DLMS_OBJECT_PROFILE_DAILYLOAD,
		1,		// num_of_buffer entries (DUMMY)
		(CHOICE_VAL *)&profile_generic_dailyload_buffer[0],							//
		SIZEOF_PROFILE_GENERIC_DAILYLOAD_OBJS,											// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_dailyload_objs[0],
		(uint32_t *)&Daily_Load_Profile_Capture_Period,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&Daily_Load_Profile_Entries_In_Use, //profile_generic_default_entries,			// entries in use (actual value is got thru' dlms_obj_getpg_value(..))
		(uint32_t *)&Daily_Load_Profile_Entries //dlms_profile_max_dl_entries			// profile entries
	},
	{ // index = 15
		DLMS_OBJECT_PROFILE_DAILYLOAD_SCALAR,
		SIZEOF_PROFILE_GENERIC_DAILYLOAD_SCALAR_BUFFER,		// num_of_buffer entries
		(CHOICE_VAL *)&profile_generic_dailyload_scalar_buffer[0],							//
		SIZEOF_PROFILE_GENERIC_DAILYLOAD_SCALAR_OBJS,				// num of capture objects
		(CAPTURE_OBJECT *)&profile_generic_dailyload_scalar_objs[0],
		(uint32_t *)&dlms_noauto_capture,											// no auto capture
		1,											// fifo
		{0,											// sort object,
		0,0,0,0,0,0,
		0,
		0
		},
		(uint32_t *)&profile_generic_default_entries,			// entries in use
		(uint32_t *)&profile_generic_default_entries                    // max profile entries
	}

};
const   uint16_t SIZEOF_PROFILE_GENERIC_CLASS = sizeof(profile_generic_class)/sizeof(profile_generic_class[0]);

#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP

//code uint8_t PASSIVE_CALENDAR_NAME[]  = "BIS_P_CAL";			// size = MAX_CALENDAR_NAME_LEN

/*
// The variable within this objects are DUMMY and are placeholders only. Actual values are stored in eeprom
const   SEASON season_profile_passive[] = {
	{
		(uint8_t *)&Passive_Season_Name[0][0],		// season #1 profile name
		(uint8_t *)&passive_season_date[0][0],			// session start
		(uint8_t *)&Passive_Season_Name[0][0],				// week name
	},
	{
		(uint8_t *)&Passive_Season_Name[1][0],		// season #2 profile name
		(uint8_t *)&passive_season_date[1][0],			// session start
		(uint8_t *)&Passive_Season_Name[1][0],				// week name
	}'
	//{
	//	(uint8_t *)&p_season_profile_name[2][0],		// season #3 profile name
	//	(uint8_t *)&p_season_start_time[2][0],			// session start
	//	(uint8_t *)&p_season_week_name[2][0],				// week name
	//}
};
*/

const   SEASON season_profile_active[] = {
	{
		(uint8_t *)&stSeason_Profile[0].Season_Name[0],		// season #1 profile name
		(uint8_t *)&stSeason_Profile[0].Season_Date[0],			// session start
		(uint8_t *)&stWeek_Profile[0].Week_Name[0],				// week name
	},
	{
		(uint8_t *)&stSeason_Profile[1].Season_Name[0],		// season #2 profile name
		(uint8_t *)&stSeason_Profile[1].Season_Date[0],			// session start
		(uint8_t *)&stWeek_Profile[1].Week_Name[0],				// week name
	},
/*	
	{
		(uint8_t *)&season_profile_name[2][0],			// season #3 profile name
		(uint8_t *)&season_start_time[2][0],				// session start
		(uint8_t *)&season_week_name[2][0],				  // week name
	}
*/	
};
#define SIZEOF_SEASON_PROFILE        (sizeof(season_profile_active)/sizeof(season_profile_active[0]))

const   WEEK_PROFILE week_profile_table_active[] = {
	{
		(uint8_t *)&stWeek_Profile[0].Week_Name[0],		// week #1 profile name  //dlms_dummy_wk[0]
		DLMS_SUN,				// day-id  for MONDAY
		DLMS_MON,				// day-id
		DLMS_TUE,				// day-id
		DLMS_WED,				// day-id
		DLMS_THU,				// day-id
		DLMS_FRI,				// day-id
		DLMS_SAT				// day-id
	},
	{
		(uint8_t *)&stWeek_Profile[1].Week_Name[0],		// week #2 profile name
		DLMS_SUN,				// day-id  for MONDAY
		DLMS_MON,				// day-id
		DLMS_TUE,				// day-id
		DLMS_WED,				// day-id
		DLMS_THU,				// day-id
		DLMS_FRI,				// day-id
		DLMS_SAT				// day-id
	},
/*	
	{
		(uint8_t *)&dlms_dummy_wk[2],		// week #3 profile name
		DLMS_SUN,				// day-id  for MONDAY
		DLMS_MON,				// day-id
		DLMS_TUE,				// day-id
		DLMS_WED,				// day-id
		DLMS_THU,				// day-id
		DLMS_FRI,				// day-id
		DLMS_SAT				// day-id
	}
*/	
};
#define  SIZEOF_WEAK_PROFILE  (sizeof(week_profile_table_active)/sizeof(week_profile_table_active[0]))

#define TARRIF_SCRIPT_LOGICAL_NAME			{0,0,10,0,100,255}		// dummy

const   DAY_PROFILE_ACTION day_schedule[] = {
	{
		(uint8_t *)&dlms_dummy_array[0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			    // script logical name
		0 //(uint16_t *)&dlms_dummy_int      		// script selector
	}
};

/*
const   DAY_PROFILE_ACTION day_schedule1[] = {
	{
		(uint8_t *)&stDay_Profile[0].tod_time[0][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[0].tariff_id[0]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[0].tod_time[1][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[0].tariff_id[1]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[0].tod_time[2][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[0].tariff_id[2]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[0].tod_time[3][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[0].tariff_id[3]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[0].tod_time[4][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[0].tariff_id[4]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[0].tod_time[5][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[0].tariff_id[5]						// script selector
	}
};

const   DAY_PROFILE_ACTION day_schedule2[] = {
	{
		(uint8_t *)&stDay_Profile[1].tod_time[0][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[1].tariff_id[0]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[1].tod_time[1][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[1].tariff_id[1]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[1].tod_time[2][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[1].tariff_id[2]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[1].tod_time[3][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[1].tariff_id[3]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[1].tod_time[4][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[1].tariff_id[4]						// script selector
	},
	{
		(uint8_t *)&stDay_Profile[1].tod_time[5][0],		// start time  ..dummy
		TARRIF_SCRIPT_LOGICAL_NAME,			// script logical name
		(uint16_t *)&stDay_Profile[1].tariff_id[5]						// script selector
	}
};
*/

const   DAY_PROFILE day_profile_table_active[] = {
	{
		(uint8_t *)&stDay_Profile[0].day_id, //dlms_day_id,						// day id
		(uint8_t *)&num_of_zone, //&dlms_no_tou,		//							
		(DAY_PROFILE_ACTION *)&day_schedule[0]
	},
	{
		(uint8_t *)&stDay_Profile[1].day_id, //dlms_day_id,						// day id
		(uint8_t *)&num_of_zone,		//dlms_no_tou,		//					
		(DAY_PROFILE_ACTION *)&day_schedule[0]
	},
/*	
	{
		(uint8_t *)&dlms_day_id,						// day id
		(uint8_t *)&dlms_no_tou,									
		(DAY_PROFILE_ACTION *)&day_schedule[0]
	}
*/	
};

/*
const   DAY_PROFILE day_profile_table_active1[] = {
	{
		(uint8_t *)&stDay_Profile[0].day_id,						// day id
		(uint8_t *)&num_of_zone,									
		(DAY_PROFILE_ACTION *)&day_schedule1[0]
	},
	{
		(uint8_t *)&stDay_Profile[1].day_id,						// day id
		(uint8_t *)&num_of_zone,									
		(DAY_PROFILE_ACTION *)&day_schedule2[0]
	},
};
*/

#define  SIZEOF_DAY_PROFILE   (sizeof(day_profile_table_active)/sizeof(day_profile_table_active[0]))

// The object values (other than NAMEs and time) are dummy. Actual values are in eeprom.

const   ACTIVITY_CALENDAR activity_calendar_class[] = {
	{	
		DLMS_OBJECT_ACTIVITY_CALENDAR,
		(uint8_t *)&Calendar_Name_Active[0],	// calendar name active
		SIZEOF_SEASON_PROFILE,							// number of active seasons.
		(SEASON *)&season_profile_active[0],
		SIZEOF_WEAK_PROFILE,							// number of week profiles.
		(WEEK_PROFILE *)&week_profile_table_active[0],
		SIZEOF_DAY_PROFILE,							// number of day profiles.
		(DAY_PROFILE *)&day_profile_table_active[0],
		(uint8_t *)&Calendar_Name_Passive[0],	// calendar name passive
		SIZEOF_SEASON_PROFILE,							// number of active seasons.
		(SEASON *)&season_profile_active[0],
		SIZEOF_WEAK_PROFILE,							// number of week profiles.
		(WEEK_PROFILE *)&week_profile_table_active[0],
		SIZEOF_DAY_PROFILE,							// number of day profiles.
		(DAY_PROFILE *)&day_profile_table_active[0],
		(uint8_t *)&Activate_Passive_Calendar_Time[0]
	}
};

const   uint16_t SIZEOF_ACTIVITY_CALENDAR_CLASS = sizeof(activity_calendar_class)/sizeof(activity_calendar_class[0]);

#endif // #ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP

const   EXECUTION_TIME_DATE execution_time_date_array[] = {
	{
	 	(uint8_t *)&execution_billing_date[0][5],   //scheduled_bill_time[0],		// time	  
		(uint8_t *)&execution_billing_date[0][0]		//scheduled_bill_day[0]  // date
	},
	{
	 	(uint8_t *)&execution_billing_date[1][5],		// time	  
		(uint8_t *)&execution_billing_date[1][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[2][5],		// time	  
		(uint8_t *)&execution_billing_date[2][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[3][5],		// time	  
		(uint8_t *)&execution_billing_date[3][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[4][5],		// time	  
		(uint8_t *)&execution_billing_date[4][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[5][5],		// time	  
		(uint8_t *)&execution_billing_date[5][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[6][5],		// time	  
		(uint8_t *)&execution_billing_date[6][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[7][5],		// time	  
		(uint8_t *)&execution_billing_date[7][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[8][5],		// time	  
		(uint8_t *)&execution_billing_date[8][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[9][5],		// time	  
		(uint8_t *)&execution_billing_date[9][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[10][5],		// time	  
		(uint8_t *)&execution_billing_date[10][0]		// date
	},
	{
	 	(uint8_t *)&execution_billing_date[11][5],		// time	  
		(uint8_t *)&execution_billing_date[11][0]		// date
	},
};

#define SIZEOF_EXECUTION_TIME_DATE_ARRAY   (sizeof(execution_time_date_array)/sizeof(execution_time_date_array[0]))		// Per BIS standard - billing cycles

const   SINGLE_ACTION_SCHEDULE single_action_schedule_class[] = {
	{
		DLMS_OBJECT_ENDOF_BILLING_PERIOD,
		{	DLMS_OBJECT_ENDOF_BILLING_SCRIPT_TABLE,	1	},		// script to execute
		4,														// sizeof execution_time;time values may be different, wildcards in date not allowed,
		SIZEOF_EXECUTION_TIME_DATE_ARRAY,
		(EXECUTION_TIME_DATE *)&execution_time_date_array
	}
};
const   uint16_t SIZEOF_SINGLE_ACTION_SCHEDULE_CLASS = sizeof(single_action_schedule_class)/sizeof(single_action_schedule_class[0]);

// execute 'end of billing period' action (profile generic)
const   ACTION action = {
		2,									// execute method
		E_CLASS_PROFILE_GENERIC,
		DLMS_OBJECT_PROFILE_BILLING_PARAMS,				
		1,									// method = 1
		{0,0,(void *)0}
};
const   SCRIPT script = {
		1,										// script_id
		1,										// number of actions = 1
		(ACTION *)&action
};
const   SCRIPT_TABLE script_table_class[] = {
	{
		DLMS_OBJECT_ENDOF_BILLING_SCRIPT_TABLE,
		1,									// onumber of scripts = 1
		(SCRIPT *)&script
	}
};

const ATTRIBUTE_ACCESS_ITEM data_access_attrib[] = 
{
// DLMS_OBJECT_LOGICAL_DEVICE_NAME_STR (index = 0)
		{	 0x01,							// attrib_id
			READ_ONLY_ATTRIBUTE,			// access mode
			{CHOICE_NULL, 0, (void *)0},	// access selector
		},
		{	 0x02,							// attrib_id
			READ_ONLY_ATTRIBUTE,			// access mode
			{CHOICE_NULL,0,(void *)0}		// access selector
		},

// 						(index = 2)		// write attribute
		{	 0x01,							// attrib_id
			READ_ONLY_ATTRIBUTE,			// access mode
		{CHOICE_NULL,0,(void *)0}			// access selector
		},
		{	 0x02,							// attrib_id
			RD_WR_ATTRIBUTE,			// access mode
		{CHOICE_NULL,0,(void *)0}			// access selector
		}

};
const ATTRIBUTE_ACCESS_ITEM register_access_attrib[] = {
//DLMS_OBJECT_NOMINAL_CURRENT_0
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x02,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x03,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}
};
const ATTRIBUTE_ACCESS_ITEM ext_register_access_attrib[] = {
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x02,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x03,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x04,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x05,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}
};
const ATTRIBUTE_ACCESS_ITEM clock_access_attrib[] = {
//DLMS_OBJECT_CLOCK
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x03,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x05,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x07,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x09,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
// For US association
//DLMS_OBJECT_CLOCK
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,			// attrib_id
		 RD_WR_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x03,			// attrib_id
		 RD_WR_ATTRIBUTE,          // 28-12-2015 ashish
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x05,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x07,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x09,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}
};
const ATTRIBUTE_ACCESS_ITEM hdlcsetup_access_attrib[] = {
//DLMS_OBJECT_CLOCK
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x03,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x05,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x07,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x09,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}
};

const ATTRIBUTE_ACCESS_ITEM ca_ln_access_attrib[] = {
//DLMS_OBJECT_PC_ASSOCIATION_LN		// index = 0
	{	0x01,								// attrib_id (logical name)
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,								// attrib_id (object list)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x03,								// attrib_id (associated partner id)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,								// attrib_id (application context name)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x05,								// attrib_id (xdlms context info)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,								// attrib_id (mechanism name)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x07,								// attrib_id (secret)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,								// attrib_id (association status)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
/*	{	 0x09,								// attrib_id (security setup)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}
*/
//DLMS_OBJECT_MR_ASSOCIATION_LN		// index = 8
	{	0x01,								// attrib_id (logical name)
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,								// attrib_id (object list)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x03,								// attrib_id (associated partner id)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,								// attrib_id (application context name)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x05,								// attrib_id (xdlms context info)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,								// attrib_id (mechanism name)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x07,								// attrib_id (secret)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,								// attrib_id (association status)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
/*	{	 0x09,								// attrib_id (security setup)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}
*/
//DLMS_OBJECT_US_ASSOCIATION_LN		// index = 16
	{	0x01,								// attrib_id (logical name)
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,								// attrib_id (object list)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x03,								// attrib_id (associated partner id)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,								// attrib_id (application context name)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x05,								// attrib_id (xdlms context info)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,								// attrib_id (mechanism name)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x07,								// attrib_id (secret)
		 RD_WR_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,								// attrib_id (association status)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}
/*	{	 0x09,								// attrib_id (security setup)
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}
*/
};
const METHOD_ACCESS_ITEM ca_ln_access_method[] = {
	{0x01, 1 },
	{0x02, 1 }
};
const ATTRIBUTE_ACCESS_ITEM sap_access_attrib[] = {
//DLMS_OBJECT_SAP_ASSIGNMENT
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}
};
const ATTRIBUTE_ACCESS_ITEM pg_access_attrib[] = {
//DLMS_OBJECT_PROFILE_GENERIC
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// attribute access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x03,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x05,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x07,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	// Access attributes with selective access (by entry) (index = 8)
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// attribute access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_ARRAY, 1, (void *)&by_entry}			// access selector
	},
	{	0x03,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x05,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x07,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	// Access attributes with selective access (by range) (index = 16)
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// attribute access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_ARRAY, 1, (void *)&by_range}			// access selector
	},
	{	0x03,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x05,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x07,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}

};
#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
const ATTRIBUTE_ACCESS_ITEM ac_access_attrib[] = {
//DLMS_OBJECT_ACTIVITY_CALENDAR	for MR
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x03,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x05,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x07,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x09,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x0A,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
//DLMS_OBJECT_ACTIVITY_CALENDAR	for US	 				// index = 9
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x03,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x05,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x06,			// attrib_id
		 RD_WR_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x07,			// attrib_id
		RD_WR_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x08,			// attrib_id
		 RD_WR_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x09,			// attrib_id
		 RD_WR_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x0A,			// attrib_id
		 RD_WR_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}

};

const METHOD_ACCESS_ITEM ac_access_method[] = {
	{0x01, ACCESS_MODE_METHOD },
};
#endif //#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP

const ATTRIBUTE_ACCESS_ITEM sac_access_attrib[] = {
//DLMS_OBJECT_SINGLE ACTION SCHEDULE	for MR
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x03,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
//DLMS_OBJECT_SINGLE ACTION SCHEDULE	for UA
	{	0x01,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x02,			// attrib_id
		 READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	0x03,			// attrib_id
		READ_ONLY_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	},
	{	 0x04,			// attrib_id
		 RD_WR_ATTRIBUTE,
		{CHOICE_NULL, 0, (void *)0}			// access selector
	}

};

const LN_OBJ_ELEMENT ln_obj_element_pc[] = {

	{	E_CLASS_DATA,													// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_LOGICAL_DEVICE_NAME_STR,							// logical name
		{0x02,(ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],			//attribute_access_descriptor
		0x00, NULL}														// 
	},
	{	E_CLASS_ASSOCIATION_LN,											// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_CURRENT_ASSOCIATION_LN,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&ca_ln_access_attrib[0],			//attribute_access_descriptor
		0x00, NULL}														// 
	},
	{	 E_CLASS_CLOCK,													// class id
		 CLASS_VERSION_0,												// version
		 DLMS_OBJECT_CLOCK,												// logical name
		{0x09, (ATTRIBUTE_ACCESS_ITEM *)&clock_access_attrib[0],		//attribute_access_descriptor
		 0x00, NULL}													// 
	},
	{	E_CLASS_DATA,													// class id
		CLASS_VERSION_0,												// version
		DLMS_OBJECT_DEVICE_ID,											// logical name
		{0x02,(ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],			//attribute_access_descriptor
			 0x00, NULL}												// 
	},
	{	E_CLASS_ASSOCIATION_LN,											// class id
		CLASS_VERSION_0,												// version
		DLMS_OBJECT_PC_ASSOCIATION_LN,									// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&ca_ln_access_attrib[0],			//attribute_access_descriptor
			 0x00, NULL}												
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_NAMEPLATE,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	}
	// Add user objetcs here....
};
const LN_OBJ_ELEMENT ln_obj_element_mr[] = {
	{	E_CLASS_DATA,													// class id
		CLASS_VERSION_0,												// version
		DLMS_OBJECT_LOGICAL_DEVICE_NAME_STR,							// logical name
		{0x02,(ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],			//attribute_access_descriptor
		0x00, NULL}														//
	},
	{	E_CLASS_ASSOCIATION_LN,											// class id
		CLASS_VERSION_0,												// version
		DLMS_OBJECT_CURRENT_ASSOCIATION_LN,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&ca_ln_access_attrib[8],			//attribute_access_descriptor
		0x00, NULL}														//
	},
	{	 E_CLASS_CLOCK,													// class id
		 CLASS_VERSION_0,												// version
		 DLMS_OBJECT_CLOCK,												// logical name
		{0x09, (ATTRIBUTE_ACCESS_ITEM *)&clock_access_attrib[0],	//attribute_access_descriptor
		 0x00, NULL}												// 
	},
	{	E_CLASS_DATA,													// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_DEVICE_ID,											// logical name
		{0x02,(ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_ASSOCIATION_LN,													// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_PC_ASSOCIATION_LN,									// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&ca_ln_access_attrib[8],			//attribute_access_descriptor
			 0x00, NULL}												//
	},
	{	E_CLASS_ASSOCIATION_LN,													// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_MR_ASSOCIATION_LN,									// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&ca_ln_access_attrib[8],			//attribute_access_descriptor
			 0x00, NULL}												// 
	},

	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_PHASE_CURRENT,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_NEUTRAL_CURRENT,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},
	{
		 E_CLASS_REGISTER,		// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_VOLTAGE,	// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_PF,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},
	{
		 E_CLASS_REGISTER,		// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_FREQUENCY,	// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}													// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_APPARENT_POWER_KVA,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},		
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_SIGNED_ACTIVE_POWER_KW,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	   // reactive signed power KVAR is not in the list for single phase meters
/*	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_SIGNED_REACTIVE_POWER_KVAR,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
     // number of power fail is not in the list for single phase meters
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_NUM_POWERFAILS,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
*/	{
		 E_CLASS_REGISTER,								// class id
		 0x00,					// version
		 DLMS_OBJECT_TOTAL_POWERON_DUR,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 0x00,					// version
		 DLMS_OBJECT_TOTAL_POWERON_DUR_BILLING,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_TAMPERCOUNT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_BILLING_COUNT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},

	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_PROGRAMMINGCOUNT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_PROFILE_CAPTUREPERIOD,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_DAYPROFILE_CAPTUREPERIOD,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_BILLING_DATE,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_ENERGY_KVAH,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_IEC_HDLC_SETUP,			// class id
		 CLASS_VERSION_1,								// version
		 DLMS_OBJECT_IEC_HDLC_SETUP,		// logical name
		 {0x09, (ATTRIBUTE_ACCESS_ITEM *)&hdlcsetup_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{	E_CLASS_SAP_ASSIGNMENT,											// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_SAP_ASSIGNMENT,										// logical name
		{0x02,(ATTRIBUTE_ACCESS_ITEM *)&sap_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},

	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_INSTANT_SNAPSHOT,										// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_INSTANT_SCALAR,										// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_VOLTAGE_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[8],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},	
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_CURRENT_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[8],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_POWER_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[8],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_TRANSACTION_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[8],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_OTHER_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[8],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_NON_ROLLOVER_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[8],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_EVENTS_SCALAR,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_BLOCKLOAD,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[16],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_BLOCKLOAD_SCALAR,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_BILLING_PARAMS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[8],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_BILLING_SCALAR,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_MANUFACTURER_NAME,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_FIRMWARE_VERSION,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_METER_TYPE,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CATEGORY,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CURRENT_RATING,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_MANUFACTURE_YEAR,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
	{
		 E_CLASS_ACTIVITY_CALENDAR,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_ACTIVITY_CALENDAR,					// logical name
		 {0x0A, (ATTRIBUTE_ACCESS_ITEM *)&ac_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}										// 

	},
#endif // #ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
	{
		 E_CLASS_SINGLE_ACTION_SCHEDULE,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_ENDOF_BILLING_PERIOD,					// logical name
		 {0x04, (ATTRIBUTE_ACCESS_ITEM *)&sac_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}										// 

	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_AVAILABLE_BILLING_PERIOD,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_DEMAND_INTEGRATION_PERIOD,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	// Add user objects here
 	{
 		 E_CLASS_DATA,								// class id
 		 CLASS_VERSION_0,					// version
 		 DLMS_OBJECT_USEROBJECT,					// logical name
 		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
 			  0x00, NULL}												// 
 	},
 	{
 		 E_CLASS_DATA,								// class id
 		 CLASS_VERSION_0,					// version
 		 DLMS_OBJECT_DIAGNOSTICS,					// logical name
 		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
 			  0x00, NULL}												// 
 	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_DAILYLOAD,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[16],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_DAILYLOAD_SCALAR,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,									// class id
		CLASS_VERSION_1,											// version
		DLMS_OBJECT_PROFILE_NAMEPLATE,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[8],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{
		 E_CLASS_EXTENDED_REG,										// class id
		 CLASS_VERSION_0,											// version
		 DLMS_OBJECT_BILLING_MD_KW,									// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},
	{
		 E_CLASS_EXTENDED_REG,										// class id
		 CLASS_VERSION_0,											// version
		 DLMS_OBJECT_BILLING_MD_KVA,								// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},

	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CURRENT_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_POWER_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_TRANSACTION_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_OTHER_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_NON_ROLLOVER_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},

};

const LN_OBJ_ELEMENT ln_obj_element_us[] = {
	{	E_CLASS_DATA,													// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_LOGICAL_DEVICE_NAME_STR,							// logical name
		{0x02,(ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],			//attribute_access_descriptor
		0x00, NULL}												// 
	},
	{	E_CLASS_ASSOCIATION_LN,											// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_CURRENT_ASSOCIATION_LN,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&ca_ln_access_attrib[16],			//attribute_access_descriptor
		0x02, (METHOD_ACCESS_ITEM *)&ca_ln_access_method[0]}												// 
	},
	{	 E_CLASS_CLOCK,									// class id
		 CLASS_VERSION_0,											// version
		 DLMS_OBJECT_CLOCK,								// logical name
		{0x09, (ATTRIBUTE_ACCESS_ITEM *)&clock_access_attrib[9],	//attribute_access_descriptor
		 0x00, NULL}												// 
	},
	{	E_CLASS_DATA,													// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_DEVICE_ID,											// logical name
		{0x02,(ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_ASSOCIATION_LN,													// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_PC_ASSOCIATION_LN,									// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&ca_ln_access_attrib[0],			//attribute_access_descriptor
			 0x00, NULL}												// 
	},
	{	E_CLASS_ASSOCIATION_LN,													// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_MR_ASSOCIATION_LN,									// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&ca_ln_access_attrib[16],			//attribute_access_descriptor
			 0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_PHASE_CURRENT,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_NEUTRAL_CURRENT,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,		// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_VOLTAGE,	// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_PF,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,		// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_FREQUENCY,	// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}													// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_APPARENT_POWER_KVA,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},		
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_SIGNED_ACTIVE_POWER_KW,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	  // reactive signed power KVAR is not in the list for single phase meters
/*	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_SIGNED_REACTIVE_POWER_KVAR,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	// number of power failures is not in the list for single phase meters	
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_NUM_POWERFAILS,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
*/	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_TOTAL_POWERON_DUR,					// logical name 
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 0x00,					// version
		 DLMS_OBJECT_TOTAL_POWERON_DUR_BILLING,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_TAMPERCOUNT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_BILLING_COUNT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_PROGRAMMINGCOUNT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_PROFILE_CAPTUREPERIOD,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[2],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_DAYPROFILE_CAPTUREPERIOD,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_BILLING_DATE,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_ENERGY_KWH,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_REGISTER,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CUMULATIVE_ENERGY_KVAH,					// logical name
		 {0x03, (ATTRIBUTE_ACCESS_ITEM *)&register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	}, 		
	{
		 E_CLASS_IEC_HDLC_SETUP,			// class id
		 CLASS_VERSION_1,								// version
		 DLMS_OBJECT_IEC_HDLC_SETUP,		// logical name
		 {0x09, (ATTRIBUTE_ACCESS_ITEM *)&hdlcsetup_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{	E_CLASS_SAP_ASSIGNMENT,											// class id
		0x00,															// version
		DLMS_OBJECT_SAP_ASSIGNMENT,										// logical name
		{0x02,(ATTRIBUTE_ACCESS_ITEM *)&sap_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_INSTANT_SNAPSHOT,										// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_INSTANT_SCALAR,										// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_VOLTAGE_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},	
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_CURRENT_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_POWER_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_TRANSACTION_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_OTHER_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_NON_ROLLOVER_EVENTS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_EVENTS_SCALAR,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_BLOCKLOAD,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[16],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_BLOCKLOAD_SCALAR,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_BILLING_PARAMS,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_BILLING_SCALAR,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
/*
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CURRENT_EVENT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_POWER_EVENT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_TRANSACTION_EVENT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_OTHER_EVENT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_NON_ROLLOVER_EVENT,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
*/
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_MANUFACTURER_NAME,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_FIRMWARE_VERSION,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_METER_TYPE,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CATEGORY,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CURRENT_RATING,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_MANUFACTURE_YEAR,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
	{
		 E_CLASS_ACTIVITY_CALENDAR,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_ACTIVITY_CALENDAR,					// logical name
		 {0x0A, (ATTRIBUTE_ACCESS_ITEM *)&ac_access_attrib[10],	//attribute_access_descriptor
		  0x01, (METHOD_ACCESS_ITEM *)&ac_access_method[0]}	// 
		  										
	},
#endif // #ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
	{	E_CLASS_ASSOCIATION_LN,											// class id
		CLASS_VERSION_0,															// version
		DLMS_OBJECT_US_ASSOCIATION_LN,									// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&ca_ln_access_attrib[16],			//attribute_access_descriptor
		 0x02, (METHOD_ACCESS_ITEM *)&ca_ln_access_method[0]}				// 
	},
	{
		 E_CLASS_SINGLE_ACTION_SCHEDULE,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_ENDOF_BILLING_PERIOD,					// logical name
		 {0x04, (ATTRIBUTE_ACCESS_ITEM *)&sac_access_attrib[4],	//attribute_access_descriptor
			  0x00, NULL}										// 

	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_AVAILABLE_BILLING_PERIOD,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_DATA,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_DEMAND_INTEGRATION_PERIOD,					// logical name
		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[2],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	// Add user objects here
 	{
 		 E_CLASS_DATA,								// class id
 		 CLASS_VERSION_0,					// version
 		 DLMS_OBJECT_USEROBJECT,					// logical name
 		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[2],	//attribute_access_descriptor
 			  0x00, NULL}												// 
 	},
 	{
 		 E_CLASS_DATA,								// class id
 		 CLASS_VERSION_0,					// version
 		 DLMS_OBJECT_DIAGNOSTICS,					// logical name
 		 {0x02, (ATTRIBUTE_ACCESS_ITEM *)&data_access_attrib[2],	//attribute_access_descriptor
 			  0x00, NULL}												// 
 	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_DAILYLOAD,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[16],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_DAILYLOAD_SCALAR,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[0],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{	E_CLASS_PROFILE_GENERIC,											// class id
		CLASS_VERSION_1,															// version
		DLMS_OBJECT_PROFILE_NAMEPLATE,								// logical name
		{0x08,(ATTRIBUTE_ACCESS_ITEM *)&pg_access_attrib[8],		//attribute_access_descriptor
			 0x00, NULL}											// 
	},
	{
		 E_CLASS_EXTENDED_REG,										// class id
		 CLASS_VERSION_0,											// version
		 DLMS_OBJECT_BILLING_MD_KW,									// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},
	{
		 E_CLASS_EXTENDED_REG,										// class id
		 CLASS_VERSION_0,											// version
		 DLMS_OBJECT_BILLING_MD_KVA,								// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												//
	},

	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_CURRENT_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_POWER_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_TRANSACTION_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_OTHER_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},
	{
		 E_CLASS_EXTENDED_REG,								// class id
		 CLASS_VERSION_0,					// version
		 DLMS_OBJECT_NON_ROLLOVER_EVENT,					// logical name
		 {0x05, (ATTRIBUTE_ACCESS_ITEM *)&ext_register_access_attrib[0],	//attribute_access_descriptor
			  0x00, NULL}												// 
	},

};

#define NUM_LN_OBJ_ELEMENTS_PC  (sizeof(ln_obj_element_pc)/sizeof(ln_obj_element_pc[0]))
#define NUM_LN_OBJ_ELEMENTS_MR  (sizeof(ln_obj_element_mr)/sizeof(ln_obj_element_mr[0]))
#define  NUM_LN_OBJ_ELEMENTS_US  (sizeof(ln_obj_element_us)/sizeof(ln_obj_element_us[0]))

const ASSOCIATION_LN association_ln_class_pc[] = {
	{
		DLMS_OBJECT_CURRENT_ASSOCIATION_LN,					//  current association = LN 
		NUM_LN_OBJ_ELEMENTS_PC,							// number of objects
		(LN_OBJ_ELEMENT *)&ln_obj_element_pc,
		(ASSOCIATED_PARTNERS_TYPE *)&associated_partners_type[0],		// associated partners
		{CHOICE_OCTSTR,7,(void *)&app_context},					// app context
		(XDLMS_CONTEXT_TYPE *)&xdlms_context_type,
		{CHOICE_OCTSTR,7,(void *)&authentication_mechanism_name_pc},		// mechansim
		{CHOICE_OCTSTR,0,(void *)0},
		ASSOCIATED
//		{0,0,43,1,0,255}							// needed only if HLS w/ GMAC is implemented
	},
	{
		DLMS_OBJECT_PC_ASSOCIATION_LN,						//  PC association = LN 
		NUM_LN_OBJ_ELEMENTS_PC,							// number of objects
		(LN_OBJ_ELEMENT *)&ln_obj_element_pc,
		(ASSOCIATED_PARTNERS_TYPE *)&associated_partners_type[0],		// associated partners
		{CHOICE_OCTSTR,7,(void *)&app_context},					// app context
		(XDLMS_CONTEXT_TYPE *)&xdlms_context_type,
		{CHOICE_OCTSTR,7,(void *)&authentication_mechanism_name_pc},		// mechansim
		{CHOICE_OCTSTR,0,(void *)0},
		ASSOCIATED,                                                             // {0,0,43,1,0,255}												// needed only if HLS w/ GMAC is implemented
	}
};
const ASSOCIATION_LN association_ln_class_mr[] = {
	{
		DLMS_OBJECT_CURRENT_ASSOCIATION_LN,					//  current association = LN 
		NUM_LN_OBJ_ELEMENTS_MR,							// number of objects
		(LN_OBJ_ELEMENT *)&ln_obj_element_mr,
		(ASSOCIATED_PARTNERS_TYPE *)&associated_partners_type[1],		// associated partners
		{CHOICE_OCTSTR,7,(void *)&app_context},					// app context
		(XDLMS_CONTEXT_TYPE *)&xdlms_context_type,
		{CHOICE_OCTSTR,7,(void *)&authentication_mechanism_name_mr},            // mechansim
		{CHOICE_OCTSTR,MAX_SECRET_LLS_LEN,(void *)&dlms_lls_password},
//		{CHOICE_OCTSTR,0,(void *)0},
		ASSOCIATED
//		{0,0,43,1,0,255}							// needed only if HLS w/ GMAC is implemented
	},
	{
		DLMS_OBJECT_PC_ASSOCIATION_LN,						//  PC association = LN 
		NUM_LN_OBJ_ELEMENTS_PC,							// number of objects
		(LN_OBJ_ELEMENT *)&ln_obj_element_pc,
		(ASSOCIATED_PARTNERS_TYPE *)&associated_partners_type[0],		// associated partners
		{CHOICE_OCTSTR,7,(void *)&app_context},					// app context
		(XDLMS_CONTEXT_TYPE *)&xdlms_context_type,
		{CHOICE_OCTSTR,7,(void *)&authentication_mechanism_name_pc},            // mechansim
		{CHOICE_OCTSTR,0,(void *)0},
		NON_ASSOCIATED
//		{0,0,43,1,0,255}							// needed only if HLS w/ GMAC is implemented
	},
	{
		DLMS_OBJECT_MR_ASSOCIATION_LN,						//  current association = LN 
		NUM_LN_OBJ_ELEMENTS_MR,							// number of objects
		(LN_OBJ_ELEMENT *)&ln_obj_element_mr,
		(ASSOCIATED_PARTNERS_TYPE *)&associated_partners_type[1],		// associated partners
		{CHOICE_OCTSTR,7,(void *)&app_context},					// app context
		(XDLMS_CONTEXT_TYPE *)&xdlms_context_type,
		{CHOICE_OCTSTR,7,(void *)&authentication_mechanism_name_mr},            // mechansim
		{CHOICE_OCTSTR,MAX_SECRET_LLS_LEN,(void *)&dlms_lls_password},
//		{CHOICE_OCTSTR,0,(void *)0},
		ASSOCIATED
//		{0,0,43,1,0,255}							// needed only if HLS w/ GMAC is implemented
	}
};
const ASSOCIATION_LN association_ln_class_us[] = {
	{
		DLMS_OBJECT_CURRENT_ASSOCIATION_LN,					//  current association = LN 
		NUM_LN_OBJ_ELEMENTS_US,							// number of objects
		(LN_OBJ_ELEMENT *)&ln_obj_element_us,
		(ASSOCIATED_PARTNERS_TYPE *)&associated_partners_type[2],		// associated partners
		{CHOICE_OCTSTR,7,(void *)&app_context},					// app context
		(XDLMS_CONTEXT_TYPE *)&xdlms_context_type,
		{CHOICE_OCTSTR,7,(void *)&authentication_mechanism_name_us},            // mechansim
//		{CHOICE_OCTSTR,MAX_SECRET_HLS_LEN,(void *)&dlms_hls_password},
		{CHOICE_OCTSTR,0,(void *)0},
		ASSOCIATED
//		{0,0,43,1,0,255}							// needed only if HLS w/ GMAC is implemented
	},
	{
		DLMS_OBJECT_PC_ASSOCIATION_LN,						//  PC association = LN 
		NUM_LN_OBJ_ELEMENTS_PC,							// number of objects
		(LN_OBJ_ELEMENT *)&ln_obj_element_pc,
		(ASSOCIATED_PARTNERS_TYPE *)&associated_partners_type[0],		// associated partners
		{CHOICE_OCTSTR,7,(void *)&app_context},					// app context
		(XDLMS_CONTEXT_TYPE *)&xdlms_context_type,
		{CHOICE_OCTSTR,7,(void *)&authentication_mechanism_name_pc},            // mechansim
		{CHOICE_OCTSTR,0,(void *)0},
		NON_ASSOCIATED
//		{0,0,43,1,0,255}                                                        // needed only if HLS w/ GMAC is implemented
	},
	{
		DLMS_OBJECT_MR_ASSOCIATION_LN,                                          //  MR association = LN 
		NUM_LN_OBJ_ELEMENTS_MR,							// number of objects
		(LN_OBJ_ELEMENT *)&ln_obj_element_mr,
		(ASSOCIATED_PARTNERS_TYPE *)&associated_partners_type[1],		// associated partners
		{CHOICE_OCTSTR,7,(void *)&app_context},                                 // app context
		(XDLMS_CONTEXT_TYPE *)&xdlms_context_type,
		{CHOICE_OCTSTR,7,(void *)&authentication_mechanism_name_mr},		// mechansim
		{CHOICE_OCTSTR,MAX_SECRET_LLS_LEN,(void *)&dlms_lls_password},
		NON_ASSOCIATED
//		{0,0,43,1,0,255}							// needed only if HLS w/ GMAC is implemented
	},
	{
		DLMS_OBJECT_US_ASSOCIATION_LN,						//  US association = LN 
		NUM_LN_OBJ_ELEMENTS_US,							// number of objects
		(LN_OBJ_ELEMENT *)&ln_obj_element_us,
		(ASSOCIATED_PARTNERS_TYPE *)&associated_partners_type[2],		// associated partners
		{CHOICE_OCTSTR,7,(void *)&app_context},					// app context
		(XDLMS_CONTEXT_TYPE *)&xdlms_context_type,
		{CHOICE_OCTSTR,7,(void *)&authentication_mechanism_name_us},		// mechansim
//		{CHOICE_OCTSTR,MAX_SECRET_HLS_LEN,(void *)&dlms_hls_password},
		{CHOICE_OCTSTR,0,(void *)0},
		ASSOCIATED
//		{0,0,43,1,0,255}							// needed only if HLS w/ GMAC is implemented
	}
};
const uint8_t SIZEOF_ASSOCIATION_LN_CLASS_PC = sizeof(association_ln_class_pc)/sizeof(association_ln_class_pc[0]);
const uint8_t SIZEOF_ASSOCIATION_LN_CLASS_MR = sizeof(association_ln_class_mr)/sizeof(association_ln_class_mr[0]);
const uint8_t SIZEOF_ASSOCIATION_LN_CLASS_US = sizeof(association_ln_class_us)/sizeof(association_ln_class_us[0]);


