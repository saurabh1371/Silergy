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
 * $Id: dlmsif.c 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/ 
/********************************************************************************
*
*     Description: 
*        Filename: dlmsif.c
*
*        Compiler: uVision ARMCC.exe
*
*         Version: uVision 5.27.1
*
*         Created: 				
*							
*   Modifications: 
*         
*         
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "stdbool.h"
//#include <string_c.h>               // specific to Rowley MAXQ30 compiler to copy constant strings
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "wd.h"
#include "system_SY7T_M11.h"
#include "ce.h"
#include "afe.h"
#include "dlmsif.h"
#include "silergy_hdlc_lib.h"
#include "dlmsif_timer.h"
#include "silergy_dlms_lib.h"

#include "dlms_objects.h"

#include "dlms_variables.h"

#include "dlmsif_instparams.h"
#include "dlmsif_misc_params.h"
#include "dlmsif_loadsurvey.h"
#include "dlmsif_tampers.h"
#include "dlmsif_tou.h"
#include "dlmsif_dayprofile.h"

#include "dlms_eeprom.h"
#include "gpio.h"
#include "Defines.h"
#include "asdaq_variables.h"


//#define METER_UNINITIALISED 0
//#define METER_INITIALISED   (~METER_UNINITIALISED)

const char DLMS_LLS_PASSWORD[] ="ABCDEFGH";// "00000000";//Hello123";//"ABCDEFGH";
const char DLMS_HLS_PASSWORD[] = "RENESAS_P6wRJ21F";//"aaaaaaaaaaaaaaaa";//SIL_SY7T_M11_pwd";        // Max 16 chars ONLY

hdlc_struct_t hdlc_param;

uint8_t dlms_data_buffer[512/*256*/];

//static uint8_t wakeup_state = 0;          // state to verify if brownout mode occurs after complete cycle


//void dlmsif_energy_zero(void);
//void dlmsif_energy_get(void);
//void dlmsif_energy_set(void);

#define __TEST_EEPROM__ 0

#if __TEST_EEPROM__
uint8_t temp[64],temp_read[64];
int pass_count =0;
int fail_count = 0;

void test_eeprom(void)
{
  volatile uint32_t addr,i;
	
  memset(temp,0x00,64);

	//wd_reset();
	eeprom_init(); // setup the uwire interface for 2 wire I2C
	wd_reset();
  //eeprom_erase();
	
  for(addr = 0;addr < 128*1024;addr += 64)    // 128 KBytes
  {

    eeprom_write(addr,temp,64);//Write2eeprom(temp,addr,64);
		
    //memset(temp_read,0xaa,64);

		wd_reset();
		
    eeprom_read(addr, temp_read, 64);//Readeeprom(temp_read,addr,64);

		if ( 0 == memcmp(temp, temp_read, 64))
			pass_count++;
		else
			fail_count++;
  
		wd_reset();
  }

}
#endif //#if __TEST_EEPROM__
/*****************************************************************************
* Description:
*    Initialize dlms server
* Parameters:
*   Input:
*         None.
*   Output:
*         hdlc_param structure us initialised to default configuration.
* Return Status:
*         handle to HDLC channel.
*****************************************************************************/
uint8_t dlms_process_init(void)
{
  uint8_t  hdlc_handle=0;  
  uint16_t logical_addr = 0x0001;
	hdlc_param.baud_rate = 5;         //BAUD_RATE_9600;

	hdlc_param.window_size_tx = iec_hdlc_setup_class[0].window_size_trans;            // 1
	hdlc_param.window_size_rx = iec_hdlc_setup_class[0].window_size_recv;             // 1
	hdlc_param.max_info_txlen = iec_hdlc_setup_class[0].max_info_field_len_trans; 		// 512;
	hdlc_param.max_info_rxlen = iec_hdlc_setup_class[0].max_info_field_len_recv;		  // 128
	hdlc_param.inter_octet_to = iec_hdlc_setup_class[0].inter_octet_timeout;		      // 500 'milliseconds'								    
	hdlc_param.inactivity_to  = (uint16_t)(1000 * iec_hdlc_setup_class[0].inactivity_timeout);		//60000 'milli seconds'
	hdlc_param.dev_addr       = iec_hdlc_setup_class[0].dev_addr;	                    //0x0100;		// physical address
  hdlc_param.SystemCoreClock = SystemCoreClock;
				
//	p_hdlc.iframe_len = 512;		// I frame info block length

	ce_ver[0]=0x314D4543;
	ce_ver[1]=0x32306131;
	ce_ver[2]=0x00000066;
	ce_ver[3]=0x00000000;
	
	hdlc_init(&hdlc_param,dlmsif_timer0_reset);

        //serial1_handle_baudrate();
        //PR1 = 5 * PR1;

	hdlc_open(&hdlc_handle,logical_addr);

  dlms_server_init();

        return hdlc_handle;

}


/*****************************************************************************
* Description:
*    Heart of DLMS server processing
* Parameters:
*   Input:
*         handle to HDLC channel.
*   Output:
*         none
* Return Status:
*         none
*****************************************************************************/
void dlms_server_process(uint8_t hdlc_handle)
{

	int8_t error;
  uint16_t frame_size;
	
  frame_size = 512;
	error = hdlc_receive(hdlc_handle, 0, dlms_data_buffer, &frame_size);

	if(error == ERROR_HDLC_NDM)
	{
			dlms_server_error();      //	HDLC disconnected. Process accordingly
			hdlc_setparam(&hdlc_param);         
	}

	if(error == HDLC_ERROR_NOERROR)
	{

			dlms_server_process_request(hdlc_handle,512,dlms_data_buffer);
	}

	return;
}

/******************************************************************************
*     Description : set default password
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_password_init(void)
{
  memset(dlms_lls_password,0,MAX_SECRET_LLS_LEN);
	strncpy((char *)dlms_lls_password,DLMS_LLS_PASSWORD,MAX_SECRET_LLS_LEN);
  dlms_server_set_lls_password(dlms_lls_password,MAX_SECRET_LLS_LEN);

	memset(dlms_hls_password,0,MAX_SECRET_HLS_LEN);
	strncpy((char *)dlms_hls_password,DLMS_HLS_PASSWORD,MAX_SECRET_HLS_LEN);
  dlms_server_set_hls_password(dlms_hls_password,MAX_SECRET_HLS_LEN);
}
*/

/******************************************************************************
*     Description : get password from eeprom
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
void dlmsif_get_password(void)
{
	uint8_t i,j, checksum;
	
	Readeeprom(LOW_LEVEL_PASSWORD_LOC,(uint8_t *)&dlms_lls_password[0],MAX_SECRET_LLS_LEN);
	Readeeprom(LOW_LEVEL_PASSWORD_LOC+8,(uint8_t *)&checksum,1);//AUTH1_LOC
	j=0xAA;
	for(i=0;i<8;i++)		
	{
		j=j+dlms_lls_password[i];
	}	
	j=j&0xff;
	
	if(checksum!=j)
	{		
		  memset(dlms_lls_password,'0',MAX_SECRET_LLS_LEN);
			strncpy((char *)dlms_lls_password,DLMS_LLS_PASSWORD,MAX_SECRET_LLS_LEN);
			dlmsif_set_lls_password(dlms_lls_password,MAX_SECRET_LLS_LEN);
	}
	dlms_server_set_lls_password(dlms_lls_password,MAX_SECRET_LLS_LEN);
	
	
	Readeeprom(HIGH_LEVEL_PASSWORD_LOC,(uint8_t *)&dlms_hls_password[0],MAX_SECRET_HLS_LEN);
	Readeeprom(HIGH_LEVEL_PASSWORD_LOC+16,(uint8_t *)&checksum,1);
	j=0xAA;
	for(i=0;i<16;i++)		
	{
		j=j+dlms_hls_password[i];
	}	
	j=j&0xff;
	
	if(checksum!=j)
	{
			memset(dlms_hls_password,'0',MAX_SECRET_HLS_LEN);
			strncpy((char *)dlms_hls_password,DLMS_HLS_PASSWORD,MAX_SECRET_HLS_LEN);
			dlmsif_set_hls_password(dlms_hls_password,MAX_SECRET_HLS_LEN);
	}
	dlms_server_set_hls_password(dlms_hls_password,MAX_SECRET_HLS_LEN);
	
}
/******************************************************************************
*     Description : set LLS password in eeprom
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/

uint8_t dlmsif_set_lls_password(uint8_t *password, uint8_t password_len)
{
	uint8_t i,j;

  memcpy(dlms_lls_password,password,password_len);
	Write2eeprom(LOW_LEVEL_PASSWORD_LOC,password,password_len);

	j=0xAA;
	for(i=0;i<8;i++)		
	{
		j=j+dlms_lls_password[i];
	}	
	j=j&0xff;
	Write2eeprom(LOW_LEVEL_PASSWORD_LOC+8,(uint8_t *) &j,1);

	return 0;

}

/******************************************************************************
*     Description : set HLS password in eeprom
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/

void dlmsif_set_hls_password(uint8_t *password, uint8_t password_len)
{
	uint8_t i,j;

  memcpy(dlms_hls_password,password,password_len);
	Write2eeprom(HIGH_LEVEL_PASSWORD_LOC,password,password_len);
	
	j=0xAA;
	for(i=0;i<16;i++)		
	{
		j=j+dlms_hls_password[i];
	}	
	j=j&0xff;
	Write2eeprom(HIGH_LEVEL_PASSWORD_LOC+16,(uint8_t *) &j,1);
	
	return;
}


/******************************************************************************
*     Description : set passwords in eeprom
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_lls_password_set(void)
{
	uint8_t i,j;
	
  Write2eeprom(LOW_LEVEL_PASSWORD_LOC,(uint8_t *)&dlms_lls_password[0],MAX_SECRET_LLS_LEN);
	
	j=0xAA;
	for(i=0;i<8;i++)		
	{
		j=j+dlms_lls_password[i];
	}	
	j=j&0xff;
	Write2eeprom(LOW_LEVEL_PASSWORD_LOC+8,(uint8_t *) &j,1);
}

void dlmsif_hls_password_set(void)
{
	uint8_t i,j;
	
	Write2eeprom(HIGH_LEVEL_PASSWORD_LOC,(uint8_t *)&dlms_hls_password[0],MAX_SECRET_HLS_LEN);
	
	j=0xAA;
	for(i=0;i<16;i++)		
	{
		j=j+dlms_hls_password[i];
	}	
	j=j&0xff;
	Write2eeprom(HIGH_LEVEL_PASSWORD_LOC+16,(uint8_t *) &j,1);
}
*/

/******************************************************************************
*     Description : Configuration initialization
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: Called on power-up or wakeup. It takes care of a condition where,
*                   system power off before the meter is fully booted 
*
******************************************************************************/
/*
void dlmsif_bis_init(void)
{
  uint16_t temp;
  
  wakeup_state = METER_UNINITIALISED;    
  if(pb_read() == true)
  {
        //  push_button is pressed
        // initialise all of DLMS EEPROM data
        //dlmsif_initmeter();
  }
//	temp=0xbb;
//	 Write2eeprom((uint8_t *)&temp,ADDRESS_INITMETER,2);
//temp=87;
  Readeeprom(MEM_END,(uint8_t *)&temp,2);

  //temp = 0;

  if(temp == 0x0)
  {
    // meter installed afresh

#if __TEST_EEPROM__
    test_eeprom();            // test eeprom
#endif //#if __TEST_EEPROM__
		eeprom_erase(0);
    dlmsif_energy_zero();

    dlmsif_password_init();
		dlmsif_password_set();

    dlmsif_inst_params_init();

    dlmsif_misc_params_init();

    dlmsif_tampers_init();

    wd_reset();

    dlmsif_loadsurvey_init();

    wd_reset();

#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
    //dlmsif_tou_actcal_init();
#endif //#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP

    wd_reset();

		//dlmsif_tou_init();
		
    wd_reset();

    //dlmsif_tou_init_billing_period_date();

    wd_reset();

    dlmsif_dayloadsurvey_init();
		temp = 0xAAAA;
    Write2eeprom( MEM_END,(uint8_t *)&temp,  2);
		
		
  //		 dlmsif_password_init();//remove
	//		dlmsif_password_set();//remove
                  
  // other initializations go here
  }
  //else
	//{  
	//  dlms_bis_get();
  //}
	
	dlms_bis_get();
  wakeup_state = METER_INITIALISED;       // it is safe to store variables into EEPROM on brownout mode
   
//strncpy((char *)dlms_hls_password,DLMS_HLS_PASSWORD,MAX_SECRET_HLS_LEN);
}

void dlms_bis_get(void)
{ 	
    //// power on reset
    //dlmsif_energy_get();
    //wd_reset();
    //dlmsif_password_get();
    //wd_reset();
    //dlmsif_inst_params_get();
    //wd_reset();
    //dlmsif_misc_params_get();
    //wd_reset();
    //dlmsif_tampers_get();
    ////wd_reset();
    ////dlmsif_loadsurvey_get();
		////wd_reset();
//#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
//    //dlmsif_tou_actcal_get();
//    //wd_reset();	
//#endif //#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
//    //dlmsif_tou_get();          
//    //wd_reset();
//    //dlmsif_tou_get_billing_period_date();
//    //wd_reset();
		
		//init_tod();
		//wd_reset();
    ////dlmsif_dayloadsurvey_get();
    //wd_reset();
}
*/

/******************************************************************************
*     Description : Called when meter just about goes to sleep. Store the variables into EEPROM
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: if system power goes off before the previous meter is fully booted, then 
*                   the meter state will not be stored in EEPROM
*
******************************************************************************/
/*
void dlms_bis_set(void)
{
    if(METER_UNINITIALISED == wakeup_state)
      return;

    //dlmsif_energy_set();

    //dlmsif_tampers_set();

    //wd_reset();
    // log load survey
    //dlmsif_loadsurvey_set();		// store load survey parameters
    //wd_reset();
    
		//log current billing parameters
    //dlmsif_tou_set(0);				// store current billing parameters
    //wd_reset();
    //dlmsif_password_set();

    // log instantaneous parameters
    dlms_num_powerfails++;
    wd_reset();
    dlmsif_inst_params_set();

    wd_reset();
    dlmsif_misc_params_set();

    wd_reset();
    //dlmsif_tou_set_billing_period_date();

    //wd_reset();
    //dlmsif_dayloadsurvey_set();

#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP
    //wd_reset();
    //dlmsif_tou_actcal_set();
#endif //#ifndef REMOVE_ACTIVITY_CALENDAR_FROM_APP

}
*/

/******************************************************************************
*     Description : Initialize the 'meter_init' variable, so that when the meter wakes up on power up,
*                   meter is intialised afresh.
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_initmeter(void)
{
    uint16_t temp = 0;
    Write2eeprom( (uint8_t *)&temp, ADDRESS_INITMETER, 2);


}
*/

/******************************************************************************
*     Description : Initialize energy values,
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_energy_zero(void)
{
#if 0
    gCeData.wpulse_ctr = 0;
    gCeData.wsum_accum = 0xC0000000;
    gCeData.rpulse_ctr = 0;
    gCeData.vsum_accum = 0xC0000000;
#endif
	meter_dlms_reset();
}
*/

/******************************************************************************
*     Description : Gets energy values from eeprom,
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_energy_get(void)
{
#if 0
    afe_read_energy();
#endif 
	reg_get();
}
*/

/******************************************************************************
*     Description : Save energy values to eeprom,
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_energy_set(void)
{
#if 0	
    afe_write_energy();
#endif
	meter_save_data();
}
*/

/******************************************************************************
*     Description : update metrology application state variables.
*        Input(s) : None
*       Output(s) : None
*        
*            Notes: 
*
******************************************************************************/
/*
void dlmsif_update(void)
{
				dlmsif_inst_params_update();    // dlms variable updates
        //dlmsif_tampers_checks();         // tamper updates
				select_season();
				//md_func();
				history_func();
				update_tod_data();
				tamper_func();
        //dlmsif_tou_tou();
        //dlmsif_loadsurvey();            // load survey updates
      	//dlmsif_tou_billing_period_reset();
	      //dlmsif_tou_activity_calendar_reset();
        //dlmsif_tou_season_reset();
        //dlmsif_dayloadsurvey();
}
*/

