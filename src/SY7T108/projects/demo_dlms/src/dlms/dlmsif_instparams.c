/******************************************************************************* 
 * Copyright (C) 2013 .. 2020 Silergy Corp., All rights Reserved.
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
 * $Id: dlmsif_instparams.c 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/ 
/********************************************************************************
*
*     Description: 
*        Filename: dlmsif_instparams.c
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
#include <stdlib.h>
#include <math.h>
#include "mmath.h"
//#include "options.h"
#include "afe.h"
#include "dlms_utils.h"
#include "dlms_variables.h"
#include "dlmsif_instparams.h"
#include "dlms_eeprom.h"
#include "dlms_utils.h"
#include "rtc.h"

#include "Defines.h"
#include "asdaq_variables.h"

uint32_t dlms_poweron_dur_seconds;

/******************************************************************************
*   Function Name : dlmsif_inst_params_init
*     Description : initialize instantaneous parameter values
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_inst_params_init(void)
{
	dlms_apparent_power_kva = 0;
	dlms_active_power_kw = 0;
	dlms_reactive_power_kvar = 0;

	dlms_num_powerfails = 0;
	dlms_poweron_dur_seconds = 0;
	dlms_poweron_dur_minutes = 0;
	dlms_num_tampercount = 0;
	dlms_cumulative_billing_count = 0;
	dlms_cumulative_pgm_count = 0;
	memset(dlms_billing_date,0xFF,12);
	dlms_cumulative_energy_kwh = 0;
	dlms_cumulative_energy_kvarh_lag = 0;
	dlms_cumulative_energy_kvarh_lead = 0;
	dlms_cumulative_energy_kvah = 0;

	kvarh_lag = 0;
	kvarh_lead = 0;
	//varh_lead = 0;
	//varh_lag = 0;

	//kwh_wh = 0;
	//kVah_vah = 0;

	//wh_atpowerdown = 0;
	//vah_atpowerdown = 0;
}
*/
/******************************************************************************
*   Function Name : dlmsif_inst_params_update
*     Description : updates the dlms variables (dlms variables = meter values)
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_inst_params_update(void)
{

				
        //float l_kva,l_kvarh_lag,l_kvarh_lead,l_kwh;
        
        //dlms_hz = afe_frequency() * 10.0;

        //// energy in watt-hours and VAHours
        //kwh_wh = wh(&global.reg.wh_cnt);//afe_get(AFE_WH);
        //kvarh_lead  = wh(&global.reg.varh_cnt_ex);//afe_get(AFE_VARHLEAD);//wh(&rpulse_cnt_ex);
        //kvarh_lag = wh(&global.reg.varh_cnt);//afe_get(AFE_VARHLAG);

        //l_kwh = kwh_wh/1000.0;
        //l_kvarh_lead = kvarh_lead/1000.0;
        //l_kvarh_lag = kvarh_lag/1000.0;

	      //kVah_vah = 
        //1000.0 * sqrtf((float)((l_kwh*l_kwh) + ((l_kvarh_lag + l_kvarh_lead) * (l_kvarh_lag + l_kvarh_lead))));
              
        //dlms_cumulative_energy_kwh = kwh_wh;                  // DLMS units are in Watt-hour
        //dlms_cumulative_energy_kvarh_lag  = kvarh_lag;
        //dlms_cumulative_energy_kvarh_lead = kvarh_lead;
        //dlms_cumulative_energy_kvah = kVah_vah;              //kVah;

        ////dlms_meter_id = meter_id;
        //dlms_date_time[0] = ((YEAR_100 * 100) + YEAR) >> 8;
        //dlms_date_time[1] = ((YEAR_100 * 100) + YEAR) & 0xFF;
        //dlms_date_time[2] = MONTH;
        //dlms_date_time[3] = DATE;
        //dlms_date_time[4] = (DAY == 1)? 7 : (DAY-1);	  // DLMS (1=monday)

        //dlms_date_time[5] = HOUR;
        //dlms_date_time[6] = MIN;
        //dlms_date_time[7] = SEC;
        //dlms_date_time[8] = 0xFF;//operating_sec/360;

        //dlms_date_time[9] = ((dlms_time_deviation &0xFF00) >> 8);
        //dlms_date_time[10] = (int8_t)dlms_time_deviation;
        //dlms_date_time[11] = 0;

        //dlms_voltage = labs((long)(10.0 * afe_vrms(PHASE_A)));//afe_get(AFE_VR);

        //dlms_phase_current = labs((long)(1000.0 * afe_arms(PHASE_A)));//afe_get(AFE_IR);
        //dlms_neutral_current = labs((long)(1000.0 * afe_arms(PHASE_B)));//afe_get(AFE_IY);
        //dlms_measured_current = MAX(dlms_phase_current,dlms_neutral_current);

        //dlms_active_power_kw = 	afe_w(PHASE_A);//afe_get(AFE_WATTS);
        //dlms_reactive_power_kvar = afe_var(PHASE_A);//afe_get(AFE_VAR);

        //// As per BIS - kvar is +ve for lag, -ve for lead under all conditions 
        //// current forward and reverse (current leads reversed)
        //// PF       Current Leads    VAR      Watts
        //// Lag         Normal        +ve       +ve
        //// Lag         Reverse       +ve       -ve
        //// Lead        Normal        -ve       +ve
        //// Lead        Reverse       -ve       +ve
 
        //if(dlms_active_power_kw < 0)
        //  dlms_reactive_power_kvar *= -1;

        //l_kva = (dlms_active_power_kw * dlms_active_power_kw) + (dlms_reactive_power_kvar * dlms_reactive_power_kvar);
        //dlms_apparent_power_kva = sqrtf((float)(l_kva));

        //dlms_pf = labs((long)(100.0 * afe_power_factor(PHASE_A)));//afe_get(AFE_PF);
				
				//if( (dlms_active_power_kw * dlms_reactive_power_kvar) < 0)   // both are opposite sign
				//{	// PF is LEAD
				//	dlms_pf *= -1;
				//}
				//if(dlms_reactive_power_kvar == 0)
				//{ // PF is unity
				//	dlms_pf = 100;
				//}

        //dlms_avg_pf = 100;
				
				
				dlms_voltage = vrms_reg3;
				dlms_phase_current = irms1_reg3;
				dlms_neutral_current = irms2_reg3;
				dlms_measured_current = irms_reg3;
				dlms_active_power_kw = kw_reg3;
				dlms_reactive_power_kvar = kvar_reg3;

				if(dlms_active_power_kw < 0)
				dlms_reactive_power_kvar *= -1;
				
				dlms_apparent_power_kva= kva_reg3;

				dlms_pf = pf_reg3;
				
				if( (dlms_active_power_kw * dlms_reactive_power_kvar) < 0)   // both are opposite sign
				{	// PF is LEAD
					dlms_pf *= -1;
				}
				if(dlms_reactive_power_kvar == 0)
				{ // PF is unity
					dlms_pf = 100;
				}
	
		
        dlms_cumulative_energy_kwh = load_val[0];                  // DLMS units are in Watt-hour
        dlms_cumulative_energy_kvarh_lag  = kvarh_lag;
        dlms_cumulative_energy_kvarh_lead = kvarh_lead;
        dlms_cumulative_energy_kvah = load_val[1];              //kVah;
				
//        //dlms_meter_id = meter_id;
//        dlms_date_time[0] = ((YEAR_100 * 100) + YEAR) >> 8;
//        dlms_date_time[1] = ((YEAR_100 * 100) + YEAR) & 0xFF;
//        dlms_date_time[2] = MONTH;
//        dlms_date_time[3] = DATE;
//        dlms_date_time[4] = (DAY == 1)? 7 : (DAY-1);	  // DLMS (1=monday)

//        dlms_date_time[5] = HOUR;
//        dlms_date_time[6] = MIN;
//        dlms_date_time[7] = SEC;
//        dlms_date_time[8] = 0xFF;//operating_sec/360;

//        dlms_date_time[9] = ((dlms_time_deviation &0xFF00) >> 8);
//        dlms_date_time[10] = (int8_t)dlms_time_deviation;
//        dlms_date_time[11] = 0;

        dlms_avg_pf = 100;
}
*/

/******************************************************************************
*   Function Name : dlmsif_inst_params_get
*     Description : Gets the dlms variables from EEPROM
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_inst_params_get(void)
{
      uint32_t pf_pr_date,pf_ref_date;

      Address1 = INSTANT_PARAM_BASEADDRESS;
      Readeeprom((uint8_t *)&Tamper[0],Address1,48);


      copy_from_buf(0,(void *)&dlms_num_powerfails,4);
      copy_from_buf(4,(void *)&dlms_poweron_dur_seconds,4);	
      copy_from_buf(8,(void *)&dlms_num_tampercount,4);	
      copy_from_buf(12,(void *)&dlms_cumulative_billing_count,4);	
      copy_from_buf(16,(void *)&dlms_cumulative_pgm_count,4);	
      copy_from_buf(20,(void *)&dlms_billing_date,12);	
      copy_from_buf(32,(void *)&dlms_cumulative_energy_kwh,4);	
      copy_from_buf(36,(void *)&dlms_cumulative_energy_kvarh_lag,4);	
      copy_from_buf(40,(void *)&dlms_cumulative_energy_kvarh_lead,4);	
      copy_from_buf(44,(void *)&dlms_cumulative_energy_kvah,4);	

      Address1 = INSTANT_PARAM_BASEADDRESS + 48;
      Readeeprom((uint8_t *)&Tamper[0],Address1,6);		// YEAR,MONTH,DATE,HOUR,MIN,SEC
      Address1 += 6;

// not needed. stored as part of reg_data save
//      Readeeprom((uint8_t *)&kvarh_lag,Address1,4);
//      Readeeprom((uint8_t *)&kvarh_lead,Address1+4,4);
//      Readeeprom((uint8_t *)&varh_lag,Address1+8,4);
//      Readeeprom((uint8_t *)&varh_lead,Address1+12,4);

      // calculate power fail duration
      pf_ref_date = DateGen(Tamper[0],Tamper[1],Tamper[2]);
      pf_pr_date     = DateGen(YEAR,MONTH,DATE);
      switch(pf_pr_date - pf_ref_date)
      {
              case 0:		// power fail same day
              dlms_num_powerfail_dur += ( (HOUR * 3600) + (MIN * 60) + SEC ) - ( (Tamper[3] * 3600) + (Tamper[4] * 60) + Tamper[5]);
                              
              break;

              case 1: 	// power fail to next day
                      dlms_num_powerfail_dur +=  (24 * 3600) - ( (Tamper[3] * 3600) + (Tamper[4] * 60) + Tamper[5]);
                      dlms_num_powerfail_dur += 	( (HOUR * 3600) + (MIN * 60) + SEC );			
              break;

              default:	// power fail more than a day
                      dlms_num_powerfail_dur +=  (24 * 3600) - ( (Tamper[3] * 3600) + (Tamper[4] * 60) + Tamper[5]);
                      dlms_num_powerfail_dur += 	( (HOUR * 3600) + (MIN * 60) + SEC );
                      dlms_num_powerfail_dur += ((pf_pr_date - (pf_ref_date+1)) * 24 * 3600);
              break;

      }

    // not needed. stored as part of reg_data save
    // Read energy
//    Readeeprom( (uint8_t *)&wh_atpowerdown, ADDRESS_KWH_WH, 4);
//    Readeeprom( (uint8_t *)&vah_atpowerdown, ADDRESS_KVAH_VAH, 4);

}
*/

/******************************************************************************
*   Function Name : dlmsif_inst_params_set
*     Description : wriets the dlms variables to EEPROM
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
/*
void dlmsif_inst_params_set(void)
{
//    copy_to_buf(0,(void *)&dlms_num_powerfails,4);            // backward compatibility (older specs)
//    copy_to_buf(4,(void *)&dlms_poweron_dur_seconds,4);	
//    copy_to_buf(8,(void *)&dlms_num_tampercount,4);	
//    copy_to_buf(12,(void *)&dlms_cumulative_billing_count,4);	
//    copy_to_buf(16,(void *)&dlms_cumulative_pgm_count,4);	
//    copy_to_buf(20,(void *)&dlms_billing_date,12);	
//    copy_to_buf(32,(void *)&dlms_cumulative_energy_kwh,4);	
//    copy_to_buf(36,(void *)&dlms_cumulative_energy_kvarh_lag,4);	
//    copy_to_buf(40,(void *)&dlms_cumulative_energy_kvarh_lead,4);	
//    copy_to_buf(44,(void *)&dlms_cumulative_energy_kvah,4);	

//    Address1 = INSTANT_PARAM_BASEADDRESS;
//    Write2eeprom((uint8_t *)&Tamper[0],Address1,48);

//    // log date of power fail
//    Address1 = INSTANT_PARAM_BASEADDRESS + 48;
//    Tamper[0] = YEAR;Tamper[1] = MONTH;Tamper[2] = DATE;
//    Tamper[3] = HOUR;Tamper[4] = MIN;  Tamper[5] = SEC;
//    Write2eeprom((uint8_t *)&Tamper[0],Address1,6);



  // not needed. stored as part of reg_data save
//    Write2eeprom((uint8_t *)&kvarh_lag,Address1,16);
//    Write2eeprom((uint8_t *)&kvarh_lead,Address1+4,4);

//    Write2eeprom((uint8_t *)&varh_lag,Address1+8,4);
//    Write2eeprom((uint8_t *)&varh_lead,Address1+12,4);

    // store energy
//    Write2eeprom( (uint8_t *)&wh_atpowerdown, ADDRESS_KWH_WH, 4);
//    Write2eeprom( (uint8_t *)&vah_atpowerdown, ADDRESS_KVAH_VAH, 4);

}
*/

/*
void dlms_instparam_poweron_dur_incr(void)
{
    // Total (cumulative) power on duration
    dlms_poweron_dur_seconds++;
    dlms_poweron_dur_minutes = dlms_poweron_dur_seconds/60;
}
*/
