/******************************************************************************* 
 * Copyright (C) 2013..2020 Silergy Corp, All rights Reserved.
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
  * $Id: dlms_objects.h 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/ 

/********************************************************************************
*
*     Description: 
*        Filename: dlms_objects.h
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

/******************************************************************************
*         DO NOT MODIFY THIS FILE.
*******************************************************************************/


/** \file dlms_objects.h
 *  \brief DLMS objects listing
 *
 */

#ifndef __DLMS_OBJECTS_H__
#define __DLMS_OBJECTS_H__

#include <stdint.h>
#include "dlms_objmgr_lib.h"
#include "silergy_dlms_lib.h"



extern uint16_t app_context[];						///<Application context		
extern uint16_t authentication_mechanism_name[];
extern uint16_t authentication_mechanism_name_pc[];	///<Mechanism name for the Public client
extern uint16_t authentication_mechanism_name_mr[];	///<Mechanism name for the Meter reader
extern uint16_t authentication_mechanism_name_us[];	///<Mechanism name for the Utility Association

extern uint8_t dedicated_key[], conformance[];

extern uint8_t hls_ctos[];							///<HLS C_to_S challenge
extern uint8_t hls_stoc[];							///<HLC S-to_C challenge
extern const uint8_t sizeof_hls_stoc;				///<size of hls_stoc

extern const  DATA_CLASS data_class[];				///<Data object(s)
extern const  uint16_t SIZEOF_DATA_CLASS;

extern const  REGISTER_CLASS register_class[];		///<Register object(s)
extern const  uint16_t SIZEOF_REGISTER_CLASS;

extern const  EXTENDED_REGISTER extended_register_class[];		///<Register object(s)
extern const  uint16_t SIZEOF_EXTENDED_REGISTER_CLASS;

extern const  CLOCK_CLASS clock_class[];				///<Clock object(s)
extern const  uint16_t SIZEOF_CLOCK_CLASS;

extern const  IEC_HDLC_SETUP iec_hdlc_setup_class[];	///<HDLC setup object(s)
extern const  uint16_t SIZEOF_HDLCSETUP_CLASS;

extern const  SAP_ASSIGNMENT sap_assignment_class[];	///<SAP assignment object(s)
extern const  uint16_t SIZEOF_SAP_ASSIGNMENT;

extern const  ATTRIBUTE_ACCESS_ITEM data_access_attrib[];
extern const   ATTRIBUTE_ACCESS_ITEM register_access_attrib[];

extern const   LN_OBJ_ELEMENT ln_obj_element[];
extern const   uint16_t NUM_LN_OBJ_ELEMENTS;

extern const ASSOCIATION_LN association_ln_class_pc[];	///<PC association object(s)
extern const ASSOCIATION_LN association_ln_class_mr[];	///<MR association object(s)
extern const ASSOCIATION_LN association_ln_class_us[];	///<US association object(s)

extern const uint8_t SIZEOF_ASSOCIATION_LN_CLASS_PC;
extern const uint8_t SIZEOF_ASSOCIATION_LN_CLASS_MR;
extern const uint8_t SIZEOF_ASSOCIATION_LN_CLASS_US;

extern const   CHOICE_VAL scalar_current[];
extern const   CHOICE_VAL scalar_voltage[];
extern const   CHOICE_VAL profile_generic_instant_params_buffer[];

extern const   CAPTURE_OBJECT profile_generic_instant_params_objs[];

extern const   CHOICE_VAL profile_generic_instant_scalar_buffer[];
extern const   uint16_t PROFILE_GENERIC_INSTANT_SCALAR_BUFFER;

extern const   CAPTURE_OBJECT profile_generic_instant_scalar_objs[];
extern const   uint16_t PROFILE_GENERIC_INSTANT_SCALAR_OBJS;

extern const   PROFILE_GENERIC profile_generic_class[];
extern const   uint16_t SIZEOF_PROFILE_GENERIC_CLASS;

extern const   SINGLE_ACTION_SCHEDULE single_action_schedule_class[];
extern const   uint16_t SIZEOF_SINGLE_ACTION_SCHEDULE_CLASS;

extern const   ACTIVITY_CALENDAR activity_calendar_class[];
extern const   uint16_t SIZEOF_ACTIVITY_CALENDAR_CLASS;

extern const   uint8_t PASSIVE_CALENDAR_NAME[];

extern ACCESS_BY_ENTRY access_by_entry;
extern ACCESS_BY_RANGE access_by_range;
extern uint8_t access_selection;
extern const ACCESS_SELECTOR access_selector;

extern const   uint8_t NUM_LN_OBJ_ELEMENTS_PC;
extern const   uint8_t NUM_LN_OBJ_ELEMENTS_MR;
extern const   uint8_t NUM_LN_OBJ_ELEMENTS_US;

#endif	// __DLMS_OBJECTS_H__
