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
  * $Id: dlmsif_loadsurvey.c 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/

/********************************************************************************
*
*     Description: 
*        Filename: dlmsif_loadsurvey.c
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
#include "eeprom.h"
#include "dlms_eeprom.h"
#include "dlms_silergy_stubs.h"
#include "silergy_dlms_lib.h"
#include "dlms_variables.h"

#include "dlmsif_loadsurvey.h"
#include "dlms_utils.h"
#include "rtc.h"
#include "afe.h"

#include "Defines.h"

/*
#define FORWARD 	0
#define BACKWORD 	1	

#define FS_INT            ce_ptr->fs    //fs                // defined in afe.h
#define SUM_SAMP          ce_ptr->sum_samp//SUMSAMPS          // defined in afe.h



// variables

uint8_t LS_adjust_refmin;

uint8_t	LS_refyear,LS_refmonth,LS_refdate;
uint8_t	LS_refday, LS_refhour, LS_refmin,LS_refsec;
uint32_t LS_refir = 0,LS_refiy = 0,LS_refib = 0;
uint32_t LS_refvr = 0,LS_refvy = 0,LS_refvb = 0;
uint32_t LS_refkwh=0,LS_refkvarh_lag=0,LS_refkvarh_lead=0,LS_refkvah=0;
uint32_t LS_kwh_start = 0,LS_kvarhlag_start = 0,LS_kvarhlead_start = 0,LS_kvah_start = 0;

int32_t LS_StartDate;
uint16_t LS_numentries;
*/


/******************************************************************************
*   Function Name : dlmsif_loadsurvey_lsdata_log
*     Description : log the data to eeprom. 
*                   Each entry has time/date,ir,iy,ib,vr,vy,vb,kwh,kvarhlag,kvarhlead,kvah
*        Input(s) : fromaddr - load survey (initial) address offset in the eeprom
*                   toaddr -   load survey (end) address offset in the eeprom
*                   direction - FORWARD = fill the log from present time to mid night
*                               BACKWARD = fill the log from mod night to present time.
*       Output(s) : None
*        
*            Notes: if the power was off for more than a 'day', 
*                   we need to re-create the date/time in forward and backward directions.
*
******************************************************************************/
/*
void dlmsif_loadsurvey_lsdata_log(int32_t fromaddr,int32_t toaddr,uint8_t direction)
{
	int32_t j;
	uint8_t temphour=0;
	uint8_t tempmin,tempmon,tempdate,tempday;
  tempmin=tempmon=tempdate=tempday=0;        // remove compiler warnings.
	
//	if(dlms_profile_capture_period == 900)
//		tempmin = 15;
//	else
//		tempmin = 30;

//	for(j=fromaddr;j<toaddr;j++)
//	{
//		switch(direction)
//		{
//		 	case FORWARD:				// re-create date/time in forward direction
//				if(j!=fromaddr)
//				{   // for power off, the block_load is zero
//					LS_refvr = 0;
//					LS_refir = 0;
//					LS_refkwh = 0;
//					LS_refkvarh_lag = 0;
//					LS_refkvarh_lead = 0;
//					LS_refkvah = 0;

//					LS_refmin += (dlms_profile_capture_period/60);
//					if(LS_refmin >= 60)
//					{	LS_refmin = 0;
//						LS_refhour++;
//						if(LS_refhour >= 24)
//						{	LS_refhour = 0;
//							LS_refdate++;
//							LS_refday++;
//							if(LS_refday >= 8)	LS_refday = 1;
//							if(LS_refdate > MonthArray[LS_refmonth])
//							{	LS_refdate = 1;
//								LS_refmonth++;
//								if(LS_refmonth > 12)
//								{	LS_refmonth = 1;
//									LS_refyear++;
//								}
//							}
//						}
//					}
//				}
//				

//			break;

//			case BACKWORD:				// re-create date/time in backword direction
//					tempmon = MONTH;
//					tempdate = DATE;
//					tempday = DAY;

//					LS_refvr = 0;
//			    LS_refir = 0;
//					LS_refkwh = 0;
//					LS_refkvarh_lag = 0;
//					LS_refkvarh_lead = 0;
//					LS_refkvah = 0;
//		
//					if(j!=fromaddr)
//					{
//						tempmin += (dlms_profile_capture_period/60);
//						if(tempmin >= 60)
//						{	tempmin = 0;
//							temphour++;
//						}
//					}
//			break;
//		}

//		tempday = (direction == FORWARD) ? LS_refday : DAY;

//		Tamper[0] = ((YEAR_100 * 100) + LS_refyear) >> 8;
//		Tamper[1] = ((YEAR_100 * 100) + LS_refyear) & 0xFF;
//		Tamper[2] = (direction == FORWARD) ? LS_refmonth : tempmon;
//		Tamper[3] = (direction == FORWARD) ? LS_refdate : tempdate;
//		Tamper[4] = (tempday == 1)? 7 : (tempday - 1);	  // DLMS (1=monday), 6543 (1=sunday)
//		Tamper[5] = (direction == FORWARD) ? LS_refhour : temphour;
//		Tamper[6] = (direction == FORWARD) ? LS_refmin  : tempmin;
//		Tamper[7] = (direction == FORWARD) ? LS_refsec  : 0;
//		Tamper[8] = 0xFF;//operating_sec/360;
//		Tamper[9] = ((dlms_time_deviation &0xFF00) >> 8);
//		Tamper[10] = (int8_t)dlms_time_deviation;
//		Tamper[11] = 0;

//		copy_to_buf(12,(void *)&LS_refvr,4);

//		copy_to_buf(16,(void *)&LS_refkwh,4);
//		copy_to_buf(20,(void *)&LS_refkvah,4);

//    // write ir,max_demand to EEPROM
//    copy_to_buf(24,(void *)&LS_refir,4);
//		copy_to_buf(28,(void *)&billing_param.kw.kw,2);
//		copy_to_buf(30,(void *)&billing_param.kw.date[0],12);
//		
//		Address1 = LOADSURVEY_BASE_ADDRESS + (j * LOADSURVEY_BUF_SIZE);
//		Write2eeprom( Tamper, Address1, LOADSURVEY_BUF_SIZE);            // Write to eeprom (buffer,start address, no of bytes)   
//		
//		// log number of load survey entries
//		if(LS_numentries < (LS_Maxdays * LS_SlotsPerDay))
//			LS_numentries++;


//		Address1 = LOADSURVEY_REF_ADDRESS;
//		Write2eeprom( (uint8_t *)&LS_numentries, Address1, 2);            // Write to eeprom (buffer,start address, no of bytes)   

//	} 	
}
*/

/******************************************************************************
*   Function Name : dlmsif_loadsurvey_init
*     Description : Called on fresh meter initialization. Clear the entries, Present date/time becomes a reference
*                   Each entry has time/date,ir,iy,ib,vr,vy,vb,kwh,kvarhlag,kvarhlead,kvah
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_loadsurvey_init(void)
{
//	uint8_t i,j;
//	uint16_t location;
//	
//	LS_adjust_refmin = 0;

//	LS_RefAddress = 0;
//	LS_numentries = 0;
//	
//	Billing_Profile_Capture_Period = 1800;
//	Write2eeprom((uint8_t *) &Billing_Profile_Capture_Period,MD_INTGR_LOC,2);         // MD INTGR   
//	Load_Profile_Capture_Period = 1800;
//	Write2eeprom((uint8_t *) &Load_Profile_Capture_Period,SURVEY_INTGR_LOC,2);         // SURV INTGR  
//	
	
//	memset(Tamper,0x00,TAMPER_BUF_SIZE);
//	LS_SlotsPerDay = 24 * (3600/dlms_profile_capture_period);							// 15 or 30 minutes interval.
	
//	if(LS_SlotsPerDay == 48)
//		LS_Maxdays = MAX_SURVEY_DAYS;//35;		
//	else
//		LS_Maxdays = MAX_SURVEY_DAYS/2;//17;					

//        dlms_profile_max_bl_entries = LS_Maxdays * LS_SlotsPerDay;

//	Address1 = LOADSURVEY_REF_ADDRESS;
//	Write2eeprom(Tamper, Address1, 2+4+4);	// clear LS_numentries,LS_startdate,LS_refAddress            
//	Address1 = LOADSURVEY_REF_ADDRESS+10;
//	Write2eeprom(Tamper, Address1, LOADSURVEY_REFBUF_SIZE);	// clear current LS data

//	Address1 = LOADSURVEY_BASE_ADDRESS;
//	for(i=0;i<LS_Maxdays;i++)
//	{
//		for(j=0;j<LS_SlotsPerDay;j++)
//		{
//			Write2eeprom( Tamper, Address1, 2);		// CLEAR first 2 bytes for each entry.
//			Address1 += LOADSURVEY_BUF_SIZE;
//		}
//	}
//#if 0   // debugging only
//	Address1 = LOADSURVEY_BASE_ADDRESS;
//        memset(Tamper,0xFF,52);
//	for(i=0;i<LS_Maxdays;i++)
//	{
//		for(j=0;j<LS_SlotsPerDay;j++)
//		{
//			Readeeprom( Tamper, Address1, 2);		// CLEAR first 2 bytes for each entry.
//			Address1 += LOADSURVEY_BUF_SIZE;
//                        if( (Tamper[0] != 0x00) && (Tamper[01] != 0x00))
//                          for(;;);     //error
//		}
//	}
//#endif

//	LS_StartDate = DateGen(YEAR,MONTH,DATE);

//	LS_refyear = YEAR;LS_refmonth = MONTH;LS_refdate = DATE;LS_refday  = DAY; 
//	LS_refhour = 0;	LS_refmin  = 30;	LS_refsec  = 0;

//	if(dlms_profile_capture_period == 900)		// 15 * 60
//		LS_refmin  = 15;
//					
//	// initial values
//	LS_refvr = 0;
//	LS_refir = 0;
//	LS_kwh_start = kwh_wh;//kwh;
//	LS_kvah_start = kVah_vah;

//	LS_refkwh        = 0;
//	LS_refkvarh_lag  = 0;
//	LS_refkvarh_lead = 0;
//	LS_refkvah		 = 0;
	
}
*/

/******************************************************************************
*   Function Name : dlmsif_loadsurvey_get
*     Description : Called on POR. Gets the reference date/time, load survey parameters (10 numbers) and other parameters
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_loadsurvey_get(void)
{
//	int32_t LS_PresentDate;
//	uint8_t LS_RelativeDate;
//	uint8_t LS_SlotNumber;
//	int32_t LS_PresentAddress;

//	//Block load survey	init
//	Readeeprom((uint8_t *) &g_Class07_Blockload_CurrentEntry,HRPOS_LOC,2);         // Read from eeprom   
//	Readeeprom((uint8_t *) &g_Class07_Blockload_EntriesInUse,HRCNT_LOC,2);         // Read from eeprom   
//	if(g_Class07_Blockload_EntriesInUse>g_Class07_Blockload_MaxEntries)
//			g_Class07_Blockload_EntriesInUse=g_Class07_Blockload_MaxEntries;
//	
//	//daily load survey init	
//	Readeeprom((uint8_t *) &g_Class07_Dailyload_EntriesInUse,DAYCNT_LOC,2);         // Read from eeprom   
//	if(g_Class07_Dailyload_EntriesInUse>=g_Class07_Dailyload_MaxEntries)
//		g_Class07_Dailyload_EntriesInUse=g_Class07_Dailyload_MaxEntries;	
}
*/

/******************************************************************************
*   Function Name : dlmsif_loadsurvey_set
*     Description : Called every block period (15/30 minutes). Store the Load survey parameters ((10 entries) and others in EEPROM
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_loadsurvey_set(void)
{
//		copy_to_buf(0,(void *)&LS_numentries,2);
//		copy_to_buf(2,(void *)&LS_StartDate,4);
//		copy_to_buf(6,(void *)&LS_RefAddress,4);

//		Address1 = LOADSURVEY_REF_ADDRESS;
//		Write2eeprom(Tamper,Address1,2+4+4);         // write to eeprom   

//		Tamper[0] = ((YEAR_100 * 100) + LS_refyear) >> 8;
//		Tamper[1] = ((YEAR_100 * 100) + LS_refyear) & 0xFF;
//		Tamper[2] = LS_refmonth;
//		Tamper[3] = LS_refdate;
//		Tamper[4] = (LS_refday == 1)? 7 : (LS_refday-1);	  // DLMS (1=monday), 6543 (1=sunday)
//		Tamper[5] = LS_refhour;
//		Tamper[6] = LS_refmin;
//		Tamper[7] = LS_refsec;
//		Tamper[8] = 0xFF;//operating_sec/360;
//		Tamper[9] = ((dlms_time_deviation &0xFF00) >> 8);
//		Tamper[10] = (int8_t)dlms_time_deviation;
//		Tamper[11] = 0;																	 

//		copy_to_buf(12,(void *)&LS_refvr,4);

//		copy_to_buf(16,(void *)&LS_kwh_start,4);
//		copy_to_buf(20,(void *)&LS_kvarhlag_start,4);
//		copy_to_buf(24,(void *)&LS_kvarhlead_start,4);
//		copy_to_buf(28,(void *)&LS_kvah_start,4);
//		copy_to_buf(0,(void *)&LS_refir,4);
//		
//		Address1 += 10;
//		Write2eeprom(Tamper,Address1,LOADSURVEY_REFBUF_SIZE);         // write to eeprom   	
}
*/

/******************************************************************************
*   Function Name : dlmsif_loadsurvey
*     Description : Called every second.
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_loadsurvey(void)
{
    int32_t LS_PresentDate;
    uint8_t LS_RelativeDate;
    uint8_t LS_SlotNumber;
    int32_t LS_PresentAddress;
    float accumulation_time;
    
	  uint32_t current_r = dlms_phase_current;
    uint32_t volt_r = dlms_voltage;

//    LS_PresentDate  = DateGen(YEAR,MONTH,DATE);
//    
//    LS_RelativeDate = (LS_PresentDate - LS_StartDate) ;
//    
//	// if the relative date is >= LS_Maxdays then we had power loss for more than 'LS_Maxdays' days. 
//	//   					  Re-initialise EEPROM data 

//    LS_SlotNumber = SlotGen(LS_SlotsPerDay,MIN);
//    
//    LS_PresentAddress = LS_RelativeDate * LS_SlotsPerDay + LS_SlotNumber ;

//	if(LS_PresentAddress >= (LS_SlotsPerDay*LS_Maxdays))
//	{
//		LS_PresentAddress = 0;
//		LS_StartDate = LS_PresentDate;
//	}

//	if( (LS_PresentAddress > LS_RefAddress) || ( (LS_PresentAddress == 0) && (LS_RefAddress == (LS_SlotsPerDay*LS_Maxdays)-1)))
//	{	
//		// we are into NEW time-slot as per RTC time

//                // Note that samples accumulation is not same as RTC seconds. So,
//                //adjust the accumulation time as per CE accumulation interval
//                accumulation_time = dlms_profile_capture_period * ((float)FS_INT/(float)SUM_SAMP);

//		if( (LS_PresentAddress - LS_RefAddress) >= 2)	
//		{   // power off condition; 
//			if( ((LS_PresentAddress/LS_SlotsPerDay) - (LS_RefAddress/LS_SlotsPerDay)) >= 2)
//			{
//				// power off condition for whole days. Adjust for 'power off' days (do not store values during power off days)
//				// 1.	fill remaining day with '0'
//				LS_refvr /= accumulation_time;
//        LS_refir /= accumulation_time;
//				
//				dlmsif_loadsurvey_lsdata_log(LS_RefAddress,(1 + LS_RefAddress/LS_SlotsPerDay) * LS_SlotsPerDay,FORWARD);

//				// 2. Adjust startdate 
//				LS_StartDate = LS_StartDate + (LS_PresentAddress/LS_SlotsPerDay) - (LS_RefAddress/LS_SlotsPerDay) - 1;
//				LS_RelativeDate = LS_PresentDate - LS_StartDate;
//				LS_RefAddress = LS_RelativeDate * LS_SlotsPerDay;
//				LS_PresentAddress = LS_RelativeDate * LS_SlotsPerDay + LS_SlotNumber ;

//				if(LS_adjust_refmin == 1)
//					LS_adjust_refmin = 0;
//				else
//				{
//					LS_refyear = YEAR;LS_refmonth = MONTH;LS_refdate = DATE;
//					LS_refday  = DAY; LS_refhour = HOUR; 	LS_refmin  = MIN;
//					LS_refsec  = SEC;
//				}
//				// 3.  fill start of the day till present address with '0'

//				dlmsif_loadsurvey_lsdata_log(LS_RefAddress,LS_PresentAddress,BACKWORD);

//				LS_RefAddress = LS_PresentAddress;

//				LS_refvr = volt_r;
//				LS_refir = current_r;
//		
//				LS_kwh_start       = kwh_wh;//kwh;
//				LS_kvarhlag_start  = kvarh_lag;
//				LS_kvarhlead_start = kvarh_lead;
//				LS_kvah_start	   = kVah_vah;//kVah;

//			}
//			else
//			{	// power off condition; we did not have power for at-least one time slot 
//				LS_refvr /= accumulation_time;
//        LS_refir /= accumulation_time;
//				
//				dlmsif_loadsurvey_lsdata_log(LS_RefAddress,LS_PresentAddress,FORWARD);	// log data for powered-off slots
//				LS_adjust_refmin = 0;
//				LS_RefAddress = LS_PresentAddress;

//				LS_refvr = volt_r;
//		    LS_refir = current_r;
//				
//				LS_kwh_start       = kwh_wh;//kwh;
//				LS_kvarhlag_start  = kvarh_lag;
//				LS_kvarhlead_start = kvarh_lead;
//				LS_kvah_start	   = kVah_vah;//kVah;

//			}
//		}
//		else
//		{
//			// log the values of previous time slot in EEPROM ; Could be a power failure within the 'capture_period' time
//			LS_refkwh        = kwh_wh - LS_kwh_start; //kwh
//			LS_refkvarh_lag  = kvarh_lag - LS_kvarhlag_start;
//			LS_refkvarh_lead = kvarh_lead - LS_kvarhlead_start;
//			LS_refkvah		 = sqrtf((float)((LS_refkwh*LS_refkwh)+(LS_refkvarh_lag+LS_refkvarh_lead)*(LS_refkvarh_lag+LS_refkvarh_lead)));
//			//LS_refkvah = kVah_vah - LS_kvah_start; //kVah
//	
//			LS_refvr += volt_r;
//	    LS_refir += current_r;
//			
//			LS_refvr /= accumulation_time;
//      LS_refir /= accumulation_time;
//			
//			if(LS_adjust_refmin == 1)
//				LS_adjust_refmin = 0;
//			else
//			{
//                                LS_refyear = YEAR;LS_refmonth = MONTH;LS_refdate = DATE;
//                                LS_refday  = DAY; LS_refhour = HOUR; 	LS_refmin  = MIN;
//                                LS_refsec  = SEC;
//			}

//			dlmsif_loadsurvey_lsdata_log(LS_RefAddress,LS_PresentAddress,FORWARD);
//			LS_RefAddress = LS_PresentAddress;
//			// record current values
//			LS_refvr = volt_r;
//	    LS_refir = current_r;
//			
//			LS_kwh_start       = kwh_wh;//kwh;
//			LS_kvarhlag_start  = kvarh_lag;
//			LS_kvarhlead_start = kvarh_lead;
//			LS_kvah_start	   = kVah_vah;//kVah;
//		}

//	}
//	else
//	{
//                LS_refyear = YEAR;LS_refmonth = MONTH;LS_refdate = DATE;
//                LS_refday  = DAY; LS_refhour = HOUR; 	LS_refmin  = MIN;
//                LS_refsec  = SEC;
//		LS_refvr += volt_r;
//		LS_refir += current_r;
//	}

}
*/
