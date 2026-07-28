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
 * $Id: dlmsif_timer.c 25648 2020-06-10 11:00:29Z phuddar $
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

#include "silergy_hdlc_lib.h"
#include "silergy_dlms_lib.h"
//#include "dlmsif_instparams.h"
//#include "dlmsif_tou.h"
#include "dlmsif_timer.h"
#include "wd.h"
#include "dlms_variables.h"

static uint16_t msec_100; // 100 msec counter
static uint16_t sec_1;    // 1 sec counter

static volatile uint32_t timer_counter=0;
// convert seconds (to milli seconds) to ticks. ex: 60 seconds = 600 ticks
static volatile uint16_t t_inactivity = DLMS_INACTIVITY_TIME*1000/100; 
//	convert milli seconds to ticks. ex: 500 milli seconds = 5 ticks
static volatile uint16_t t_interoctet = DLMS_INTEROCTET_TIME/100;

unsigned char poweron_dur_seconds;
	
void dlmsif_timer0_init(void)
{
    msec_100 = 0;
	  sec_1 = 0;
	  timer_counter = 0;
}	
// param = MSEC10_COUNT OR SEC1_COUNT OR (MSEC10_COUNT|SEC1_COUNT)
void dlmsif_timer0_reset(uint8_t param)
{
    switch(param)
    {
        case MSEC10_COUNT:
          msec_100 = 0;
          break;
        case SEC1_COUNT:
          sec_1 = 0;
          break;
        default:
          msec_100 = 0;
          sec_1 = 0;
          break;
    }
}
// This is called every timer0 interrupt. Timer0 interrupt is configured to occur every 100 mill seconds.
void dlmsif_timer0(void)
{
	msec_100++;            // 100 milli seconds counter
	sec_1++;               // 1 second counter

	wd_reset();
	if(msec_100 >= t_interoctet)
	{
		hdlc_interframe_timeout();  // reset milli seconds ticks
	}

	if(sec_1 >= t_inactivity)	
	{
		hdlc_inactivity_timeout();  // resets seconds ticks
	}

  if(timer_counter++ >= 10)     // equal to 1 seconds OR (100 * 10) msecs
  {
      //dlms_tou_pon_incr();           // TOU power on duration
		  //dlms_instparam_poweron_dur_incr();    // cumulative power on duration
		  //dlms_poweron_dur_billing_seconds++;
      //dlms_poweron_dur_billing_minutes = dlms_poweron_dur_billing_seconds/60;
      
		  poweron_dur_seconds++;
      dlms_poweron_dur_minutes = poweron_dur_seconds/60;
      timer_counter = 0;
  }

}
