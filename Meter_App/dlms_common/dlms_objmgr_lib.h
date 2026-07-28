/*******************************************************************************
 * Copyright (C) 2013 Silergy Corp, All rights Reserved.
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

 * $Id: dlms_objmgr_lib.h 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/

/********************************************************************************
*
*     Description:
*        Filename: dlms_obj_mgr.h
*
*          Author: PBH
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

/** \file dlms_obj_mgr_public.h
 *  \brief DLMS objects definitions.
 *         Refer to DLMS BLUE book for detailed documentation
 *
 */

#ifndef __DLMS_OBJ_MGR_PUBLIC_H__
#define __DLMS_OBJ_MGR_PUBLIC_H__

#include <stdint.h>

//    //
#define LOGICAL_NAME_LEN 6
#define DATE_TIME_LEN    12
#define DATE_LEN 5
#define TIME_LEN 4
#define FLOAT32_LEN 4
#define FLOAT64_LEN 8

#define EXEC_WEEKDAY_LEN   10
#define EXEC_SPECDAY_LEN   10
#define DEDICATED_KEY_SIZE 32 // holds maximum SIZE OF DEDICATED KEY.
#define MAX_MASK_NAME_SIZE 20 //it is vendor specific
#define MAX_SEASON_PROFILE_NAME_SIZE 8 //it is vendor specific
#define MAX_WEEK_PROFILE_NAME_SIZE 8 //it is vendor specific

#define CONFORMANCE_BLOCK_SIZE	24		// in bit length
/** \enum E_CLASS_ID DLMS class enumerations
 *
 */
typedef enum
{
		E_CLASS_DATA = 1,
		E_CLASS_REGISTER = 3,
		E_CLASS_EXTENDED_REG = 4,
		E_CLASS_DEMAND_REG = 5,
		E_CLASS_REG_ACTIVE =6,
		E_CLASS_PROFILE_GENERIC = 7,
		E_CLASS_CLOCK = 8,
		E_CLASS_SCRIPT_TABLE = 9,
		E_CLASS_SCHEDULE = 10,
		E_CLASS_SPL_DAYS_TABLE = 11,
		E_CLASS_ASSOCIATION_SN = 12,
		E_CLASS_ASSOCIATION_LN = 15,
		E_CLASS_SAP_ASSIGNMENT = 17,
		//CLASS_IMAGE_TRANSFER = 18,

		E_CLASS_IEC_LOCALPORT_SETUP = 19,
		E_CLASS_ACTIVITY_CALENDAR = 20,
		E_CLASS_REGISTER_MONITOR = 21,
		E_CLASS_SINGLE_ACTION_SCHEDULE = 22,
		E_CLASS_IEC_HDLC_SETUP = 23,
		E_CLASS_UTILITY_TABLE = 26,
		E_CLASS_REGISTER_TABLE = 61,
		E_CLASS_STATUS_MAPPING = 63
}E_CLASS_ID;

/** \enum E_CHOICE_TYPES DLMS data types
 *
 */
typedef enum
{
	CHOICE_NULL = 0,
	CHOICE_ARRAY,
	CHOICE_STRUCTURE,
	CHOICE_BOOL,
	CHOICE_BITSTR,
	CHOICE_DL,					   	// int32
	CHOICE_UDL,						// uint32
	CHOICE_FP,
	CHOICE_OCTSTR = 9,
	CHOICE_VSTR,
	CHOICE_BCD = 13,
	CHOICE_INT = 15,				// integer8
	CHOICE_L,						// integer16
	CHOICE_UINT,					// Unsigned8
	CHOICE_UL,						// Unsigned16
	CHOICE_COMPACT_ARRAY,
	CHOICE_LONG64,			//20
	CHOICE_ULONG64,
	CHOICE_ENUM,
	CHOICE_FLOAT32,
	CHOICE_FLOAT64,
	CHOICE_DT,				//25
	CHOICE_DATE,
	CHOICE_TIME,
	CHOICE_XXX = 255
}E_CHOICE_TYPES;

/** \enum E_UNIT_TYPES DLMS units
 *
 */
typedef enum
{
	UNIT_YEAR = 1,
	UNIT_MONTH,
	UNIT_WEEK,
	UNIT_DAY,
	UNIT_HOUR,
	UNIT_MINUTE,
	UNIT_SECOND,
	UNIT_DEGREE,
	UNIT_CELCIUS,
	UNIT_CURRENCY,			// 10
	UNIT_METRE,
	UNIT_M_P_SECOND,
	UNIT_CUBIC_METRE_CONST,
	UNIT_CUBIC_METRE,
	UNIT_CUBIC_METRE_PER_HOUR,	//15
	UNIT_CUBIC_METRE_PER_HOUR_C,
	UNIT_VOLUME_FLUX,
	UNIT_VOLUME_FLUX_C,
	UNIT_LITRE,
	UNIT_KG,				//20
	UNIT_NEWTON,
	UNIT_NEWTON_METRE,
	UNIT_PASCAL,
	UNIT_BAR,
	UNIT_JOULE,				//25
	UNIT_JOULE_PER_HOUR,
	UNIT_WATT,
	UNIT_VOLT_AMP,
	UNIT_VAR,
	UNIT_WATT_HOUR,			//30
	UNIT_VOLT_AMP_HOUR,
	UNIT_VAR_HOUR,
	UNIT_AMPERE,
	UNIT_COULOMB,
	UNIT_VOLT,				//35
	UNIT_VOLT_PER_METRE,
	UNIT_FARAD,
	UNIT_OHM,
	UNIT_RESISTIVITY,
	UNIT_WEBER,
	UNIT_TESLA,
	UNIT_AMP_PER_METRE,
	UNIT_HENRY,
	UNIT_HERTZ,
	UINT_RW_CONST,			//45
	UINT_RB_CONST,
	UINT_RS_CONST,
	UNIT_VS_HR,
	UNIT_AS_HR,
	UNIT_KG_PER_SEC,		//50
	UNIT_SIEMENS,
	UNIT_KELVIN,
	UNIT_RU2H_PULSE_VAL,
	UNIT_RI2H_PULSE_VAL,
	UNIT_RV_METER_CONSTANT,	//55
	UNIT_PERCENTAGE,
	UNIT_AMP_HOUR,
	UNIT_ENERGY_PER_VOL = 60,
	UNIT_CAL_VAL,
	UNIT_MOLE_PERCENT,
	UNIT_MASS_DENSITY,
	UNIT_PASCAL_SECOND,
	UNIT_JOULE_PER_KG,		//65
	UNIT_SIGNAL_STRENGTH = 70,
	UNIT_COUNT = 255
}E_UNIT_TYPES;

/** \enum E_SORT_METHOD DLMS sort types
 *
 */
typedef enum
{
	CAPTURE_SORT_FIFO = 1,
	CAPTURE_SORT_LIFO,
	CAPTURE_SORT_LARGEST,
	CAPTURE_SORT_SMALLEST,
	CAPTURE_SORT_NEAREST_TO_ZERO,
	CAPTURE_SORT_FAREST_FROM_ZERO
}E_SORT_METHOD;

/** \enum E_ACCESS_METHOD DLMS security access methods
 *
 */
typedef enum
{
	NO_ACCESS_MODE_METHOD,
	ACCESS_MODE_METHOD,
	AUTHENTICATED_ACCESS_MODE_METHOD
}E_ACCESS_METHOD;

/** \enum E_ATTRIBUTES DLMS security attributes
 *
 */
typedef enum
{
	NO_ACCESS_ATTRIBUTE,
	READ_ONLY_ATTRIBUTE,
	WRITE_ONLY_ATTRIBUTE,
	RD_WR_ATTRIBUTE,
	AUTH_READ_ATTRIBUTE,
	AUTH_WRITE_ATTRIBUTE,
	AUTH_RD_WR_ATTRIBUTE
}E_ATTRIBUTES;

/** \enum E_ASSOCIATION_TYPES DLMS association status
 *
 */
typedef enum
{
	NON_ASSOCIATED,
	ASSOCIATION_PENDING,
	ASSOCIATED
}E_ASSOCIATION_TYPES;

/** @struct SCALAR_UNIT
 *	@brief This structure holds the Scalar unit of a object.
 *

 */
typedef struct
{

 	int8_t scale;			///< cotains sclar value
	uint8_t unit_type;		///<contains units
}SCALAR_UNIT;

/** \struct CHOICE_VAL
 *	@brief This structure holds choice data type.
 *
 */
typedef struct
{
	uint8_t choice_type;	///<data types
	uint16_t length;		///<length of data
	void* data_ptr;			///<data pointer
}CHOICE_VAL;

/** \struct CAPTURE_OBJECT
 *	@brief This structure holds capture object.
 *
 */
typedef struct
{
	uint16_t class_id;				///<class Id
	uint8_t  ln[LOGICAL_NAME_LEN];	///<Logical name
	int8_t   attribute_idx;			///<Attribute Index
	uint16_t data_idx;				///<data index
} CAPTURE_OBJECT;

/** \struct ACTION
 *	@brief This structure holds Action
 *
 */
typedef struct
{
	int8_t		service_id;			///<type of service
	uint16_t	class_id;			///<class id
	uint8_t		ln[LOGICAL_NAME_LEN];	///<Logical name
	int8_t		index;				///<index of service
	CHOICE_VAL		param;			///<extra parameters
} ACTION;

/** \struct SCRIPT
 *	@brief
 *
 */
typedef	struct
{
	uint16_t script_identifier;		///<script identifier
	uint16_t num_of_actions;		///<number of actions
	ACTION	 *act_ptr;			    ///<see ACTION pointer to Action
} SCRIPT;

/** \struct ATTRIBUTE_ACCESS_ITEM
 *	@brief
 *
 */
typedef struct
{
	int8_t attribute_id;		///<attribute index
	int8_t access_mode;			///<See E_ACCESS_METHOD access mode
	CHOICE_VAL	value;			///<See CHOICE_VAL

} ATTRIBUTE_ACCESS_ITEM;

/** \struct METHOD_ACCESS_ITEM
 *	@brief
 *
 */
typedef struct
{
	int8_t method_id;			///<Method index
	int8_t access_mode;			///<access mode of method
} METHOD_ACCESS_ITEM;

/** \struct ACCESS_RIGHTS_ELEMENT
 *	@brief
 *
 */
typedef struct
{
	uint16_t num_of_attr;					///< number of attributes
	ATTRIBUTE_ACCESS_ITEM *attr_access;		///<See ATTRIBUTE_ACCESS_ITEM
	uint16_t num_of_method;					///<number of methods
	METHOD_ACCESS_ITEM *method_access;		///<See METHOD_ACCESS_ITEM
} ACCESS_RIGHTS_ELEMENT;

/** \struct LN_OBJ_ELEMENT
 *	@brief
 *
 */
typedef struct
{
	uint16_t class_id;					///<Class id
	uint8_t  version;					///<Version od the class
	uint8_t		ln[LOGICAL_NAME_LEN];	///<Logical Name
	ACCESS_RIGHTS_ELEMENT ar_element;	///<See ACCESS_RIGHTS_ELEMENT
} LN_OBJ_ELEMENT;

/** \struct ASSOCIATED_PARTNERS_TYPE
 *	@brief
 *
 */
typedef struct
{
	int8_t	 client_SAP;			///<Client SAP
	uint16_t server_SAP;			///<Server SAP
} ASSOCIATED_PARTNERS_TYPE;

/** \struct XDLMS_CONTEXT_TYPE
 *	@brief
 *
 */
typedef struct
{
	uint8_t *conformance;			///<Pointer to conformance block
	uint16_t max_receive_pdu_size;	///<Maximum recieve PDU size
	uint16_t max_send_pdu_size;		///<Maximum send PDU size
	uint8_t  dlms_version_number;	///<DLMS version number
	int8_t   quality_of_service;	///<quality of service
	CHOICE_VAL cyphering_info;		///<Cypher info.  See CHOICE_VAL
} XDLMS_CONTEXT_TYPE;

/** \struct ASSLIST_ELEMENT
 *	@brief Association List element
 *
 */
typedef struct
{
	uint16_t SAP;				///< Server logical address
	CHOICE_VAL	value;			///<See CHOICE_VAL - Logical device name
}ASSLIST_ELEMENT;

/** \struct SEASON
 *	@brief Season profile
 *
 */
typedef struct
{
	uint8_t* season_profile_name;	///<Season profile name
	uint8_t* season_start;			///<Season start time
	uint8_t* week_name;				///<Week name
}SEASON;

/** \struct WEEK_PROFILE
 *	@brief Week profile
 *
 */
typedef struct
{
	uint8_t* week_name;				///<Week name
	uint8_t monday;				    ///<Id of Monday
	uint8_t tuesday;			    ///<Id of Tuesday
	uint8_t wednesday;			    ///<Id of Wed
	uint8_t thursday;			    ///<Id of Thursday
	uint8_t friday;			    	///<Id of Friday
	uint8_t saturday;			    ///<Id of Saturday
	uint8_t sunday;			    	///<Id of Sunday
} WEEK_PROFILE;

/** \struct DAY_PROFILE_ACTION
 *	@brief Day Profile Action
 *
 */
typedef struct
{
	uint8_t *start_time;			///<Start time
	uint8_t  script_ln[LOGICAL_NAME_LEN];	///< Script logical name
	uint16_t *script_sel;			///<Script selector
} DAY_PROFILE_ACTION;

/** \struct DAY_PROFILE
 *	@brief Day Profile
 *
 */
typedef struct
{
	uint8_t *day_id;			///<Day Id
	uint8_t *num_of_tou;		///<Number of TOU
	DAY_PROFILE_ACTION *day_schedule;	///<See DAY_PROFILE_ACTION
} DAY_PROFILE;

/** \struct VALUE_DEFINITION
 *	@brief
 *
 */
typedef struct
{
	uint16_t class_id;
	uint8_t ln[LOGICAL_NAME_LEN];
	int8_t  attr_idx;
} VALUE_DEFINITION;

/** \struct ACTION_ITEM
 *	@brief
 *
 */
typedef struct
{
	uint8_t  script_ln[LOGICAL_NAME_LEN];
	uint16_t script_sel;
} ACTION_ITEM;

/** \struct ACTION_SET
 *	@brief
 *
 */
typedef struct
{
	ACTION_ITEM action_up;
	ACTION_ITEM action_down;
} ACTION_SET;

/** \struct EXECUTION_TIME_DATE
 *	@brief date and time of script execution
 *
 */
typedef struct
{
	uint8_t *time;		///<time of execution of script
	uint8_t *date;		///<Date of execution of script
} EXECUTION_TIME_DATE;

enum {
	BAUD_300,
	BAUD_600,
	BAUD_1200,
	BAUD_2400,
	BAUD_4800,
	BAUD_9600,
	BAUD_19200,
	BAUD_38400,
	BAUD_57600,
	BAUD_115200
};

typedef struct
{
	uint16_t class_id;
	uint8_t  ln[LOGICAL_NAME_LEN];
	uint16_t num_of_group_e_val;
	uint8_t  *group_e_val;
	int8_t   attribute_index;
} TABLE_CELL_DEFINITION;

typedef struct
{
	uint8_t ref_table_id;
	union
	{
		uint16_t first_entry;
		uint16_t num_of_tbl_entries;
		uint16_t *table_entries;
	} te;
} MAPPING_TABLE;


/** \struct DATA_CLASS
 *	@brief
 *
 */
typedef struct
{
	uint8_t		ln[LOGICAL_NAME_LEN];	///<logical name of data object
	volatile CHOICE_VAL	value;			///<See CHOICE_VAL
} DATA_CLASS;

/** \struct REGISTER_CLASS
 *	@brief
 *
 */
typedef struct
{
	uint8_t		ln[LOGICAL_NAME_LEN]; 	///<logical name of register object
	CHOICE_VAL	value;				 	///<See CHOICE_VAL
	SCALAR_UNIT su; 					///<See SCALAR_UNIT
} REGISTER_CLASS;

/** \struct EXTENDED_REGISTER
 *	@brief
 *
 */
typedef struct
{
	uint8_t		ln[LOGICAL_NAME_LEN]; 	///<Logical name of extended register object
	CHOICE_VAL	value;				  	///<See CHOICE_VAL
	SCALAR_UNIT su;						///<See SCALAR_UNIT
	CHOICE_VAL	status;					///<See	CHOICE_VAL
	uint8_t*	capture_time;			///<Time of capture
} EXTENDED_REGISTER;

/** \struct DEMAND_REGISTER
 *	@brief
 *
 */
typedef struct
{
	uint8_t     ln[LOGICAL_NAME_LEN]; //static
	CHOICE_VAL  current_avg_val;
	CHOICE_VAL  last_avg_val;
	SCALAR_UNIT su;  //static
	CHOICE_VAL  status;
	uint8_t		capture_time[DATE_TIME_LEN];
	uint8_t		state_time_current[DATE_TIME_LEN];
	uint32_t	period;   //static
	uint16_t	num_of_periods;   //static
} DEMAND_REGISTER;
#if 0
/**** REGISTER ACTIVATION IC ****/
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];  //static

	// register assignment list
	int num_of_rl_elements;       //static
	REGISTER_LIST *rl_ptr;        //static

	// mask list
	int num_of_mask_names;        //static
	MASK_LIST *ml_ptr;            //static

	uint8_t active_mask[MAX_MASK_NAME_SIZE];

} REGISTER_ACTIVATION;
#endif
/** \struct PROFILE_GENERIC
 *	@brief
 *
 */
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];		///<logical name of profile generic object
	uint16_t num_of_buf_entries;   		///<number of buffer entries
	CHOICE_VAL *buf_ptr;				///<Pointer to buffer. See CHOICE_VAL
	uint16_t num_of_cap_objects;	   	///<number of capture objects
	CAPTURE_OBJECT *cap_obj_ptr;		///<Pointer to Capture object. See 	CAPTURE_OBJECT
	uint32_t *capture_period;		   	///<Capture perios
	uint8_t sort_method;      		   	///<Sort method
	CAPTURE_OBJECT sort_obj;      		///<Sort object. See CAPTURE_OBJECT
	uint32_t *entries_in_use;			///<Entries in profile buffer
	uint32_t *profile_entries;    		///<Maximum entries in profile buffer
} PROFILE_GENERIC;

/** \struct CLOCK_CLASS
 *	@brief
 *
 */
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];	///<Logical name of clock object
	uint8_t *date_time;				///< date and time
	uint16_t *time_zone;			///<Time zone
	uint8_t status;					///<clock status
	uint8_t daylight_begin[DATE_TIME_LEN];	///<dayalight begin values
	uint8_t daylight_end[DATE_TIME_LEN];	///<daylight end value
	int8_t  daylight_deviation;				///<daylight deviation
	int8_t  daylight_enabled;				///<daylight enable
	int8_t  clock_base;						///<clock status
}CLOCK_CLASS;

/** \struct SCRIPT_TABLE
 *	@brief
 *
 */
typedef struct
{
	uint8_t  ln[LOGICAL_NAME_LEN];
	uint16_t num_of_scripts;
	SCRIPT	 *scr_ptr;
} SCRIPT_TABLE;
#if 0
/****  SCHEDULE IC ****/
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];
	uint16_t num_of_ste;
	SCHEDULE_TABLE_ENTRY *ste_ptr;
} SCHEDULE;


/**** SPECIAL DAY ENTRY IC ****/
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];
	uint16_t num_of_entries;
	SPEC_DAY_ENTRY *ent_ptr;
} SPEC_DAY_TABLE;


/**** ASSOCIATION SN IC ****/
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];
	uint16_t num_of_objs;
	SN_OBJ_ELEMENT *obj_ptr;
	uint16_t num_of_ar_element;
	ACCESS_RIGHTS_ELEMENT *ar_ptr;
	uint8_t sec_ref[LOGICAL_NAME_LEN];
} ASSOCIATION_SN;
#endif
/** \struct ASSOCIATION_LN
 *	@brief
 *
 */
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];		///<Logicalname of association object
	uint16_t num_of_objs;				///<number of objects
	LN_OBJ_ELEMENT *obj_ptr;			///pointer to object elements. See LN_OBJ_ELEMENT
	ASSOCIATED_PARTNERS_TYPE* ap_id;	///<pointer toassociated pertenrs. See ASSOCIATED_PARTNERS_TYPE
	CHOICE_VAL app_context;				///<See CHOICE_VAL
	XDLMS_CONTEXT_TYPE* xdlms_info;		///<xdlms info. See XDLMS_CONTEXT_TYPE
	CHOICE_VAL auth_mech;				///<authenticaation mechanism. See CHOICE_VAL
	CHOICE_VAL secret;					///secret value. See CHOICE_VAL
	int8_t  association_status;			/// Association status
//	uint8_t sec_ref[LOGICAL_NAME_LEN];

} ASSOCIATION_LN;

/** \struct SAP_ASSIGNMENT
 *	@brief
 *
 */
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];		///<logical name
	uint16_t num_of_element;			///<number of elements
	ASSLIST_ELEMENT	*asslist_ptr;		///pointer assocation list
} SAP_ASSIGNMENT;

/** \struct ACTIVITY_CALENDAR
 *	@brief
 *
 */
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];		///<logical name
	uint8_t *calendar_name_act;		   	///<Pointer to Active calendar name
	uint16_t num_of_act_seasons;		///<Active calendar seasons
	SEASON *act_season_ptr;				///<pointer to active calendar seaon. See SEASON
	uint16_t num_of_act_week_profile;	///<Active week profiles
	WEEK_PROFILE *act_week_profile_ptr;	///<pointer to active week profile. See WEEK_PROFILE
	uint16_t num_of_act_day_profile;	///<Active day profiles.
	DAY_PROFILE	 *act_day_profile_ptr;	///<Pointer to Active day profile. See DAY_PROFILE
	uint8_t *calendar_name_pas;			///<Pointer to Passive calendar name
	uint16_t num_of_pas_seasons;		///<Passive season profiles
	SEASON *pas_season_ptr;				///<pointer to Passive season profile. See 	SEASON
	uint16_t num_of_pas_week_profile;	///<Passive week profiles
	WEEK_PROFILE *pas_week_profile_ptr;	///<pointer to Passive week profile. See WEEK_PROFILE
	uint16_t num_of_pas_day_profile;	///<Passive day profiles.
	DAY_PROFILE	 *pas_day_profile_ptr;	///<Pointer to Passive day profile. See DAY_PROFILE
	uint8_t	 *act_pas_cal_time;			///<Pointer to Activate calendar time
} ACTIVITY_CALENDAR;

/** \struct REGISTER_MONITOR
 *	@brief
 *
 */
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];
	uint8_t num_of_thresholds;
	REGISTER_CLASS *threshold_ptr;
	VALUE_DEFINITION valdef;
	uint16_t num_of_actions;
	ACTION_SET *act_ptr;
}REGISTER_MONITOR;

/** \struct SINGLE_ACTION_SCHEDULE
 *	@brief
 *
 */
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];	///<Logical Name
	ACTION_ITEM executed_script;	///<Executed script
	uint8_t type;					///<Type
	uint16_t num_of_times;			///<Number of execution times
	EXECUTION_TIME_DATE *time_ptr;	///<pointer to execution time. See EXECUTION_TIME_DATE
} SINGLE_ACTION_SCHEDULE;

/** \struct IEC_LOCALPORT_SETUP
 *	@brief
 *
 */
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];
	uint8_t default_mode;
	uint8_t default_baud;
	uint8_t prop_baud;
	uint8_t response_time;
	uint8_t dev_addr[LOGICAL_NAME_LEN];
	uint8_t pass_p1[20];
	uint8_t pass_p2[20];
	uint8_t pass_w5[20];
} IEC_LOCALPORT_SETUP;


/** \struct IEC_HDLC_SETUP
 *	@brief
 *
 */
typedef struct
{
	uint8_t ln[LOGICAL_NAME_LEN];	///<Logical Name
	uint8_t comm_speed;				///<Communication Speed
	uint8_t window_size_trans;		///<Transmit Window Size
	uint8_t window_size_recv;		///<Recieve Windows size
	uint16_t max_info_field_len_trans;	///<Maximum Transmit info field
	uint16_t max_info_field_len_recv;	///<Maximum Recieve info field
	uint16_t inter_octet_timeout;		///<Inter Octet time in milliseconds
	uint16_t inactivity_timeout;		///<Inactivity Time in seconds
	uint16_t dev_addr;					///<Device Physical address
} IEC_HDLC_SETUP;

/** \struct ACCESS_BY_ENTRY
 *	@brief Access Entry descriptor
 *
 */
typedef struct
{
	uint32_t	from_entry;	///<first entry to retrieve
	uint32_t	to_entry;	///<last entry to retrieve
	uint16_t	from_val;	///<Index of first value to retieve
	uint16_t	to_val;		///<Index of last value to retrieve
}ACCESS_BY_ENTRY;

/** \struct ACCESS_BY_RANGE
 *	@brief Access Range descriptor
 *
 */
typedef struct
{
	CAPTURE_OBJECT	cap_object;	///<capture object restricting the range
	uint8_t from_val[12];		///<smallest entry to retrieve
	uint8_t	to_val[12];		///<largest entry to retireve
	uint8_t num_selected_val;	///<Number of seleced values. Only value '0' supported
	CAPTURE_OBJECT	*selected_val;   ///<List of columns to retrieve
}ACCESS_BY_RANGE;

/** \struct ACCESS_SELECTOR
 *	@brief 	Selective Access parameters
 *
 */
typedef struct
{
	uint8_t* access_param;			///<Access Selector
	ACCESS_BY_ENTRY *access_entry;	///<pointer to Access entry descriptor
	ACCESS_BY_RANGE *access_range;	///<Pointer to Access range descriptor
}ACCESS_SELECTOR;


#endif //__DLMS_OBJ_MGR_PUBLIC_H__


