/******************************************************************************* 
 * Copyright (C) 2013 .. 2020 Silergy Corp, All rights Reserved.
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
  * $Id: dlms_silergy_stubs.c 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/ 

/********************************************************************************
*
*     Description: 
*        Filename: dlms_maxim_stubs.c
*
*        Compiler: uVision ARMCC.exe
*
*         Version: uVision 5.27.1
*
*         Created: 				
*							
*   Modifications: 
*         14/Aug/2019 - Re-arranged header files includes
*         07/Feb/2020 - Added callback API uint8_t dlms_obj_is_billing()
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "dlms_variables.h"
#include "silergy_dlms_lib.h"
#include "dlms_objmgr_lib.h"
#include "eeprom.h"
#include "dlms_eeprom.h"

#include "dlms_silergy_stubs.h"

#include "rtc.h"
#include "time.h"
#include "dlmsif_tou.h"

#include "Defines.h"
#include "asdaq_variables.h"

unsigned char test_val;
unsigned char test_arr[3][100];
unsigned char call_ctr;

// set date and time
// temp: data and time in DLMS data_time format
void dlms_set_date_time(uint8_t temp[])
{
	tm_t test_time;

  int32_t year = (temp[0] << 8) | temp[1];
	if(year != 0xFFFF)
	{
			// YEAR_100 = 20;
			test_time.tm_year = (int8_t)(year % 100L);
	}
    
  test_time.tm_mon = -1 + (int8_t)((temp[2] >= 1) ? temp[2] : 1);
  test_time.tm_mday = temp[3];
  test_time.tm_wday = (int8_t)((temp[4] >= 7) ? 0 : (temp[4]));

  test_time.tm_hour = (int8_t)((temp[5] > 23) ? 23 : temp[5]);
  test_time.tm_min =  (int8_t)((temp[6] > 59) ? 59 : temp[6]);
  test_time.tm_sec = (int8_t)((temp[7] > 59) ? 50 : temp[7]);

	test_time.tm_isdst = 1;
	test_time.tm_isvalid = false;
	
	dlms_time_deviation = (uint16_t)temp[9] << 8 | temp[10];
	
	time_write(&test_time);
	//rtc_write (&test_time);
#if 0	
  int32_t year = (temp[0] << 8) | temp[1];
	if(year != 0xFFFF)
	{
			// YEAR_100 = 20;
			global.reg.tm.tm_year = (int8_t)(year % 100L);
	}
    
  global.reg.tm.tm_mon = -1 + (int8_t)((temp[2] >= 1) ? temp[2] : 1);
  global.reg.tm.tm_mday = temp[3];
  global.reg.tm.tm_wday = (int8_t)((temp[4] >= 7) ? 0 : (temp[4]));

  global.reg.tm.tm_hour = (int8_t)((temp[5] > 23) ? 23 : temp[5]);
  global.reg.tm.tm_min =  (int8_t)((temp[6] > 59) ? 59 : temp[6]);
  global.reg.tm.tm_sec = (int8_t)((temp[7] > 59) ? 50 : temp[7]);

	global.reg.tm.tm_isdst = 1;
	global.reg.tm.tm_isvalid = false;


  dlms_time_deviation = (uint16_t)temp[9] << 8 | temp[10];

  //rtc_isdst = true;

//  if ( rtc_valid () )
    //rtc_write (&global.reg.tm);
#endif   
   
}

/** \fn uint8_t dlms_obj_setval(uint8_t class,int16_t object_index,int8_t attribute_id,uint8_t choice,void *data_ptr,uint8_t size);
 *  \brief set the value for the object attribute
 *
 *  @param[in] class class identifier.
 *  @param[in] object_index 	object index in the list.
 *  @param[in] attribute_id		attribute index in the object.
 *  @param[in] choice			data type of value.
 *  @param[in,out] data_ptr		pointer to the value.
 *  @param[in] size			size of the value.
 *	@return 0 on success, else fail
 */

uint8_t dlms_obj_setval(uint8_t class,int16_t object_index,int8_t attribute_id,uint8_t choice,void *data_ptr,uint8_t size)
{
	uint8_t error = 1;
  //      tm_t temp_time;
	uint32_t *temp32;
	//uint16_t temp16;
	uint8_t *temp;
	uint8_t tmp_hr, tmp_min; //day_id,tou, 
	//uint32_t l_address;
  uint16_t i, j, tmp_int;

	temp = 0;        // remove compiler warnings.
	temp32 = 0;      // remove compiler warnings.
	
	if(choice == CHOICE_UDL)
		temp32 = (uint32_t *)data_ptr;
	//else if(choice == CHOICE_UL)
	//	temp16 = (uint16_t *)data_ptr;
	else if(choice == CHOICE_OCTSTR)
		temp = (uint8_t *)data_ptr;
	else
		;

	switch(class)
	{
	 	case E_CLASS_DATA:

			if(attribute_id != 2)
				return error;

			switch(object_index)
			{
			 	case 5: // write = data::profile_capture_period											//See DLMS_OBJECT_PROFILE_CAPTUREPERIOD	 object
					if((*temp32 == 900) || (*temp32 == 1800))
					{
						//tamper_Profile_capture_Occured = 1;
						Load_Profile_Capture_Period=*temp32;
						set_capture_period=1;
						error = 0;
					}
				break;
				case 14: // write = data::demand_integration_period	
					if((*temp32 == 900) || (*temp32 == 1800) || (*temp32 == 3600))	
					{
						//tamper_Demand_integration_Occured = 1;
						Max_Demand_Capture_Period=*temp32;
						set_demand_intgr_period=1;
						error = 0;
						
						/*
						if(*temp32 == 3600)
						{
							*temp32 = 1800;
							temp16 = 0;
							Write2eeprom( (uint8_t *)&temp16, ADDRESS_INITMETER, 2);
							SYS->MOD_CNTL |= BIT31;
							while(1);
						}
						*/
						
					}

				break;
				case 15:  // write - data::DLMS_OBJECT_USEROBJECT
					error = 0;
				break;					
				case 16:  // write - data::DLMS_OBJECT_DIAGNOSTICS
					error = 0;
				break;
			}
			break;

		case E_CLASS_CLOCK:

		if(attribute_id != 2)
				return error;

			memcpy(time_string,temp,12); //	
			set_rtc_data=1;
		  error=0;
		
		/*
		// set the tamper flag only if date/time is different (don't care about seconds)
		if((temp[0] != (((YEAR_100 * 100) + YEAR) >> 8)) || (temp[1] != (((YEAR_100 * 100) + YEAR) & 0xFF)) ||
			(temp[2] != MONTH) || (temp[3] != DATE) || 
				(temp[5] != HOUR) || (temp[6] != MIN))		
		{
				tamper_Rtc_Occured = 1;

				rtc_tamper_date[0] = ((YEAR_100 * 100) + YEAR) >> 8;
				rtc_tamper_date[1] = ((YEAR_100 * 100) + YEAR) & 0xFF;
				rtc_tamper_date[2] = MONTH;
				rtc_tamper_date[3] = DATE;
				rtc_tamper_date[4] = (DAY == 1)? 7 : (DAY-1);	  // DLMS (1=monday), ZON (1=sunday)

				rtc_tamper_date[5] = HOUR;
				rtc_tamper_date[6] = MIN;
				rtc_tamper_date[7] = SEC;
				rtc_tamper_date[8] = 0xFF;//operating_sec/360;

				rtc_tamper_date[9] = ((dlms_time_deviation &0xFF00) >> 8);
				rtc_tamper_date[10] = (int8_t)dlms_time_deviation;
				rtc_tamper_date[11] = 0;

				dlms_set_date_time(temp);
			
				error = 0;
		}
		*/
			break;
/************************************************************************************
		case E_CLASS_PROFILE_GENERIC:
			if(attribute_id != 8)
				return error;

			switch(object_index)
			{
			 	case 9:		// See block load profile
					// attrib_id = 8 requested
					*temp32 = LS_Maxdays * LS_SlotsPerDay;
					break;
			}
			error = 0;
			break;
************************************************************************************/
#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
		case E_CLASS_ACTIVITY_CALENDAR:
			// only one object in activity calendar.
			switch(attribute_id)
			{
				
				case 6:		// write - activity_calendar::calendar_name_passive
					memcpy(Calendar_Name_Passive,temp,16);
					set_cal_name_data=1;
					//l_address = PASCAL_BASEADDRESS;
					//Write2eeprom(temp,l_address,4);

					error = 0;

					break;
				case 7:       // season profile name
					
							for(j=0;j<2;j++)
							{          
								 for(i=0;i<10;i++)
										Passive_Season_Name[j][i]=*(temp+6+(j*36)+i);
								 
								 passive_season_date[j][0]=*(temp+18+(j*36)+2);
								 passive_season_date[j][1]=*(temp+18+(j*36)+3);
								 passive_season_date[j][2]=*(temp+18+(j*36)+5);
								 passive_season_date[j][3]=*(temp+18+(j*36)+6);
							}
							set_season_profile=1;
				
							// store the seasons into eeprom
							/*
							l_address = PASCAL_BASEADDRESS + MAX_CALENDAR_NAME_LEN;

							for(i=0;i<MAX_SEASONS;i++)
							{
									Write2eeprom(p_season_profile_name[i],l_address,MAX_CALENDAR_NAME_LEN);
									l_address += MAX_CALENDAR_NAME_LEN;
									Write2eeprom(p_season_start_time[i],l_address,12);
									l_address += 12;
									Write2eeprom(p_season_week_name[i],l_address,MAX_CALENDAR_NAME_LEN);
									l_address += MAX_CALENDAR_NAME_LEN;
							}
							*/
					break;
				
				case 8:       // week profile name
							// not implemented
							/*
							l_address = PASCAL_WEEKPROFILE_ADDRESS;
							for(i=0;i<MAX_SEASONS;i++)
							{

									Write2eeprom(dlms_dummy_wk[i],l_address,4);
									l_address += MAX_CALENDAR_NAME_LEN;
							}
							*/
					break;

				case 9:        // Write - activity_calendar::day_profile_table_passive
		
					set_day_profile=1;
					for(j=0;j<5;j++)//8
					{          
						 //ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME
						 tmp_hr=*(temp+12+(j*19)+0);
						 tmp_min=*(temp+12+(j*19)+1);
						 
						 if(((tmp_min%md_intgr_val)!=0)||(tmp_hr>23)||(tmp_min>59))
						 {  
								set_day_profile=0;
								break;
						 }
						 
						 tmp_hr=*(temp+18+95+(j*19)+0);
						 tmp_min=*(temp+18+95+(j*19)+1);
						 
						 if(((tmp_min%md_intgr_val)!=0)||(tmp_hr>23)||(tmp_min>59))
						 {  
								set_day_profile=0;
								break;
						 }
					}

					if(set_day_profile==1)
					{          
							for(j=0;j<5;j++)//8
							{          
								 //ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME
								 for(i=0;i<4;i++)
									 passive_tod_time[j][i]=*(temp+12+(j*19)+i);
								 
								 //ITEM_TAG_BILLING_SCHEDULE_EXEC_DAY:
								 passive_tariff_id[0][j]=*(temp+26+(j*19));
							}

							for(j=0;j<5;j++)//8
							{          
								 //ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME
								 for(i=0;i<4;i++)
										passive_tod_time[8+j][i]=*(temp+18+95+(j*19)+i);
								 
								 //ITEM_TAG_BILLING_SCHEDULE_EXEC_DAY:
								 passive_tariff_id[1][j]=*(temp+32+95+(j*19));
							}
					}

					/*
					// object_index = (day_id << 8) | tou
					day_id = object_index >> 8;
					tou = object_index & 0xFF;
		
					if((tou & 0x80) == 0x80)
					{
						// write  day_id and tou
						tou &= ~0x80;		// reset bit 7
						l_address =  PASSIVE_CAL_DAYID_OFFSET + (day_id * ((MAX_TZ * 12)+2)) + (tou * 0x0C) + 2;
						Write2eeprom((uint8_t *)&temp[0],l_address,size);
					}
					else
					{
		         		// write time[4], script_ln[6], script_id[2]
						l_address =  PASSIVE_CAL_DAYID_OFFSET + (day_id * ((MAX_TZ * 12)+2));
						Write2eeprom(temp,l_address,size);
					}
					*/
				
		
					error = 0;
					break;
				case 10:        // write - activity_calendar::activate_passive_calendar_time
					memcpy(Activate_Passive_Calendar_Time,temp,12);
					
					tmp_int=Activate_Passive_Calendar_Time[1];
					tmp_int=(tmp_int*256)+Activate_Passive_Calendar_Time[2];
					
					activate_date=Activate_Passive_Calendar_Time[4];
					activate_date=(activate_date*100)+Activate_Passive_Calendar_Time[3];
					activate_date=(activate_date*100)+(tmp_int%100);
					
					activate_time=Activate_Passive_Calendar_Time[6];
					activate_time=(activate_time*100)+Activate_Passive_Calendar_Time[7];
					set_activation_date=1;
					/*
					l_address = ACTCAL_ACTTIME_BASEADDRESS;
					Write2eeprom(temp,l_address,12);
					ac_changed = 1;
          //dlmsif_tou_actcal_set();
					*/
					error = 0;
				break;
			}

			break;
#endif // #ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP

			/*
			extern unsigned char set_auth_data;
			*/
						
		case E_CLASS_SINGLE_ACTION_SCHEDULE:
			// object_index = 0, 
			if(attribute_id == 4)    // write - single_action_schedule::execution_time
			{
				/*
				 //ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME
				for(i=0;i<2;i++)
					scheduled_bill_time[i]=*(temp+i+6);
					
				 //ITEM_TAG_BILLING_SCHEDULE_EXEC_DAY:
				for(i=0;i<4;i++)
					scheduled_bill_day[i]=*(temp+i+12);
				*/
				
				 //ITEM_TAG_BILLING_SCHEDULE_EXEC_TIME
				for(i=0;i<2;i++)
					scheduled_bill_time[i]=execution_billing_date[0][5+i];
					
				 //ITEM_TAG_BILLING_SCHEDULE_EXEC_DAY:
				for(i=0;i<4;i++)
					scheduled_bill_day[i]=execution_billing_date[0][i];
				
				i=scheduled_bill_day[0];
				i=(i*256)+scheduled_bill_day[1];
				
				if((i<2017)||(i>2099)) //year
				{
					scheduled_bill_day[0]=0;
					scheduled_bill_day[1]=0;
				}
				
				if((scheduled_bill_day[2]==0)||(scheduled_bill_day[2]>12))//month
								scheduled_bill_day[2]=0;
				if((scheduled_bill_day[3]==0)||(scheduled_bill_day[3]>31))//day
								scheduled_bill_day[3]=0;
				
				
				if((scheduled_bill_time[0]==0)||(scheduled_bill_time[0]>23))//hr
								scheduled_bill_time[0]=0;
				if((scheduled_bill_time[1]==0)||(scheduled_bill_time[1]>59))//min
								scheduled_bill_time[1]=0;
      
				set_bill_date=1;
				
				/*
							// billing_date[] is already updated by now (see objects.c:execution_time_date_array)
								 tamper_Singleaction_billing_Occured = 1;

							for(i=0;i<EXECUTION_BILLING_NUMBERS;i++)
							{
									l_address = BILLING_DATE_BASEADDRESS + (i*12);

									Write2eeprom(execution_billing_date[i],l_address,5);
									l_address = BILLING_DATE_BASEADDRESS+5;
									Write2eeprom((uint8_t *)&execution_billing_date[i][5],l_address,4);
							}
				*/
				
				error = 0;
			}

			break;
		default:
			break;
	}
	return 	 error;
}
/*
// Data structure for the billing parameters stargae in EEPROM.
const struct {
uint8_t id;
uint16_t offset;
uint8_t len;
}BILLING_PARAM_TABLE[] = {
{0,0,12},		// date/time
{1,12,4},               // avg pf
{2,16,4},               // kwh-cumulative
{3,20,4},               // kwh-t0
{4,24,4},               // kwh-t1
{5,28,4},               // kwh-t2
{6,32,4},               // kwh-t3
{7,36,4},               // kwh-t4
{8,40,4},               // kvah-cumulative
{9,44,4},               // kvah-t0
{10,48,4},              // kvah-t1
{11,52,4},              // kvah-t2
{12,56,4},              // kvah-t3
{13,60,4},              // kvah-t4
{14,64,2},		// MD-KW
{15,66,12},		// date
{16,78,2},		// MD-KVA
{17,80,12},		// date
{18,92,4}              // total power on duration for billing
};
*/

/** \fn void dlms_obj_getpg_value(uint16_t obj_id,uint8_t param,uint16_t cur_array,uint16_t cur_obj,void* data_ptr);
 *  \brief Get the value of attribute in profile generic buffer	object list
 *
 *  @param[in] obj_id 			Object index within the list
 *  @param[in] param 			0 = return num of entries/rows in the buffer,
 *  @param[in] cur_array		Current array/row number
 *  @param[in] cur_obj			Current column/object number
 *  @param[in,out] data_ptr		pointer to the value.
 *
 *	@return None
 */
void dlms_obj_getpg_value(uint16_t obj_id,uint8_t param,uint16_t cur_array,uint16_t cur_obj,void* data_ptr)
{
	/*
	uint8_t i;
	static uint8_t start;
	uint16_t min_id,max_id;
	uint32_t l_address;
	static uint8_t l_tampercount;
	static int32_t l_refaddress;
	//uint8_t type;
	uint8_t len;
	uint16_t temp;
	static uint16_t num_entries;// = (uint16_t *)data_ptr;
	static uint16_t from_array=0;

	min_id = max_id = 0;        // remove compiler warnings.
	
	//if(reset_block)
	if( (obj_id == 0) && (param == 0) && (cur_array == 0) && (cur_obj == 0) && (data_ptr == 0))
	{
		// End of the block. Reset statics
	 	start = 0;
		l_tampercount = 0;
		l_refaddress = 0;
		num_entries = 0;
		from_array = 0;
	}
	
	switch(obj_id)
	{
		case 3:				// event voltage log
			min_id = 1;max_id=12;
			break;
		case 4:				// event current log
			min_id = 51;max_id=70;
			break;
		case 5:				// event power log
			min_id = 101;max_id=102;
			break;
		case 6:				// event transaction log
			min_id = 151;max_id=155;
			break;
		case 7:				// event others log
			min_id = 201;max_id=208;
			break;
		case 8:				// event non rollover log
			min_id = 251;max_id=251;
			break;
		default:
			break;
	}
	*/

	uint16_t scan_event_type;
	//static uint16_t prev_array;
	//uint16_t tmp_int;
	//uint32_t tmp_long;
	
	/*
	#define VOLT_EVENT      0
	#define AMP_EVENT       1
	#define PFAIL_EVENT     2
	#define TRANSACT_EVENT  3
	#define OTHER_EVENT     4
	#define NOROLL_EVENT    5
	#define CTRL_EVENT      6 
	*/

	switch(obj_id)
	{
		case 3:				// event voltage log
			scan_event_type=VOLT_EVENT;
			break;
		case 4:				// event current log
			scan_event_type=AMP_EVENT;
			break;
		case 5:				// event power log
			scan_event_type=PFAIL_EVENT;
			break;
		case 6:				// event transaction log
			scan_event_type=TRANSACT_EVENT;
			break;
		case 7:				// event others log
			scan_event_type=OTHER_EVENT;
			break;
		case 8:				// event non rollover log
			scan_event_type=NOROLL_EVENT;
			break;
		default:
			break;
	}
					
	switch(obj_id)
	{
		case 3:				// event voltage log
		case 4:				// event current log
		case 5:				// event power log
		case 6:				// event transaction log
		case 7:				// event others log
		case 8:
				if(param == 0)
				{	
					*(uint32_t *)data_ptr = Tamper_Profile_Entries_In_Use[scan_event_type];
				}
				else
				{
					if(cur_obj==0)
						get_tamper_data(scan_event_type, cur_array+1);	
					
					switch(cur_obj)
					{
					case 0:		// clock
						memcpy((uint8_t *)data_ptr, stTamper_Profile.Tamper_Date, 12);
						break;
					case 1:		// id
						*(uint16_t *)data_ptr=stTamper_Profile.Tamper_ID;
						break;
					case 2:		// irms
						*(uint32_t *)data_ptr=stTamper_Profile.Irms;
						break;
					case 3:		// vrms
						*(uint32_t *)data_ptr=stTamper_Profile.Vrms;
						break;
					case 4:		// pf
						*(uint32_t *)data_ptr=stTamper_Profile.PF;
						break;
					case 5:   // kwh
						*(uint32_t *)data_ptr=stTamper_Profile.E_Active;
						break;			
					}
				}		
				
				/*
				if(param == 0)
				{
					num_entries = 0;
					start = 0;
					from_array = 0;
					// get num_entries for a category
					for(i=0;i<MAXTAMPERCOUNT;i++)
					{
						l_address = EVENTLOG_BASE_ADDRESS + (i * EVEN_LOG_BUFSIZE);
						l_address += 12;	// point to event_id
						Readeeprom((uint8_t *)&temp,l_address,2);
						if((temp >= min_id) && (temp <= max_id))
							num_entries++;
						//read_from_eeprom(address,CHOICE_UL,2,(void *)num_entries);
					}
					*(uint32_t *)data_ptr = num_entries;

				    if(num_entries)
					{
					    // get the first entry of event_id in EEPROM.
						l_tampercount = TamperCount;
	
						// Tamper Count is pointing to the oldest entry in EEPROM
						for(;;)
						{
							l_address = EVENTLOG_BASE_ADDRESS + (l_tampercount * EVEN_LOG_BUFSIZE);
							l_address += 12;	// point to event_id
							Readeeprom((uint8_t *)&temp,l_address,2);
							if((temp >= min_id) && (temp <= max_id))
							{	// we got the requested event_id
								break;
							}
							if(l_tampercount >= (MAXTAMPERCOUNT-1))
								l_tampercount = 0;
							else
								l_tampercount++;
		
							if(l_tampercount == ((TamperCount-1)%MAXTAMPERCOUNT) )
							{
								// read_ptr == write_ptr in EEPROM
								// ideally, we should not be here. equivalent to num_entries == 0
								break;
							}
						}
					}


					return;
				}
				// read the current address of EEPROM write_ptr.
				//read_from_eeprom(address+2,CHOICE_UL,2,(void *)&write_ptr);

// Note: The application stores the 'event id' sequentially as they occur. 
//       It is the job of server to identify which event has been requested
//       Code below traverses the eeprom from current_read_ptr to end_of_ptr. At the end of iteration we
//       will point to the FIRST requested 'event_id'	

			if(!start)
			{
				// l_tampercount is pointing to the first array of THIS event_id
				start = 1;
			 	while(from_array != cur_array)
				{
						from_array++;
						l_tampercount++;
						for(;;)
						{
							l_address = EVENTLOG_BASE_ADDRESS + (l_tampercount * EVEN_LOG_BUFSIZE);
							l_address += 12;	// point to event_id
							Readeeprom((uint8_t *)&temp,l_address,2);
							if((temp >= min_id) && (temp <= max_id))
							{	// we got the requested event_id
								break;
							}
							if(l_tampercount >= (MAXTAMPERCOUNT-1))
								l_tampercount = 0;
							else
								l_tampercount++;
	
							if(l_tampercount == TamperCount) 	
							{
								// we should not be here!!
								break;
							}
						}										 	
				}

			}

			if(from_array != cur_array)
			{
						from_array++;
						l_tampercount++;
						for(;;)
						{
							l_address = EVENTLOG_BASE_ADDRESS + (l_tampercount * EVEN_LOG_BUFSIZE);
							l_address += 12;	// point to event_id
							Readeeprom((uint8_t *)&temp,l_address,2);
							if((temp >= min_id) && (temp <= max_id))
							{	// we got the requested event_id
								break;
							}
							if(l_tampercount >= (MAXTAMPERCOUNT-1))
								l_tampercount = 0;
							else
								l_tampercount++;
	
							if(l_tampercount == TamperCount) 	
							{
								// we should not be here!!
								break;
							}
						}
					
			}

				
				l_address = EVENTLOG_BASE_ADDRESS + (l_tampercount * EVEN_LOG_BUFSIZE);

				switch(cur_obj)
				{
				case 0:		// clock
					//type = CHOICE_OCTSTR;
					len = 12;
					break;

				case 1:		// event id
					l_address += 12;
					//type = CHOICE_UL;
					len = 2;
					break;

				default:	// all other (note : cur_obj >= 2)
					l_address += (14 + (cur_obj-2) * 4);
					//type = CHOICE_UDL;
					len = 4;
					break;
				}
				Readeeprom((uint8_t *)data_ptr,l_address,len);
			*/
			break;

		case 10:					// block load survey buffer			

				if(param == 0)
				{	
					*(uint32_t *)data_ptr =Load_Profile_Entries_In_Use;
					//get_lp_data(cur_array); 
				}
				else
				{
					if(cur_obj==0)
						get_lp_data(cur_array+1); 
					
					switch(cur_obj)
					{
					case 0:		// clock
						memcpy((uint8_t *)data_ptr, stLoad_Profile.Load_Date, 12);
						break;
					case 1:		// vrms
						*(uint32_t *)data_ptr=stLoad_Profile.Vrms;
						break;
					case 2:		// kwh
						*(uint32_t *)data_ptr=stLoad_Profile.E_Active;
						break;
					case 3:		// kvah
						*(uint32_t *)data_ptr=stLoad_Profile.E_Apparent;
						break;
					case 4:		// irms
						*(uint32_t *)data_ptr=stLoad_Profile.Irms;
						break;
					case 5:   // max_dmd kw
						*(uint16_t *)data_ptr=stLoad_Profile.tot_kw;
						break;
					case 6:   // kw_date
						memcpy((uint8_t *)data_ptr, stLoad_Profile.Load_Date, 12);
						break;					
					}
				}		
				
				/*
				if(param == 0)
				{
					num_entries = 0;
					start = 0;
					from_array = 0;

					l_address = LOADSURVEY_REF_ADDRESS;
					Readeeprom((uint8_t *)&num_entries,l_address,2);

					if(num_entries >= Load_Profile_Entries)//Anil
								num_entries=24;//Anil
					*(uint32_t *)data_ptr = num_entries;
					

					// LS_RefAddress is pointing to the current time slot in EEPROM OR It points to the oldest entry
					if(num_entries >= Load_Profile_Entries)
							l_refaddress = LS_RefAddress;			// buffer entries are overwritten
					else
							l_refaddress = 0;						// buffer is in its first cycle

					return;
				}

				// point to the requested array number
				if(!start)
				{
				 	start = 1;
					while(from_array != cur_array)
					{
						from_array++;
					    l_refaddress++;
						
						if(l_refaddress >= (Load_Profile_Entries))
							l_refaddress = 0;
		
						if(l_refaddress == LS_RefAddress )
						{
									// read_ptr == write_ptr in EEPROM
									// ideally, we should not be here. equivalent to num_entries == 0
									break;
						}
					}

				}

				if(from_array != cur_array)
				{
					// point to the next array
							from_array++;
							l_refaddress++;
							
							if(l_refaddress >= (Load_Profile_Entries))
									l_refaddress = 0;
		
							if(l_refaddress == LS_RefAddress )
							{
									// read_ptr == write_ptr in EEPROM
									// ideally, we should not be here. equivalent to num_entries == 0
									break;
							}
				
				}

			
				l_address = LOADSURVEY_BASE_ADDRESS + (l_refaddress * LOADSURVEY_BUF_SIZE);

				switch(cur_obj)
				{
				case 0:		// clock
					//type = CHOICE_OCTSTR;
					len = 12;
					break;
        case 5:   // max_dmd kw
					l_address += (12 + (cur_obj-1) * 4);
					len = 2;
				  break;
				case 6:   // kw_date
					l_address += (12 + 18); // obj index=6, @addr_offset=30
					len = 12;					
          break;					
				default:	// all other
					l_address += (12 + (cur_obj-1) * 4);
					//type = CHOICE_UDL;
					len = 4;
					break;
				}
				Readeeprom((uint8_t *)data_ptr,l_address,len);
				*/
				break;
											
		case 12:	// billing profile buffer
				
				if(param == 0)
				{	
					*(uint32_t *)data_ptr = Billing_Profile_Entries_In_Use+1;
					get_bill_data(0); 
					//prev_array=0;//cur_array;
					//call_ctr=0;					
				}
				else
				{
					/*
					if(prev_array!=cur_array)//if(cur_obj==0)
					{
						get_bill_data(cur_array); 
						prev_array=cur_array;
					}	
					*/
					
					if(cur_obj==0)
						get_bill_data(cur_array);	
					
					
					switch(cur_obj)
					{
					case 0:		// bill date
						memcpy((uint8_t *)data_ptr, stBilling_Profile.Bill_Date, 12);
						break;
					case 1:		// Power Factor
						*(uint32_t *)data_ptr=stBilling_Profile.Sys_Power_Factor;
						break;
					case 2:		// kwh
						*(uint32_t *)data_ptr=stBilling_Profile.Cumm_Energy_KWh;
						break;
					case 3:case 4:case 5:case 6:case 7: //tod kwh		
						*(uint32_t *)data_ptr=stBilling_Profile.Cumm_Energy_KWh_TZ[cur_obj-3];
						break;
					case 8:		// kvah
						*(uint32_t *)data_ptr=stBilling_Profile.Cumm_Energy_KVAh;
						break;
					case 9:case 10:case 11:case 12:case 13: //tod kvah		
						*(uint32_t *)data_ptr=stBilling_Profile.Cumm_Energy_KVAh_TZ[cur_obj-9];
						break;
					case 14:	  // md kw
						*(uint16_t *)data_ptr=stBilling_Profile.MD_KW;
						break;
					case 15:    // md kw date and time
						memcpy((uint8_t *)data_ptr, stBilling_Profile.MD_KW_DT, 12);
						break;
					case 16:		// md kva
						*(uint16_t *)data_ptr=stBilling_Profile.MD_KVA;
						break;
					case 17:    // md kva date and time
						memcpy((uint8_t *)data_ptr, stBilling_Profile.MD_KVA_DT, 12);
						break;
					case 18:    // on time
						*(uint32_t *)data_ptr=stBilling_Profile.Power_On_Time;
						break;	
					default:
						*(uint8_t *)data_ptr=cur_obj; //0;	
					}
				}		

				/*
				if(call_ctr<100)
				{
					test_arr[0][call_ctr]=param;
					test_arr[1][call_ctr]=cur_array;
					test_arr[2][call_ctr]=cur_obj;
					call_ctr++;
				}	
				*/
				
				//*(uint8_t *)data_ptr=time_string[0];
				
				/*
				if(param == 0)
				{	
					num_entries = 0;
					start = 0;
					from_array = 1;

					// number of 'dlms_available_billing_period' entries
					l_address =BILLING_PARAM_CURRENT_ADDRESS;// BILLING_PARAM_BASEADDRESS +((BILLING_PARAM_MAX_CYCLES+1) * BILLING_PARAM_SIZE);

					Readeeprom((uint8_t *)&i,l_address,1);
					
					i=1;//Anil

					*(uint32_t *)data_ptr = i+1;			// minimum value = 1 (current billing is always reported)

					// billing_count is pointing to the current entry in EEPROM OR It points to the oldest entry
					l_refaddress = 1;						// default : buffer is in its first cycle.
					if(i == BILLING_PARAM_MAX_CYCLES)
						l_refaddress = billing_count;

					return;
				}

				if(!start)
				{
					start = 1;

					while(from_array != cur_array)
					{
					 	from_array++;
						l_refaddress++;
						if(l_refaddress > BILLING_PARAM_MAX_CYCLES)
								l_refaddress = 1;
	
						if(l_refaddress == billing_count )
						{
								// read_ptr == write_ptr in EEPROM
								// ideally, we should not be here. equivalent to num_entries == 0
								break;
						}

					}

				}

				if(from_array != cur_array)
				{
					from_array++;
					l_refaddress++;
					
					if(l_refaddress > BILLING_PARAM_MAX_CYCLES)
							l_refaddress = 1;

					if(l_refaddress == billing_count )
					{
							// read_ptr == write_ptr in EEPROM
							// ideally, we should not be here. equivalent to num_entries == 0
							break;
					}
				}				

		
				l_address = BILLING_PARAM_BASEADDRESS + (l_refaddress * BILLING_PARAM_SIZE) + BILLING_PARAM_TABLE[cur_obj].offset;
				len = BILLING_PARAM_TABLE[cur_obj].len;

				Readeeprom((uint8_t *)data_ptr,l_address,len);
				*/
				
				break;

			case 14:
				
				if(param == 0)
				{	
					*(uint32_t *)data_ptr = Daily_Load_Profile_Entries_In_Use;
				}
				else
				{
					if(cur_obj==0)
						get_dlp_data(cur_array+1); 
					
					switch(cur_obj)
					{
					case 0:		// clock
						memcpy((uint8_t *)data_ptr, stLoad_Profile.Load_Date, 12);
						break;
					case 1:		// kwh
						*(uint32_t *)data_ptr=stLoad_Profile.E_Active;
						break;
					case 2:		// kvah
						*(uint32_t *)data_ptr=stLoad_Profile.E_Apparent;
						break;
					}
				}		



				
			
			  /*
				//  daily profile as per new BIS requirements
				if(param == 0)
				{
					num_entries = 0;
					start = 0;
					from_array = 0;

					l_address = DAILYLOADSURVEY_REF_ADDRESS;
					Readeeprom((uint8_t *)&num_entries,l_address,2);

					*(uint32_t *)data_ptr = num_entries;

					// LS_RefAddress is pointing to the current time slot in EEPROM OR It points to the oldest entry
					if(num_entries >= (DLS_Maxdays))
							l_refaddress = DLS_RefAddress;			// buffer entries are overwritten
					else
							l_refaddress = 0;						// buffer is in its first cycle

					return;
				}

				// point to the requested array number
				if(!start)
				{
				 	start = 1;
					while(from_array != cur_array)
					{
						from_array++;
					    l_refaddress++;
						
						if(l_refaddress >= (DLS_Maxdays))
							l_refaddress = 0;
		
						if(l_refaddress == DLS_RefAddress )
						{
									// read_ptr == write_ptr in EEPROM
									// ideally, we should not be here. equivalent to num_entries == 0
									break;
						}
					}

				}

				if(from_array != cur_array)
				{
					// point to the next array
							from_array++;
							l_refaddress++;
							
							if(l_refaddress >= (DLS_Maxdays))
									l_refaddress = 0;
		
							if(l_refaddress == DLS_RefAddress )
							{
									// read_ptr == write_ptr in EEPROM
									// ideally, we should not be here. equivalent to num_entries == 0
									break;
							}
				
				}

			
				l_address = DAILYLOADSURVEY_BASE_ADDRESS + (l_refaddress * DAILYLOADSURVEY_BUF_SIZE);

				switch(cur_obj)
				{
				case 0:		// clock
					//type = CHOICE_OCTSTR;
					len = 12;
					break;

				default:	// all other (kwh and kvah)
					l_address += (12 + (cur_obj-1) * 4);
					//type = CHOICE_UDL;
					len = 4;
					break;
				}
				Readeeprom((uint8_t *)data_ptr,l_address,len);
				*/
				break;

	}

	return;

}
/** \fn void dlms_get_access_byrange(uint16_t id,uint32_t *cur_array,uint32_t *num_array,uint32_t from_time, uint32_t totime);
 *  \brief Get first and total number of rows for access range descriptor of profile generic buffer
 *
 *  @param[in] id	 			Object index within the list
 *  @param[in,out] cur_array 	pointer to the current row/array
 *  @param[in,out] num_array	pointer to the total number of array
 *  @param[in] from_time		unique value of date. See DateGen()
 *  @param[in] totime			unique value of date. See DateGen()
 *
 *	@return None
 */
void dlms_get_access_byrange(uint16_t id,uint32_t *cur_array,uint32_t *num_array,uint32_t fromtime, uint32_t totime)
{
/*	
	uint32_t c_val;
	uint8_t yr;
	static int32_t l_refaddress;
	uint32_t l_address;
	uint16_t l_cc,l_kk;
	uint8_t temp[7];

				if( (id == 10) || (id == 14) )               // block load=10, daily_load = 14
				{
					if(fromtime > totime)
						return;

					// traverse to FIRST block load data in EEPROM
					// LS_RefAddress is pointing to the current time slot in EEPROM OR It points to the oldest entry
                                        if(id == 10)
                                        {
                                              if( *num_array >= (Load_Profile_Entries))
							l_refaddress = LS_RefAddress;		// buffer entries are overwritten
                                              else
							l_refaddress = 0;			// buffer is in its first cycle
                                        }
                                        else
                                        {
                                              if( *num_array >= DLS_Maxdays)
							l_refaddress = DLS_RefAddress;		// buffer entries are overwritten
                                              else
							l_refaddress = 0;			// buffer is in its first cycle
                                        }
					
					// We are pointing to at the FIRST available block load data
					l_cc = 0;
					l_kk = 0;
					
					do
					{	
						if(id == 9)
									l_address = LOADSURVEY_BASE_ADDRESS + (l_refaddress * LOADSURVEY_BUF_SIZE );
						else
									l_address = DAILYLOADSURVEY_BASE_ADDRESS + (l_refaddress * DAILYLOADSURVEY_BUF_SIZE );

						// read the date/time ('year_100' & 'year' (cannot be zero!))
						Readeeprom((uint8_t *)&temp,l_address,7);
																 
						yr = ((temp[0] << 8 | temp[1]))%2000;
						c_val = DateGen(yr,temp[2],temp[3]);
					    c_val = c_val * 24 * 60 + (temp[5] * 60) + temp[6];
	
						if(c_val < fromtime)
							l_cc++;

						if( (c_val >= fromtime) && (c_val <= totime))
						{
							l_kk++;
						}

						l_refaddress++;

						if( (c_val > totime) || ((l_cc+l_kk) >= *num_array))
						{
						 	*cur_array = l_cc;
							*num_array = l_cc+l_kk;

							break;
						}

					}while(1);
				}
*/				
}
/** \fn uint8_t dlms_get_access_byentries(uint16_t object_index);
 *  \brief return success if profile generic object supports access selection by entry.
 *
 *  @param[in] object_index	 	Object index within the profile generic object list
 *
 *	@return o on success, else fail
 */
uint8_t dlms_get_access_byentries(uint16_t object_index)
{
	
		if( ((object_index >= 3) && (object_index <= 8)) || (object_index == 12))
			return 0;
		else
			return 1;


}
/** \fn void dlms_obj_set_acval();
 *  \brief Set values of active calendar object
 *  \brief Copies passive calendar date/time to active calendar date/time. This event is triggered
 *  \brief when activity_calendar::method action is activated
 *
 *	@return None
 */
void dlms_obj_set_acval(void)
{
    //dlmsif_tou_activate_calendar();
		//tamper_Activitycalender_Occured = 1;
		activate_tod_stat=1;

}

/** \fn void dlms_obj_get_acval(uint8_t choice,uint8_t type,uint8_t id,uint8_t *len,void* data_ptr)
 *  \brief Called from DLMS stack. returns the attribute values from EEPROM corresponding. 
 *
 *  @param[in] choice 	attribute index
 *  @param[in] type     Active(0)/passive(1) calendar
 *  @param[in] id 	reserved
 *  @param[in/out] *len     pointer to the uint8. Holds length of attribute value
 *  @param[in] *data_ptr -  pointer to void.
 *
 *	@return None
 */
void dlms_obj_get_acval(uint8_t choice,uint8_t type,uint8_t id,uint8_t *len,void* data_ptr)
{
	uint8_t l_len=0;
/*	
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
*/
		switch(choice)
		{
		 	case 0:
				if(type==0)
				{	
					// calendar name active
					*(uint8_t *)data_ptr=Calendar_Name_Active[0];
					get_active_season_data();
					get_active_week_data();
					get_active_day_data();
				}
				else
				{	
					// calendar name passive
					*(uint8_t *)data_ptr=Calendar_Name_Passive[0];
					get_passive_week_data();
					get_passive_day_data();
				}
				test_val=0;
				l_len = 18;
			break;
			case 1:
			    // active season profile name (See eeprom map)
					//get_active_season_data();
				if(id==0)
				{	
					*(uint8_t *)data_ptr=stSeason_Profile[0].Season_Name[0];
				}
				else
				{	
					*(uint8_t *)data_ptr=stSeason_Profile[1].Season_Name[0];
				}
				l_len = 10;
			break;
			case 2:
			  // active season start time
				if(id==0)
				{	
					*(uint8_t *)data_ptr=stSeason_Profile[0].Season_Date[0];
				}
				else
				{	
					*(uint8_t *)data_ptr=stSeason_Profile[1].Season_Date[0];
				}
				l_len = 12;
			break;
			case 3:	// week name
				if(id==0)
				{	
					*(uint8_t *)data_ptr=stWeek_Profile[0].Week_Name[0];
				}
				else
				{	
					*(uint8_t *)data_ptr=stWeek_Profile[1].Week_Name[0];
				}
				l_len = 5;
			break;
			case 4:	// week profile name
				/*
				if(type)
				{
							get_active_week_data();
				}	
				else
				{
							get_passive_week_data();

				}	
				*/
				if(id==0)
				{	
					*(uint8_t *)data_ptr=stWeek_Profile[0].Week_Name[0];
				}
				else
				{	
					*(uint8_t *)data_ptr=stWeek_Profile[1].Week_Name[0];
				}
				l_len = 5;
			break;
			case 7:
			    // script start time
				/*
				if(type)		// passive calendar
				{
					get_active_day_data();
				}	
				else
				{
					get_active_day_data();
				}
				*/
			  *(uint8_t *)data_ptr=stDay_Profile[id/8].tod_time[id%8][0];
				l_len = 4;
			break;
			case 8:
			    // script selector
				/*
				if(type)		// passive calendar
				{}	
				else
				{}
				*/
	     *(uint16_t *)data_ptr=stDay_Profile[id/8].tariff_id[id%8];	
  		 l_len = 2;
			break;
		}
		
		*len=l_len;
		
		/*
		*len = 0;
	 *(uint8_t *)data_ptr=0;
	  */	
		
	/*
		uint8_t l_len;
		uint32_t l_address;

	  l_len = 0;      // remove compiler warnings.
	
		if(type)
			l_address = PASCAL_BASEADDRESS;
		else
			l_address = ACTCAL_BASEADDRESS;


		switch(choice)
		{
		 	case 0:
				// calendar name active
				l_len = MAX_CALENDAR_NAME_LEN;

			break;
			case 1:
			    // active season profile name (See eeprom map)
				l_address += (id * 20) + 4;
				l_len = MAX_CALENDAR_NAME_LEN;
			break;
			case 2:
			    // active season start
				l_address += (id * 20) + (4+4);
				l_len = 12;
			break;
			case 3:	// week name
				l_address += (id * 20) + (4+4+12);
				l_len = MAX_CALENDAR_NAME_LEN;
			break;
			case 4:	// week profile name
				if(type)
								l_address = PASCAL_WEEKPROFILE_ADDRESS;
				else
								l_address = ACTCAL_WEEKPROFILE_ADDRESS;

				l_address += (id * 4) ;
				l_len = MAX_CALENDAR_NAME_LEN;
			break;
			case 7:
			    // script start time
				if(type)		// passive calendar
//					l_address = PASSIVE_CAL_DAYID_OFFSET + (id/8 * 0x62) + (id%8 * 0xC) + 2;
					l_address = PASSIVE_CAL_DAYID_OFFSET + (id/8 * ((MAX_TZ*12)+2)) + (id%8 * 0xC) + 2;
				else
//					l_address = ACTIVE_CAL_DAYID_OFFSET + (id/8 * 0x62) + (id%8 * 0xC) + 2;
                                        l_address = ACTIVE_CAL_DAYID_OFFSET + (id/8 * ((MAX_TZ*12)+2)) + (id%8 * 0xC) + 2;

				l_len = TIME_LEN;
			break;
			case 8:
			    // script selector
				if(type)		// passive calendar
//					l_address = PASSIVE_CAL_DAYID_OFFSET + (id/8 * 0x62) + (id%8 * 0xC) + 2;
					l_address = PASSIVE_CAL_DAYID_OFFSET + (id/8 * ((MAX_TZ*12)+2)) + (id%8 * 0xC) + 12;
				else
//					l_address = ACTIVE_CAL_DAYID_OFFSET + (id/8 * 0x62) + (id%8 * 0xC) + 2;
                                        l_address = ACTIVE_CAL_DAYID_OFFSET + (id/8 * ((MAX_TZ*12)+2)) + (id%8 * 0xC) + 12;

				l_len = 2;
			break;
		}
                *len = l_len;
		Readeeprom((uint8_t *)data_ptr,l_address,l_len);
	*/
	
}

// return value is not use for now.
uint8_t dlms_obj_getpg_billing_entries_inuse(uint16_t obj_id,void* data_ptr)
{
  uint32_t temp;

  if(obj_id == 12)
  {
        temp = *(uint32_t *)data_ptr;
        *(uint32_t *)data_ptr = temp - 1;
  }
	return 0;
}
/** \fn uint8_t dlms_obj_is_billing(uint16_t obj_id)
 *  \brief Called from DLMS stack. returns true if obj_id == billing_parameter
 *
 *  @param[in] obj_id 	object index within profile_generic_class[] array
 *
 *  @return 1 or 0
 */
uint8_t dlms_obj_is_billing(uint16_t obj_id)
{
  if(obj_id == 12)
    return 1;
  else
    return 0;

}
