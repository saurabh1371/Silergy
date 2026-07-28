/*******************************************************************************
 * Copyright (C) 2020 Silergy Corp., All rights Reserved.
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
 * $Id: dlmsif_tou.c 25648 2020-06-10 11:00:29Z phuddar $
 *
 ********************************************************************************/

/********************************************************************************
*
*     Description: 
*        Filename: dlmsif_tou.c
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
#include <math.h>
#include <string.h>
#include "mmath.h"
#include "dlms_silergy_stubs.h"
#include "silergy_dlms_lib.h"
#include "eeprom.h"
#include "dlms_eeprom.h"
#include "dlms_variables.h"
#include "dlms_utils.h"
#include "dlmsif_tou.h"
#include "rtc.h"

/*
uint32_t	g_Class07_Billing_MaxEntries;
uint16_t	g_Class07_Billing_CurrentEntry;
uint32_t	g_Class07_Billing_EntriesInUse;

volatile uint32_t MDkWhStart;
uint32_t MDkWhFinish;
uint16_t MDC,MDKVAC;
uint32_t MDKvarhFinish,MDKvarhStart;

uint32_t currentzone_mdkwh,currentzone_mdkvarh;

uint32_t MD_StartDate;
uint32_t MD_PresentAddress;
uint8_t MD_currentzone;
uint32_t MD_RefAddress;

float fract_delta_kvah=0;

uint32_t dlms_poweron_dur_billing_seconds;


uint32_t kwh_start,kwh_finish,kvah_start,kvah_finish;
*/

//void dlmsif_tou_activity_calendar_reset(void);
//void dlmsif_tou_actcal_get(void);

/******************************************************************************
*     Description : Maximum demand: 
*                       1) takes care of power off conditions
*                       2) Always logs the MD for previous slot (1/2)
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_max_demand(void)
{
	uint32_t rel_date;
	uint32_t pres_date;
	uint16_t mult_factor = 2;

	MDC = 0;
	MDKVAC = 0;

    pres_date  = DateGen(YEAR,MONTH,DATE);
    rel_date = (pres_date - MD_StartDate);

	if(dlms_demand_integration_period == DLMS_DEMAND_INTEGRATION_PERIOD_15)
	{
	   	MD_SlotNumber = (uint8_t) ((4 * HOUR ) + (MIN/15));
		mult_factor = 4;
	}
	else
		MD_SlotNumber = (uint8_t) ((2 * HOUR ) + (MIN/30));
    
    MD_PresentAddress = (rel_date * MD_SlotsPerDay) + MD_SlotNumber ;

	if(MD_PresentAddress > MD_RefAddress)
	{	
		// we are into NEW time-slot
	 		MDkWhFinish = kwh_wh;//kwh;

	// 		demand integration period can be 15 or 30 seconds.
			if(MDkWhFinish > MDkWhStart)
				MDC = (uint16_t) ( ( MDkWhFinish - MDkWhStart ) * mult_factor);		// Current MD is kwh/h, here h is 1/2 hour  

			if(MDC > billing_param.kw.kw)
			{
				billing_param.kw.kw = MDC;
				billing_param.kw.date[0] = ((YEAR_100 * 100) + YEAR) >> 8;
				billing_param.kw.date[1] = ((YEAR_100 * 100) + YEAR) & 0xFF;
				billing_param.kw.date[2] = MONTH;
				billing_param.kw.date[3] = DATE;
				billing_param.kw.date[4] = (uint8_t)((DAY == 1)? 7 : (DAY-1));
				billing_param.kw.date[5] = HOUR;
				billing_param.kw.date[6] = MIN;
				billing_param.kw.date[7] = SEC;
				billing_param.kw.date[8] = 0xFF;  //operating_sec/360
				billing_param.kw.date[9] = ((dlms_time_deviation &0xFF00) >> 8);
				billing_param.kw.date[10] = (int8_t)dlms_time_deviation;
				billing_param.kw.date[11] = 0;
			}
			MDKvarhFinish = kvarh_lag + kvarh_lead;//kVah_vah;//kVah;
 
			MDKVAC = (uint16_t)sqrtf((float)((MDkWhFinish-MDkWhStart)*(MDkWhFinish-MDkWhStart)+
						((MDKvarhFinish-MDKvarhStart)*(MDKvarhFinish-MDKvarhStart))));
			MDKVAC = (uint16_t)(MDKVAC * mult_factor); 
	// 		demand integration period can be 15 or 30 seconds.
			//if(MDKvarhFinish > MDKvarhStart)
			//	MDKVAC = (MDKvarhFinish - MDKvarhStart) * mult_factor;

			if(MDKVAC > billing_param.kva.kva)
			{
				billing_param.kva.kva = MDKVAC;
				billing_param.kva.date[0] = ((YEAR_100 * 100) + YEAR) >> 8;
				billing_param.kva.date[1] = ((YEAR_100 * 100) + YEAR) & 0xFF;
				billing_param.kva.date[2] = MONTH;
				billing_param.kva.date[3] = DATE;
				billing_param.kva.date[4] = (uint8_t)((DAY == 1)? 7 : (DAY-1));
				billing_param.kva.date[5] = HOUR;
				billing_param.kva.date[6] = MIN;
				billing_param.kva.date[7] = SEC;
				billing_param.kva.date[8] = 0xFF;  //operating_sec/360
				billing_param.kva.date[9] = ((dlms_time_deviation &0xFF00) >> 8);
				billing_param.kva.date[10] = (int8_t)dlms_time_deviation;
				billing_param.kva.date[11] = 0;				
			}
	
			MDkWhStart = kwh_wh;//kwh;
			MDKvarhStart = kvarh_lag + kvarh_lead;//kVah_vah;//kVah;

			MD_RefAddress = MD_PresentAddress;

			// read activity calendar for this day and update the TZ
			MD_currentzone = dlmsif_tou_get_current_TZ(MD_SlotNumber,season_id);

			//currentzone_mdkwh = billing_param.md_tz[MD_currentzone].kwh;
			//currentzone_mdkvah = billing_param.md_tz[MD_currentzone].kvah;

      fract_delta_kvah = 0;
	}

	return;


}
*/

/******************************************************************************
*     Description : called every second. Computes/Stores billing parameters as per Table 29.
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/

/*
void dlmsif_tou_tou(void)
{
	uint32_t delta_kwh;
	uint32_t delta_kvarh;
	uint32_t delta_kvah;

	kwh_finish = kwh_wh;
	kvah_finish = kVah_vah;

	delta_kwh = 	kwh_wh - currentzone_mdkwh;
	delta_kvarh = 	(kvarh_lag + kvarh_lead) - currentzone_mdkvarh;
	delta_kvah = sqrtf((float)(delta_kwh*delta_kwh)+(float)(delta_kvarh*delta_kvarh));

	billing_param.md_tz[MD_currentzone].kwh += delta_kwh;						
//	billing_param.md_tz[MD_currentzone].kvarh += delta_kvarh;
	billing_param.md_tz[MD_currentzone].kvah += delta_kvah;

	currentzone_mdkwh = kwh_wh;
	currentzone_mdkvarh = kvarh_lag + kvarh_lead;

	dlmsif_tou_max_demand();
	
	 	
	if((kvah_finish != kvah_start) && (kwh_finish != kwh_start))
		dlms_avg_pf = 100 * (kwh_finish-kwh_start)/(kvah_finish - kvah_start);
	else
		dlms_avg_pf = 100;
}
*/

/******************************************************************************
*     Description : initialize the billing parameters and EEPROM
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/

/*
void dlmsif_tou_init(void)
{
	uint8_t i;

	kwh_start = 0;
	kwh_finish = 0;
	kvah_start = 0;
	kvah_finish = 0;

	MD_StartDate  = DateGen(YEAR,MONTH,DATE);
	billing_count = 1;

	MDkWhStart = kwh_wh;//kwh;
	MDKvarhStart = kvarh_lag + kvarh_lead;//kVah_vah;//kVah;

      dlms_poweron_dur_billing_seconds = 0;
      dlms_poweron_dur_billing_minutes = 0;

	if(dlms_demand_integration_period == DLMS_DEMAND_INTEGRATION_PERIOD_15) 
	{
    	MD_SlotNumber = (4 * HOUR) + (MIN/15);
	}
	else
	{
    	MD_SlotNumber = (2 * HOUR) + (MIN/30);
	}

	MD_currentzone = dlmsif_tou_get_current_TZ(MD_SlotNumber,season_id);

        MD_RefAddress = MD_SlotNumber ;

	currentzone_mdkwh = kwh_wh;//kwh;
	currentzone_mdkvah = kVah_vah;//kVah;
	currentzone_mdkvarh = kvarh_lag + kvarh_lead;

	memset((void *)&Tamper[0],0,sizeof(Tamper));

	for(Address1 = BILLING_PARAM_BASEADDRESS;Address1 < (BILLING_PARAM_BASEADDRESS + ((BILLING_PARAM_MAX_CYCLES+1) * BILLING_PARAM_SIZE));Address1 += sizeof(Tamper))
	{
		if( (Address1 + sizeof(Tamper)) >= (BILLING_PARAM_BASEADDRESS + ((BILLING_PARAM_MAX_CYCLES+1) * BILLING_PARAM_SIZE)))
		{
			i =  (BILLING_PARAM_BASEADDRESS + ((BILLING_PARAM_MAX_CYCLES+1) * BILLING_PARAM_SIZE)) - Address1;
			Write2eeprom((uint8_t *)&Tamper[0],Address1,i);
			break;
		}

		Write2eeprom((uint8_t *)&Tamper[0],Address1,sizeof(Tamper));		
	}

	memset((void *)&billing_param,0,sizeof(billing_param));
	memset(billing_param.kw.date,0xFF,sizeof(billing_param.kw.date));	
	memset(billing_param.kva.date,0xFF,sizeof(billing_param.kva.date));
		
	dlms_available_billing_period = 0;
	dlms_cumulative_billing_count = 0;

		Address1 = BILLING_PARAM_CURRENT_ADDRESS;
		Write2eeprom((uint8_t *)&dlms_available_billing_period,Address1,1);
		Address1 += 1;
		Write2eeprom((uint8_t *)&dlms_cumulative_billing_count,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&billing_count,Address1,1);
		Address1 += 1;
		Write2eeprom((uint8_t *)&MD_RefAddress,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&MD_StartDate,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&MDkWhStart,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&MDKvarhStart,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&dlms_poweron_dur_billing_seconds,Address1,4);
                // initialization of kvarh_tz[0..7] is done as part of 'billing_param' structure initialization above
										  

      //profile_generic_maxbilling_entries = BILLING_PARAM_MAX_CYCLES+1;
      profile_generic_maxbilling_entries = BILLING_PARAM_MAX_CYCLES;
}
*/

/******************************************************************************
*     Description : write TOU parameters (218 bytes) into EEPROM; 
*                   writes happen multiple bytes (max = 54 bytes) at a time to optimize the RAM and code execution.
*                   initialize current billing profile after bill generation.
*
*        Input(s) : billing_number: 1 to 6. 0 = current billing period
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_set(uint8_t billing_number)
{
		uint8_t i,j;
		uint32_t l_addr = BILLING_PARAM_BASEADDRESS + (billing_number * BILLING_PARAM_SIZE);

                memcpy(Tamper,dlms_billing_date,12);
#if 0
		Tamper[0] = rtc_time.tm_year >> 8;
		Tamper[1] = rtc_time.tm_year & 0xFF;
		Tamper[2] = MONTH;
		Tamper[3] = DATE;
		Tamper[4] = (DAY == 1)? 7 : (DAY-1);	  // DLMS (1=monday), 6543 (1=sunday)
		Tamper[5] = HOUR;
		Tamper[6] = MIN;
		Tamper[7] = SEC;
		Tamper[8] = 0xFF;//operating_sec/360;
		Tamper[9] = ((dlms_time_deviation &0xFF00) >> 8);
		Tamper[10] = (int8_t)dlms_time_deviation;
		Tamper[11] = 0;
#endif
		copy_to_buf(12,(void *)&dlms_avg_pf,4);
		copy_to_buf(16,(void *)&dlms_cumulative_energy_kwh,4);

		for(i=0;i<MAX_TZ;i++)
			copy_to_buf(20+(i*4),(void *)&billing_param.md_tz[i].kwh,4);

// write 1st block to eeprom   
		Address1 = l_addr;
		Write2eeprom(Tamper,Address1,(MAX_TZ*4)+4+4+12);         
		Address1 += (MAX_TZ*4)+4+4+12;

		// lag and lead not present for 1-phase
		j = 0;
		copy_to_buf(0,(void *)&kvarh_lag,4);
		copy_to_buf(4,(void *)&kvarh_lead,4);
		copy_to_buf(j,(void *)&dlms_cumulative_energy_kvah,4);

		for(i=0;i<MAX_TZ;i++)
			copy_to_buf((j+4)+(i*4),(void *)&billing_param.md_tz[i].kvah,4);

// write 2nd block to eeprom   
		Write2eeprom(Tamper,Address1,(MAX_TZ*4)+4);         
		Address1 += ((MAX_TZ*4)+4);

		copy_to_buf(0,(void *)&billing_param.kw.kw,2);
		copy_to_buf(2,(void *)&billing_param.kw.date[0],12);

// write a  block to eeprom   
		Write2eeprom(Tamper,Address1,14);
		Address1 += 14;

		for(i=0;i<MAX_TZ;i++)
		{
			copy_to_buf(0,(void *)&billing_param.md_tz[i].md_kw.kw,2);
			copy_to_buf(2,(void *)&billing_param.md_tz[i].md_kw.date[0],12);

// write a  block to eeprom   
			Write2eeprom(Tamper,Address1,14);
			Address1 += 14;
		}

			copy_to_buf(0,(void *)&billing_param.kva.kva,2);
			copy_to_buf(2,(void *)&billing_param.kva.date[0],12);


// write a block to eeprom   
			Write2eeprom(Tamper,Address1,14);
			Address1 += 14;

		for(i=0;i<dlms_no_tou;i++)
		{
			copy_to_buf(0,(void *)&billing_param.md_tz[i].md_kva.kva,2);
			copy_to_buf(2,(void *)&billing_param.md_tz[i].md_kva.date[0],12);

// write a block to eeprom 
			Write2eeprom(Tamper,Address1,14);
			Address1 += 14;
		}

// write total power on duration (in minutes not seconds) for billing
		copy_to_buf(0,(void *)&dlms_poweron_dur_billing_minutes,4);
 write a block to eeprom   
		Write2eeprom(Tamper,Address1,4);
		Address1 += 4;

	if(billing_number == 0)				  // POWER_OFF condition
	{
		Address1 = BILLING_PARAM_CURRENT_ADDRESS;
		Write2eeprom((uint8_t *)&dlms_available_billing_period,Address1,1);
		Address1 += 1;
		Write2eeprom((uint8_t *)&dlms_cumulative_billing_count,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&billing_count,Address1,1);
		Address1 += 1;
		Write2eeprom((uint8_t *)&MD_RefAddress,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&MD_StartDate,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&MDkWhStart,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&MDKvarhStart,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&dlms_poweron_dur_billing_seconds,Address1,4);// seconds is stored not minutes

		Address1 = OTHER_ADDRESS+4;
		Write2eeprom((uint8_t *)&kwh_start,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&kwh_finish,Address1,4);
		Address1 += 4;
		Write2eeprom((uint8_t *)&kvah_start,Address1,4);

    return;
  }

// initialise billing profile after bill generation.
	dlms_poweron_dur_billing_seconds = 0;
	dlms_poweron_dur_billing_minutes = 0;

	// As per CPRI test procedure, Total cumulative energy should equal sum of timezone cumulative energy
  //         Hence, initialize all values (MD values) except cumulative energy values
	
        memset((void *)&billing_param,0,sizeof(billing_param));
        memset((void *)&billing_param.kw,0,sizeof(billing_param.kw));
        memset((void *)&billing_param.kva,0,sizeof(billing_param.kva));

	memset(billing_param.kw.date,0xFF,sizeof(billing_param.kw.date));	
	memset(billing_param.kva.date,0xFF,sizeof(billing_param.kva.date));

	MDkWhStart = kwh_wh;//kwh;
	MDKvarhStart = kvarh_lag + kvarh_lead;//kVah_vah;//kVah;

	currentzone_mdkwh = kwh_wh;//kwh;
	currentzone_mdkvah = kVah_vah;//kVah;
  currentzone_mdkvarh = kvarh_lag + kvarh_lead;

	kwh_start = kwh_wh;
	kwh_finish = kwh_wh;
	kvah_start = kVah_vah;
	kvah_finish = kVah_vah;
}
*/

/******************************************************************************
*     Description : Called every 'demand integration period' 
*                     gets the current timezone for the DAY
*                     Base address of activity calendar time zones are stored in EEPROM 
*                     = DAYX_TZ_BASE + (season_id) + (MAX_TZ * TZ_OFFSET)
*                     ex: 
*                     SEASON0_TZ0    DAYX_TZ_BASEA + (0 * 50) + (0 * 0C) = A6E8
*                     SEASON0_TZ1    DAYX_TZ_BASEA + (0 * 50) + (1 * 0C) = A6F4
*                     SEASON0_TZ2    DAYX_TZ_BASEA + (0 * 50) + (2 * 0C) = A700
*                     SEASON0_TZ3    DAYX_TZ_BASEA + (0 * 50) + (3 * 0C) = A70C
*                     ..										
*                     SEASON1_TZ0    DAYX_TZ_BASEA + (1 * 50) + (0 * 0C) = A71A
*                     SEASON1_TZ1    DAYX_TZ_BASEA + (1 * 50) + (1 * 0C) = A726
*                     SEASON1_TZ2    DAYX_TZ_BASEA + (1 * 50) + (2 * 0C) = A732
*                     SEASON1_TZ3    DAYX_TZ_BASEA + (1 * 50) + (3 * 0C) = A73E
*
*
*
*        Input(s) : slotnumber: slot number as per block period (15/30)
*                   tod_set   : one of season_profile number
*       Output(s) : returns the current timezone
*           Notes : 
*
******************************************************************************/
/*
uint8_t dlmsif_tou_get_current_TZ(uint8_t slotnumber,uint8_t tod_set)
{
	uint8_t i;
	uint8_t temp[4];
	uint8_t st,fn;
	
	for(i=0;i<MAX_TZ;i++)
	{
		//j = (i == 0)? 7 : i-1;
		Address1 = DAYX_TZ_BASE_ADDRESS + ((tod_set) * ((MAX_TZ * 12)+2)) + (i * 0xC);
		Readeeprom((uint8_t *)&temp,Address1,4);                                                // start time of this time zone
		if(dlms_demand_integration_period == DLMS_DEMAND_INTEGRATION_PERIOD_15)
			st = (4 * temp[0]) + temp[1]/((DLMS_DEMAND_INTEGRATION_PERIOD_15/60)-1);        // calculate start time slot
		else
			st = (2 * temp[0]) + temp[1]/((DLMS_DEMAND_INTEGRATION_PERIOD_30/60)-1);	// calculate start time slot

	    if(i == (MAX_TZ-1))		
		{	// end of last time zone (TZ_7) should be 24:00 hours
		 	temp[0] = 23;		// hours
			temp[1] = 59;		// minutes
		}
		else
		{
			Address1 = DAYX_TZ_BASE_ADDRESS + ((tod_set) * ((MAX_TZ * 12)+2)) + ((i+1) * 0xC);
			Readeeprom((uint8_t *)&temp,Address1,4);				// end time of this time zone
		}

		if(dlms_demand_integration_period == DLMS_DEMAND_INTEGRATION_PERIOD_15)
			fn = (4 * temp[0]) + temp[1]/((DLMS_DEMAND_INTEGRATION_PERIOD_15/60)-1);	// calculate end time slot
		else
			fn = (2 * temp[0]) + temp[1]/((DLMS_DEMAND_INTEGRATION_PERIOD_30/60)-1);	// calculate end time slot

		if((slotnumber  >= st) && (slotnumber < fn))
			break;
	}

        if(i==MAX_TZ)             // if present time is < programmed_first_time_zone, consider it as 'LAST_TIME_ZONE'
            i = MAX_TZ-1;

	return i;
}
*/

//#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
/******************************************************************************
*     Description : Called on meter installation. Initialises activity calendar                
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
const uint8_t TEMP_SCRIPT[] = {0,0,10,0,100,255};
void dlmsif_tou_actcal_init(void)
{
	uint8_t i=0,j,day;
  char str[] = "SEA_";
  char wk[] = "WWK_";
  uint8_t iteration = 0;


  for(i=0;i<MAX_SEASONS;i++)
  {
      str[3] = '0' + i;
      strncpy((char *)season_profile_name[i],str,MAX_CALENDAR_NAME_LEN);
      memset(season_start_time[i],0xFF,12);
      wk[3] = '0' + i;
      strncpy((char *)season_week_name[i],(char *)wk,MAX_CALENDAR_NAME_LEN);
            
      strncpy((char *)dlms_dummy_wk[i],(char *)wk,MAX_CALENDAR_NAME_LEN);

  }

  for(iteration = 0;iteration<2;iteration++)
  {
	    if(iteration == 0)
          Address1 = ACTCAL_BASEADDRESS;
      else
          Address1 = PASCAL_BASEADDRESS;

	    dlms_no_tou = MAX_TZ;		//6;

	// active celndar name
	    strncpy((char *)&Tamper[0],"CALX",MAX_CALENDAR_NAME_LEN);
      Write2eeprom(Tamper,Address1,MAX_CALENDAR_NAME_LEN);

      Address1 += MAX_CALENDAR_NAME_LEN;
      for(i=0;i<MAX_SEASONS;i++)
      {
            // active season profile name
          j = 0;
          strncpy((char *)&Tamper[j],(char *)season_profile_name[i],MAX_CALENDAR_NAME_LEN);
            // season start
          j += MAX_CALENDAR_NAME_LEN;	
          strncpy((char *)&Tamper[j],(char *)season_start_time[i],12);
          j += 12;
          strncpy((char *)&Tamper[j],(char *)season_week_name[i],MAX_CALENDAR_NAME_LEN);
          j += MAX_CALENDAR_NAME_LEN;

          Write2eeprom(Tamper,Address1,j);
          Address1 += (MAX_CALENDAR_NAME_LEN+12+MAX_CALENDAR_NAME_LEN);
      }
   
      for(i=0;i<MAX_SEASONS;i++)
      {   // active week name
          strncpy((char *)&Tamper[0],(char *)dlms_dummy_wk[i],MAX_CALENDAR_NAME_LEN);
          Write2eeprom(Tamper,Address1,MAX_CALENDAR_NAME_LEN);
          Address1 += MAX_CALENDAR_NAME_LEN;
      }

	    for(day = 0;day < MAX_SEASONS; day++)
	    {
		      Tamper[0] = 1;
		      Tamper[1] = MAX_TZ;                     // TOU
		      Write2eeprom(Tamper,Address1,2);
		      Address1 += 2;

		      for(j=0;j<MAX_TZ;j++)
		      {
			        Tamper[0] = j * (24/MAX_TZ);
			        Tamper[1] = 0;
			        Tamper[2] = 0;
			        Tamper[3] = 255;
			        for(i=0;i<6;i++)
				          Tamper[4+i] = TEMP_SCRIPT[i];

			        Tamper[10] = 0x00;
			        Tamper[11] = j+1;
	
			        Write2eeprom(Tamper,Address1,12);
			        Address1 += 12;
		      }
	    }
  }

	// write 'activate passive calendar time' - default value.
	memset(dlms_activate_passive_calender_time,0xFF,12);

	Write2eeprom(dlms_activate_passive_calender_time,Address1,12);         // write to eeprom   

	ac_changed = 0;
	Write2eeprom((uint8_t *)&ac_changed,AC_CHANGED_ADDRESS,1);         // write to eeprom   
}
*/

/******************************************************************************
*     Description : Called on POR. Reads flag from EEPROM        
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_actcal_get(void)
{
        uint32_t l_address;
        uint8_t i;

	// read 'activity_celandar: attrib 'activate_passive_calendar_time'
	Readeeprom(dlms_activate_passive_calender_time,ACTCAL_ACTTIME_BASEADDRESS,12);

        dlms_no_tou = MAX_TZ;

	// read 'activate_calendar' configuration flag
	Readeeprom((uint8_t *)&ac_changed,AC_CHANGED_ADDRESS,1);

        l_address = ACTCAL_BASEADDRESS + 8;

        // get season start times
        for(i=0;i<MAX_SEASONS;i++)
        {
            Readeeprom(season_start_time[i],l_address,12);
            l_address += (12+8);
        }

}
*/

/******************************************************************************
*     Description : Called on power failure (sleep). Writes flag to EEPROM        
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_actcal_set(void)
{
	// write 'activity_calendar: attrib 'activate_passive_calendar_time' is written to EEPROM 
	// at the time of object_access

	// write 'activate_calendar' configuration flag
//	Write2eeprom((uint8_t *)&ac_changed,AC_CHANGED_ADDRESS,1);

        // season start times are stored at occurrence of event

}
#endif //#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
*/
/******************************************************************************
*     Description : initialise billing period reset date (single action schedule)  
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_init_billing_period_date(void)
{
  int i;
	// initialise billing period reset
        for(i=0;i<EXECUTION_BILLING_NUMBERS;i++)
        {
          //memset(billing_date,0xFF,12);
          memset(execution_billing_date[i],0xFF,12);
          Address1 = BILLING_DATE_BASEADDRESS + (i*12);
          //Write2eeprom((uint8_t *)&billing_date[0],Address1,12);
          Write2eeprom((uint8_t *)&execution_billing_date[i],Address1,12);

        }

}
*/

/******************************************************************************
*     Description : Get billing period reset dates (single action schedule) from eeprom
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_get_billing_period_date(void)
{
  int i;
  for(i=0;i<EXECUTION_BILLING_NUMBERS;i++)
  {
    Address1 = BILLING_DATE_BASEADDRESS + (i*12);
    // date[5]
    Readeeprom((uint8_t *)&execution_billing_date[i],Address1,12);
  }
}
*/

/******************************************************************************
*     Description : Save billing period reset dates (single action schedule) to eeprom
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_set_billing_period_date(void)
{
  int i;

  for(i=0;i<EXECUTION_BILLING_NUMBERS;i++)
  {
    Address1 = BILLING_DATE_BASEADDRESS + (i*12);
    Write2eeprom((uint8_t *)&execution_billing_date[i],Address1,12);
  }

}
*/

/******************************************************************************
*     Description : initialise current billing profile when 'demand_integration_period' changes
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_init_billing_profile(void)
{
	uint8_t i;
	MD_StartDate  = DateGen(YEAR,MONTH,DATE);

	MDkWhStart = kwh_wh;//kwh;
	MDKvarhStart = kVah_vah;//kVah;
	MDKvarhStart = kvarh_lag + kvarh_lead;//kVah;

	if(dlms_demand_integration_period == DLMS_DEMAND_INTEGRATION_PERIOD_15) 
	{
            MD_SlotsPerDay = 96;
            MD_SlotNumber = (4 * HOUR) + (MIN/15);
	}
	else
	{
            MD_SlotsPerDay = 48;
            MD_SlotNumber = (2 * HOUR) + (MIN/30);
	}

	MD_currentzone = dlmsif_tou_get_current_TZ(MD_SlotNumber,season_id);

    MD_RefAddress = MD_SlotNumber ;

	currentzone_mdkwh = kwh_wh;//kwh;
	currentzone_mdkvah = kVah_vah;//kVah;
	currentzone_mdkvarh = kvarh_lag + kvarh_lead;

	 //As per CPRI test procedure, Total cumulative energy should equal sum of timezone cumulative energy
      //     Hence, initialize all values (MD values) except time zone energy values
         
        memset((void *)&billing_param,0,sizeof(billing_param));
        memset((void *)&billing_param.kw,0,sizeof(billing_param.kw));
        memset((void *)&billing_param.kva,0,sizeof(billing_param.kva));

	memset(billing_param.kw.date,0xFF,sizeof(billing_param.kw.date));	
	memset(billing_param.kva.date,0xFF,sizeof(billing_param.kva.date));
		
	for(i=0;i<MAX_TZ;i++)
	{
		memset(billing_param.md_tz[i].md_kw.date,0xFF,sizeof(billing_param.md_tz[i].md_kw.date));
		memset(billing_param.md_tz[i].md_kva.date,0xFF,sizeof(billing_param.md_tz[i].md_kva.date));
	}
}
*/

/******************************************************************************
*     Description : Generate (stores) a bill when 
*                   a) single action schedule date/time is written with new value
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_billing_period_reset(void)
{
    uint16_t i;
    static uint16_t billed = 0;

    for(i=0;i<EXECUTION_BILLING_NUMBERS;i++)
    {
        if((execution_billing_date[i][0] == ((YEAR_100 * 100) + YEAR) >> 8) && 
            (execution_billing_date[i][1] == (((YEAR_100 * 100) + YEAR) & 0xFF)) &&
		(execution_billing_date[i][2] == MONTH) && 
                  (execution_billing_date[i][3] == DATE) && 
			(execution_billing_date[i][5] == HOUR) && 
                          (execution_billing_date[i][6] == MIN))

	if((billing_date[0] == (rtc_time.tm_year >> 8)) && (billing_date[1] == (rtc_time.tm_year & 0xFF)) &&
			(billing_date[2] == MONTH) && (billing_date[3] == DATE) && 
				(billing_date[5] == HOUR) && (billing_date[6] == MIN))
	{
	   if( (billed & (1 << i)) == 0)
	   {

			dlmsif_tou_generate_bill(1);				
			billed |= (1 << i);	
           }					
	}
	else
		billed &= ~(1 << i);
    }

}
*/

/******************************************************************************
*     Description : called every second.
*                   checks if activity calendar has to change
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_activity_calendar_reset(void)
{
	uint32_t rtc_dategen,dlms_activate_passive_calender_dategen;
	uint32_t pre_seconds,dlms_activate_passive_calender_time_inseconds;
        uint8_t yr;

	if(ac_changed)			// active calendar is programmed with the new value?
	{
		rtc_dategen = DateGen(YEAR,MONTH,DATE);
                yr = ((dlms_activate_passive_calender_time[0] << 8 | dlms_activate_passive_calender_time[1]))%2000;
		dlms_activate_passive_calender_dategen = DateGen(yr,
								dlms_activate_passive_calender_time[2],
								dlms_activate_passive_calender_time[3]);
		pre_seconds = (HOUR*3600)+(MIN*60)+SEC;
		dlms_activate_passive_calender_time_inseconds = dlms_activate_passive_calender_time[5]*3600 + 
								dlms_activate_passive_calender_time[6]*60 + 
								dlms_activate_passive_calender_time[7];
	
		if( (rtc_dategen >= dlms_activate_passive_calender_dategen) && 
				(pre_seconds >= dlms_activate_passive_calender_time_inseconds))
		{
				tamper_Activitycalender_Occured = 2;
			
				ac_changed = 0;
		}			

	}

}
*/

/******************************************************************************
*     Description : Copy 'passive_calendar' to 'active_calendar'
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_activate_calendar(void)
{
	uint32_t l_address = 0;
	int8_t datax;
        uint8_t i;

		while(l_address < (PASCAL_BASEADDRESS - ACTCAL_BASEADDRESS))
		{
			// copy passive calender paramsters to active calendar parameters.
			Readeeprom(Tamper,PASCAL_BASEADDRESS+l_address,12);
			Write2eeprom(Tamper,ACTCAL_BASEADDRESS+l_address,12);

			l_address += 12;

			if((l_address+12) > (PASCAL_BASEADDRESS - ACTCAL_BASEADDRESS))
				break;

		}
		// copy the last chunk of activity calendar
		datax = PASCAL_BASEADDRESS - ACTCAL_BASEADDRESS - l_address;

		Readeeprom(Tamper,PASCAL_BASEADDRESS+l_address,datax);
		Write2eeprom(Tamper,ACTCAL_BASEADDRESS+l_address,datax);

                l_address = ACTCAL_BASEADDRESS + 8;

                for(i=0;i<MAX_SEASONS;i++)
                {
                    Readeeprom(season_start_time[i],l_address,12);
                    l_address += (12+8);

                }

}
*/


/******************************************************************************
*     Description : generate a new bill and update the billing status
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_generate_bill(uint8_t event)
{
	if(event == 2)
	{  	// date/time logged is as configured by activity_calendar_date_time.
		dlms_billing_date[0] = dlms_activate_passive_calender_time[0];
		dlms_billing_date[1] = dlms_activate_passive_calender_time[1];
		dlms_billing_date[2] = dlms_activate_passive_calender_time[2];
		dlms_billing_date[3] = dlms_activate_passive_calender_time[3];
		dlms_billing_date[4] = dlms_activate_passive_calender_time[4];
		dlms_billing_date[5] = dlms_activate_passive_calender_time[5];
		dlms_billing_date[6] = dlms_activate_passive_calender_time[6];
		dlms_billing_date[7] = dlms_activate_passive_calender_time[7];
		dlms_billing_date[8] = 0xFF;//operating_sec/360;
	}
	else
        {
		dlms_billing_date[0] = ((YEAR_100 * 100) + YEAR) >> 8;
		dlms_billing_date[1] = ((YEAR_100 * 100) + YEAR) & 0xFF;
                dlms_billing_date[2] = MONTH;
                dlms_billing_date[3] = DATE;
		dlms_billing_date[4] = (DAY == 1)? 7 : (DAY-1);	  // DLMS (1=monday), 6543 (1=sunday)
		dlms_billing_date[5] = HOUR;
                dlms_billing_date[6] = MIN;
                dlms_billing_date[7] = SEC;
                dlms_billing_date[8] = 0xFF;//operating_sec/360;
                dlms_billing_date[9] = ((dlms_time_deviation &0xFF00) >> 8);
                dlms_billing_date[10] = (int8_t)dlms_time_deviation;
                dlms_billing_date[11] = 0;
        }

   		dlmsif_tou_set(billing_count);		// log billing period parameters

		billing_count++;
   		if(billing_count > BILLING_PARAM_MAX_CYCLES)			// maximum 6 billing cycles
   			billing_count = 1;

		if(dlms_available_billing_period < BILLING_PARAM_MAX_CYCLES)	// maximum 6 billing cycles
			dlms_available_billing_period++;

		dlms_cumulative_billing_count++;				// total billings since insttallation
		//memset(dlms_billing_date,0xFF,12);


	    // update the billing history
		Tamper[0] =  dlms_available_billing_period;
		copy_to_buf(1,(void *)&dlms_cumulative_billing_count,4);
		Tamper[5] = billing_count;

		Address1 = BILLING_PARAM_BASEADDRESS+((BILLING_PARAM_MAX_CYCLES+1) * BILLING_PARAM_SIZE);
		Write2eeprom((uint8_t *)&Tamper[0],Address1,6);

}
*/

/******************************************************************************
*     Description : Get TOU state variables from EEPROM
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_tou_get(void)
{
		uint8_t i,j;
		uint8_t rel_date;
		uint32_t pres_date;
		uint16_t mult_factor = 2;
		uint8_t tempyear;
		uint8_t tempmonth;
		uint8_t tempdate;
		uint8_t tempday;
		uint8_t temphour;
		uint8_t tempmin;

		Address1 = OTHER_ADDRESS+4;
		Readeeprom((uint8_t *)&Tamper[0],Address1,12);

		copy_from_buf(0,(void *)&kwh_start,4);
		copy_from_buf(4,(void *)&kwh_finish,4);
		copy_from_buf(8,(void *)&kvah_start,4);

		Address1 = BILLING_PARAM_BASEADDRESS;
		Readeeprom(Tamper,Address1,(MAX_TZ*4)+4+4+12);

		tempyear = Tamper[1];
		tempmonth = Tamper[2];
		tempdate = Tamper[3];
		tempday = Tamper[4];	  // DAY
		temphour = Tamper[5];
		tempmin = Tamper[6];

        //profile_generic_maxbilling_entries = BILLING_PARAM_MAX_CYCLES+1;
        profile_generic_maxbilling_entries = BILLING_PARAM_MAX_CYCLES;

	if(dlms_demand_integration_period == DLMS_DEMAND_INTEGRATION_PERIOD_15) 
    	{
        	MD_SlotNumber = (uint8_t)((4 * Tamper[5]) + (Tamper[6]/15));
    	}
    	else
    	{
        	MD_SlotNumber = (uint8_t)((2 * Tamper[5] + (Tamper[6]/30)));
    	}

		// get current zone at power off 
		MD_currentzone = dlmsif_tou_get_current_TZ(MD_SlotNumber,season_id);		// DLMS_DAY

		copy_from_buf(12,(void *)&dlms_avg_pf,4);
		copy_from_buf(16,(void *)&dlms_cumulative_energy_kwh,4);
		for(i=0;i<MAX_TZ;i++)
			copy_from_buf(20+(i*4),(void *)&billing_param.md_tz[i].kwh,4);

		Address1 = BILLING_PARAM_BASEADDRESS+(MAX_TZ*4)+4+4+12;
		Readeeprom(Tamper,Address1,(MAX_TZ*4)+4+4+12);

// 		lag and lead not present for 1-phase 
		j = 0;		
//		copy_from_buf(0,(void *)&kvarhlag,4);
//		copy_from_buf(4,(void *)&kvarhlead,4);

		copy_from_buf(j,(void *)&dlms_cumulative_energy_kvah,4);
		for(i=0;i<MAX_TZ;i++)
			copy_from_buf((j+4)+(i*4),(void *)&billing_param.md_tz[i].kvah,4);

		Address1 += (MAX_TZ*4)+4;
		Readeeprom(Tamper,Address1,14);

		copy_from_buf(0,(void *)&billing_param.kw.kw,2);
		copy_from_buf(2,(void *)&billing_param.kw.date[0],12);

		for(i=0;i<MAX_TZ;i++)
		{
			Address1 = BILLING_PARAM_BASEADDRESS+52+44+14+(14*i);
			Readeeprom(Tamper,Address1,14);

			copy_from_buf(0,(void *)&billing_param.md_tz[i].md_kw.kw,2);
			copy_from_buf(2,(void *)&billing_param.md_tz[i].md_kw.date[0],12);
		}

		Address1 += 14;
		Readeeprom(Tamper,Address1,14);

		copy_from_buf(0,(void *)&billing_param.kva.kva,2);
		copy_from_buf(2,(void *)&billing_param.kva.date[0],12);

		for(i=0;i<MAX_TZ;i++)
		{
			Address1 = BILLING_PARAM_BASEADDRESS+52+44+14+(14*MAX_TZ)+14+(14*i);
			Readeeprom(Tamper,Address1,14);

			copy_from_buf(0,(void *)&billing_param.md_tz[i].md_kva.kva,2);
			copy_from_buf(2,(void *)&billing_param.md_tz[i].md_kva.date[0],12);
		}

		// Read 'dlms_available_billing_period' and 'dlms_cumulative_billing_count'

		Address1 = BILLING_PARAM_CURRENT_ADDRESS;
		Readeeprom((uint8_t *)&dlms_available_billing_period,Address1,1);
		Address1 += 1;
		Readeeprom((uint8_t *)&dlms_cumulative_billing_count,Address1,4);
		Address1 += 4;
		Readeeprom((uint8_t *)&billing_count,Address1,1);
		Address1 += 1;
		Readeeprom((uint8_t *)&MD_RefAddress,Address1,4);
		Address1 += 4;
		Readeeprom((uint8_t *)&MD_StartDate,Address1,4);
		Address1 += 4;
		Readeeprom((uint8_t *)&MDkWhStart,Address1,4);
		Address1 += 4;
		Readeeprom((uint8_t *)&MDKvarhStart,Address1,4);
		Address1 += 4;
		Readeeprom((uint8_t *)&dlms_poweron_dur_billing_seconds,Address1,4);
		

		currentzone_mdkwh = billing_param.md_tz[MD_currentzone].kwh;
		currentzone_mdkvah = billing_param.md_tz[MD_currentzone].kvah;
		currentzone_mdkwh = kwh_wh;//kwh;
		currentzone_mdkvah = kVah_vah;//kVah;
		currentzone_mdkvarh = kvarh_lag + kvarh_lead;

	// Store the previous MD parameter if power came ON in next 'demand integration' slot(s)
	MDC = 0;
	MDKVAC = 0;

    pres_date  = DateGen(YEAR,MONTH,DATE);
    rel_date = (pres_date - MD_StartDate);

	if(dlms_demand_integration_period == DLMS_DEMAND_INTEGRATION_PERIOD_15)
	{
	   	MD_SlotNumber = (4 * HOUR ) + (MIN/15);
		mult_factor = 4;
	}
	else
		MD_SlotNumber = (2 * HOUR ) + (MIN/30);
    
    MD_PresentAddress = (rel_date * MD_SlotsPerDay) + MD_SlotNumber ;

	if(MD_PresentAddress > MD_RefAddress)
	{	
		// stored values at power off instance
		for(;;)
		{
			if(tempmin % (dlms_demand_integration_period/60) == 0)
				break;
			else
			{
				tempmin++;
				if(tempmin >= 60)
				{	tempmin = 0;
					temphour++;
					if(temphour >= 24)
					{	temphour = 0;
						tempdate++;
						tempday++;
						if(tempday >= 8)	tempday = 1;
						if(tempdate >= MonthArray[tempmonth])
						{	tempdate = 1;
							tempmonth++;
							if(tempmonth > 12)
							{	tempmonth = 1;
								tempyear++;
							}
						}
					}
				}
			}
		}

		// we are into NEW time-slot
	 		MDkWhFinish = kwh_wh;

                // demand integration period can be 15 or 30 seconds.
			if(MDkWhFinish > MDkWhStart)
				MDC = ( MDkWhFinish - MDkWhStart ) * mult_factor;		// Current MD is kwh/h, here h is 1/2 hour  

			if(MDC > billing_param.kw.kw)
			{
				billing_param.kw.kw = MDC;
				billing_param.kw.date[0] = ((YEAR_100 * 100) + YEAR) >> 8;
				billing_param.kw.date[1] = ((YEAR_100 * 100) + YEAR) & 0xFF;
				billing_param.kw.date[2] = tempmonth;
				billing_param.kw.date[3] = tempdate;
				billing_param.kw.date[4] = tempday;
				billing_param.kw.date[5] = temphour;
				billing_param.kw.date[6] = tempmin;
				billing_param.kw.date[7] = 0;
				// other date params not used
			}
			MDKvarhFinish = kvarh_lag + kvarh_lead;//kVah_vah;//kVah;
	// 		demand integration period can be 15 or 30 seconds.

			MDKVAC = sqrtf((float)((MDkWhFinish-MDkWhStart)*(MDkWhFinish-MDkWhStart)+
						((MDKvarhFinish-MDKvarhStart)*(MDKvarhFinish-MDKvarhStart))));
			MDKVAC = MDKVAC * mult_factor; 

			if(MDKvarhFinish > MDKvarhStart)
				MDKVAC = (MDKvarhFinish - MDKvarhStart) * mult_factor;

			if(MDKVAC > billing_param.kva.kva)
			{
				billing_param.kva.kva = MDKVAC;
				billing_param.kva.date[0] = ((YEAR_100 * 100) + YEAR) >> 8;
				billing_param.kva.date[1] = ((YEAR_100 * 100) + YEAR) & 0xFF;
				billing_param.kva.date[2] = tempmonth;
				billing_param.kva.date[3] = tempdate;
				billing_param.kva.date[4] = tempday;
				billing_param.kva.date[5] = temphour;
				billing_param.kva.date[6] = tempmin;
				billing_param.kva.date[7] = 0;
				// other date params not used				
			}
	
			MDkWhStart = kwh_wh;//kwh;
			MDKvarhStart = kvarh_lag + kvarh_lead;//kVah_vah;//kVah;

			MD_RefAddress = MD_PresentAddress;

			// read activity calendar for this day and update the TZ
			MD_currentzone = dlmsif_tou_get_current_TZ(MD_SlotNumber,season_id);

			currentzone_mdkwh = billing_param.md_tz[MD_currentzone].kwh;
			currentzone_mdkvah = billing_param.md_tz[MD_currentzone].kvah;

	}

}
*/

/******************************************************************************
*     Description : Called through timer interrupts (per minute). updates on duration of meter
*
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlms_tou_pon_incr(void)
{

    // Total (cumulative) power on duration ONLY for billing period
    // gets reset every billing cycle
    dlms_poweron_dur_billing_seconds++;
    dlms_poweron_dur_billing_minutes = dlms_poweron_dur_billing_seconds/60;
}
*/

/******************************************************************************
*     Description : check season change and increment the season_id
*
*        Input(s) : none
*       Output(s) : season_id
*           Notes : season_id gets updated based on the season profile start_time
*
******************************************************************************/
/*
void dlmsif_tou_season_reset(void)
{
    uint32_t rtc_dategen,season_dategen[MAX_SEASONS];
    uint32_t rtc_secs,season_seconds[MAX_SEASONS];
    //uint8_t yr[MAX_SEASONS];
    uint8_t i;

    rtc_dategen = DateGen(YEAR,MONTH,DATE);
    rtc_secs = (HOUR*3600)+(MIN*60);

    for(i=0;i<MAX_SEASONS;i++)
    {
        season_dategen[i] = DateGen(YEAR,season_start_time[i][2],season_start_time[i][3]);
        season_seconds[i] = season_start_time[i][5]*3600 + season_start_time[i][6]*60;

        //yr[(i+1)%MAX_SEASONS] = ((season_start_time[(i+1)%MAX_SEASONS][0] << 8 | season_start_time[(i+1)%MAX_SEASONS][1]))%2000;

        season_dategen[(i+1)%MAX_SEASONS] = DateGen(YEAR,season_start_time[(i+1)%MAX_SEASONS][2],season_start_time[(i+1)%MAX_SEASONS][3]);
        season_seconds[(i+1)%MAX_SEASONS] = season_start_time[(i+1)%MAX_SEASONS][5]*3600 + season_start_time[(i+1)%MAX_SEASONS][6]*60;
                
        if(i == (MAX_SEASONS-1))    // this should be next year
          season_dategen[(i+1)%MAX_SEASONS] = DateGen(YEAR+1,season_start_time[(i+1)%MAX_SEASONS][2],season_start_time[(i+1)%MAX_SEASONS][3]);

        if( ((rtc_dategen >= season_dategen[i]) && (rtc_dategen < season_dategen[(i+1)%MAX_SEASONS])) ||
                ((rtc_dategen >= season_dategen[i]) && (rtc_dategen == season_dategen[(i+1)%MAX_SEASONS]) && (rtc_secs < season_seconds[(i+1)%MAX_SEASONS])) )
        {
            season_id = i;
            break;
        }
    }

    if(i == MAX_SEASONS)
      season_id = 0;        // error case. ideally code should not be here.

    return;   
}
*/
