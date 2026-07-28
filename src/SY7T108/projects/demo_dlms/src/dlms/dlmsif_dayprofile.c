/******************************************************************************* 
 * Copyright (C) 2020 Silergy Corp, All rights Reserved.
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
 * $Id: dlmsif_dayprofile.c 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/ 

/********************************************************************************
*
*     Description: 
*        Filename: dlmsif_dayprofile.c
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
#include "stdbool.h"
#include <math.h>
#include <string.h>
#include "eeprom.h"
#include "dlms_eeprom.h"
#include "dlms_silergy_stubs.h"
#include "silergy_dlms_lib.h"
#include "dlms_variables.h"

#include "dlmsif_dayprofile.h"
#include "dlms_utils.h"
#include "rtc.h"

/*
int32_t DLS_StartDate;
uint16_t DLS_numentries;

uint8_t	DLS_refyear,DLS_refmonth,DLS_refdate;
uint8_t	DLS_refday, DLS_refhour, DLS_refmin,DLS_refsec;
*/


/* Log the daily load survey data to EEPROM
 */
/* 
void DayLoadSurvey_log(void)
{

//		Tamper[0] = ((YEAR_100 * 100) + DLS_refyear) >> 8;
//		Tamper[1] = ((YEAR_100 * 100) + DLS_refyear) & 0xFF;
//		Tamper[2] = DLS_refmonth;
//		Tamper[3] = DLS_refdate;
//		Tamper[4] = (DLS_refday == 1)? 7 : (DLS_refday - 1);	  // DLMS (1=monday), 6543 (1=sunday)
//                // daily load profiles shall be logged at midnight (00 h)
//		Tamper[5] = 0;//DLS_refhour;
//		Tamper[6] = 0;//DLS_refmin;
//		Tamper[7] = 0;//DLS_refsec;
//		Tamper[8] = 0xFF;//operating_sec/360;
//		Tamper[9] = ((dlms_time_deviation &0xFF00) >> 8);
//		Tamper[10] = (int8_t)dlms_time_deviation;
//		Tamper[11] = 0;

//		//copy_to_buf(12,(void *)&kwh_wh,4);
//		//copy_to_buf(16,(void *)&kVah_vah,4);
//                // dlms_cumulative_energy_kvah contains correct value always (even after waking from power off)
//    copy_to_buf(12,(void *)&dlms_cumulative_energy_kwh,4);
//		copy_to_buf(16,(void *)&dlms_cumulative_energy_kvah,4);

//		Address1 = DAILYLOADSURVEY_BASE_ADDRESS + (DLS_RefAddress * DAILYLOADSURVEY_BUF_SIZE);
//		Write2eeprom( Tamper, Address1, DAILYLOADSURVEY_BUF_SIZE);

//		if(DLS_numentries < DLS_Maxdays)
//		{
//			DLS_numentries++;
//		}

//		Address1 = DAILYLOADSURVEY_REF_ADDRESS;
//		Write2eeprom( (uint8_t *)&DLS_numentries, Address1, 2);

}
*/

/* Called on POR.
   Initialise Daily Load survey data
 */
/* 
void dlmsif_dayloadsurvey_init(void)
{
//	DLS_RefAddress = 0;
//	DLS_numentries = 0;

//	memset(Tamper,0x00,DAILYLOADSURVEY_BUF_SIZE);

////	BIS amendment 2 : The storage time for daily load survey parameters is same as Block Load Survey.
////      LS_SlotsPerDay = 24 * (3600/dlms_profile_capture_period);							// 15 or 30 minutes interval.
//	if(LS_SlotsPerDay == 48)
//		DLS_Maxdays = 35;		
//	else
//		DLS_Maxdays = 17;

//        dlms_profile_max_dl_entries = DLS_Maxdays;

//	Address1 = DAILYLOADSURVEY_REF_ADDRESS;
//	Write2eeprom(Tamper, Address1, 2+4+4);	// clear DLS_numentries,DLS_startdate,LS_refAddress

////      clear current LS data (date/time, kwh and kvah) is already done in dlmsif_inst_params_init()
////	Address1 += 10;
////	Write2eeprom(Tamper, Address1, DAILYLOADSURVEY_BUF_SIZE);	

//	DLS_StartDate = DateGen(YEAR,MONTH,DATE);

//	DLS_refyear = YEAR;DLS_refmonth = MONTH;DLS_refdate = DATE;DLS_refday  = DAY; 
//	DLS_refhour = 0;	DLS_refmin  = 30;	DLS_refsec  = 0;
		
}
*/
/* Called on Wakeup of meter.
   Restore Daily Load Survey data (snapshot at sleep) from EEPROM
 */
/*
void dlmsif_dayloadsurvey_get(void)
{

//    int32_t DLS_PresentDate;

//    int32_t DLS_PresentAddress;

////	BIS amendment 2 : The storage time for daily load survey parameters is same as Block Load Survey.
////      LS_SlotsPerDay = 24 * (3600/dlms_profile_capture_period);							// 15 or 30 minutes interval.
//	if(LS_SlotsPerDay == 48)
//		DLS_Maxdays = 35;		
//	else
//		DLS_Maxdays = 17;

//        dlms_profile_max_dl_entries = DLS_Maxdays;

//		Address1 = DAILYLOADSURVEY_REF_ADDRESS;
//		Readeeprom(Tamper,Address1,2+4+4);         // Read from eeprom   

//		copy_from_buf(0,(void *)&DLS_numentries,2);
//		copy_from_buf(2,(void *)&DLS_StartDate,4);
//		copy_from_buf(6,(void *)&DLS_RefAddress,4);

//		Address1 += 10;
//		Readeeprom(Tamper,Address1,12);         // Read date/time from eeprom   
//		
//		DLS_refyear = (((uint16_t)Tamper[0] << 8) | Tamper[1]) - (YEAR_100 * 100);

//		DLS_refmonth = Tamper[2];DLS_refdate = Tamper[3];
//		// LS_refday refers to ZON day
//		DLS_refday  = Tamper[4];              //ZON day (1=sunday)
//		DLS_refhour = Tamper[5]; 	
//                DLS_refmin  = Tamper[6];
//		DLS_refsec  = 0;


//    DLS_PresentDate  = DateGen(YEAR,MONTH,DATE);
//      
//    DLS_PresentAddress = (DLS_PresentDate - DLS_StartDate);

//	if(DLS_PresentAddress >= DLS_Maxdays)
//	{
//		DLS_PresentAddress = 0;
//		DLS_StartDate = DLS_PresentDate;
//	}
//	if( (DLS_PresentAddress > DLS_RefAddress) || ( (DLS_PresentAddress == 0) && (DLS_RefAddress == (DLS_Maxdays-1))))
//	{	
//		// we are into NEW day
//		// log the values of previous DAY in EEPROM ; Could be a power failure within the 'capture_period' time
//			DLS_refmin = 0;
//                        DLS_refsec = 0;
//                        for(;;)
//                        {                          
//                            DLS_refhour++;          // advance till next date (time = 00:00:00)
//                            if(DLS_refhour >= 24)
//                            {	
//                                DLS_refday++;
//                                if(DLS_refday>=8)
//                                  DLS_refday = 1;
//                                DLS_refdate++;
//				DLS_refhour = 0;
//				if(DLS_refdate > MonthArray[DLS_refmonth])
//				{	DLS_refdate = 1;
//					DLS_refmonth++;
//					if(DLS_refmonth > 12)
//					{	DLS_refmonth = 1;
//						DLS_refyear++;
//					}
//				}
//                                break;                                        
//                            }                            
//                        }
//                        DayLoadSurvey_log();
//			DLS_RefAddress = DLS_PresentAddress;
//	}

}
*/
/* Called before meter goes to sleep
   Store Daily Load survey data (snapshot) to EEPROM
 */
/*
void dlmsif_dayloadsurvey_set(void)
{
//		copy_to_buf(0,(void *)&DLS_numentries,2);
//		copy_to_buf(2,(void *)&DLS_StartDate,4);
//		copy_to_buf(6,(void *)&DLS_RefAddress,4);

//		Address1 = DAILYLOADSURVEY_REF_ADDRESS;
//		Write2eeprom(Tamper,Address1,2+4+4);         // write to eeprom   

//		Tamper[0] = ((YEAR_100 * 100) + DLS_refyear) >> 8;
//		Tamper[1] = ((YEAR_100 * 100) + DLS_refyear) & 0xFF;
//		Tamper[2] = DLS_refmonth;
//		Tamper[3] = DLS_refdate;
//		Tamper[4] = DLS_refday;                     // ZON day
//		Tamper[5] = DLS_refhour;
//		Tamper[6] = DLS_refmin;
//		Tamper[7] = DLS_refsec;
//		Tamper[8] = 0xFF;//operating_sec/360;
//		Tamper[9] = ((dlms_time_deviation &0xFF00) >> 8);
//		Tamper[10] = (int8_t)dlms_time_deviation;
//		Tamper[11] = 0;																	 

//		Address1 = DAILYLOADSURVEY_REF_ADDRESS + 10;
//		Write2eeprom(Tamper,Address1,12);


}
*/

/* Called every second.
   Check if it is time to log the data to EEPROM, if yes, log it.
 */
/* 
void dlmsif_dayloadsurvey(void)
{
//    int32_t DLS_PresentDate;

//    int32_t DLS_PresentAddress;

//    DLS_PresentDate  = DateGen(YEAR,MONTH,DATE);
//      
//    DLS_PresentAddress = (DLS_PresentDate - DLS_StartDate);

//	if(DLS_PresentAddress >= DLS_Maxdays)
//	{
//		DLS_PresentAddress = 0;
//		DLS_StartDate = DLS_PresentDate;
//	}

//	if( (DLS_PresentAddress > DLS_RefAddress) || ( (DLS_PresentAddress == 0) && (DLS_RefAddress == (DLS_Maxdays-1))))
//	{	
//		// we are into NEW day
//		// log the values of previous DAY in EEPROM ; Could be a power failure within the 'capture_period' time
//		DLS_refyear = YEAR;DLS_refmonth = MONTH;DLS_refdate = DATE;
//		DLS_refday  = DAY; DLS_refhour = HOUR; 	DLS_refmin  = MIN;
//		DLS_refsec  = SEC;
//		DayLoadSurvey_log();
//		DLS_RefAddress = DLS_PresentAddress;
//	}
//	else
//	{

//		DLS_refyear = YEAR;DLS_refmonth = MONTH;DLS_refdate = DATE;
//		DLS_refday  = DAY; DLS_refhour = HOUR; 	DLS_refmin  = MIN;
//		DLS_refsec  = SEC;
//	}
}
*/
