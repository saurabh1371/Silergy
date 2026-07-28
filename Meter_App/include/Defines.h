#include <stdint.h>     // Standard integer definitions.
#include"Prototypes.h"
extern uint8_t NM_CT_Detected;

#define VCC_EEPROM 22
#define OPTICAL_POWER 30 
//#define LCD_BACKLIGHT_DIO 10// Removed in new rev
#define PULSE_VCC 29

#define SUMSAMPS 4096
#define WRATE 7717//6632//10910

#define WRATE_NM 1527//3050//8628//7071
#define IDP 1000    // PCB_I_MAXA 
#define VDP 10  

#define PCB_I_MAXA     356//62.9A
#define PCB_I_MAXB     528//44.0 2500, 10 Ohm
#define PCB_I_MIN       6//30   // milliamps
#define PCB_I_LIMIT    840  // I_MAXA * .95, 0.1A

// Voltage sensors depend on the electronics of the PCB.
#define PCB_V_MAX     5073 //405.5V
#define PCB_V_MIN     80  // 80V
#define PCB_V_NOM     2400  // 240V
#define PCB_V_LIMIT   2880  // 288.0 V (V_NOM * 120%)

// These depend on the range of i_max, and v_max,
// which depend on the electronics of the PCB.
#define PCB_WH_WHPP      1  // 1 WH of Wh/pulse
#define PCB_P_WHPP       1  // 1 pulse of Wh/pulse
// The nominal Hz is used only for simulating a signal.
#define PCB_HZ_NOM      50  // 50Hz is normal.

// By convention, if an input or output port is defined, the pin's
// facility exists. e.g. if PCB_BUTTON_SEG is defined, a pushbutton exists.

// Pushbutton's locations
#define PCB_BUTTON_SEG  26              // Port's LCD-segment number
#define PCB_BUTTON_INT  PCB_BUTTON_SEG  // External interrupt number.
//#define PCB_BUTTON_PUSHED false         // low = pushed
#define PCB_BUTTON_EW EW_PB             // Enable wake bit mask.
#define PCB_BUTTON_WS EW_PB             // Wake reason bit mask.

// Scroll button's locations
//#define PCB_SCROLL_SEG  3       // Port's bit number for scroll button
//#define PCB_SCROLL_PUSHED false         // low = pushed

// Case tamper-detect's locations. (sw3, low=off)

		#define PCB_CASE_TAMPER_SEG  24         // Port's LCD-segment number
		#define PCB_CASE_TAMPER_INT PCB_CASE_TAMPER_SEG // External interrupt number.
		#define PCB_CASE_TAMPER 0               // low = tamper
		#define PCB_CASE_TAMPER_EW EW_WK1       // Enable wake bit mask.
		#define PCB_CASE_TAMPER_WF EW_WK1       // Wake reason bit mask.


// Case tamper-detect's locations. (sw3, low=off)
//#define PCB_CASE_TAMPER_SEG  34         // Port's LCD-segment number
//#define PCB_CASE_TAMPER_INT PCB_CASE_TAMPER_SEG // External interrupt number.
//#define PCB_CASE_TAMPER 1              // low = tamper
//#define PCB_CASE_TAMPER_EW EW_WK1       // Enable wake bit mask.
//#define PCB_CASE_TAMPER_WF EW_WK1       // Wake reason bit mask.//*/

#define MAGNET_PIN 40
#define NM_DETECT_PIN 21


#define CLR1 10
#define CLR2 20
#define CLR3 30
#define UNLOCK 40
#define CLRC 50
#define CAL 60
#define SLNO 70
#define RESET 80




// Case tamper-detect's locations. (sw3, low=off)
#define PCB_MAGNET_TAMPER_SEG  40         // Port's LCD-segment number
#define PCB_MAGNET_TAMPER_INT PCB_MAGNET_TAMPER_SEG // External interrupt number.
#define PCB_MAGNET_TAMPER 0               // low = tamper
#define PCB_MAGNET_TAMPER_EW EW_WK1       // Enable wake bit mask.
#define PCB_MAGNET_TAMPER_WF EW_WK1       // Wake reason bit mask.


#define TIMER1_MSEC 100


//For dlms variables and definitions
#define CLASS07_BLOCKLOAD_MAX_ENTRY			2880		/* Max number of entries */
#define CLASS07_DAILYLOAD_MAX_ENTRY			60		/* Max number of entries */
#define CLASS07_BILLING_MAX_ENTRY				12		/* Max number of entries */

#define CLASS07_EVENT_MAX_PROFILE				5		/* Max index of profile objects (from 0 ~ max) */
#define CLASS07_EVENT_MAX_ENTRY					60//100		/* Max number of entries */

/* Recording interval 1, for blockload profile (in second) */
#define CLASS07_BLOCKLOAD_INTERVAL			1800// for minutes USER
#define CLASS07_DAILYLOAD_INTERVAL			86400 /* 24 h*/


#define KWH_ARR  		50

#define HISTORY_SIZE        		13 	//13
#define TOD_HISTORY_SIZE        13 	//13
#define TOD_SIZE                5		//6 	//2   

#define TAMPER_OCC_DELAY     		60//in seconds
#define TAMPER_RES_DELAY    		60//in seconds

#define MAG_TAMPER_OCC_DELAY     	15	//in seconds
#define MAG_TAMPER_RES_DELAY     	15	//in seconds

#define EVENT_SIZE   	 60 //100
#define TAMPER_TYPE			5  //16
#define TOT_EVENT_TYPE	7  //8

#define VOLT_EVENT      0
#define AMP_EVENT       1
#define PFAIL_EVENT     2
#define TRANSACT_EVENT  3
#define OTHER_EVENT     4
#define NOROLL_EVENT    5
#define CTRL_EVENT      6 

//memory location

#define MTRNO_LOC 							1   //3
#define VOLT_FACT_LOC           4   //2
#define AMP_FACT_LOC            6  //2*2
#define IPH_LOC                 10  //2*2
#define IGAIN_LOC               14  //2*2
#define NCFDEN_LOC              18  //2*2

#define CAL_CHKSUM_LOC					30  //1
#define DISP_MD_TYPE_LOC				31  //1

//#define MD_INTGR_LOC          32  //2
//#define SURVEY_INTGR_LOC      34  //2

#define MANUFACT_DATE_LOC       36  //2
#define LOCK_STAT_LOC						38  //1	
#define TEST_LOC                39  //1


//#define AUTH0_LOC             40 //10
#define LOW_LEVEL_PASSWORD_LOC	40	//10 //AUTH1_LOC
#define HIGH_LEVEL_PASSWORD_LOC	50	//20 //AUTH2_LOC
#define AUTH3_LOC               70 //10



#define KWH_LOC									150	//4*50 = 200
#define KVAH_LOC								350	//4*50 = 200
#define KVARH_LEAD_LOC					550	//4*50 = 200
#define KVARH_LAG_LOC						750	//4*50 = 200

#define KWH_CHKSUM_LOC          950  //1*50
#define KVAH_CHKSUM_LOC         1000 //1*50
#define KVARH_LEAD_CHKSUM_LOC   1050 //1*50
#define KVARH_LAG_CHKSUM_LOC    1100 //1*50

#define TIME_LOC 								1156  	//3
#define DATE_LOC 								1159  	//3
#define HR_LOC 		 							1162 	//1

#define MNTHPOS_LOC             	1163	//1
#define DAYPOS_LOC 		        		1164	//1
#define HRPOS_LOC 		        		1165	//2
#define MD_RESET_LOC            	1167	//2
#define ONTIME_LOC              	1169	//3
#define RST_ONTIME_LOC          	1172	//3
#define PREV_MIN_SLOT_LOC	      	1175    //1
#define PREV_SURVEY_MIN_SLOT_LOC	1176    //1
#define NUM_ZONE_LOC		        	1177	//1
//#define TOD_ID_LOC              1178    //1*8 = 8
#define BILL_DAY_LOC              1186    //1
#define BILL_TIME_LOC             1187    //2
#define HRCNT_LOC 		        		1189	//2

#define OVER_CURRENT_VAL_LOC 			1191	//2
#define OVER_LOAD_VAL_LOC 				1193	//2
#define CONN_TIME_INTERVAL_LOC 		1195	//2
#define CONN_LOCKOUT_TIME_LOC 		1197	//2
#define OLOAD_STAT_LOC            1199    //1
#define CONN_TIME_REPEAT_LOC      1200    //1

#define TAMPER_OCC_TIME_LOC       1201    //2
#define TAMPER_RES_TIME_LOC       1203    //2
#define LOAD_CONTROL_STAT_LOC     1205    //1
#define DAYCNT_LOC 		        		1206	//1
#define PROGRAMCNT_LOC            1207    //4
//#define PASSIVE_TOD_ID_LOC      1211    //1*8 = 8
#define IST_DIFF_LOC              1219    //2
//#define LAST_BILL_DAY           1221    //1        
#define LAST_BILL_TIME_LOC        1222    //2
#define LAST_BILL_DATE_LOC        1224    //3
#define SCHEDULED_BILL_DAY_LOC	  1227	//4
#define SCHEDULED_BILL_TIME_LOC	  1231	//2
#define MD_INTGR_LOC              1233    //2
#define SURVEY_INTGR_LOC          1235    //2

#define KWMD_LOC                  1240 //(3+2+4+2+3+2+2+1)*13 =(19*13)= 247
                                     //bdate,btime,kwh,kwmd,date,time,ontime,pf

#define KVAMD_LOC                 1500 //(4+2+3+2)*13 =(11*13)= 143
                                       	    //kvah,kvamd,date,time

#define T_ZONE_LOC                1680    //1                                       
#define TOD_LOC                   1681    //2*16 = 32
#define TOD_KWMD_LOC              1715    //(4+2+3+2)*8*13=88*13=1144    
#define TOD_KVAMD_LOC             2859    //(4+2+3+2)*8*13=88*13=1144    

#define PASSIVE_TOD_LOC           4010    //2*16 = 32
#define ACTIVATE_DATE_LOC         4045    //3
#define ACTIVATE_TIME_LOC         4048    //2

#define TOD_ID_LOC                4050    //1*16 = 16
#define PASSIVE_TOD_ID_LOC        4066    //1*16 = 16


#define LAST_DEMAND_KWH_LOC	      4110 //4+4
#define LAST_STORE_TOD_KWH_LOC    4118 //4+4+4+4
#define LAST_HR_KWH_LOC           4134 //4+4+4+4 
//#define LAST_DAY_KWH_LOC        2954   //4 
//#define MD_BSET_LOC			        2458   //2
#define KWCD_LOC		              4150   //4

#define LST_MNTH_DATA_LOC         4160 //2,2,2,3,4,4  //power_on, tamper cnt, kwh, kvah
#define BILLPT_LOC                4180 //2,2,2,2   //power_on, tamper cnt

//--------------------------->    2289
#define EVENTPOS_LOC		          4200    //2*10 //10 type of event groups
#define ALLTAMPER_LOC		          4220    //3
#define TAMPERCNTARR_LOC	        4224    //2*18=36 //16 meter related tamper events

#define POFF_CNT_LOC              4300 //4
#define POFF_DUR_LOC              4304 //4
#define PON_DUR_LOC               4308 //4

#define VOLT_EVENT_LOC            4400	//(2+3+3+2+2+1+4)*60 =17*60=1020
#define AMP_EVENT_LOC             5500	//(2+3+3+2+2+1+4)*60 =17*60=1020
#define PFAIL_EVENT_LOC           6600	//(2+3+3)*60 =8*60=480
#define TRANSACT_EVENT_LOC        7100	//(2+3+3)*60 =8*60=480
#define OTHER_EVENT_LOC           7600	//(2+3+3+2+2+1+4)*60 =17*60=1020       
#define NOROLL_EVENT_LOC          8700	//(2+3+3)*1  =7
#define CTRL_EVENT_LOC            8710	//(2+3+3)*60 =8*60=480

#define EVENTCNT_LOC	            9200    //2*10 //10 type of event groups
#define CUOPEN_LOC	              9230	//1+3+3

#define KWMD_MIN_FLAG_LOC         9300    //60
#define VOLT_MIN_LOC              9360    //120									   
#define AMP_MIN_LOC               9480    //120						
//#define PF_MIN_LOC              7600    //60						

#define LSTTAMPER_OCC_LOC         9660    //1+3+3
#define LSTTAMPER_RES_LOC         9670    //1+3+3
#define LAST_TAMPER_ID_LOC        9680    //2*7         
#define CAL_NAME_ACTIVE_LOC       9700    //20  
#define CAL_NAME_PASSIVE_LOC      9720    //20
#define SEASON_NAME_ACTIVE_LOC    9740    //10*2  
#define SEASON_NAME_PASSIVE_LOC   9760    //10*2
#define SEASON_DATE_ACTIVE_LOC    9780    //4*2
#define SEASON_DATE_PASSIVE_LOC   9788    //4*2
#define LAST_TAMPER_DT_LOC        9800    //(3+3)*7 //42


#define INST_TAMPER_OCC_LOC       10000   //(1+3+3+4+2+2+1)*10 //160
#define INST_TAMPER_RES_LOC       10200   //(1+3+3+4+2+2+1)*10 //160

#define LOAD_SURVEY_LOC           10496 //10400  	//(4+2+2+2+2)*48*60=34560 //43200
					                              // date+time,kwh,kvah,voltage,current					
#define DAILY_SURVEY_LOC          53760 //53600	//(3+4+4)*60=660 //990
					                              // date,kwh,kvah

#define MEM_END                   54800 //54600 //eeprom keeps valid data upto this address.							   


/*
#define AUTH1_LOC               40 //10
#define AUTH2_LOC               50 //20
#define AUTH3_LOC               70 //10

#define KWH_LOC									130	//4*50 = 200
#define KVAH_LOC								330	//4*50 = 200
#define KWH_CHECKSUM_LOC				530 //1*50
#define KVAH_CHECKSUM_LOC				580 //1*50

#define LOW_LEVEL_PASSWORD_LOC	630	//10
#define HIGH_LEVEL_PASSWORD_LOC	640	//20


#define LAST_STORE_TOD_KWH_LOC  660 //4+4
//#define KWMD_MIN_LOC  				680 //60
#define KWMD_MIN_FLAG_LOC				680 //60
#define VOLT_MIN_LOC						740	//120
#define AMP_MIN_LOC							860	//120


#define ONTIME_LOC              990     //4
#define TIME_LOC								998     //3
#define DATE_LOC 								1001	//3
#define HR_LOC									1004 	//1
#define PREV_MIN_SLOT_LOC				1005 	//1
#define PREV_SURVEY_MIN_SLOT_LOC	1006	//1
#define MNTHPOS_LOC             1007	//1
#define MD_RESET_LOC            1008    //2
#define MD_INTGR_LOC						1010	//2
#define SURVEY_INTGR_LOC				1012	//2
#define HRCNT_LOC								1018	//2
#define LAST_HR_KWH_LOC					1020	//4+4
#define KWCD_LOC								1028  //4
#define LAST_DEMAND_KWH_LOC			1032	//4+4
#define DAYCNT_LOC							1040	//2
#define LAST_BILL_TIME_LOC      1042  //2
#define LAST_BILL_DATE_LOC      1044  //3

#define SCHEDULED_BILL_DAY_LOC	1047	//4
#define SCHEDULED_BILL_TIME_LOC 1051	//2
#define CKWMD_LOC								1053	//3

#define KWMD_LOC                1100    //(12+4+4+4+4+4+4+4+4+4+4+4+4+4+2+12+2+12+4)=>96*13 = //1248
//bdate-12,
//pf-4,
//kwh-4,
//kwh tz0-4,
//kwh tz1-4,
//kwh tz2-4,
//kwh tz3-4,
//kwh tz4-4,
//kvah-4,
//kvah tz0-4,
//kvah tz1-4,
//kvah tz2-4,
//kvah tz3-4,
//kvah tz4-4,
//kw md-2,
//kw md date time-12
//kvamd-2
//kva md date time-12
//Power on time-4

#define T_ZONE_LOC              2500    //1                                       
#define TOD_LOC                 2501    //2*16 = 32
#define PASSIVE_TOD_LOC         2533    //2*16 = 32
#define ACTIVATE_DATE_LOC       2565    //3
#define ACTIVATE_TIME_LOC       2567    //2
#define TOD_ID_LOC              2570    //1*16 = 16
#define PASSIVE_TOD_ID_LOC      2586    //1*16 = 16
#define CAL_NAME_ACTIVE_LOC     2700    //20  
#define CAL_NAME_PASSIVE_LOC    2720    //20
#define SEASON_NAME_ACTIVE_LOC  2740    //10*2  
#define SEASON_NAME_PASSIVE_LOC 2760    //10*2
#define SEASON_DATE_ACTIVE_LOC  2780    //4*2
#define SEASON_DATE_PASSIVE_LOC 2788    //4*2

#define LST_MNTH_DATA_LOC       2800 //2,2,2,3,4,4  //power_on, tamper cnt, kwh, kvah
#define BILLPT_LOC              2820 //2,2,2,2   //power_on, tamper cnt

#define LSTTAMPER_OCC_LOC       3000 //9660    //1+3+3
#define LSTTAMPER_RES_LOC       3010 //9670    //1+3+3
#define LAST_TAMPER_ID_LOC      3020 //9680    //2*7         

#define INST_TAMPER_OCC_LOC     3050   //(1+3+3+4+2+2+1)*10 //160
#define INST_TAMPER_RES_LOC     3250   //(1+3+3+4+2+2+1)*10 //160

#define EVENTPOS_LOC		        3500    //2*10 //10 type of event groups
#define ALLTAMPER_LOC		        3520    //3
#define TAMPERCNTARR_LOC	      3524    //2*18=36 //16 meter related tamper events

#define POFF_CNT_LOC            3600  //4
#define POFF_DUR_LOC            3604  //4
#define PON_DUR_LOC             3608  //4
#define CUOPEN_LOC	            3620	//1+3+3
#define PROGRAMCNT_LOC          3630  //4

#define TAMPER_LOC              4000
//date-time 12
//TamperCode 2
//current 4
//voltage 4
//pf      4
//kwh     4

#define VOLT_EVENT_LOC          4400	//(2+3+3+2+2+1+4)*60 =17*60=1020
#define AMP_EVENT_LOC           5500	//(2+3+3+2+2+1+4)*60 =17*60=1020
#define PFAIL_EVENT_LOC         6600	//(2+3+3)*60 =8*60=480
#define TRANSACT_EVENT_LOC      7100	//(2+3+3)*60 =8*60=480
#define OTHER_EVENT_LOC         7600	//(2+3+3+2+2+1+4)*60 =17*60=1020       
#define NOROLL_EVENT_LOC        8700	//(2+3+3)*1  =7
#define CTRL_EVENT_LOC          8710	//(2+3+3)*60 =8*60=480

#define EVENTCNT_LOC	          9200    //2*10 //10 type of event groups



#define HRPOS_LOC								10500	//2
#define DAYPOS_LOC							10502	//1
#define LOAD_SURVEY_LOC   			10510 //(4+2+2+2+2)*48*60=34560
																// date,voltage,kwh,kvah,current

#define DAILY_SURVEY_LOC  50000	//(4+4+4)*60=720
																// date,kwh,kvah

#define MEM_END									65536
*/

extern unsigned long int scratch, scratch1, scratch2;
extern unsigned char d_yr,d_mnth,d_day,t_hr,t_min,t_sec;
extern unsigned long real_time, real_date;//,prev_real_date;

extern unsigned char set_cal_name_data;
extern unsigned char set_auth_data;
extern unsigned char set_rtc_data;
extern unsigned char set_demand_intgr_period;
extern unsigned char set_capture_period;
extern unsigned char set_bill_date;
extern unsigned char set_day_profile;
extern unsigned char set_season_profile;
extern unsigned char set_activation_date;

extern uint8_t auth0[2];
extern uint8_t auth1[10];
extern uint8_t auth2[18];
extern uint8_t auth3[10];

//extern uint8_t Meter_Sr_No[9];
//extern uint16_t Year_of_Manufacture;

/*
extern unsigned char set_load_control_stat;
extern unsigned char set_over_current_val;
extern unsigned char set_over_load_val;
extern unsigned char set_conn_time_interval;
extern unsigned char set_conn_lockout_time;
extern unsigned char set_conn_time_repeat;
extern unsigned char set_tamper_time;
*/
