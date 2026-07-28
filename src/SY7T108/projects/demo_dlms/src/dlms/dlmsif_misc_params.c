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
 * $Id: dlmsif_misc_params.c 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/ 
/********************************************************************************
*
*     Description: 
*        Filename: dlmsif_misc_params.c
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
#include "dlmsif_misc_params.h"
#include "dlms_eeprom.h"
/*****************************************************************************
* Description:
*    Initialize Loadsurvey and MD variables
* Parameters:
*   Input:
*         None.
*   Output:
*         None
* Return Status:
*        
*****************************************************************************/
/*
void dlmsif_misc_params_init(void)
{

  dlms_demand_integration_period = DLMS_DEMAND_INTEGRATION_PERIOD_30; 	//30 * 60;
  dlms_profile_capture_period = DLMS_DEMAND_INTEGRATION_PERIOD_30;	//30 * 60;

  if(dlms_demand_integration_period == DLMS_DEMAND_INTEGRATION_PERIOD_15)
    LS_SlotsPerDay = 96;
  else
    LS_SlotsPerDay = 48;

  if(dlms_profile_capture_period == DLMS_DEMAND_INTEGRATION_PERIOD_15)
    MD_SlotsPerDay = 96;
  else
    MD_SlotsPerDay = 48;

    profile_generic_default_entries = 1;
    dlms_profile_dailyload_capture_period = DLMS_DAYPROFILE_CAPTURE_PERIOD;
}
*/
/*****************************************************************************
* Description:
*    Read miscellaneous parameters from EEPROM
* Parameters:
*   Input:
*         None.
*   Output:
*         None
* Return Status:
*        
*****************************************************************************/
/*
void dlmsif_misc_params_get(void)
{
//	Readeeprom((uint8_t *)&dlms_demand_integration_period,DLMS_MISC_BASEADDRESS,4);
//	Readeeprom((uint8_t *)&dlms_profile_capture_period,DLMS_MISC_BASEADDRESS+4,4);

//	Readeeprom((uint8_t *)&dlms_time_deviation,DLMS_EXTRA_ADDRESS,2);   // time zone

//	if(dlms_demand_integration_period == DLMS_DEMAND_INTEGRATION_PERIOD_30)
//		MD_SlotsPerDay = 48;
//	else
//		MD_SlotsPerDay = 96;

//	if(dlms_profile_capture_period == DLMS_DEMAND_INTEGRATION_PERIOD_30)
//		LS_SlotsPerDay = 48;
//	else
//		LS_SlotsPerDay = 96;

    profile_generic_default_entries = 1;
    dlms_profile_dailyload_capture_period = DLMS_DAYPROFILE_CAPTURE_PERIOD;                            
}
*/
/*****************************************************************************
* Description:
*    Write miscellaneous parameters to EEPROM
* Parameters:
*   Input:
*         None.
*   Output:
*         None
* Return Status:
*        
*****************************************************************************/
/*
void dlmsif_misc_params_set(void)
{
//	Write2eeprom((uint8_t *)&dlms_demand_integration_period,DLMS_MISC_BASEADDRESS,4);
//	Write2eeprom((uint8_t *)&dlms_profile_capture_period,DLMS_MISC_BASEADDRESS+4,4);
//                
//	Write2eeprom((uint8_t *)&dlms_time_deviation,DLMS_EXTRA_ADDRESS,2);   // time zone
}
*/
