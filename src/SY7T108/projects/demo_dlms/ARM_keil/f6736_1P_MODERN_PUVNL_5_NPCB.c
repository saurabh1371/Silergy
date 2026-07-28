
/*
calendar name(passive/active)
array of
season name, season start(date_time), week name
array of
week name, 7 day_ids
array of day_id. 8 (start_times, script_logical_name, script_ids)
*/

/*
 Manoj Tyagi: 05-01-10
 Msp47197 3Phase Design
 
 to do:20-11-10
 1. kw, pf factor to be fixed
 2. pulse eerror in pf is -20%
 3. one phase and neutral removed.. error is -20%
*/
/*****************************************************************************/

#define MCLK_DEF        8


//#define TAMPER_DELAY     	30 //182//in seconds
//#define TAMPER_RESTORE_DELAY    30 //28//in seconds
#define TAMPER_TYPE		5 //18 //15 //12 //16

#define EVENT_SIZE     	        60 //occ+res //100 //200 //200 occ and 200 res 
#define TOT_EVENT_TYPE          7 

#define VOLT_EVENT      0
#define AMP_EVENT       1
#define PFAIL_EVENT     2
#define TRANSACT_EVENT  3
#define OTHER_EVENT     4
#define NOROLL_EVENT    5
#define CTRL_EVENT      6 


#define KWH_ARR  		50
//#define TOD_VAL_ARR           20
#define HISTORY_SIZE            13 //7
#define TOD_SIZE                5  //8
#define LOCK_CONST              100  

//memory
#define MTRNO_LOC 		1   //3
#define VOLT_FACT_LOC           4   //2
#define AMP_FACT_LOC            6  //2*2
#define IPH_LOC                 10  //2*2
#define IGAIN_LOC               14  //2*2
#define NCFDEN_LOC              18  //2*2

#define CAL_CHKSUM_LOC		30  //1
#define DISP_MD_TYPE_LOC	31  //1

//#define MD_INTGR_LOC            32  //2
//#define SURVEY_INTGR_LOC        34  //2

#define MANUFACT_DATE_LOC       36  //2
#define LOCK_STAT_LOC		38  //1	
#define TEST_LOC                39  //1


//#define AUTH0_LOC               40 //10
#define AUTH1_LOC               40 //10
#define AUTH2_LOC               50 //20
#define AUTH3_LOC               70 //10

#define KWH_LOC			150	//4*50 = 200
#define KVAH_LOC		350	//4*50 = 200
#define KVARH_LEAD_LOC		550	//4*50 = 200
#define KVARH_LAG_LOC		750	//4*50 = 200

#define KWH_CHKSUM_LOC          950  //1*50
#define KVAH_CHKSUM_LOC         1000 //1*50
#define KVARH_LEAD_CHKSUM_LOC   1050 //1*50
#define KVARH_LAG_CHKSUM_LOC    1100 //1*50

#define TIME_LOC 		1156  	//3
#define DATE_LOC 		1159  	//3
#define HR_LOC 		 	1162 	//1

#define MNTHPOS_LOC                     1163	//1
#define DAYPOS_LOC 		        1164	//1
#define HRPOS_LOC 		        1165	//2
#define MD_RESET_LOC                    1167	//2
#define ONTIME_LOC                      1169	//3
#define RST_ONTIME_LOC                  1172	//3
#define PREV_MIN_SLOT_LOC	        1175    //1
#define PREV_SURVEY_MIN_SLOT_LOC	1176    //1
#define NUM_ZONE_LOC		        1177	//1
//#define TOD_ID_LOC                      1178    //1*8 = 8
#define BILL_DAY_LOC                    1186    //1
#define BILL_TIME_LOC                   1187    //2
#define HRCNT_LOC 		        1189	//2

#define OVER_CURRENT_VAL_LOC 		1191	//2
#define OVER_LOAD_VAL_LOC 		1193	//2
#define CONN_TIME_INTERVAL_LOC 		1195	//2
#define CONN_LOCKOUT_TIME_LOC 		1197	//2
#define OLOAD_STAT_LOC                  1199    //1
#define CONN_TIME_REPEAT_LOC            1200    //1

#define TAMPER_OCC_TIME_LOC             1201    //2
#define TAMPER_RES_TIME_LOC             1203    //2
#define LOAD_CONTROL_STAT_LOC           1205    //1
#define DAYCNT_LOC 		        1206	//1
#define PROGRAMCNT_LOC                  1207    //4
//#define PASSIVE_TOD_ID_LOC              1211    //1*8 = 8
#define IST_DIFF_LOC                    1219    //2
//#define LAST_BILL_DAY                 1221    //1        
#define LAST_BILL_TIME_LOC              1222    //2
#define LAST_BILL_DATE_LOC              1224    //3
#define SCHEDULED_BILL_DAY_LOC	        1227	//4
#define SCHEDULED_BILL_TIME_LOC	        1231	//2
#define MD_INTGR_LOC                    1233    //2
#define SURVEY_INTGR_LOC                1235    //2

#define KWMD_LOC                1240 //(3+2+4+2+3+2+2+1)*13 =(19*13)= 247
                                     //bdate,btime,kwh,kwmd,date,time,ontime,pf

#define KVAMD_LOC               1500 //(4+2+3+2)*13 =(11*13)= 143
                                       	    //kvah,kvamd,date,time

#define T_ZONE_LOC              1680    //1                                       
#define TOD_LOC                 1681    //2*16 = 32
#define TOD_KWMD_LOC            1715    //(4+2+3+2)*8*13=88*13=1144    
#define TOD_KVAMD_LOC           2859    //(4+2+3+2)*8*13=88*13=1144    

#define PASSIVE_TOD_LOC         4010    //2*16 = 32
#define ACTIVATE_DATE_LOC       4045    //3
#define ACTIVATE_TIME_LOC       4048    //2

#define TOD_ID_LOC              4050    //1*16 = 16
#define PASSIVE_TOD_ID_LOC      4066    //1*16 = 16


#define LAST_DEMAND_KWH_LOC	4110 //4+4
#define LAST_STORE_TOD_KWH_LOC  4118 //4+4+4+4
#define LAST_HR_KWH_LOC         4134 //4+4+4+4 
//#define LAST_DAY_KWH_LOC      2954   //4 
//#define MD_BSET_LOC			2458   //2
#define KWCD_LOC		4150   //4

#define LST_MNTH_DATA_LOC       4160 //2,2,2,3,4,4  //power_on, tamper cnt, kwh, kvah
#define BILLPT_LOC              4180 //2,2,2,2   //power_on, tamper cnt

//--------------------------->  2289
#define EVENTPOS_LOC		4200    //2*10 //10 type of event groups
#define ALLTAMPER_LOC		4220    //3
#define TAMPERCNTARR_LOC	4224    //2*18=36 //16 meter related tamper events

#define POFF_CNT_LOC            4300 //4
#define POFF_DUR_LOC            4304 //4
#define PON_DUR_LOC             4308 //4

#define VOLT_EVENT_LOC          4400	//(2+3+3+2+2+1+4)*60 =17*60=1020
#define AMP_EVENT_LOC           5500	//(2+3+3+2+2+1+4)*60 =17*60=1020
#define PFAIL_EVENT_LOC         6600	//(2+3+3)*60 =8*60=480
#define TRANSACT_EVENT_LOC      7100	//(2+3+3)*60 =8*60=480
#define OTHER_EVENT_LOC         7600	//(2+3+3+2+2+1+4)*60 =17*60=1020       
#define NOROLL_EVENT_LOC        8700	//(2+3+3)*1  =7
#define CTRL_EVENT_LOC          8710	//(2+3+3)*60 =8*60=480

#define EVENTCNT_LOC	        9200    //2*10 //10 type of event groups
#define CUOPEN_LOC	        9230	//1+3+3

#define KWMD_MIN_FLAG_LOC       9300    //60
#define VOLT_MIN_LOC            9360    //120									   
#define AMP_MIN_LOC             9480    //120						
//#define PF_MIN_LOC              7600    //60						

#define LSTTAMPER_OCC_LOC       9660    //1+3+3
#define LSTTAMPER_RES_LOC       9670    //1+3+3
#define LAST_TAMPER_ID_LOC      9680    //2*7         
#define CAL_NAME_ACTIVE_LOC     9700    //20  
#define CAL_NAME_PASSIVE_LOC    9720    //20
#define SEASON_NAME_ACTIVE_LOC  9740    //10*2  
#define SEASON_NAME_PASSIVE_LOC 9760    //10*2
#define SEASON_DATE_ACTIVE_LOC  9780    //4*2
#define SEASON_DATE_PASSIVE_LOC 9788    //4*2

#define INST_TAMPER_OCC_LOC    10000   //(1+3+3+4+2+2+1)*10 //160
#define INST_TAMPER_RES_LOC    10200   //(1+3+3+4+2+2+1)*10 //160

#define LOAD_SURVEY_LOC        10496 //10400  	//(4+2+2+2+2)*48*60=34560 //43200
					        // date+time,kwh,kvah,voltage,current					
#define DAILY_SURVEY_LOC       53760 //53600	//(3+4+4)*60=660 //990
					        // date,kwh,kvah

#define MEM_END                54800 //54600 //eeprom keeps valid data upto this address.							   

#define INST_CMD        0
#define IR_INST_CMD     80
#define TIME_CMD        1
#define DATE_CMD        2
#define MTR_NO_CMD      3
//#define CLR_CMD         4
#define CLR1_CMD        4 
#define CLR2_CMD        85
#define DLOAD_CMD       5
#define CLR_TAMPER_CMD  6
#define TOD_CMD         8
#define CLR_TOD_CMD     9
#define CTRATIO_CMD     10
#define CAL_CMD         11
#define OFFSET_CMD      12
#define VI_CAL_CMD      13
#define CUOPEN_CMD      14
#define PHCAL_CMD       15
#define NCUT_CAL_CMD    16
#define RENERGY_CMD     17
#define RESET_CMD       18
#define INTGRTIME_CMD   19
#define LOCK_CMD        20

#define COMM_CMD			51
#define MAG_PULSE_CMD   	52
#define CBYP_CMD        	53
#define MDRESET_CMD     	54
#define NUM_ZONE_CMD    	55
#define DISP_MD_TYPE_CMD 	57

#define UNLOCK_CMD      170

// display data names

#define I_MTR_NO  0
#define I_TIME    10
#define I_LOCK_TIME    11
#define I_DATE    20
#define I_LOCK_DATE    21
#define I_VRMSR   30
#define I_VRMSY   31
#define I_VRMSB   32
#define I_VRMS    33
#define I_IRMSR   40
#define I_IRMSY   41
#define I_IRMSB   42
#define I_IRMS    43
#define I_IRMSN   44
#define I_PFR     50
#define I_PFY     51
#define I_PFB     52
#define I_PF      53
#define I_KWH     60
#define I_KVAH    70
#define I_RKVAH   80
#define I_KWMD    90
#define I_KVAMD   100
#define I_KWR	  110
#define I_KWY	  111
#define I_KWB	  112
#define I_KW	  113
#define I_KVAR	  120
#define I_KVAY	  121
#define I_KVAB	  122
#define I_KVA	  123
#define I_FREQR	  130
#define I_FREQY	  131
#define I_FREQB	  132
#define I_FREQ	  133

#define LCDCHK         	0
#define DATE           	1
#define TIME           	2	
#define KWH            	3
#define KWMD	       	4	
#define KVAMD	       	5
#define TCNT           	6
#define POWERON        	7
#define RSTCOUNT       	8
#define TSTATUS        	9
#define INSTKW 	      	10		
#define VRMS          	11
#define IRMS          	12
#define INSTPF        	13 
#define HRESKWH       	14
#define BKWH          	15 
#define BPF           	16
#define BKWMD         	17 
#define BKVAMD        	18
#define BTCNT         	19
#define BPOWERON      	20
#define BTKWH         	21
#define BTKWMD        	22
#define BTKVAMD       	23
#define CODE          	24
#define RTCOK         	25
#define MEMOK         	26
#define DISP_OFF      	27
#define SLNO          	28
#define HKWH   		29
#define MAGNET		30
#define CUOPEN		31
#define D_KWH		32
#define BKWMDONLY	33
#define KWMDONLY	34
#define FREQ		35
#define RTCSTAT  	36
#define UEI		37
#define HKWMD		38
#define FLASH		39
#define CUOPENONLY	40
#define HPF		41
#define THD		42
#define DLC		46
#define IRMSN		47
#define APF		48
#define BLANK		49
#define KVAH		50
#define BKVAH		51
#define TKWH		52
#define SLNOL		53
#define SLNOH		54
#define INSTKVA		55
#define HRESKVAH	58
#define DKWH		59
#define LASTOCC         60
#define LASTREST        61
#define NONE            62
#define HKVAH           63
#define TSTAT           64
#define VER             65
#define CKWMDONLY	66
#define HKVAMD          67
#define KVAMDONLY       68

/*
#define LCDCHK          1
#define SLNO            2
#define CTRATIO         3
#define DATE            4
#define TIME            5	
#define KWH             6
#define KVAH            7
#define KVARH_LEAD      8
#define KVARH_LAG       9
#define VRMS            10
#define IRMS            11
#define IRMSN           12
#define INSTPF          13
#define TOTPF           14
#define AVGPF           15
#define INSTKW          16
#define TOTKW           17
#define INSTKVA         18
#define TOTKVA          19
#define FREQ            20
#define TOTFREQ         21
#define KWMD	        22
#define KWMDONLY        23
#define KVAMD	        24
#define KVAMDONLY       25
#define KWRD	        26
#define KVARD	        27
#define BKWMD           28         
#define BKWMDONLY       29         
#define BKVAMD          30
#define BKVAMDONLY      31
#define TKWH            32
#define TKVAH           33
#define TKVARH_LEAD     34
#define TKVARH_LAG      35
#define TKWMD	        36
#define TKWMDONLY       37
#define BTKWMDONLY      38
#define TKVAMD	        39
#define TKVAMDONLY      40
#define BTKVAMDONLY     41
#define TPF             42
#define BTKWMD	        43
#define BTKVAMD	        44
#define BTPF            45
#define VFTCNT	        46
#define IFTCNT	        47
#define VUBTCNT	        48
#define IUBTCNT	        49
#define REVTCNT	        50
#define LPFTCNT	        51
#define MAGTCNT	        52
#define TCNT	        53
#define VPSEQ	        54
#define IPSEQ	        55
#define HRESKWH	        56
#define HRESKVAH	57
#define HRESKVARH_LEAD	58
#define HRESKVARH_LAG	59
#define KWCD            60        
#define CUOPEN          61
#define RSTCOUNT        62
#define MDATE           63
#define RSTDATE         64
#define RSTTIME         65
*/


//#include  <msp430x42x.h>
//#include  <msp430x471x7.h>
#include <msp430F6736A.h>
#include <math.h>
#include <string.h>
#include "iec62056_46_demo.c"

#include "hal_UCS.c"
#include "hal_pmm.c"
#include "sd24.c"


#define sbi(port,bit)  (port |= (1<<bit))   //set bit in port
#define cbi(port,bit)  (port &= ~(1<<bit))  //clear bit in port

//3 Phase
//vm(0 1 2),hv(3),lv(4),vub(5)
//rev(6 7 8), im(9 10 11), iub(12)
//mag(13),nd(14),lpf(15)    
//voltage 	Phase Miss(1-6), Over Voltage(7,8), 
//		Low Voltage(9,10), Voltage Unbalance(11,12)
//current 	Reverse(51-56), CT Open(57-62), Current Unbalance(63,64),
//		CT Bypass(65,66),Over Current(67,68)
//other events	magnet(201,202), nd(203,204), Low PF(205,206)
//const unsigned char event_ids[18]={1,3,5,7,9,11,51,53,55,57,59,61,63,65,67,201,203,205};

//1 Phase
//rev(0),el(1),mag(2),nd(3),nm(4)    
//current 	Reverse(51-52),EL(69-70)
//other events	magnet(201,202), nd(203,204), NM(207,208)
const unsigned char event_ids[10]={51,69,201,203,207};

unsigned char months[12]={31,28,31,30,31,30,31,31,30,31,30,31};
//unsigned char lookup[10]={0xFA,0x60,0xBC,0xF4,0x66,0xD6,0xDE,0x70,0xFE,0x76};
//unsigned char lookup[10]={0xAF,0x06,0xCB,0x4F,0x66,0x6D,0xED,0x07,0xEF,0x67};
//const unsigned char lookup[10]={0xF5,0x60,0xB6,0xF2,0x63,0xD3,0xD7,0x70,0xF7,0x73};
//unsigned char lookup[10]={0xFA,0x0A,0xBC,0x9E,0x4E,0xD6,0xF6,0x8A,0xFE,0xDE};
//unsigned char lookup[10]={0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB};

//unsigned char lookup[10]={0xAF,0x06,0x6D,0x4F,0xC6,0xCB,0xEB,0x0E,0xEF,0xCE};//AMTL
//unsigned char lookup[10]={0x5F,0x06,0x6B,0x2F,0x36,0x3D,0x7D,0x07,0x7F,0x3F};//AMTL2
//unsigned char lookup1[10]={0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//AMTL
//unsigned char lookup1[10]={0xF5,0x30,0xE3,0xF2,0x36,0xD6,0xD7,0x70,0xF7,0xF3};//PROGRESSIVE-FLASH

unsigned char data[8];
unsigned char read_data_arr[2][30];

unsigned long int scratch, scratch1;
unsigned long real_time, real_date, prev_real_date;
unsigned char t_hr, t_min, t_sec, prev_sec, d_day, d_mnth, d_yr;
//unsigned char prev_min1, prev_hr1;
unsigned char ep_status, rtc_status;

long int vrms_adc;
long int irms_adc[2], prev_irms_adc[2];

long int pulse_reg0;
unsigned long int pulse_reg1,pulse_va,pulse_va_reg1, pulse_fact;
unsigned long int pulse_reg_rmndr;//,pulse_va_reg_rmndr,pulse_var_ld_reg_rmndr, pulse_var_lg_reg_rmndr;

int iph_val[2];
unsigned char demo_led_off, led_off, led_va_off, led_var_off, pulse_va_on_ctr, pulse_var_on_ctr, pulse_on_ctr;//, pulse_r_on_cnt;
unsigned int volt_fact, amp_fact[2],igain_val[2],n_cfden_val[2];
unsigned int sample;
unsigned int freq_reg1,freq_reg2;
float kw_reg1[2];
long int kw_reg2[2], prev_kw_reg2;//, prev_kw_reg2[10], max_prev_kw_reg2;
unsigned long int kva_reg2, kvar_reg2;
unsigned long int vrms_reg1,irms_reg1[2];
unsigned long int vrms_reg2,irms_reg2[2],prev_irms_reg2;//,prev_irms_reg2[2][10], max_prev_irms_reg2[2];
extern uint32_t irms_reg3[2], kw_reg3, kva_reg3,kvar_reg3;
extern uint16_t vrms_reg3,freq_reg3,Mains_Freq;
extern int16_t pf_reg3;

extern uint8_t pf_sign;


long int irms_offset_adc[2], vrms_offset_adc;
long int irms_cumm_offset_adc[2], vrms_cumm_offset_adc;
//unsigned char pf_sign0, pf_sign_ctr;

unsigned int disp_ctr, disp_cnt;
int disp_index;

unsigned char mains_stat;

unsigned char batt_push_ctr, comm_delay_ctr;    
extern uint8_t channel;

extern uint32_t load_val[2];

unsigned long int last_demand_load_val[2], last_hr_load_val[2], last_stored_tod_load_val[2]; //last_kwh_val, 
unsigned char load_ctr[2], load_index[2], inst_load[2], load_rmndr[2];
unsigned long int meter_no;

unsigned char hist_ctr, disp_sub_ctr, disp_zone_ctr;

//unsigned int md_reset_cnt;
unsigned char prev_min;

unsigned int mnth_pos, day_pos;
unsigned int tmpr_time_on_ctr[TAMPER_TYPE], tmpr_time_off_ctr[TAMPER_TYPE];
//unsigned char occur_stat[TAMPER_TYPE];
unsigned int event_pos[TOT_EVENT_TYPE];
unsigned long int store_tamper_stat; //,tamper_stat;
unsigned long int day_hr_pos, tamper_cnt[TAMPER_TYPE];
uint32_t event_cnt[TOT_EVENT_TYPE];
extern uint32_t Tamper_Profile_Entries_In_Use[TOT_EVENT_TYPE];
extern uint32_t Tamper_Profile_Entries[TOT_EVENT_TYPE];
extern uint32_t all_tamper_cnt;

extern uint8_t md_intgr_val; 
unsigned char survey_intgr_val; 

unsigned int t_zone[16], t_prev_zone;
unsigned long int tod_kwh_val[8], tod_kvah_val[8];

unsigned char cuopen_stat;

unsigned char metering_data_ready, rtc_update_stat;

unsigned int imax, imax1;
unsigned int rev1_ctr, rev2_ctr;
unsigned char rev1_stat, rev2_stat;

unsigned int reset_on_time;
extern uint32_t md_reset_cnt; 
extern uint32_t kwmd_val; 
extern uint32_t kvamd_val; 
//extern uint8_t kwmd_stat;
//extern uint8_t kvamd_stat;

uint32_t bkp_kwmd_val; 
uint32_t bkp_kvamd_val; 

uint32_t kwmd_date, kwmd_time; 
uint32_t bkp_kwmd_date, bkp_kwmd_time; 

uint32_t kvamd_date, kvamd_time; 
uint32_t bkp_kvamd_date, bkp_kvamd_time; 

unsigned char bkp_pf_val;
unsigned char disp_mode_stat;

unsigned char reset_on_min_ctr;

long int max_val, min_val;
unsigned char program_reset, init_stat;

//unsigned long int kwh_bkp;
//unsigned int kwmd_bkp;
//unsigned char lock_val;
unsigned char pulse_cnt;
unsigned char delay_sec;
unsigned char cnt,scrl_b, scrl_b_up, md_reset_b, md_reset_b_up;
unsigned int scrl_b_dn_ctr, scrl_b_up_ctr, md_reset_b_ctr;
unsigned char auto_stat;
unsigned char magnetic_stat, eload_stat, rev_stat[2], ctn_stat, nd_stat, v440_stat, cbyp_stat;//,nm_stat;
unsigned char mag_detect_start_ctr, mag_pulse_stat;
unsigned int mag_on_ctr,mag_on_ctr1;
unsigned int mag_off_ctr,mag_off_ctr1;
unsigned int mag_imax_on_ctr, mag_imax_off_ctr;


unsigned long int mag_pulse_val;
unsigned long int kwcd_val;
unsigned char vrms_adc_zero_cross, irms_adc_zero_cross[3], vrms_adc_zero_cross_ctr;
unsigned char checksum, checksum_calc;
unsigned char last_tamper_type;
unsigned int u10_delay_ctr;
unsigned long int comm_checksum;
unsigned char disp_md_type;

unsigned char prev_yr,  prev_mnth, prev_day, prev_hr;
unsigned long int prev_date;
unsigned char num_of_zone;
unsigned long int manfact_date;
unsigned int rd_time;
unsigned char power_fail_check_stat;
unsigned long int bkp_kwh_val, bkp_kvah_val;

extern uint16_t avg_pf;
extern uint16_t tot_pf_reg3;
extern uint32_t tot_kw_reg3, tot_kva_reg3, tot_kvar_reg3;

unsigned long int tot_kw_reg2, tot_kva_reg2, tot_kvar_reg2;

unsigned char mag_imax_stat, mag_restore_ctr;
unsigned char mag_chk_delay;

//unsigned long int mag_pulse_val[3];
unsigned char save_kw_reg2_ctr;
//unsigned long int avg_kw[3],avg_vrms[3];		
unsigned char nmiss_stat;
unsigned long int dial_load[2], dial_pulse[2];
unsigned char push_lock_ctr, lock_stat;

//unsigned char repeat_data_stat, bkp_disp_index, bkp_disp_sub_ctr, bkp_disp_zone_ctr, bkp_hist_ctr;

unsigned char phase_stat, vpseq_stat, ipseq_stat;

unsigned char recv_buf[11],recv_ctr, enable_recv_buf, comm_stat;

unsigned char pass_step,pass_ctr;
unsigned char prev_season;

//unsigned char last_bill_day;
unsigned int last_bill_time;
unsigned long int last_bill_date;
unsigned long int test_val;

union{
sBilling_Profile stBilling_Profile;
sLoad_Profile stLoad_Profile;
sTamper_Profile stTamper_Profile;
sSeason_Profile stSeason_Profile;
sWeek_Profile stWeek_Profile;
sDay_Profile stDay_Profile;
};

//extern uint8_t bill_date[3];
extern uint8_t time_string[14];
extern uint8_t Last_MD_Rst_DT[14];
extern uint8_t KWMD_DT[14];
extern uint8_t KVAMD_DT[14];

uint8_t tmp_time_string[12];//={0,0,0,0,0,0,0,0,0,0,0,0};

extern uint8_t set_cal_name_data;
extern uint8_t set_auth_data;
extern uint8_t set_rtc_data;
extern uint8_t set_demand_intgr_period;
extern uint8_t set_capture_period;
extern uint8_t set_bill_date;
extern uint8_t set_day_profile;
extern uint8_t set_season_profile;
extern uint8_t set_activation_date;

extern uint8_t set_load_control_stat;
extern uint8_t set_over_current_val;
extern uint8_t set_over_load_val;
extern uint8_t set_conn_time_interval;
extern uint8_t set_conn_lockout_time;
extern uint8_t set_conn_time_repeat;
extern uint8_t set_tamper_time;

extern uint8_t set_clock_time_zone;
extern int16_t clock_time_zone;

extern uint8_t activate_tod_stat;

extern uint32_t Over_Current_Val;
extern uint32_t Over_Load_Val;
extern uint32_t Conn_Time_Interval;
extern uint32_t Conn_Lockout_Time;
extern uint32_t Conn_Time_Repeat;
extern uint32_t Tamper_Occ_Time;
extern uint32_t Tamper_Res_Time;

extern uint32_t Load_Profile_Capture_Period;
extern uint32_t Load_Profile_Entries_In_Use;
extern uint32_t Load_Profile_Entries;

extern uint32_t Daily_Load_Profile_Capture_Period;
extern uint32_t Daily_Load_Profile_Entries_In_Use;
extern uint32_t Daily_Load_Profile_Entries;

extern uint32_t Max_Demand_Capture_Period;
extern uint32_t Billing_Profile_Entries_In_Use;
extern uint32_t Billing_Profile_Entries;

extern uint8_t bill_day[5];
extern uint8_t bill_time[4];
extern uint8_t scheduled_bill_day[5];
extern uint8_t scheduled_bill_time[4];

extern uint8_t Calendar_Name_Active[18];
extern uint8_t Calendar_Name_Passive[18];
extern uint8_t Active_Season_Name[2][10];
extern uint8_t Passive_Season_Name[2][10];
extern uint8_t active_season_date[2][4];
extern uint8_t passive_season_date[2][4];
extern uint8_t tod_time[16][4]; //[8*2][4]
extern uint8_t passive_tod_time[16][4]; //[8*2][4]
extern uint8_t tariff_id[2][8];
extern uint8_t passive_tariff_id[2][8];
extern uint8_t Activate_Passive_Calendar_Time[14];

extern uint32_t activate_date;
extern uint32_t activate_time;

extern uint8_t Meter_Sr_No[9];

extern uint8_t load_control_stat;
extern uint8_t load_conn_stat;

unsigned int bkp_sample;

unsigned int load_break_time_ctr, load_break_time;
unsigned char over_limit_time_ctr, under_limit_time_ctr, load_break_index; 
unsigned char over_limit_stat, relay_delay_ctr[3];

unsigned long int demand_volt, demand_amp, demand_pf;  

extern uint32_t Cum_Power_Off_Count;
extern uint32_t Cum_Power_Off_Dur;
extern uint32_t Cum_Power_On_Dur;

extern uint32_t Cum_Prog_Count;
extern uint16_t Last_Tamper_Id[7];

extern uint8_t auth0[2];
extern uint8_t auth1[10];
extern uint8_t auth2[18];
extern uint8_t auth3[10];


typedef struct
{
   uint16_t Year;
   uint8_t Month;
   uint8_t Date;
   uint8_t Day;
   uint8_t Hr;
   uint8_t Min;  
} sSA_Range;

extern sSA_Range SA_Range[2];
extern uint32_t  range_num_entries, range_start_entry;

unsigned int co_disable_ctr;
unsigned char last_tamper_occ_type, last_tamper_res_type;
unsigned int tamper_snap_index;
unsigned char blink_stat;

unsigned char kva_pulse_type, kva_pulse_switch_ctr;
unsigned char mains_pulse_ctr, mains_pulse_ctr1, kvar_pulse_start_ctr;

unsigned char ep_clear_stat;
unsigned long int cuopen_date, cuopen_time;
unsigned char nmi_err_ctr; 
//unsigned char crystal_off_ctr;
unsigned char adc_delay_ctr;

unsigned char batt_disp_stat;

//long int adc_data[50], adc_data_ctr;

extern uint16_t Year_of_Manufacture;

//extern uint32_t req_tod_val;
extern uint32_t block_load_val[2], block_vrms, block_irms;
extern uint8_t self_diagnostics_status;

unsigned char inst_tmpr_time_on_ctr[5];
unsigned char rem_disp_val, rem_disp_val2;
unsigned long int tmp_md_date;
unsigned char batt_mode_comm_en_stat;

#define NCF1DEN         1000        
#define VOLT_CONST	1260 //3250 //1970 //1760		
#define AMP_CONST	1500 //1024 //256 //400		

#define IPH1_CONST  	 40   //230 //180 //1.5
#define IPH2_CONST  	 40   //230 //100 //3.5
#define IGAIN1_CONST	8200  //7500 //6000 //5700
#define IGAIN2_CONST	10100 //9600 //6750 //6600 

#define RTC		3
#define EEPROM		4


/*
I2->SD0
I1->SD1
V->SD2

COPEN->P1.0
NMISS->P1.1
MGT->P1.2
PUSHB->P1.6
PULSE->P1.7

Optical->UCA2
RS232->UCA1

BKLIT->P2.6

COMM_EN->P3.0
SDA->P3.4
SCL->P3.5

#define CUOPEN_PIN		0 //P1.0
#define MAINS_SENSE		1 //P1.1
#define MAGNET_PIN		2 //P1.2
#define PUSHDN_B	        6 //P1.6
#define PULSE1_PIN		7 //P1.7

#define BKLIT_PIN		6 //P2.6

#define COMM_CTRL_PIN	        0 //P3.0
#define I2C_DATA		4 //P3.4
#define EEPROM_CLOCK	        5 //P3.5

//#define MD_RESET_PIN		2 //P2.2
//#define PUSHUP_B		6 //P2.6
//#define PULSE2_PIN		7 //P2.7		
//#define RTC_CLOCK             1 //P3.1

//#define PUSH_SUPPLY		0 //P4.0
*/

/*
ct 0
shunt 1
voltage 2

PULSE P1.4
MAG P1.7
SW CO P2.0
SW PB P2.1
BKLIT P2.2
NM	P2.5
SDA	P2.6
SCL	P2.7

TX_EXT P3.3 
RX_EXT P3.4
TX_OPT P3.5
RX_OPT P3.6
COMM_CTRL P3.7
*/


/*
#define PULSE1_PIN		4 //P1.4
#define MAGNET_PIN		7 //P1.7

#define CUOPEN_PIN		0 //P2.0
#define PUSHDN_B	        1 //P2.1
#define BKLIT_PIN		2 //P2.2
#define MAINS_SENSE		5 //P2.5
#define I2C_DATA		6 //P2.6
#define EEPROM_CLOCK	        7 //P2.7

#define COMM_CTRL_PIN	        7 //P3.7
*/

/*
CO P1.0
PB P1.1
MAG P1.2
PULSE P1.3

MAINS P2.5
SDA P2.6
SCL P2.7

TX P3.3
RX P3.4
TX OPT P3.5
RX OPT P3.6
CONN CTRL P3.7
I2
I1
V
*/

/*
#define CUOPEN_PIN		0 //P1.0
#define PUSHDN_B	        1 //P1.1
#define MAGNET_PIN		2 //P1.2
#define PULSE1_PIN		4 //P1.4

#define MAINS_SENSE		5 //P2.5
#define I2C_DATA		6 //P2.6
#define EEPROM_CLOCK	        7 //P2.7

#define COMM_CTRL_PIN	        7 //P3.7
*/

/*
A0           I2
A1           I1
A2           V
 
11           TC.B       P1.0
12           MAINS  P1.1
13           MAG     P1.2
14           PULSE   P1.3
 
31           PUSH     P1.6
32           SCL         P1.7
33           SDA        P2.0      
34           BKLIT     P2.1
 
35           RXD3     OPTICAL
36           TXD3      OPTICAL
37           RXD        RS232
38           TXD        RS232
 
43-64     SEG1-SEG20
74           TRIGGER P6.7
*/

#define CUOPEN_PIN		0 //P1.0
#define MAINS_SENSE		1 //P1.1
#define MAGNET_PIN		2 //P1.2
#define PULSE1_PIN		3 //P1.3
#define PUSHDN_B	        4 //P1.4

//#define NMISS_SENSE		4 //P1.4

//#define I2C_DATA		7 //P1.7

//#define EEPROM_CLOCK	        0 //P2.0
//#define BKLIT_PIN		1 //P2.1
#define I2C_DATA		6 //P2.6
#define EEPROM_CLOCK	        7 //P2.7

#define COMM_CTRL_PIN	        7 //P6.7


void delay1ms(unsigned char);
void delay10us(unsigned char);	
//unsigned char delay1ms(unsigned char);
//unsigned char delay10us(unsigned char);
void init(void);
void init_adc(void);
void switch_index(void);
void select_disp_index(void);
void get_disp_index(void);
void disp_glass(unsigned char, unsigned char, unsigned char);
void eload_check(void);

void bstart(unsigned char);
void bstop(unsigned char);

void write_eeprom(unsigned long int,unsigned char);
void write_page_eeprom(unsigned long int,unsigned char);
unsigned char read_eeprom(unsigned long int);
void read_page_eeprom(unsigned long int, unsigned char, unsigned char);
void to_eeprom(unsigned long int nAddr, unsigned long int ldata, unsigned char size);
unsigned long int from_eeprom(unsigned long int nAddr,unsigned char size);


//rtc funcs & variables
void set_rtc_time(unsigned char,unsigned char, unsigned char);
void set_rtc_date(unsigned char,unsigned char, unsigned char);

unsigned char rtc_read_byte(unsigned char);
void rtc_read_date(void);
void rtc_read_time(void);

void tx_data(unsigned char, unsigned char);
unsigned char rx_data(unsigned char, unsigned char);

/*
void calib_func(unsigned char, int);
void ph_calib_func(unsigned char, int);
//void offset_func(int);
void calib_vif(unsigned char);
//void ncut_calib_func(int);
*/

void calib_func(int);
void ph_calib_func(int);
void get_avg_val(void);


void default_eeprom(unsigned char);
void init_eeprom(void);
unsigned char cyclic(unsigned char, unsigned char);
void load_func(void);
void md_func(void);
void history_func(void);
void tamper_func(void);

void init_tod(void);//unsigned int, unsigned long int);
void dflt_tod_zone(void);
void update_tod_data(void);
void store_energy_in_zone(void);

void serial_comm(void);
void transmit_char(unsigned char);
void init_usart(void);
void disp_code(void);
void disp_tamper(unsigned char);
void send_data(unsigned long int, unsigned char);
void send_inst_param(void);
void old_style_send_inst_param(void);
void old_style_send_data(unsigned long int, unsigned char);

void button_chk(void);
void clear_bstat(void);
unsigned char crc8(unsigned long int);
void mem_check(void);
unsigned char get_cal_crc(void);
void store_cal_data(void);
void store_md_data(unsigned int);

void power_fail_func(void);
unsigned char days_in_month(unsigned char);
void disp_all_lcd(unsigned char);
void disp_mode(void);
void init_io_param(void);
//unsigned char rtc_log_func(unsigned char);
void disp_lock(void);

void get_dlp_data(unsigned int); 
void get_tamper_data(unsigned char, unsigned int); 
void get_lp_data(unsigned int); 
void get_bill_data(unsigned int); 
void get_time_data(unsigned long int, unsigned long int);
void find_num_between_lp_entries(void);
void find_num_between_dlp_entries(void);
void fill_load_survey_entries(unsigned long int, unsigned char, unsigned char);
unsigned long int find_next_date(unsigned long int);
void reset_ls_data(void);

unsigned char convt_byte_to_bcd(unsigned char);
void store_event_data(unsigned char, unsigned int, unsigned char); 
void oload_check(void);
void set_all_phase(void);
void unset_all_phase(void);
//void disp_copen(void);
void init_dlms(void);
void disp_kva_type(void);
void tod_passive_to_active(void);
unsigned long int from_data_arr(unsigned long int,unsigned char, unsigned char);
unsigned char convt_bcd_to_byte(unsigned char);
void write_data_in_lcd(void);
unsigned long int get_present_tod_data(unsigned char, unsigned char);
void read_time_date(void);
void select_season(void);

void get_active_season_data(unsigned int); 
void get_passive_season_data(unsigned int); 

void get_active_week_data(unsigned int); 
void get_passive_week_data(unsigned int); 

void get_active_day_data(unsigned int); 
void get_passive_day_data(unsigned int); 

unsigned char persist_time(unsigned char, unsigned char);

void find_tamper_stat(void);
unsigned char hv_stat, lv_stat, hc_stat;
void disp_ver(void);
unsigned char seg_7_lookup( unsigned char);
unsigned char seg_7_map( char *, unsigned char);

void disp_all_lcd(unsigned char val)
{
	unsigned char i;
	for(i=0;i<10;i++)
  	{
          if(i<6)  
            LCDMEM[3+i]=val;	//all seg check
          else
            LCDMEM[3+i+1]=val;	//all seg check            
  	}
        return;
	
}

int __low_level_init(void)
{  
      WDTCTL = WDTPW+WDTSSEL__ACLK+WDTHOLD;
      SFRRPCR |= SYSNMI;//MAKING RESET AN NMI 
      //RTCCTL13 &= ~RTCHOLD;
      return 1;
}

// SYSNMI Interrupt Service Routine

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)

#pragma vector=SYSNMI_VECTOR

__interrupt void SYSNMI_ISR(void)

#elif defined(__GNUC__)

void __attribute__ ((interrupt(SYSNMI_VECTOR))) SYSNMI_ISR (void)

#else

#error Compiler not supported!

#endif

{

 WDTCTL=WDT_ARST_1000;//WDTPW+WDTSSEL__ACLK+WDTCNTCL;
 while(1);
 
}

// UNMI Interrupt Service Routine

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)

#pragma vector=UNMI_VECTOR

__interrupt void UNMI_ISR(void)

#elif defined(__GNUC__)

void __attribute__ ((interrupt(UNMI_VECTOR))) UNMI_ISR (void)

#else

#error Compiler not supported!

#endif

{

 WDTCTL=WDT_ARST_1000;//WDTPW+WDTSSEL__ACLK+WDTCNTCL;
 while(1);

}

/*
void usart_init(void) // uart Initialisation 
{
    P1SEL |=BIT4;
    P1SEL |=BIT5;        // special function register
    UCA1CTL1 |= UCSSEL0;                     //UCLK = ACLK
    UCA1BR1 =  0x00;
    UCA1BR0 =  0x0D;
    UCA1MCTLW = (UCBRS7+UCBRS5+UCBRS4+UCBRS2+UCBRS1);
    UCA1CTL1 &= ~UCSWRST;
    UCA1IE |= UCRXIE;
}
*/

void init(void)
{
	//unsigned char i;
	
        WDTCTL = WDTPW+WDTSSEL__ACLK+WDTHOLD;                 // Stop watchdog timer
 	AUX3CHCTL = AUXCHKEY | AUXCHC_3 | AUXCHV_1 | AUXCHEN;  // Enable Charger for AUX3 to enable RTC

        //sbi(P1DIR,PULSE1_PIN);sbi(P1OUT,PULSE1_PIN);
        // Port Configuration

        
        P1OUT = 0x00; P2OUT = 0x00; P3OUT = 0x00; P4OUT = 0x00; P5OUT = 0x00; P6OUT = 0x80;
        P7OUT = 0x00; P8OUT = 0x00; P9OUT = 0x00; PJOUT = 0x00;
        P1DIR = 0x08; P2DIR = 0x3F; P3DIR = 0xFF; P4DIR = 0xFF; P5DIR = 0xFF; P6DIR = 0xFF;
        P7DIR = 0xFF; P8DIR = 0xFF; P9DIR = 0xFF; PJDIR = 0xFF;   

        
        //RX_OPT P2.2, TX_OPT P2.3, RX_EXT P2.4, TX_EXT P2.5
        
        PMAPPWD = PMAPKEY;      // Enable Write-access to modify port mapping registers
        PMAPCTL = PMAPRECFG;    // Allow reconfiguration during runtime
        //P2MAP2= PM_UCA0RXD;
        //P2MAP3= PM_UCA0TXD;
        P2MAP4= PM_UCA0RXD;
        P2MAP5= PM_UCA0TXD;

        PMAPPWD = 0;            // Disable Write-Access to modify port mapping registers by writing incorrect key

        // Setup P1.2 UCA0RXD, P1.3 UCA0TXD
        P2SEL |= BIT2 | BIT3| BIT4 | BIT5;                   // Set P3.3, P3.4 to non-IO
        //P2DIR |= BIT2 | BIT3| BIT4 | BIT5;                   //output
        P2DIR |= BIT3| BIT5;                   //output

        /*
        FLL_CTL0=DCOPLUS;//|XCAP18PF;	//SCO+
 	//FLL_CTL0=XCAP18PF;	//SCO+
	
  	SCFI0|=FN_2;				//DC OPERATING RANGE: 1.3 to 12.1
 	SCFQCTL=SCFQ_4M;	//128;				//1/2*(121+1)*32768
       */
	

	md_intgr_val=Max_Demand_Capture_Period/60;
        survey_intgr_val=Load_Profile_Capture_Period/60;
        
        if(((survey_intgr_val%5)!=0)||(survey_intgr_val==0)||(survey_intgr_val>30))
            survey_intgr_val=30;//15;
        if(((md_intgr_val%5)!=0)||(md_intgr_val==0)||(md_intgr_val>30))
            md_intgr_val=30;//15;

        
	rd_time=0;
	reset_on_min_ctr=0;
	cuopen_stat=0;
	power_fail_check_stat=0;
	mag_imax_stat=0;
	mains_stat=0;

        /*
        load_break_index=0;
        load_break_time_ctr=0;
        load_break_time=60;//300;
        over_limit_time_ctr=0;
        under_limit_time_ctr=0;
        
        load_conn_stat=1;
        over_limit_stat=0;
        load_control_stat=1;
        */
        
        co_disable_ctr=10;
        nmi_err_ctr=0;
        self_diagnostics_status=self_diagnostics_status&0xFB;//|0x04;
}

void init_io_param(void)
{
	unsigned char i;//, j;

  	//SVSCTL = VLD0+PORON; //1.9v  
        //SVSCTL = VLD1+PORON; //2.1v  
        //SVSCTL = VLD1+VLD0+PORON; //2.2v  
        //SVSCTL = VLD2+VLD1+VLD0+PORON; //2.65v  
/*
#define CUOPEN_PIN		0 //P1.0
#define MAINS_SENSE		1 //P1.1
#define MAGNET_PIN		2 //P1.2
#define PULSE1_PIN		3 //P1.3
#define PUSHDN_B	        6 //P1.4
        
#define BKLIT_PIN		1 //P2.1
#define I2C_DATA		6 //P2.6
#define EEPROM_CLOCK	        7 //P2.7

#define COMM_CTRL_PIN	        7 //P6.7
*/
        
        cbi(P1DIR,CUOPEN_PIN);cbi(P1IES,CUOPEN_PIN);sbi(P1IE,CUOPEN_PIN);
	cbi(P1DIR,MAINS_SENSE);sbi(P1IES,MAINS_SENSE);sbi(P1IE,MAINS_SENSE);
	sbi(P1REN,MAGNET_PIN);sbi(P1OUT,MAGNET_PIN);cbi(P1DIR,MAGNET_PIN);
        sbi(P1DIR,PULSE1_PIN);cbi(P1OUT,PULSE1_PIN);
        cbi(P1DIR,PUSHDN_B);sbi(P1IES,PUSHDN_B);sbi(P1IE,PUSHDN_B);
	//cbi(P1DIR,NMISS_SENSE);
	
        sbi(P2DIR,I2C_DATA);
	sbi(P2DIR,EEPROM_CLOCK);	
        //sbi(P2DIR,BKLIT_PIN);sbi(P2OUT,BKLIT_PIN);
	
	
	sbi(P6DIR,COMM_CTRL_PIN);cbi(P6OUT,COMM_CTRL_PIN);
	
        
        pulse_reg1=0;
	pulse_va=0;    	    
	pulse_va_reg1=0;    	    
	
	pulse_reg_rmndr=0;  	    
	pulse_fact=0x35A4E900;//0x35A4E900;//0x1AD27480;//0x0D693A40;//0x06B49820; 
	pulse_reg0=0;
	for(i=0;i<2;i++)
	{
            irms_reg1[i]=0;
            irms_reg2[i]=0;
            irms_offset_adc[i]=0;
            irms_cumm_offset_adc[i]=0;
            irms_adc_zero_cross[i]=0;
            kw_reg1[i]=0;
            kw_reg2[i]=0;
        }        
        
        vrms_reg1=0;
        vrms_reg2=0;
        vrms_cumm_offset_adc=0;
        vrms_offset_adc=0;
        freq_reg1=0;
        freq_reg2=0;
        vrms_adc_zero_cross=0;
        vrms_adc_zero_cross_ctr=0;		
        //pf_sign0=0;
        pf_sign=0;
        kva_reg2=0;
        kvar_reg2=0;
        
        phase_stat=0;
        vpseq_stat=0;
        ipseq_stat=0;
	led_off=1;
	led_va_off=1;				
	led_var_off=1;				
	demo_led_off=1;
        rev1_ctr=0;
	rev2_ctr=0;
	sample=0;
        bkp_sample=0;
	  				
	for(i=0;i<TAMPER_TYPE;i++)
	{
		tmpr_time_on_ctr[i]=0;
		tmpr_time_off_ctr[i]=0;
	}


	metering_data_ready=0;
			
	
	for(i=0;i<2;i++)
	{
		dial_load[i]=0;
		dial_pulse[i]=0;
	}
	

	batt_push_ctr=10;
	comm_delay_ctr=20; //17
        push_lock_ctr=0;
	
	magnetic_stat=0;
        mag_on_ctr=0;
	mag_on_ctr1=0;
	mag_off_ctr=0;
	mag_off_ctr1=0;
        mag_imax_stat=0;
	mag_imax_on_ctr=0;
	mag_imax_off_ctr=0;
       	mag_chk_delay=0;
	
	for(i=0;i<2;i++) 
		rev_stat[i]=0; 
	ctn_stat=0;
	nd_stat=0; 
	v440_stat=0;
	cbyp_stat=0;
	nmiss_stat=0;
	scratch=0;
	scratch1=0;
	hist_ctr=0;
	disp_sub_ctr=0;
	disp_zone_ctr=0;
        rtc_update_stat=1;
        recv_ctr=0;
        comm_stat=0;
	enable_recv_buf=1;
	
	disp_mode_stat=1;
        //repeat_data_stat=0;
        //bkp_disp_index=0;
        //bkp_disp_sub_ctr=0;
        //bkp_disp_zone_ctr=0;
        //bkp_hist_ctr=0;
        
	scrl_b=0;
	scrl_b_up=0;
	
	scrl_b_up_ctr=0;
	scrl_b_dn_ctr=0;
        md_reset_b_ctr=0;
	md_reset_b=0;
	md_reset_b_up=0;
	mag_restore_ctr=0;
        
        set_auth_data=0;
        set_rtc_data=0;
        set_demand_intgr_period=0;
        set_capture_period=0;
        set_bill_date=0;
        set_day_profile=0;
        set_season_profile=0;
        set_activation_date=0;
        
        /*
        set_load_control_stat=0;
        set_over_current_val=0;
        set_over_load_val=0;
        set_conn_time_interval=0;
        set_conn_lockout_time=0;
        set_conn_time_repeat=0;
        */
        
        set_clock_time_zone=0;
        //crystal_off_ctr=0;
        tamper_snap_index=0;
        kva_pulse_type=0; 
        kva_pulse_switch_ctr=0;
        mains_pulse_ctr=0;
        mains_pulse_ctr1=0;
        kvar_pulse_start_ctr=0;
        pass_step=0;pass_ctr=0;
        batt_disp_stat=0;
        batt_mode_comm_en_stat=0;
          
        for(i=0;i<3;i++)
          relay_delay_ctr[i]=0;

	init_stat=1;
	adc_delay_ctr=0;
	prev_season=0;
        self_diagnostics_status=self_diagnostics_status|0xF8;
        
        
        LCDCCTL0 =  LCDDIV_31 | LCDPRE_1 | LCD8MUX | LCDON | LCDLP;
        //Charge pump generated internally at 2.96V, external bias (V2-V4) generation
        //Internal reference for charge pump
        LCDCVCTL = LCDCPEN + VLCD_3_08; //Internal voltage 3.08V
        //LCDCVCTL = VLCDEXT;
        //LCDCVCTL = 0;
        LCDCMEMCTL = LCDCLRM;
        REFCTL0 &= ~REFMSTR;
        LCDCPCTL0 = 0x3FF8; //0x3DF8
        LCDCPCTL1 = 0x0000;
        LCDCPCTL2 = 0x0000;
 
	//disp_all_lcd(0xff); //0xff
        disp_ver();
        
	WDTCTL=WDT_ARST_1000;//WDTPW+WDTSSEL__ACLK+WDTCNTCL;
}


void init_dlms()
{
    unsigned int i;
  
    //------------------------------DLMS------------------------------------------
    
    SetVCore(1); //1
    
    //enable SVS and SVM
    PMMCTL0_H=PMMPW_H;   
    SVSMHCTL |= SVMHE | SVSHE;
    SVSMLCTL |= SVMLE | SVSLE;
    PMMCTL0_H=0;   

    LFXT_Start(XT1DRIVE_3);

    Init_FLL_Settle(MCLK_DEF*8388608/8/1000, MCLK_DEF*32768*32/32768);
    /* There seems no benefit in waiting for the FLL to settle at this point. */
    // Initialize FLL in high frequency mode by default  
    //  FLL_Init_HF();  

    for (i = 0; i < 10000; i++)              // Delay for 32 kHz crystal to
    { /*asm("nop");*/ }                                      // stabilize

    
    
    RTCCTL0_H = RTCKEY_H;     
    RTCOCAL=0;
    RTCOCAL|=RTCOCAL2 |RTCOCAL1|RTCOCAL0;  // 
    RTCCTL13 &= ~RTCHOLD;    
    RTCCTL13 |= RTCMODE;          //+RTCTEV_0;       // Init RTC        
    RTCCTL13 |= RTCLOCK;
    RTCCTL0_H = 0;               // LOCK RTC

  //SFRIE1=OFIE|ACCVIE;  	

    /* Set up timer A to use as a timeout interrupt function */
    /* TACCR0 determines the next interrupt, clocked from ACLK */
    TA0R = 0;
    TA0CCR0 = 0;
    TA0CCTL0 = CCIE;
    TA0CTL = TACLR | MC_2 | TASSEL_1;
    
  //  aes_encrypt(challenge_s2c,aes_key);
  //  Capture_Billing_Profile_Data(Data_Buffer,ATTR_READ);
    /* Configure all available USCI UART ports as DLMS ports */
       
      max_rx_len[0]=MAX_RX_LEN_PORT0;
      max_tx_len[0]=MAX_TX_LEN_PORT0;
      rx_window[0]=RX_WINDOW_PORT0;
      tx_window[0]=TX_WINDOW_PORT0;
      
      max_rx_len[1]=MAX_RX_LEN_PORT1;
      max_tx_len[1]=MAX_TX_LEN_PORT1;
      rx_window[1]=RX_WINDOW_PORT1;
      tx_window[1]=TX_WINDOW_PORT1;
      
#if MAX_USCI_PORTS > 2
      max_rx_len[2]=MAX_RX_LEN_PORT2;
      max_tx_len[2]=MAX_TX_LEN_PORT2;
      rx_window[2]=RX_WINDOW_PORT2;
      tx_window[2]=TX_WINDOW_PORT2;

#endif      

#if MAX_USCI_PORTS > 3
      max_rx_len[3]=MAX_RX_LEN_PORT3;
      max_tx_len[3]=MAX_TX_LEN_PORT3;
      rx_window[3]=RX_WINDOW_PORT3;
      tx_window[3]=TX_WINDOW_PORT3;

#endif
 
    
    for (i = 0;  i < MAX_USCI_PORTS;  i++)
    {
        configure_uart_port(i, 1);

        iec62056_46_link_init(&link[i], i,0x4100);
        //iec62056_46_set_far_msap(&link[i], 0x40000);

        //iec62056_46_link_init(&link[i], i, 0x100);
        hdlc_async_rx_init(&rx[i]);
        //rx_data_to_process[i]=0;
        Frame_Received[i]=0;
    }
        
//------------------------------------------------------------------        

  return;
}

void read_time_date(void)
{
    t_sec=RTCSEC;
    t_min=RTCMIN;
    t_hr=RTCHOUR;
    d_day=RTCDAY;
    d_mnth=RTCMON;
    d_yr=RTCYEAR;  

    scratch=t_hr;
    scratch=scratch*100+t_min;
    real_time=scratch*100+t_sec;
    
    scratch=d_day;
    scratch=scratch*100+d_mnth;
    real_date=scratch*100+d_yr;
}

void main(void)
{
	unsigned int i,j,k;
        long long tmp_float;
        long long tmp_float1;
        
start:	
        test_val=SYSRSTIV;
        
        init();
        init_io_param();
        init_dlms();
        
        /*
        while(1)
        {
            disp_all_lcd(0x00);
            delay1ms(250);
            delay1ms(250);
            for(i=0;i<10;i++)
  	    {
              if(i<6)
                LCDMEM[3+i]=seg_7_lookup(i); //0x6D;	//all seg check
              else
                LCDMEM[3+i+1]=seg_7_lookup(i); //0x6D;	//all seg check
              delay1ms(250);
              delay1ms(250);
            }
        }  
        */
        
        //default_eeprom(1);
        
        
        t_sec=RTCSEC;
   	t_min=RTCMIN;
   	t_hr=RTCHOUR;
   	d_day=RTCDAY;
   	d_mnth=RTCMON;
   	d_yr=RTCYEAR;  
        
	for(i=0;i<2;i++)
	{
		load_ctr[i]=0;
		inst_load[i]=0;
		load_rmndr[i]=0;
	}
	
        mem_check();
	
        P1IFG=0;
        P2IFG=0;
        
        _BIS_SR(GIE);

        // Configure timer A0 for Metering //LED calibration pulse
        // Setup TA1
        TA1R = 0;
        TA1CCR0 = 32768;
        TA1CCTL0 = CCIE;
        TA1CTL = TACLR | MC_1| TASSEL_1;

        
        
        /* 6736 mismatch
        BTCTL = BT_fCLK2_ACLK+BT_ADLY_1000;             // STK LCD freq = ACLK/128, 1s Interrupt
  	IE2 |= BTIE;                              // Enable BT interrupt
	
        IFG1=0;
        IFG2=0;
        P1IFG=0;
        P2IFG=0;
        */
        

	init_eeprom();
  	program_reset=1;
  	clear_bstat();
  	//disp_index=LCDCHK;
	//disp_sub_ctr=0;
        //disp_ctr=10;//1;
	//disp_cnt=10;
	//auto_stat=1;
	
	//rtc_read_date();
	//rtc_read_time();  	
  	init_tod();//mnth_pos,load_val[0]);

        /*
        if(mains_stat==1)
        {
            if((load_control_stat==1)&&(over_limit_stat==0))
            {
                set_all_phase();
                load_conn_stat=1;
            }
            else
            {
                unset_all_phase();
                load_conn_stat=0;
            }
        }
        */
        

        while(1)
	{
                
                read_time_date();
                
                if(set_auth_data!=0)
                {
                    if(set_auth_data==1)
                    {
                      j=0xAA;
                      for(i=0;i<8;i++)
                      {
                           write_eeprom(AUTH1_LOC+i,auth1[2+i]);
                           j=j+auth1[2+i];
                      }
                      j=j&0xff;
                      write_eeprom(AUTH1_LOC+9,j);
                    }
                    else if(set_auth_data==2)
                    {
                      j=0xAA;
                      for(i=0;i<16;i++)
                      {
                           write_eeprom(AUTH2_LOC+i,auth2[2+i]);
                           j=j+auth2[2+i];
                      }
                      j=j&0xff;
                      write_eeprom(AUTH2_LOC+19,j);
                    }
                    set_auth_data=0;
                }

                if(set_cal_name_data==1)
                {
                    for(i=0;i<16;i++)
                           write_eeprom(CAL_NAME_PASSIVE_LOC+i,Calendar_Name_Passive[2+i]);
                    set_cal_name_data=0;
                }

                if(set_season_profile==1)
                {
                    for(j=0;j<2;j++)
                    {  
                      for(i=0;i<10;i++)
                        write_eeprom(SEASON_NAME_PASSIVE_LOC+(j*10)+i, Passive_Season_Name[j][i]);
                      
                      for(i=0;i<4;i++)
                        write_eeprom(SEASON_DATE_PASSIVE_LOC+(j*4)+i, passive_season_date[j][i]);
                    }
                    set_season_profile=0;
                }
                
        
                if(set_clock_time_zone==1)
                {
                  to_eeprom(IST_DIFF_LOC, clock_time_zone,2);
                  set_clock_time_zone=0;
                }  
                
                if(set_rtc_data==1)
                {
                  i=time_string[2];
                  i=(i*256)+time_string[3];
                  
                  /*
                  i=convt_byte_to_bcd(i%100);//yr
                  j=convt_byte_to_bcd(time_string[3]);//mnth
                  k=convt_byte_to_bcd(time_string[4]);//day
                  //set_rtc_date(k,j,i);
                  
                  i=convt_byte_to_bcd(time_string[6]);//hr
                  j=convt_byte_to_bcd(time_string[7]);//min
                  k=convt_byte_to_bcd(time_string[8]);//sec
                  //set_rtc_time(i,j,k);
                  */
                  
                  RTCCTL0_H = RTCKEY_H;                           
                  RTCCTL13 = RTCHOLD+RTCMODE;            
                  RTCCTL0_L &= ~RTCOFIFG_L;                     

                  RTCSEC   = time_string[9];
                  RTCMIN   = time_string[8];
                  RTCHOUR  = time_string[7]; 
                  
                  RTCDAY   = time_string[5];  
                  RTCMON   = time_string[4];  
                  RTCYEAR  = i%100; 
                  RTCCTL13 &= ~RTCHOLD;        
                  RTCCTL13 |= RTCLOCK;
                  RTCCTL0_H = 0;                 
                  
                  store_event_data(TRANSACT_EVENT, 151,0);//occ     
                  set_rtc_data=0;
                }
                else
                {
                  
                  get_time_data(real_date, real_time);
                  time_string[0]=13;
                  time_string[1]=12;
                  for(i=0;i<12;i++)
                    time_string[i+2]=tmp_time_string[i];
                }  
		
                if(set_demand_intgr_period==1)
                {
                  if((Max_Demand_Capture_Period<300)||(Max_Demand_Capture_Period>1800))
                    Max_Demand_Capture_Period=1800;//900;

                  to_eeprom(MD_INTGR_LOC,Max_Demand_Capture_Period,2);        
                  md_intgr_val=Max_Demand_Capture_Period/60;
                  if(((md_intgr_val%5)!=0)||(md_intgr_val==0)||(md_intgr_val>30))
                      md_intgr_val=30;//15;
                
                  
                  i=t_min/md_intgr_val;
                  write_eeprom(PREV_MIN_SLOT_LOC,i);

                  store_event_data(TRANSACT_EVENT, 152,0);//occ     
                
                  set_demand_intgr_period=0;
                }
                
                if(set_capture_period==1)
                {
                  //if((Load_Profile_Capture_Period<300)||(Load_Profile_Capture_Period>1800))
                  if((Load_Profile_Capture_Period!=900)&&(Load_Profile_Capture_Period!=1800))
                    Load_Profile_Capture_Period=1800;//900;

                  to_eeprom(SURVEY_INTGR_LOC,Load_Profile_Capture_Period,2); 
                  survey_intgr_val=Load_Profile_Capture_Period/60;
                  if(((survey_intgr_val%5)!=0)||(survey_intgr_val==0)||(survey_intgr_val>30))
                      survey_intgr_val=30;//15;

                  i=t_min/survey_intgr_val;
                  write_eeprom(PREV_SURVEY_MIN_SLOT_LOC,i);
                
                  store_event_data(TRANSACT_EVENT, 153,0);//occ     
                  reset_ls_data();
                  set_capture_period=0;
                }

                
                if(set_bill_date==1)
                {
                  //write_eeprom(BILL_DAY_LOC,bill_day[3]);
                  //write_eeprom(BILL_TIME_LOC,bill_time[0]);
                  //write_eeprom(BILL_TIME_LOC+1,bill_time[1]);
                  
                  if((scheduled_bill_day[0]==0)&&(scheduled_bill_day[1]==0)&&(scheduled_bill_day[2]==0))	
                  {
                          bill_day[3]=scheduled_bill_day[3];
                          bill_time[0]=scheduled_bill_time[0];
                          bill_time[1]=scheduled_bill_time[1];
                          write_eeprom(BILL_DAY_LOC,bill_day[3]);
                          for(i=0;i<2;i++)  
                            write_eeprom(BILL_TIME_LOC+i,bill_time[i]);
                          
                          scheduled_bill_day[3]=0;
                          
                  }
                  else
                  {
                          for(i=0;i<4;i++)  
                            write_eeprom(SCHEDULED_BILL_DAY_LOC+i,scheduled_bill_day[i]);
                          
                          for(i=0;i<2;i++)  
                            write_eeprom(SCHEDULED_BILL_TIME_LOC+i,scheduled_bill_time[i]);
                  }
                  
                  
                  store_event_data(TRANSACT_EVENT, 154,0);//occ     
                  set_bill_date=0;
                }
                
                /*
                if(set_tod_zone==1)
                {
                  for(j=0;j<2;j++)
                  {  
                    for(i=0;i<TOD_SIZE;i++)
                    {
                      write_eeprom(TOD_ID_LOC+(j*8)+i,tariff_id[j][i]);

                      t_zone[(j*8)+i]=tod_time[(j*8)+i][0];
                      t_zone[(j*8)+i]=(t_zone[(j*8)+i]*100)+tod_time[(j*8)+i][1];
                      to_eeprom(TOD_LOC+(j*16)+(i*2),t_zone[(j*8)+i],2);
                    }
                  }
                  
                  for(i=TOD_SIZE;i>0;i--)
                  {
                          if(t_zone[i-1]>0)
                                  break;
                  }	
                                      
                  num_of_zone=i;
                  if((num_of_zone<2)||(num_of_zone>TOD_SIZE))
                          num_of_zone=TOD_SIZE;	
                  write_eeprom(NUM_ZONE_LOC,num_of_zone);
                  
                  store_event_data(TRANSACT_EVENT, 155,0);//occ     
                  set_tod_zone=0;
                }  
                */
                
                if(set_day_profile==1)
                {
                  for(j=0;j<2;j++)
                  {  
                    for(i=0;i<TOD_SIZE;i++)
                    {
                      write_eeprom(PASSIVE_TOD_ID_LOC+(j*8)+i,passive_tariff_id[j][i]);

                      k=passive_tod_time[(j*8)+i][0];
                      k=(k*100)+passive_tod_time[(j*8)+i][1];
                      to_eeprom(PASSIVE_TOD_LOC+(j*16)+(i*2),k,2);
                    }
                  }
                  
                  //to_eeprom(ACTIVATE_DATE_LOC,activate_date,3);
                  //to_eeprom(ACTIVATE_TIME_LOC,activate_time,2);

                  //store_event_data(TRANSACT_EVENT, 155,0);//occ     
                  set_day_profile=0;
                }
                
                if(set_activation_date==1)
                {
                  to_eeprom(ACTIVATE_DATE_LOC,activate_date,3);
                  to_eeprom(ACTIVATE_TIME_LOC,activate_time,2);
                  store_event_data(TRANSACT_EVENT, 155,0);//occ     
                  set_activation_date=0;
                }
                
                if(activate_tod_stat==1)
                {
                  tod_passive_to_active();
                  md_reset_b=1;
                  store_event_data(TRANSACT_EVENT, 155,0);//occ     
                  activate_tod_stat=0;
                }  

                
                /*
                if((set_over_current_val==1)||(set_over_load_val==1)||(set_conn_time_interval==1)||(set_conn_lockout_time==1)||(set_conn_time_repeat==1))
                {
                  
                  if((Over_Current_Val<100)||(Over_Current_Val>9000))
                    Over_Current_Val=9000;
                  if((Over_Load_Val<1200)||(Over_Load_Val>60000))
                    Over_Load_Val=60000;
                  if((Conn_Time_Interval<60)||(Conn_Time_Interval>3600))
                    Conn_Time_Interval=300;
                  if((Conn_Lockout_Time<60)||(Conn_Lockout_Time>60000))
                    Conn_Lockout_Time=1800;
                  if((Conn_Time_Repeat<1)||(Conn_Time_Repeat>100))
                    Conn_Time_Repeat=3;

                  to_eeprom(OVER_CURRENT_VAL_LOC,Over_Current_Val,2);
                  to_eeprom(OVER_LOAD_VAL_LOC,Over_Load_Val,2);
                  to_eeprom(CONN_TIME_INTERVAL_LOC,Conn_Time_Interval,2);
                  to_eeprom(CONN_LOCKOUT_TIME_LOC,Conn_Lockout_Time,2);
                  write_eeprom(CONN_TIME_REPEAT_LOC,Conn_Time_Repeat);
                  set_over_current_val=0;
                  set_over_load_val=0;
                  set_conn_time_interval=0;
                  set_conn_lockout_time=0;
                  set_conn_time_repeat=0;
                }

                if(set_load_control_stat==1)
                {
                  if(load_control_stat>1)
                    load_control_stat=1;
                  
                  if(load_control_stat==1)
                  {
                      set_all_phase();
                      load_conn_stat=1;
                      over_limit_time_ctr=0;
                      under_limit_time_ctr=0;
                      load_break_time_ctr=0;
      
                      if(over_limit_stat==1)
                      {  
                        store_event_data(CTRL_EVENT, 302,1);//discon res
                        over_limit_stat=0;
                        write_eeprom(OLOAD_STAT_LOC, over_limit_stat);  
                      }
                  }
                  else
                  {
                      unset_all_phase();
                      load_conn_stat=0;
                      over_limit_time_ctr=0;
                      load_break_time_ctr=0;
                  }
                  write_eeprom(LOAD_CONTROL_STAT_LOC,load_control_stat);
                  set_load_control_stat=0;
                }  
                */
                
                if(set_tamper_time==1)
                {

                  if((Tamper_Occ_Time<15)||(Tamper_Occ_Time>1800))
                    Tamper_Occ_Time=15;//900;

                  if((Tamper_Res_Time<15)||(Tamper_Res_Time>1800))
                    Tamper_Res_Time=15;//300;

                  to_eeprom(TAMPER_OCC_TIME_LOC,Tamper_Occ_Time,2);
                  to_eeprom(TAMPER_RES_TIME_LOC,Tamper_Res_Time,2);
                  set_tamper_time=0;
                }
                
                //rtc_read_date();
		//rtc_read_time();
                
                prev_date=from_eeprom(DATE_LOC,3);
		prev_yr=prev_date%100;
		prev_mnth=(prev_date/100)%100;
		prev_day=(prev_date/10000);
		prev_hr=read_eeprom(HR_LOC);

		if((prev_day>31)||(prev_day==0)||(prev_mnth>12)||(prev_mnth==0)||(prev_yr>99)||(prev_yr<11)||((prev_hr&0x7f)>23))
                {
                        ep_status=0;
                        self_diagnostics_status=self_diagnostics_status|0x02;
                }	
                else
                {
                        ep_status=1;
                        self_diagnostics_status=self_diagnostics_status&0xFD;
                }
                        
                if((d_day>31)||(d_day==0)||(d_mnth>12)||(d_mnth==0)||(d_yr>99)||(d_yr<11)||(t_hr>23)||(t_min>59)||(t_sec>59))
                {	
                        rtc_status=0;
                        self_diagnostics_status=self_diagnostics_status|0x01;
                        
                        if(ep_status==1)
                        {
                            RTCCTL0_H = RTCKEY_H;                           
                            RTCCTL13 = RTCHOLD+RTCMODE;            
                            RTCCTL0_L &= ~RTCOFIFG_L;                     

                            RTCSEC   = 0;
                            RTCMIN   = 0;
                            RTCHOUR  = (prev_hr&0x7f); //0; // 
                            
                            RTCDAY   = prev_day;  
                            RTCMON   = prev_mnth;  
                            RTCYEAR  = prev_yr; 
                            RTCCTL13 &= ~RTCHOLD;
                            RTCCTL13 |= RTCLOCK;
                            RTCCTL0_H = 0;                 
                        }  
                }
                else
                {
                        rtc_status=1;
                        self_diagnostics_status=self_diagnostics_status&0xFE;
                        
                        if(ep_status==0)
                        {
                                to_eeprom(DATE_LOC,real_date,3);
                                //if(t_min>=30)
                                        //write_eeprom(HR_LOC,(t_hr|0x80));
                                //else
                                        write_eeprom(HR_LOC,t_hr);	
                        }
                }
 
		
                //if((co_disable_ctr>=10)&&(meter_no>0)&&(meter_no<0xFFFFFF))////300)//600 //
	  	//{ 	
                  if(cuopen_stat!=1)
                  {
                    if((P1IN & 0x01)==0x01)  //low on cover open
                    {
                      store_event_data(NOROLL_EVENT, 251,0);//occ     
                      to_eeprom(CUOPEN_LOC+1,real_date,3);
                      to_eeprom(CUOPEN_LOC+4,real_time,3);
                      write_eeprom(CUOPEN_LOC,1);
                      cuopen_date=real_date;
                      cuopen_time=real_time;
                      cuopen_stat=1;
                      //all_tamper_cnt=all_tamper_cnt+1;
                      //disp_copen();
                    }
                  }
                //}
                
                if(mains_stat==1)
		{
                        if(power_fail_check_stat==0)
			{	
				power_fail_func();
				power_fail_check_stat=1;
			}	

/****************/			
                        if(mag_chk_delay>=5)
                        {    
                                /*  
                                //magnetic tamper
                                if((P1IN & 0x04) == 0x00)
                                        magnetic_stat=1;
                                else
                                        magnetic_stat=0;
                                */
                          
                                if((mag_on_ctr1>600)&&(mag_on_ctr1<2000))
                                {
                                        if(mag_imax_on_ctr>=6) //3
                                                mag_imax_stat=1;
                                        else
                                                mag_imax_on_ctr++;
                                        mag_imax_off_ctr=0;	
                                }
                                else
                                {
                                        if((mag_on_ctr1<=600)||(mag_on_ctr1>=2000))
                                        {
                                                if(mag_imax_off_ctr>=16) //5, 3
                                                        mag_imax_stat=0;
                                                else
                                                        mag_imax_off_ctr++;
                                                mag_imax_on_ctr=0;	
                                        }
                                }	

                        }
                        else
                                mag_chk_delay++;
                        
                        if(mag_imax_stat==0)
                        {  
                            if(mag_on_ctr1>=2000)
                              magnetic_stat=1;
                            else
                              magnetic_stat=0;
                            
                            /*
                            if(magnetic_stat==1)
                            {
                                    if(channel==1)
                                    {
                                            mag_pulse_val=prev_kw_reg2;
                                            kw_reg2[1]=prev_kw_reg2;
                                            //irms1_reg2=prev_irms1_reg2;
                                            irms_reg2[1]=prev_irms_reg2;
                                            mag_pulse_stat=1;
                                    }
                                    else
                                    {       
                                            mag_pulse_stat=0;
                                            if((irms_reg2[0] > irms_reg2[1])&& (irms_reg2[1] >160))//.20 A
                                              irms_reg2[1]=irms_reg2[0];
                                    }
                                    
                                    save_kw_reg2_ctr=0;
                            }
                            else
                            {
                                    mag_pulse_stat=0;
                                    if(save_kw_reg2_ctr>=3)
                                    {
                                            prev_kw_reg2=kw_reg2[1];
                                            //prev_irms1_reg2=irms1_reg2;
                                            prev_irms_reg2=irms_reg2[1];
                                            save_kw_reg2_ctr=0;
                                            //running_meter_stat=1;
                                    }
                                    else
                                            save_kw_reg2_ctr++;	
                            }
                            */
                        }
                        else
                            magnetic_stat=0;
                        
                        
                        eload_check();
			
                        vrms_reg3=(vrms_reg2*1000)/volt_fact;   	//240.0
                        if(vrms_reg2<500)
				vrms_reg3=0;
			for(i=0;i<2;i++)
                            irms_reg3[i]=(irms_reg2[i]*100)/amp_fact[i];  

                        
                        kva_reg2=vrms_reg2*irms_reg2[channel];
                        kva_reg2=(kva_reg2>>6); //7
                        kva_reg2=(unsigned long int)(((long long)kva_reg2*1237)/1000); //1234 1232 1230 1227 1225 1234 1240
                        //------------test done-----------
                        if((kva_reg2<kw_reg2[channel])||(nd_stat==1)||(nmiss_stat==1)||((magnetic_stat==1)||(mag_imax_stat==1)))
                                kva_reg2=kw_reg2[channel];

                        //ON RAW DATA    
                        if(kw_reg2[channel]>500)//0.002
                        {
                                scratch=kva_reg2/10;
                                pf_reg3=(int)((kw_reg2[channel]*100)/scratch); //(120*10)/120
                                
                                pulse_va=kva_reg2;
                        }
                        else
                        {
                                pf_reg3=1000;
			        pulse_va=0;
                        }
                                
                        tmp_float=kw_reg2[channel];
                        tmp_float=tmp_float*kw_reg2[channel];
                        tmp_float1=kva_reg2;
                        tmp_float1=tmp_float1*kva_reg2;
                        
                        tmp_float=tmp_float1-tmp_float;//(kvah^2-kwh^2)
                        tmp_float=sqrt(tmp_float);
                        kvar_reg2=(unsigned long int)tmp_float;
                  
                        kvar_reg2=(unsigned long int)(((long long)kvar_reg2*996)/1000); //1234 1232 1230 1227 1225 1234 1240
                        //kvar_reg2[i]=(kvar_reg2[i]*996)/1000; //1234 1232 1230 1227 1225 1234 1240
                  
                        //kvar_reg2[i]=0;
                        
                        //if((pf_reg3<980)&&(pf_sign==0))//ld
                        //        kva_reg2=kw_reg2;
				
                        //(x*1950/900000000)*2250	=>(x*1097/225000)=>x/205
                        kw_reg3=kw_reg2[channel]/391; //195;//205;
                        kva_reg3=kva_reg2/391; //195;//205;
                        kvar_reg3=kvar_reg2/391; //195;//205;
                        
                        if((kw_reg3==0)&&(irms_reg3[channel]==0))
                        {
                          kva_reg3=0;
                          kvar_reg3=0;
                        }  
                        
                        Mains_Freq=freq_reg3;

                        /*
                        if(vrms_reg3<1100)//Anil
                                sbi(P2OUT,BKLIT_PIN);//Anil
                        else if(vrms_reg3>1120)//Anil
                                cbi(P2OUT,BKLIT_PIN);//Anil
                        */
                        
                        
                        if(vrms_offset_adc>0)
                                i=vrms_offset_adc;
                        else
                                i=((~vrms_offset_adc)+1)&0x7fff;
                        
                        if(i>600)//&&(rev_stat==0))
                        {
                                rev_stat[0]=0;rev_stat[1]=0;
                                nd_stat=1;
                        }
                        else
                                nd_stat=0;
                        
                        if(nd_stat==0)
                        {
                                if(vrms_reg3<500)
                                        nmiss_stat=1;
                                else
                                        nmiss_stat=0;
                                
                                if((nmiss_stat==0)&&(rev_stat[0]!=1)&&(rev_stat[1]!=1)&&(magnetic_stat==0)&&(mag_imax_stat==0))
                                {		
                                        //if(((irms1_reg3>25)&&((vrms_reg3<1200)||(vrms_reg3>2900)))||((vrms_reg3>900)&&(vrms_reg3<2900)&&(irms1_reg3>25)&&(irms1_reg3<2000)&&(irms2_reg2>=7)&&(irms2_reg2<=45)))//50
                                        //if((vrms_reg3<2900)&&(irms1_reg3>25)&&(irms1_reg3<2000)&&(irms2_reg2>=5)&&(irms2_reg2<=45))//50
                                        /*
                                        if(
                                                ((irms_reg3[0]>25)&&(irms_reg3[0]<2000)&&(vrms_reg3<=1200))//&&(irms_reg2[1]<=45)
                                                ||((vrms_reg3>1200)&&(vrms_reg3<=2900)&&(irms_reg3[0]>25)&&(irms_reg3[0]<2000)&&(irms_reg2[1]>=9)&&(irms_reg2[1]<=45))
                                        )//50//(vrms_reg3<2900)&&
                                        */
                                        if(
                                                //((irms_reg3[0]>25)&&(irms_reg3[0]<2000)&&(vrms_reg3<=1200))//&&(irms_reg2[1]<=45)
                                                //||((vrms_reg3>1200)&&(vrms_reg3<=2380)&&(irms_reg3[0]>25)&&(irms_reg3[0]<2000)&&(irms_reg2[1]>=9)&&(irms_reg2[1]<=45))
                                        
                                                ((irms_reg3[0]>25)&&(irms_reg3[0]<2000)&&(vrms_reg3<=940)&&(irms_reg2[1]<=45))
                                                ||((vrms_reg3>940)&&(irms_reg3[0]>25)&&(irms_reg3[0]<2000)&&(irms_reg2[1]>=9)&&(irms_reg2[1]<=45))
                                        )
                                                nd_stat=1;
                                        else
                                                nd_stat=0;		
                                }
                                else
                                        nd_stat=0;
                        }
                        else
                                nmiss_stat=0;

                        nmiss_stat=persist_time(nmiss_stat,0);
                        nd_stat=persist_time(nd_stat,1);
                        
                        //eload_stat=persist_time(eload_stat0,2);
                        //rev_stat=persist_time(rev_stat0,1);
                        //mag_stat=persist_time(mag_stat,2);

                        if(nmiss_stat==1)
                        {
                                vrms_reg3=2400;
                                pf_reg3=1000;
                                freq_reg3=500;			
                        }
                        else if(nd_stat==1)
                        {
                                vrms_reg3=2400;
                                pf_reg3=1000;
                                freq_reg3=500;			
                        }			
                        if(mag_imax_stat==1)
                        {
                                vrms_reg3=2400;
                                irms_reg3[0]=3000;  
                                irms_reg3[1]=3000;  
                                kw_reg3=7200; 
                                kva_reg3=7200;
                                kvar_reg3=0;
                                pf_reg3=1000;
                        }
                        
                        
			//avg pf
			if(load_val[0]>bkp_kwh_val)
				scratch=load_val[0]-bkp_kwh_val;
			else
				scratch=0;
			
			if(load_val[1]>bkp_kvah_val)
				scratch1=load_val[1]-bkp_kvah_val;
			else
				scratch1=0;
			
			if(scratch1>0)
                          avg_pf=(scratch*100)/scratch1;
                        else
                          avg_pf=100; //0;//
	        
                        if(avg_pf>100)
                          avg_pf=100;
			
			
                        if((irms_reg3[0]<7200)&&(irms_reg3[1]<7200))
                        {  
                            load_func();
                            if((rtc_status==1)&&(ep_status==1))
                            {
                                  select_season();
                                  //md_func();
                                  history_func();
                                  update_tod_data();
                                  tamper_func();
                            }
                        }
                        
                        //if(load_control_stat==1)
                        //  oload_check();
                              
                              
                      for(i=0;i<2;i++)
                      {
                              scratch=load_val[i]%10000;
                              scratch=scratch*100;
                              dial_pulse[i]=load_rmndr[i];
                              dial_load[i]=scratch+((dial_pulse[i]*100)>>5);//1 watt
                      }       
                      
		}
		else
		{
                        for(i=0;i<2;i++)
                        {    
                            irms_reg3[i]=0;
                            rev_stat[i]=0;
			}
                        
                        vrms_reg3=0;
                        kw_reg3=0;
                        kva_reg3=0;
                        kvar_reg3=0;
                        pf_reg3=0;
                        freq_reg3=0;
                        Mains_Freq=0;
			magnetic_stat=0;
			ctn_stat=0;
			nd_stat=0;
			nmiss_stat=0;
                        eload_stat=0;
		}

                //display statement
		if(mains_stat==1)
		{
                      //if(scrl_b==1) //&&(disp_mode_stat!=1))
                      //  disp_ctr=disp_cnt;

                      if(disp_mode_stat==1)
                      {
                              if(scrl_b==1)
                              {
                                      disp_mode_stat=2;
                                      disp_mode();
                                      disp_index=LCDCHK;
                                      hist_ctr=0;
                                      disp_sub_ctr=0;
                                      disp_ctr=1;
                                      disp_cnt=10;
                              }	        
                      }
                      else if(disp_mode_stat==2)
                      {
                              if(scrl_b_dn_ctr>=5) //10
                              {	
                                      disp_mode_stat=3;
                                      disp_mode();
                                      disp_sub_ctr=0;
                                      disp_index=HRESKWH; //DATE;//
                                      hist_ctr=0;
                                      disp_ctr=1;
                                      disp_cnt=120;						
                              }
                              if(scrl_b_up_ctr>=10) //10
                              {	
                                      disp_mode_stat=1;
                                      disp_mode();
                                      disp_sub_ctr=0;
                                      disp_index=LCDCHK; //DATE;//
                                      hist_ctr=0;
                                      disp_ctr=1;
                                      disp_cnt=10;						
                              }
                      }
                      else if(disp_mode_stat==3)
                      {
                              if((scrl_b_up_ctr>=120)||(scrl_b==1)) //10
                              {	
                                      disp_mode_stat=1;
                                      disp_mode();
                                      disp_sub_ctr=0;
                                      disp_index=LCDCHK; //DATE;//
                                      hist_ctr=0;
                                      disp_ctr=1;
                                      disp_cnt=10;						
                              }
                      }
                      if(scrl_b_dn_ctr<100)
                        scrl_b_dn_ctr++;
                      if(scrl_b_up_ctr<2000)
                        scrl_b_up_ctr++;
		}    


                select_disp_index();
                
                //disp_index=LASTREST;//IRMS;//HRESKWH;//SLNO;
                //disp_index=SLNO;//CUOPEN;
                //disp_sub_ctr=0;
                
                switch_index();
                
                clear_bstat();
                
                
                if((mains_stat==0)&&(batt_push_ctr>=10)&&(comm_delay_ctr>=20))
                {
                        _BIC_SR(GIE);

                        if(power_fail_check_stat==1)
                                to_eeprom(TIME_LOC,real_time,3);//to calculate power fail
                        power_fail_check_stat=0;			
                        self_diagnostics_status=self_diagnostics_status&0xFB;
                        
                        SD24_Disable();
                        REFCTL0=0; 
                        //REFCTL0 &= ~(REFMSTR + REFVSEL_1 + REFON); 
                        
                        LCDCVCTL &= (~LCDCPEN);
                        LCDCMEMCTL = LCDCLRM;
                        LCDCCTL0 &= (~LCDON);
                        
                        WDTCTL = WDTPW+WDTSSEL__ACLK+WDTHOLD;                 // Stop watchdog timer

                         
                        TA0CCTL0 = 0x00;
                        TA0CTL = 0x00;
                        
                        
                        //P1SEL=0;
                        //P2SEL=0;
                        P2SEL=0;
                        UCA0IE&=(~UCRXIE);
                        //UCA1IE&=(~UCRXIE);
                        UCA2IE&=(~UCRXIE);

                        
                        //push_lock_ctr++;
                        //if(push_lock_ctr<=3)
                        //{
                                sbi(P1IE,PUSHDN_B);
                        //}					

                        /*
                        sbi(P2DIR,5);sbi(P2OUT,5);//RS232 TX output high
                        
                        //sbi(P2DIR,2);cbi(P2OUT,2);//Opto RX output high
                        sbi(P2DIR,3);sbi(P2OUT,3);//Opto TX output high
                        */
                        
                        // Port Configuration
                        P1OUT = 0x00; P2OUT = 0x00; P3OUT = 0x00; P4OUT = 0x00; P5OUT = 0x00; P6OUT = 0x80;
                        P7OUT = 0x00; P8OUT = 0x00; P9OUT = 0x00; PJOUT = 0x00;
                        P1DIR = 0x08; P2DIR = 0x3F; P3DIR = 0xFF; P4DIR = 0xFF; P5DIR = 0xFF; P6DIR = 0xFF;
                        P7DIR = 0xFF; P8DIR = 0xFF; P9DIR = 0xFF; PJDIR = 0xFF;   

                        sbi(P2DIR,2);cbi(P2OUT,2);//RS232 RX output low
                        sbi(P2DIR,3);cbi(P2OUT,3);//RS232 TX output low
                        sbi(P2DIR,4);cbi(P2OUT,4);//Opto RX output low
                        sbi(P2DIR,5);cbi(P2OUT,5);//Opto TX output low
                        
                        sbi(P6DIR,COMM_CTRL_PIN);sbi(P6OUT,COMM_CTRL_PIN);
	    		//sbi(P2DIR,BKLIT_PIN);sbi(P2OUT,BKLIT_PIN);
                        
                        sbi(P1DIR,PULSE1_PIN);cbi(P1OUT,PULSE1_PIN);
                                                
                        
                        //rtc values can not be  read after SVS is disabled.
                        PMMCTL0_H=PMMPW_H;   
                        SVSMHCTL &= ~(SVMHE | SVSHE);
                        SVSMLCTL &= ~(SVMLE | SVSLE);
                        PMMCTL0_H=0;   
                          
                        batt_mode_comm_en_stat=0;
                        batt_disp_stat=0;
			batt_push_ctr=10;
                        mains_stat=0;
                        mains_pulse_ctr=0;
                        mains_pulse_ctr1=0;
                        init_stat=0;
                        _BIS_SR(LPM3_bits+GIE);// Enter LPM3 w/ interrupts 		
                }	

                serial_comm();
                //cbi(P1OUT,PULSE1_PIN);  
                do
		{
                    //UCA1TXBUF='*';
                    //while((UC1IFG & 0x02)!=0x02);
                  
                    //if((P2IN & 0x08)==0x00)  //high on cover open
                    //{  
                      //-------------------------DLMS-----------------------------------------        
                      // As we need to priortize Meterology Interrupts, we are doing this processing in the main routine.
                      for(i=0;i < MAX_USCI_PORTS;  i++)
                       {
                        if(Frame_Received[i]==1)
                         {
                           //LCDMEM[1]=LCDMEM[1]&0xfe;
                           
                           rx[i].state=0;
                           Frame_Received[i]=0;
                           process_rx_frame(&link[i], &rx[i]);
                           check_and_enable_transmit(i);
                           WDTCTL=WDT_ARST_1000;//WDTPW+WDTSSEL__ACLK+WDTCNTCL;
                           comm_delay_ctr=0;
                           //LCDMEM[1]=LCDMEM[1]|0x01;
                         }
                       }  
                      //------------------------------------------------------------------                            
                    //}
                    //else    
                    delay1ms(1);
                    //delay_sec=1;
                    
                    if((P1IN & 0x10)==0x00)	//scroll dn 3-2-1
                    {	
                      scrl_b_up_ctr=0;
                      if(scrl_b_up==1)
                      {  
                        scrl_b_up=0;
                        scrl_b=1;

                        if(disp_mode_stat==2)
                        {  
                           disp_ctr=disp_cnt;
                           select_disp_index();
                           switch_index();
                           scrl_b=0;
                        }   
                      }
                    }
                    else
                    {
                      scrl_b_dn_ctr=0;
                      if(scrl_b_up==0)
                      {
                        scrl_b_up=1;	
                      }  
                    }
                    
		}
		while(delay_sec!=1);
		delay_sec=0;
                //sbi(P1OUT,PULSE1_PIN);
                
		if(program_reset==0)
			break;
	}
	
        if(program_reset==0)
		goto start;
	return;		
}  				


unsigned char persist_time(unsigned char present_stat, unsigned char index)
{
	//unsigned char i;
	
	/*
	if(index==2)
		i=3;
	else
		i=5;
	*/
	
	if(present_stat==1)
    	{
	    	if(inst_tmpr_time_on_ctr[index]<3)//i
	    	{
	    		inst_tmpr_time_on_ctr[index]++;
	    		present_stat=0;
	    	}
	}
	else
    		inst_tmpr_time_on_ctr[index]=0;		
	return present_stat;
}


void select_disp_index(void)
{
	if(disp_ctr>=disp_cnt)
	{	
		//cuopen_reg_stat=1;
		disp_ctr=1;
		if((disp_index==HKWH)||(disp_index==HKVAH))
		{
			disp_sub_ctr=0;
			if(hist_ctr>=5)//2
			{
				get_disp_index();
				hist_ctr=0;
			}
			else
				hist_ctr++;
		}	
		else if(disp_index==HKWMD)
		{
			//if(disp_sub_ctr>=4)
			//{
				disp_sub_ctr=2;
				if(hist_ctr>=5)//2
				{
					get_disp_index();
					hist_ctr=0;
				}
				else
					hist_ctr++;
			//}
			//else
			//	disp_sub_ctr++;
		}	
		else if(disp_index==HPF)
		{
			disp_sub_ctr=5;
			if(hist_ctr>=2)
			{
				get_disp_index();
				hist_ctr=0;
			}
			else
				hist_ctr++;
		}
		else if((disp_index==BKWMD)||(disp_index==KWMD))
		{
			if(disp_sub_ctr>=4)
				get_disp_index();
			else
				disp_sub_ctr++;
		}	
		else if(disp_index==TKWH)
		{
			disp_sub_ctr=0;
			if(disp_zone_ctr>=(num_of_zone-1))
			{
				disp_zone_ctr=0;
				disp_sub_ctr=0;
				get_disp_index();
			}
			else
				disp_zone_ctr++;
		}
		else if((disp_index==CUOPEN)||(disp_index==TSTATUS)||(disp_index==LASTOCC)||(disp_index==LASTREST))
		{
			if(disp_sub_ctr>=2)
			{
				disp_sub_ctr=0;
				get_disp_index();
			}
			else
				disp_sub_ctr++;
		}
                else if(disp_index==CODE)
                {
                    if(disp_sub_ctr>=3)
                    {
                      disp_sub_ctr=0;
                      get_disp_index();
                    }
                    else
                        disp_sub_ctr++;
                }
		else if(disp_index==TSTAT)
		{			
			disp_sub_ctr++;
			find_tamper_stat();
			if(disp_sub_ctr>=8)
			{
				disp_sub_ctr=0;
				get_disp_index();
			}
		}
                
		else
			get_disp_index();
	}
	else
		disp_ctr++;
	//scrl_b=0;

	//if(dlc_time>0)
	//{
	//	disp_index=DLC;
	//	disp_ctr=disp_cnt;
	//	batt_push_ctr=10;
	//}		

	return;
}

/*    
r=0x44 y=0xEA b=0xCE L=0x0E d=0xEC g=0xEB C=0x0F O=0xAF
-=0x40 P=0x67 E=0x4F n=0xC4 N=0xA7 h=0xC6 U=0xAE c=0x4C 
i=0xA0 t=0x4E S=0xCB F=0x47 A=0xE7 H=0xE6 o=0xCC u=0x8C
*/

void disp_ver(void)
{
	disp_all_lcd(0x00);
        LCDMEM[11]=seg_7_lookup('U');
        LCDMEM[10]=seg_7_lookup('P');
        LCDMEM[8]=seg_7_lookup('P');
        LCDMEM[7]=seg_7_lookup('C');
        LCDMEM[6]=seg_7_lookup('L');
	LCDMEM[5]=seg_7_lookup(5);//5 02-12-2022 //4 
	LCDMEM[5]=LCDMEM[5] | 0x80;	
	//delay1ms(250);
	//delay1ms(250);
	//delay1ms(250);
	//delay1ms(250);
}

void disp_mode(void)
{
	unsigned char i;
	
        disp_all_lcd(0x00);
	if(disp_mode_stat==1)
	{
		LCDMEM[10]=seg_7_lookup('A');
		LCDMEM[8]=seg_7_lookup('u');
		LCDMEM[7]=seg_7_lookup('t');
		LCDMEM[6]=seg_7_lookup('o');
	}
	else if(disp_mode_stat==2)
	{
		LCDMEM[10]=seg_7_lookup('P');
		LCDMEM[8]=seg_7_lookup('U');
		LCDMEM[7]=seg_7_lookup('S');
		LCDMEM[6]=seg_7_lookup('H');
	}
        
	else if(disp_mode_stat==3)
	{
		LCDMEM[11]=seg_7_lookup('H');
		LCDMEM[10]=seg_7_lookup('I');
		LCDMEM[8]=seg_7_lookup('-');
		LCDMEM[7]=seg_7_lookup('r');
		LCDMEM[6]=seg_7_lookup('E');
		LCDMEM[5]=seg_7_lookup('S');
	}
        
        
        for(i=0;i<2;i++)
   		delay1ms(250);	
}

/*
void oload_check(void)
{
      unsigned char i,over_current_stat,over_load_stat;
      //unsigned int over_current_val, over_load_val;
      
      over_current_stat=0;
      for(i=0;i<3;i++)
      {
          if(irms_reg3[i]>=Over_Current_Val)//90.00 A //over_current_val
          {
            over_current_stat=1;
            break;
          }  
      }

      scratch=0;
      for(i=0;i<3;i++)
        scratch=scratch+kw_reg3[i];
      //scratch=scratch/10;		

      if(scratch>Over_Load_Val)//42.000 kw //over_load_val
        over_load_stat=1;
      else
        over_load_stat=0;

      
      if(load_conn_stat==1)//connected
      {
        if((over_current_stat==1)||(over_load_stat==1))
        {
          if(over_limit_time_ctr>=10)
          {
              unset_all_phase();
              load_conn_stat=0;
              over_limit_time_ctr=0;
              load_break_time_ctr=0;

              if(over_limit_stat==0)
              {
                store_event_data(CTRL_EVENT, 301,0);//discon occ     
                over_limit_stat=1;
                write_eeprom(OLOAD_STAT_LOC, over_limit_stat);  
              }
              
              if(load_break_index>=Conn_Time_Repeat)
              {
                load_break_time=Conn_Lockout_Time;//1800;
                load_break_index=0;
              }
              else
              {
                load_break_time=Conn_Time_Interval;//300;
                load_break_index++;
              }  
          }
          else
            over_limit_time_ctr++;
          under_limit_time_ctr=0;
        }
        else
        {
          if((under_limit_time_ctr>=10)&&(over_limit_stat==1))
          {
              store_event_data(CTRL_EVENT, 302,1);//discon res
              over_limit_stat=0;
              write_eeprom(OLOAD_STAT_LOC, over_limit_stat);  
          }
          else
            under_limit_time_ctr++;
          over_limit_time_ctr=0;
        }  
      }
      else
      {
        if(load_break_time_ctr>=load_break_time)
        {  
            set_all_phase();
            load_conn_stat=1;
            over_limit_time_ctr=0;
            under_limit_time_ctr=0;
            load_break_time_ctr=0;
        }
        else
          load_break_time_ctr++;
      }  
}
*/

void set_all_phase(void)
{
        /*
	relay_delay_ctr[0]=0;
	sbi(P4OUT,R_ON);
	delay1ms(250);delay1ms(250);
	relay_delay_ctr[1]=0;
	sbi(P4OUT,Y_ON);
	delay1ms(250);delay1ms(250);
	relay_delay_ctr[2]=0;
	sbi(P5OUT,B_ON);
        */
	return;
}

void unset_all_phase(void)
{
        /*
	relay_delay_ctr[0]=0;
	sbi(P4OUT,R_OFF);
	delay1ms(250);delay1ms(250);
	relay_delay_ctr[1]=0;
	sbi(P4OUT,Y_OFF);
	delay1ms(250);delay1ms(250);
	relay_delay_ctr[2]=0;
	sbi(P5OUT,B_OFF);
        */
	return;
}

void get_disp_index(void)
{
	//unsigned long int tmp_long;	
	if(mains_stat==0)
	{
		switch(disp_index)
		{
			//case LCDCHK:disp_index=VER;break;	
			case VER:disp_index=KWH;break;	
			case KWH:disp_index=KWMDONLY;break;
      			case KWMDONLY:disp_index=BPF;hist_ctr=0;break;
	  		case BPF:disp_index=POWERON;break;
      			case POWERON:/*disp_index=TCNT;break;
      			case TCNT:*/
				if(cuopen_stat==1)
				{
					disp_index=CUOPEN;
					disp_sub_ctr=0;
				}
				else
				{
					batt_push_ctr=10;
				}	
				break;
			case CUOPEN:disp_index=CUOPEN;disp_sub_ctr=2;/*disp_index=LCDCHK;*/batt_push_ctr=10;break;
			default:disp_index=KWH; //LCDCHK; //KWH;//
		}
	}
	else
	{
		if(disp_mode_stat==1)
		{
			switch(disp_index)
			{
                                case VER:disp_index=LCDCHK;break;	
				case LCDCHK:disp_index=DATE;break;
				case DATE:disp_index=TIME;break;
				case TIME:disp_index=SLNO;break;
				case SLNO:disp_index=KWH;break;	
                                case KWH:disp_index=CKWMDONLY;break;
                                case CKWMDONLY:disp_index=KVAH;break;	
				case KVAH:
					if(cuopen_stat==1)
					{
						disp_index=CUOPEN;
						disp_sub_ctr=0;
					}
					else
					{
						disp_index=LCDCHK;//DATE;
					}	
					break;
				case CUOPEN:disp_index=LCDCHK;break;//DATE;
				default:disp_index=LCDCHK;//DATE;
			}
		}
		else
		{
			switch(disp_index)
			{
				case LCDCHK:disp_index=DATE;break;
				case DATE:disp_index=TIME;break;
				case TIME:disp_index=SLNO;break;
				case SLNO:disp_index=KWH;break;
				case KWH:disp_index=CKWMDONLY;break;
				case CKWMDONLY:disp_index=KVAH;break;
				case KVAH:disp_index=HKWH;hist_ctr=0;disp_sub_ctr=0;break;
				case HKWH:disp_index=HKWMD;hist_ctr=0;disp_sub_ctr=2;break;
	      			case HKWMD:disp_index=HKVAH;hist_ctr=0;disp_sub_ctr=0;break;
				case HKVAH:disp_index=VRMS;break;
				case VRMS:disp_index=IRMS;break;
				case IRMS:disp_index=INSTKW;break;
				case INSTKW:/*disp_index=RSTCOUNT;break;
	      			case RSTCOUNT:*/disp_index=INSTPF;break;
	      			case INSTPF:disp_index=APF;hist_ctr=0;break;
                                case APF:disp_index=BPF;break;
	      			case BPF:/*disp_index=HPF;hist_ctr=0;disp_sub_ctr=5;break;
	      			case HPF:*/disp_index=POWERON;break;
	      			case POWERON:disp_index=TCNT;break;
	      			case TCNT:disp_index=TSTAT;disp_sub_ctr=0;find_tamper_stat();break;
	      			case TSTAT:disp_index=LASTOCC;disp_sub_ctr=0;break;
       				case LASTOCC:disp_index=LASTREST;disp_sub_ctr=0;break;
				case LASTREST:
					if(cuopen_stat==1)
					{
						disp_index=CUOPEN;
						disp_sub_ctr=0;
					}
					else
					{
						disp_index=CODE;disp_sub_ctr=0;
					}
					break;		
				case CUOPEN:disp_index=CODE;disp_sub_ctr=0;break;
				case CODE:disp_index=LCDCHK;break;
				default:disp_index=LCDCHK;
			}
		}	  
	}
}

void find_tamper_stat(void)
{
	//unsigned char i;
	
	for(;disp_sub_ctr<9;disp_sub_ctr++)
	{
		if((disp_sub_ctr==0)&&((rev_stat[0]==1)||(rev_stat[1]==1)))
			break;
		if((disp_sub_ctr==1)&&(eload_stat==1))
			break;
		//if((disp_sub_ctr==5)&&(magnetic_stat==1))
		if((disp_sub_ctr==2)&&(mag_imax_stat==1))
			break;
		if((disp_sub_ctr==3)&&(nd_stat==1))
			break;
		if((disp_sub_ctr==4)&&(nmiss_stat==1))
			break;
                if((disp_sub_ctr==5)&&(hv_stat==1))
			break;
		if((disp_sub_ctr==6)&&(lv_stat==1))
			break;
		if((disp_sub_ctr==7)&&(hc_stat==1))
			break;
		
		//if((disp_sub_ctr==9)&&(cuopen_stat==1))
		//	break;
	}
	
	return;
}

/*
void get_disp_index(void)
{
	//unsigned long int tmp_long;	
	if(mains_stat==0)
	{
		switch(disp_index)
		{
			//case LCDCHK:disp_index=KWH;break;
			case KWH:disp_index=KWMDONLY;break;
			case KWMDONLY:disp_index=BPF;break;
			case BPF:disp_index=POWERON;break;
                        case POWERON:
				if(cuopen_stat==1)
				{
					disp_index=CUOPEN;
					disp_sub_ctr=0;
				}
				else
				{
					//disp_index=LCDCHK;
					batt_push_ctr=10;
				}
				break;		
			case CUOPEN:batt_push_ctr=10;break;
			default:disp_index=KWH;//LCDCHK;
		}
	}
	else
	{
		if(disp_mode_stat==1)
		{
                        switch(disp_index)
			{
				//case LCDCHK:disp_index=DATE;break;
				case DATE:disp_index=TIME;break;
				case TIME:disp_index=KWH;break;
				case KWH:
					if(cuopen_stat==1)
					{
						disp_index=CUOPEN;
						disp_sub_ctr=0;
					}
					else
						disp_index=DATE;//LCDCHK;
					break;
				case CUOPEN:disp_index=DATE;break;	
				default:disp_index=DATE; //LCDCHK;
			}
		}
		else
		{
			switch(disp_index)
			{
				case LCDCHK:disp_index=DATE;break;
				case DATE:disp_index=TIME;break;
				case TIME:disp_index=KWH;break;
				case KWH:disp_index=KWMD;disp_sub_ctr=2;break;
				case KWMD:disp_index=SLNO;break;
				case SLNO:disp_index=HKWH;disp_sub_ctr=0;hist_ctr=0;break;
				case HKWH:disp_index=HKWMD;disp_sub_ctr=2;hist_ctr=0;break;
                                case HKWMD:disp_index=VRMS;break;
				case VRMS:disp_index=IRMS;break;
				case IRMS:disp_index=INSTKW;break;
				case INSTKW:disp_index=RSTCOUNT;break;
                                case RSTCOUNT:disp_index=INSTPF;break;
				case INSTPF:disp_index=APF;break;
				case APF:disp_index=BPF;hist_ctr=0;break;
				case BPF:disp_index=POWERON;break;
				case POWERON:disp_index=TCNT;break;
				case TCNT:
					if(cuopen_stat==1)
					{
						disp_index=CUOPEN;
						disp_sub_ctr=0;
					}
					else
					{
						disp_index=CODE;disp_sub_ctr=0;
                                                //disp_index=LCDCHK;//RTCOK;
					}
					break;		
				case CUOPEN:disp_index=CODE;disp_sub_ctr=0;break;
				case CODE:disp_index=HRESKWH;break;
				case HRESKWH:disp_index=LCDCHK;break;
				default:disp_index=LCDCHK;
			}
		}	  
	}
}
*/

void switch_index(void)
{
	unsigned char i,j;//,k;
	//unsigned int location;
	//unsigned char tmp_byte;
	unsigned long int location;
	
//TOD CO ON S1 D2 BP D3 NM
//tag1=DATE D1 REV TIME MAG AV MD CUM
//tag2=K V \ / Z EL h A

/*
    P3,T2,T3,T4,P1,T5,P2,T1
    .D,ON,< ,EL,4D,NM,2D,1D

tag1=T6,T7,T8,T9,T20,T14,T18,T19
      DATE,TIME,AV,MAG,K,W,TOD,|

tag2=P4,T10,T11,T12,T13,T15,T16,T17
      ::,CUM, CO, MD, BP,  V, A,  h 
*/        
        
	i=0;j=0;
	switch(disp_index)
	{		
                case NONE:
                  break;
                case LCDCHK:
                  break;
		case BLANK:
                  break;
		case SLNO:
                        rem_disp_val=meter_no%10;
			scratch=meter_no/10; //mains_stat_cnt; //SYSRSTIV; //meter_no;
                        
                        //scratch=mains_pulse_ctr1;
                        //rem_disp_val=0;
                        
			break;
		case TIME:
			scratch=real_time;
			i=0x40;j=0x80;				
			break;
		case DATE:
			scratch=real_date;
			i=0x80;j=0x80;				
			break;
                        
		case KWH:
		case DKWH:
			rem_disp_val=load_val[0]%100;
			rem_disp_val2=rem_disp_val%10;
			rem_disp_val=rem_disp_val/10;
			scratch=load_val[0]/100;// /100;
			i=0x0C; j=0x41;
			break;
			
		case KVAH:
		//case DKVAH:
			rem_disp_val=load_val[1]%100;
			rem_disp_val2=rem_disp_val%10;
			rem_disp_val=rem_disp_val/10;
			scratch=load_val[1]/100;// /100;
			i=0x08; j=0x47;
			break;

                        
                case BKWH:
			rem_disp_val=bkp_kwh_val%100;
			rem_disp_val2=rem_disp_val%10;
			rem_disp_val=rem_disp_val/10;
			scratch=bkp_kwh_val/100;// /100;
			i=0x0C; j=0x49;
			break;
		
                case BKVAH:
			rem_disp_val=bkp_kvah_val%100;
			rem_disp_val2=rem_disp_val%10;
			rem_disp_val=rem_disp_val/10;
			scratch=bkp_kvah_val/100;// /100;
			i=0x08;j=0x4F;
			break;
		        
		case KWMDONLY:
			scratch=kwmd_val;
    			i=0x0C; j=0x10;
			break;
			
    		case BKWMDONLY:
			scratch=bkp_kwmd_val;
    			i=0x0C; j=0x18;
			break;

                case CKWMDONLY:
			scratch=kwcd_val+kwmd_val;
    			i=0x0C; j=0x50;
			break;
		case BPF:
			scratch=bkp_pf_val;
                        i=0x20;j=0x08;			
		        break;
          			
	        case HKWH:
		case HKWMD:
	        case HPF:
                //case BKWH:
		case BKWMD:
		//case BKWMDONLY:
                case KWMD:
                          if(disp_index==KWMD)//||(disp_index==KWMDONLY))  
	                    	location=KWMD_LOC+(mnth_pos*19);
	                  else
			  {
			    	location=cyclic(mnth_pos+11-hist_ctr, HISTORY_SIZE); //12 month history
	                    	location=KWMD_LOC+(location*19);
	                  }
	                  switch(disp_sub_ctr)
	                  {
	                     case 0:
	                        //kwh;
		                scratch=from_eeprom(location+5,4);//0
			        rem_disp_val=scratch%100;
				rem_disp_val=rem_disp_val/10;
                        	scratch=scratch/100;
				i=0x0C;j=0x49;
				break;
	                     
			     case 1:
	                        //bill day;
				scratch=from_eeprom(location,3);
				scratch1=read_eeprom(location+3);
				scratch=(scratch1*10000)+(scratch%10000);
				i=0x80; j=0x80;
                                break;
	                     	
			     case 2:
	                        //kwmd;
				if(mains_stat==1)
					scratch=from_eeprom(location+9,2);//3
				else
					scratch=bkp_kwmd_val;
				//scratch=scratch*10;
				i=0x0C; j=0x10;
				break;
                                
	                     case 3:
	                        //date;
				if(mains_stat==1)
					scratch=from_eeprom(location+11,3);//5
				else
					scratch=bkp_kwmd_date;
                                tmp_md_date=scratch;
				i=0x80; j=0x90;
	                        break;
				
	                     case 4:
	                        //time;
				if(mains_stat==1)
					scratch=from_eeprom(location+14,2);//8
				else
					scratch=bkp_kwmd_time;					
	                        scratch=scratch*100;
	                        i=0x40;j=0x90;
	                        break;
				
        	             case 5:
	               		//pf
            			scratch=read_eeprom(location+18);
		            	i=0x20;j=0x08;			
		                break;
	                 } 
	                 break;
                         
                      case HKVAH:
                          /*
                          if((disp_index==KWMD)||(disp_index==KWMDONLY))  
	                    	location=KVAMD_LOC+(mnth_pos*11);
	                  else
			  {
                          */
			    	location=cyclic(mnth_pos+11-hist_ctr, HISTORY_SIZE); //12 month history
	                    	location=KVAMD_LOC+(location*11);
	                  //}
	                  switch(disp_sub_ctr)
	                  {
	                     case 0:
	                        //kvah;
		                scratch=from_eeprom(location,4);//0
			        rem_disp_val=scratch%100;
				rem_disp_val=rem_disp_val/10;
                        	scratch=scratch/100;
				i=0x08; j=0x4F;
				break;
                          }      
                          break;
            
            		case CUOPEN:
                		switch(disp_sub_ctr)
                		{
                 			case 1:
                  				scratch=cuopen_date;//from_eeprom(CUOPEN_LOC+1,3);
				  		i=0x80;j=0x80;				
				  		break;
						
                 			case 2:
                  				scratch=cuopen_time;//from_eeprom(CUOPEN_LOC+4,3);
				  		i=0x40;j=0x80;				
				  		break;
                		}
                		break;      			
                                
            		case VRMS:
				scratch=vrms_reg3;
				i=0x00; j=0x04;
				break;
				
			case IRMS:
				scratch=irms_reg3[channel];	///10;
				i=0x00; j=0x02;
				break;
			case IRMSN:
				scratch=irms_reg3[1];
				i=0x00; j=0x02;
				break;
				
			case INSTPF:
				scratch=pf_reg3/10;
            			i=0x00;j=0x00;			
            			break;
				
			case APF:
				scratch=avg_pf;
            			i=0x20;j=0x00;		
				break;
				
            		case FREQ:
            			scratch=freq_reg3;
            			i=0x00; j=0x00;
            			break;
                               
            		case INSTKW:
				scratch=kw_reg3/10;	///10;
            			i=0x0C; j=0x00; //i=0x04; 
				break;
				
			case HRESKWH:					
				scratch=dial_load[0];//1 watt
				//scratch=hkwh_val/10;
				i=0x0C; j=0x01;
				break;
				
			case RSTCOUNT:
                                scratch=md_reset_cnt;
                                break;
			//case COMM:
			//	scratch=comm_delay_ctr;
			//	break;	
			case TCNT:
				scratch=all_tamper_cnt;
                                j=0x40;
                                break;
                                
                        case POWERON:
                                scratch=Cum_Power_On_Dur/60;
                                i=0x40; j=0x41;
                                break;
                                
			case CODE:
				//disp_code();
				break;	
                                
                        case CUOPENONLY:
                              break;
                              
                        case LASTOCC:
                        case LASTREST:
                                //if(disp_index==MAGNET)
                                //        location=MAG_LST_TAMPER_OCC_LOC;
                                //else 
                                if(disp_index==LASTOCC)
                                        location=LSTTAMPER_OCC_LOC;
                                else
                                        location=LSTTAMPER_RES_LOC;
                                        
                                switch(disp_sub_ctr)
                                {
                                        case 1:
                                                scratch=from_eeprom(location+1,3);//cuopen_date;//from_eeprom(CUOPEN_LOC+1,3);
                                                i=0x80;j=0x80;				
				  		break;
                                        case 2:
                                                scratch=from_eeprom(location+4,3);//cuopen_time;//from_eeprom(CUOPEN_LOC+4,3);
                                                //scratch=scratch/100;
                                                i=0x40;j=0x80;				
				  		break;
                                }
                                break;
                            
                              
              
            /*	
			case IRMSN:
				scratch=irms2_reg3; //test_val2;//irms2_reg3;	///10;
			    break;		
                        case AVGPF:
				scratch=avg_pf/10;
                                break;
            */
    }
    
/*    
r=0x44 y=0xEA b=0xCE L=0x0E d=0xEC g=0xEB C=0x0F O=0xAF
-=0x40 P=0x67 E=0x4F n=0xC4 N=0xA7 h=0xC6 v=0xAE c=0x4C 
i=0xA0 t=0x4E S=0xCB F=0x47 A=0xE7 H=0xE6 o=0xCC
*/
    if(disp_index==CODE)
      disp_code();
    else if(disp_index==VER)
      disp_ver();
    else if((disp_index==CUOPENONLY)||((disp_index==CUOPEN)&&(disp_sub_ctr==0)))
    {
       disp_all_lcd(0x00);
       LCDMEM[11]=seg_7_lookup('C');
       LCDMEM[10]=seg_7_lookup('-');
       LCDMEM[8]=seg_7_lookup('O');
       LCDMEM[7]=seg_7_lookup('P');
       LCDMEM[6]=seg_7_lookup('E');
       LCDMEM[5]=seg_7_lookup('n');
    }
    else if((disp_index==TSTAT)||(((disp_index==LASTOCC)||(disp_index==LASTREST))&&(disp_sub_ctr==0)))
    {
            disp_all_lcd(0x00);
            if(disp_index==LASTOCC)
            {
                    i=last_tamper_occ_type;
                    LCDMEM[12]=seg_7_lookup('L');	
                    LCDMEM[13]=seg_7_lookup('o');
            }	
            else if(disp_index==LASTREST)
            {
                    i=last_tamper_res_type;
                    LCDMEM[12]=seg_7_lookup('L');	
                    LCDMEM[13]=seg_7_lookup('r');
            }
            else if(disp_index==TSTAT)
            {
                    i=disp_sub_ctr+1;
                    LCDMEM[12]=seg_7_lookup('p');	
                    LCDMEM[13]=seg_7_lookup('t');
            }
            
            switch(i)
            {
                    case 1://rev
                            LCDMEM[11]=seg_7_lookup('r');	
                            LCDMEM[10]=seg_7_lookup('E');
                            LCDMEM[8]=seg_7_lookup('V');
                            break;
                    case 2://earth
                            LCDMEM[11]=seg_7_lookup('E');	
                            LCDMEM[10]=seg_7_lookup('A');
                            LCDMEM[8]=seg_7_lookup('r');
                            LCDMEM[7]=seg_7_lookup('t');
                            LCDMEM[6]=seg_7_lookup('h');
                            break;
                    case 3://mag
                            LCDMEM[11]=seg_7_lookup('M');	
                            LCDMEM[10]=seg_7_lookup('/');	
                            LCDMEM[8]=seg_7_lookup('A');
                            LCDMEM[7]=seg_7_lookup('g');
                            break;
                    case 4://nd
                            LCDMEM[11]=seg_7_lookup('n');	
                            LCDMEM[10]=seg_7_lookup('d');
                            break;
                    case 5://nmiss
                            LCDMEM[11]=seg_7_lookup('n');	
                            LCDMEM[10]=seg_7_lookup('_');
                            LCDMEM[8]=seg_7_lookup('c');
                            LCDMEM[7]=seg_7_lookup('u');
                            LCDMEM[6]=seg_7_lookup('t');
                            break;
                    /*        
                    case 6://freq
                            lcdmem(7,0x6C);//F
                            lcdmem(6,0x0C);//r
                            break;
                    */
                    default:	
                            LCDMEM[11]=seg_7_lookup('-');	
                            LCDMEM[10]=seg_7_lookup('-');
                            LCDMEM[8]=seg_7_lookup('-');	
                            LCDMEM[7]=seg_7_lookup('-');
                            break;
            }
    }        
    
    /*
    else if(disp_index==RTCOK)
    {
            disp_all_lcd(0x00);
            LCDMEM[7]=0x06;//r
            LCDMEM[6]=0x07;//t
            LCDMEM[5]=0x86;//c
            LCDMEM[4]=0x82;//-			
            if(rtc_status==1)
            {
                    LCDMEM[3]=0x07;// P
                    LCDMEM[2]=0x30;
            }	
            else
            {
                    LCDMEM[3]=0x07;//F			
                    LCDMEM[2]=0x10;			
            }	
    }
    */  
    else if(disp_index==LCDCHK)
    	disp_all_lcd(0xff);
    else if(disp_index==BLANK)
    	disp_all_lcd(0x00);
    else if(disp_index!=NONE)
	disp_glass(0,i,j);
    
    return;
}

void disp_code(void)
{
   //unsigned int location;
   unsigned char i, j, tmp_byte;

   //load_val[0]=12345678;
   //kwh_bkp=23456789;
   //kwmd_val=1234;
   //kwmd_bkp=2345;
   			
   if(disp_sub_ctr==0)
   		scratch=(load_val[0]/100)%1000000;
   else if(disp_sub_ctr==1)
   		scratch=(bkp_kwh_val/100)%1000000;
   else if(disp_sub_ctr==2)
   { 	
   		scratch=kwmd_val/10;
   		scratch=(scratch*1000)+(bkp_kwmd_val/10);
   }
   else if(disp_sub_ctr==3)
   {
                /*
		scratch=(store_tamper_stat&0x1F);
		if(cuopen_stat==1)
			scratch=scratch+0x20;//0x80;
                */     
		scratch=0x00;
		if((rev_stat[0]==1)||(rev_stat[1]==1))
			scratch=scratch+0x01;
		if(eload_stat==1)
			scratch=scratch+0x02;
		if(mag_imax_stat==1)
			scratch=scratch+0x04;
		if(nmiss_stat==1)
			scratch=scratch+0x08;
		if(nd_stat==1)
			scratch=scratch+0x10;
		if(cuopen_stat==1)
			scratch=scratch+0x20;//0x80;

   }
   
   disp_all_lcd(0x00);		
   
   if(disp_sub_ctr==3)
   	j=2;
   else
   	j=6;
   		
   for(i=0;i<j;i++)
   {
      tmp_byte=scratch%10;
      scratch=(scratch-tmp_byte)/10;
 	  
      tmp_byte=seg_7_lookup(tmp_byte);
      if(i<4)
        LCDMEM[5+i]=tmp_byte;
      else
        LCDMEM[5+i+1]=tmp_byte;
      
   }

   LCDMEM[12]=seg_7_lookup(disp_sub_ctr+1); //8th digit
   LCDMEM[13]=seg_7_lookup('C');//7th digit
   
}

	  

void disp_glass(unsigned char index,unsigned char tag1, unsigned char tag2)
{
	unsigned int i,tmp_byte, disp_stat;
	unsigned char tmp_byte2,tmp_byte3;
	unsigned long int tmp_long;
	
	disp_stat=0;
	
  	blink_stat=(~blink_stat)&0x01;    
        tmp_long=scratch;

	scratch1=0;
	for (i=0; i<6; i++)                     
    	{
   	  	tmp_byte=(unsigned int)(scratch%10);
   	  	scratch=(scratch-tmp_byte); 	
		scratch=scratch/10;      	
      		scratch1=(scratch1<<4)|tmp_byte;	
    	}
	
	for (i=0; i<6; i++)                     // Display "6543210"
    	{
   	 	tmp_byte=(scratch1>>(i*4))&0xf;
   	  	if(tmp_byte!=0)
      			disp_stat=1;
      		tmp_byte=seg_7_lookup(tmp_byte);

                if((disp_index==BKVAH)||(disp_index==D_KWH)||(disp_index==TKWH))//(disp_index==HRESKWH)||
                {
                        if(i>=5)
                                disp_stat=1;
                }	
                else if((disp_index==TCNT)||(disp_index==RSTCOUNT)||(disp_index==POWERON))
                {	
                        if(i>=5)
                                disp_stat=1;
                }
                else if(((disp_index==CODE)&&(disp_sub_ctr==3)))
                {
                        if(i>=4)
                                disp_stat=1;
                }
                else if((disp_index==INSTPF)||(disp_index==HPF)||(disp_index==BPF)||(disp_index==APF)||(disp_index==IRMS)||(disp_index==IRMSN)||(disp_index==FREQ))
                {
                        if(i>=3)
                                disp_stat=1;
                }
                else if((((disp_index==HKWMD)||(disp_index==KWMD)||(disp_index==BKWMD)||(disp_index==KVAMD)||(disp_index==BKVAMD))&&(disp_sub_ctr==2))||(disp_index==CKWMDONLY)||(disp_index==KWMDONLY)||(disp_index==BKWMDONLY)||(disp_index==VRMS)||(disp_index==INSTKW)||(disp_index==INSTKVA))	  	  
                {	
                        if(i>=2)
                                disp_stat=1;
                }
                else if(((disp_index==CODE)&&(disp_sub_ctr<3))||(disp_index==SLNO)||(disp_index==KVAH)||(disp_index==KWH)||(disp_index==DKWH)||(disp_index==HKVAH)||(disp_index==HKWH)||(disp_index==BKWH)||(disp_index==HRESKWH)||(disp_index==HRESKVAH)||(disp_index==TSTATUS)||(disp_index==TIME)||(disp_index==DATE)||(((disp_index==CUOPEN)||(disp_index==LASTOCC)||(disp_index==LASTREST)||(disp_index==MAGNET))&&(disp_sub_ctr>=1))||(((disp_index==KWMD)||(disp_index==BKWMD)||(disp_index==KVAMD)||(disp_index==BKVAMD)||(disp_index==HKWMD))&&(disp_sub_ctr>2)))
                        disp_stat=1;
	
		if(disp_stat==0)
			tmp_byte=0;
                                
                /*
                if((disp_index==KWMD)||(disp_index==BKWMD)||(disp_index==HKWMD)||(disp_index==KVAMD)||(disp_index==BKVAMD)||(disp_index==HKVAMD))
		{
			if(((((disp_sub_ctr==3)&&(i>=0))||((disp_sub_ctr==4)&&(i>=0))))&&(tmp_md_date==0))
				tmp_byte=seg_7_lookup('-');

		}
                */
                
                if((disp_index==LASTOCC)&&((last_tamper_occ_type==0)||(last_tamper_occ_type>5)))
                  tmp_byte=seg_7_lookup('-');
                
                if((disp_index==LASTREST)&&((last_tamper_res_type==0)||(last_tamper_res_type>5)))
                  tmp_byte=seg_7_lookup('-');
          
                if(disp_index==FREQ)
		{
			if(i==0)
	  			tmp_byte=seg_7_lookup('F');
		}
                if(disp_index==TCNT)
		{
			if(i==0)
	  			tmp_byte=seg_7_lookup('t');
			else if(i==1)
	  			tmp_byte=seg_7_lookup('c');
	  	}
                if(((disp_index==HPF)||(disp_index==BPF)||(disp_index==APF)||(disp_index==INSTPF)))
                {  
			if(i==0)
	  			tmp_byte=seg_7_lookup('P');
			if(i==1)	
	  			tmp_byte=seg_7_lookup('F');
                        
                        if(disp_index==INSTPF)
                        {
                              if(((pf_reg3<900)&&(pf_reg3>200))&&(i==2))         
                              {
                                      if((pf_sign==0)&&(nmiss_stat==0)&&(nd_stat==0))
                                              tmp_byte=seg_7_lookup('-');
                              }
                        }

                        if(tmp_long==0)
                        {
                          if((i==3)||(i==4)||(i==5))
                            tmp_byte=seg_7_lookup('-');
                        }
                    
		}
                
                if(((rev_stat[0]==1)||(rev_stat[1]==1))&&(i==0))
                    tmp_byte=tmp_byte|0x80;
                if((eload_stat==1)&&(i==1))
                    tmp_byte=tmp_byte|0x80;
                if((disp_index==HRESKWH)&&(i==2))//||(disp_index==INSTKW))
		    tmp_byte=tmp_byte|0x80;
		if(((nmiss_stat==1)||((nd_stat==1)&&(blink_stat==1)))&&(i==3))
                    tmp_byte=tmp_byte|0x80;
                
                //||(((disp_index==CUOPEN)||(disp_index==LASTOCC)||(disp_index==LASTREST)||(disp_index==TSTATUS))&&(disp_sub_ctr==2))
		if(((disp_index==INSTKW)||(disp_index==INSTKVA)||(disp_index==IRMS)||(disp_index==IRMSN)||(disp_index==INSTPF)||(disp_index==APF)||(disp_index==BPF)||(disp_index==HPF)||(disp_index==CKWMDONLY)||(disp_index==KWMDONLY)||(disp_index==BKWMDONLY)||(((disp_index==KWMD)||(disp_index==BKWMD)||(disp_index==HKWMD)||(disp_index==KVAMD)||(disp_index==BKVAMD)||(disp_index==HKVAMD))&&(disp_sub_ctr==2)))&&(i==4)) //||(disp_index==INSTKW)
			tmp_byte=tmp_byte|0x80;//P2
			
		if(((disp_index==VRMS)||(disp_index==FREQ))&&(i==5))//||(disp_index==BKWH)||(disp_index==HKWH)||(disp_index==KWH)
			tmp_byte=tmp_byte|0x80;//T1
                
                if(i<2)
                  LCDMEM[11-i]=tmp_byte;
                else
                  LCDMEM[11-i-1]=tmp_byte;
                  
	}
        
        tmp_byte2=0;
        tmp_byte3=0;
	switch(disp_index)
	{
                case SLNO:
                    tmp_byte2=seg_7_lookup(rem_disp_val); //7th digit
                    tmp_byte3=seg_7_lookup('S');          //8th digit 
                    break;
		
                //case TCNT:
                //    tmp_byte2=seg_7_lookup('c'); //7th digit
                //    tmp_byte3=seg_7_lookup('t'); //8th digit 
                //    break;
		    
                /*
                case MAGNET:
			if(disp_sub_ctr==0)
			{
				tmp_byte2=0x71;
				tmp_byte3=0x59;
			}
			tag1=tag1|0x80;
			break;
                
                case IRMS:
                        tmp_byte3=0x3E;//P;
			break;
                        
                case IRMSN:
                        tmp_byte3=0x6E;//N;
			break;
                */
                  
		case RSTCOUNT:
			tmp_byte3=seg_7_lookup('r');
			break;
                        
		case POWERON:
			tmp_byte3=seg_7_lookup('P');
			break;

		case LASTOCC:
			tmp_byte3=seg_7_lookup('L');
			tmp_byte2=seg_7_lookup('o');
			break;
                        
		case LASTREST:
			tmp_byte3=seg_7_lookup('L');
			tmp_byte2=seg_7_lookup('r');
			break;
                  
        
                case INSTPF:
			if(pf_reg3>990)
                          tmp_byte3=0;//seg_7_lookup('U');
                        else
                        {
                          if(pf_reg3>0)
                          {  
                            if(pf_sign==1)
                                  tmp_byte3=seg_7_lookup('L');
                            else if(pf_sign==0)
                                  tmp_byte3=seg_7_lookup('C');
                          }
			}
                        break;
                        
                /*        
                case FREQ:
                    tmp_byte3=0x6C;//F
		    break;
		case IRMS:
                    if(channel==0)
                            tmp_byte3=0x7C;//phase
                    else
                            tmp_byte3=0x76;//neutral
                    break;
                */
                        
		case BKWH:	
		case HKWH:
                case BKVAH:	
		case HKVAH:
                    tmp_byte3=seg_7_lookup(hist_ctr+1);
                    //tmp_byte2=lookup[rem_disp_val];
                    break;
                  
		case BKWMD:
		case BKWMDONLY:
		case HKWMD:
		case BPF:
		case HPF:
                case BKVAMD:
		//case BKVAMDONLY:
		case HKVAMD:
		    tmp_byte3=seg_7_lookup(hist_ctr+1);
                    break;
                    
		case HRESKWH:
                    tmp_byte3=seg_7_lookup('H');
                    break;
                        
                /*    
                case KWH:
                    tmp_byte2=lookup[rem_disp_val];
                    
                    //tmp_byte=load_val[0]%100;
                    //tmp_byte2=lookup[tmp_byte/10];
                    //tmp_byte2=tmp_byte2|0x80;
                    break;
                
                case KVAH:
                    tmp_byte2=lookup[rem_disp_val];
                    
                    //tmp_byte=load_val[0]%100;
                    //tmp_byte2=lookup[tmp_byte/10];
                    //tmp_byte2=tmp_byte2|0x80;
                    break;
                */    
                    
		default:break;
	}
	        
        
        
        if(comm_stat==1)
        {
            if(blink_stat==1)
                tmp_byte3=tmp_byte3|0x80;//T2
            comm_stat=0;                           
        }    
        else
        {
          if(((mains_stat==1)&&(nmiss_stat==0))||(batt_mode_comm_en_stat==1))
             tmp_byte3=tmp_byte3|0x80;//T2
        }
          
        /*  
        if((mains_stat==1)&&(nmiss_stat==0))
	{
		if((nd_stat==1)&&(vrms_reg3<2000))	
		{
			if(blink_stat==1)
				tmp_byte3=tmp_byte3|0x80;//T2
		}
		else
		{
			tmp_byte3=tmp_byte3|0x80;//T2
		}  			
	}
        */
              
        if((mag_imax_stat==1)&&(mains_stat==1)) //((disp_index==TSTATUS)&&(last_tamper_type==6))||
	    tag1=tag1|0x10;
        

/*
    P3,T2,T3,T4,P1,T5,P2,T1
    .D,ON,< ,EL,4D,NM,2D,1D

tag1=T6,T7,T8,T9,T20,T14,T18,T19
      DATE,TIME,AV,MAG,K,W,TOD,|

tag2=P4,T10,T11,T12,T13,T15,T16,T17
      ::,CUM, CO, MD, BP,  V, A,  h 
*/        
	if((disp_index==BKWH)||(disp_index==BKWMD)||(disp_index==BKWMDONLY)||(disp_index==HKWH)||(disp_index==HKVAH)||(disp_index==HKWMD)||(disp_index==HKVAMD)||(disp_index==HPF))
		tag2=tag2|0x08;//BP

        if((disp_index==CUOPEN)||((cuopen_stat==1)&&(blink_stat==1)))
		tag2=tag2|0x20;
	
        //if((disp_index==KWH)||(disp_index==KVAH)||((disp_index==HKWH)&&(disp_sub_ctr==0))||(disp_index==HKVAH)||(disp_index==BKWH))
	//	tmp_byte2=tmp_byte2|0x01;//80;//P3

	LCDMEM[12]=tmp_byte3; //8th digit
        LCDMEM[13]=tmp_byte2; //7th digit
	LCDMEM[4]=tag1;
        LCDMEM[3]=tag2;
        
        return;	
}


unsigned char seg_7_lookup( unsigned char val)
{
    unsigned char seg_val;
    
        switch(val)
	{
            
            case 'O':
            case '0':
            case 0:
              seg_val= seg_7_map("ABCDEF",6);
              break;
            case 1:
            case '1':
              seg_val= seg_7_map("BC",2);
              break;
            case 2:
            case '2':
              seg_val= seg_7_map("ABGED",5);
              break;
            case 3:
            case '3':
              seg_val= seg_7_map("ABCDG",5);
              break;
            case 4:
            case '4':
              seg_val= seg_7_map("FGBC",4);
              break;
            case 5:
            case '5':
              seg_val= seg_7_map("AFGCD",5);
              break;
            case 6:
            case '6':
              seg_val= seg_7_map("AFEDCG",6);
              break;
            case 7:
            case '7':
              seg_val= seg_7_map("ABC",3);
              break;
            case 8:
            case '8':
              seg_val= seg_7_map("ABCDEFG",7);
              break;
            case 9:
            case '9':
              seg_val= seg_7_map("ABCDFG",6);
              break;
            case 'A':
              seg_val= seg_7_map("EFABCG",6);
              break;
            case 'B':
              //seg_val= seg_7_map("AFGHCD",6);
              break;
            case 'b':
              seg_val= seg_7_map("EFGCD",5);
              break;
            case 'C':
              seg_val= seg_7_map("AFED",4);
              break;
            case 'c':
              seg_val= seg_7_map("GED",3);
              break;
            case 'D':
              //seg_val= seg_7_map("AFGHCD",6);
              break;
            case 'd':
              seg_val= seg_7_map("BCDEG",5);
              break;
            case 'E':
              seg_val= seg_7_map("AFEDG",5);
              break;
            case 'F':
              seg_val= seg_7_map("EFAG",4);
              break;
            case 'G':
              //seg_val= seg_7_map("AFEDCH",6);
              break;
            case 'g':
              seg_val= seg_7_map("FABGCD",6);
              break;
            case 'H':
              seg_val= seg_7_map("EFBCG",5);
              break;
            case 'h':
              seg_val= seg_7_map("EFGC",4);
              break;
            case 'I':
              seg_val= seg_7_map("BC",2);
              break;
            case 'i':
              seg_val= seg_7_map("C",1);
              break;
            case 'J':
              //seg_val= seg_7_map("AFGHCD",6);
              break;
            case 'K':
            case 'k':
              seg_val= seg_7_map("EFG",3);
              break;
            case 'L':
              seg_val= seg_7_map("EFD",3);
              break;
            case 'M':
              seg_val= seg_7_map("EFAB",4);
              break;
            case '/':
              seg_val= seg_7_map("FABC",4);
              break;
            case 'N':
              seg_val= seg_7_map("EFABC",5);
              break;
              //seg_val= seg_7_map("EFINBC",6);
              break;
            case 'n':
              seg_val= seg_7_map("EGC",3);
              break;
            case 'o':
              seg_val= seg_7_map("EGCD",4);
              break;
            case 'P':
            case 'p':
              seg_val= seg_7_map("EFABG",5);
              break;
            case 'Q':
              //seg_val= seg_7_map("ABCDEFN",7);
              break;
            case 'R':
            case 'r':
              seg_val= seg_7_map("EG",2);
              break;
            case 'S':
              seg_val= seg_7_map("AFGCD",5); //AFGHCD
              break;
            case 'T':
              //seg_val= seg_7_map("AJM",3);
              break;
            case 't':
              seg_val= seg_7_map("EFDG",4);
              break;
            case 'U':
              seg_val= seg_7_map("BCDEF",5);
              break;
            case 'u':
              seg_val= seg_7_map("ECD",3);
              break;
            case 'V':
              seg_val= seg_7_map("BCDEF",5);
              break;
            case 'W':
              //seg_val= seg_7_map("EFLNBC",6);
              break;
            case 'X':
              //seg_val= seg_7_map("IKLN",4);
              break;
            case 'Y':
            case 'y':
              seg_val= seg_7_map("FGBCD",5);
              break;
            case 'Z':
              //seg_val= seg_7_map("AKLD",4);
              break;
            case '-':
              seg_val= seg_7_map("G",1);
              break;
            case '_':
              seg_val= seg_7_map("D",1);
              break;
            default:
              seg_val=0;   
        }
        
        return seg_val;
}

unsigned char seg_7_map( char *str_val, unsigned char size)
{
    unsigned char i;
    unsigned char tot_map_val, map_val;
    
    tot_map_val=0;
    //xAFB GECD
    for(i=0;i<size;i++)
    {
      switch(str_val[i])
      {
          case 'A':
            map_val=0x40;
            break;
          case 'B':
            map_val=0x10;
            break;
          case 'C':
            map_val=0x02;
            break;
          case 'D':
            map_val=0x01;
            break;
          case 'E':
            map_val=0x04;
            break;
          case 'F':
            map_val=0x20;
            break;
          case 'G':
            map_val=0x08;
            break;
          default:
            map_val=0;   
      }
      tot_map_val=tot_map_val+map_val;
    }      
    return tot_map_val;
}

void eload_check(void)
{
	//eload detect
	if(irms_reg2[0]>irms_reg2[1])
	{
		scratch=irms_reg2[0];
		scratch1=irms_reg2[1];
	}
	else
	{
		scratch=irms_reg2[1];
		scratch1=irms_reg2[0];
	}

	//if(((irms_reg2[0] > 150)||(irms_reg2[1] > 150))&&(nmiss_stat==0)&&(nd_stat==0)&&(magnetic_stat==0)&&(mag_imax_stat==0)) //||(nmiss_stat==1)) //||(vrms_reg2<600))	//current .5% , voltage 2.5% threshold
	if(((irms_reg2[0] > 150)||(irms_reg2[1] > 150))&&(nmiss_stat==0)&&(nd_stat==0)) //&&(magnetic_stat==0)&&(mag_imax_stat==0)  //||(nmiss_stat==1)) //||(vrms_reg2<600))	//current .5% , voltage 2.5% threshold
	{
		if(scratch>((scratch1*106)/100))
				eload_stat=1;	
		else if(scratch<((scratch1*105)/100))
				eload_stat=0;			
	}
	else
		eload_stat=2;
		
	//if(((irms2_reg2 < 100)&&(irms1_reg2 < 100))||(vrms_reg2<200))	//current .5% , voltage 2.5% threshold
	//	eload_stat=0;
	
	return;
}


#pragma vector=SD24B_VECTOR                    // SD24 Interrupt....
__interrupt void adc_interrupt(void)
{
	unsigned char i;//sign, tmp_adc_data_index;	
	//int tmp_int;
	long int tmp_long;
	
        //cbi(P1OUT,PULSE1_PIN);

	switch (SD24BIV)
	{
		case 0x02:                                   // SD24BIV_SD24OVIFG
			break;
		case 0x04:                                   // SD24BIV_SD24TRGIFG         
			break;                               
		case 0x06:	                             // SD24BIV_SD24IFG0           
			break;
		case 0x08:                                   // SD24BIV_SD24IFG1	
			break;
		case 0x0A:                                   // SD24BIV_SD24IFG2	
			tmp_long=SD24BMEMH0;
			tmp_long=(tmp_long<<16)+SD24BMEML0;
                        //tmp_int=(tmp_long>>8)&0xffff;
                        irms_adc[1]=(tmp_long>>11);//(long int)tmp_int;
			
                        tmp_long=SD24BMEMH1;
			tmp_long=(tmp_long<<16)+SD24BMEML1;
			//tmp_int=(tmp_long>>8)&0xffff;
                        irms_adc[0]=(tmp_long>>11);//(long int)tmp_int;
			
                        tmp_long=SD24BMEMH2;
			tmp_long=(tmp_long<<16)+SD24BMEML2;
			//tmp_int=(tmp_long>>8)&0xffff;
                        vrms_adc=(tmp_long>>12);//(long int)tmp_int;			
                        
                        adc_delay_ctr=0;
                        if(metering_data_ready==1)
			{    
                                
                                u10_delay_ctr=u10_delay_ctr+23; //230 us   //270 us
			
                                
                                vrms_cumm_offset_adc=vrms_cumm_offset_adc+vrms_adc;			    
				vrms_adc=vrms_adc-vrms_offset_adc;

                                for(i=0;i<2;i++)
                                {
				    irms_adc[i]=((irms_adc[i]*igain_val[i])>>11);//13		
				    irms_cumm_offset_adc[i]=irms_cumm_offset_adc[i]+irms_adc[i];
				    irms_adc[i]=irms_adc[i]-irms_offset_adc[i];
                                }
                                
                                if(vrms_adc>=0)
                                {
                                        vrms_reg1=vrms_reg1+vrms_adc;
                                        vrms_adc_zero_cross=0;
                                }
                                else
                                {
                                        vrms_reg1=vrms_reg1-vrms_adc;
                                        if(vrms_adc_zero_cross==0)
                                        {		
                                                vrms_adc_zero_cross=1;
                                                
                                                if(irms_adc[channel]>0) pf_sign=1; else pf_sign=0;
                                                if(rev_stat[channel]==1)pf_sign=(~pf_sign)&0x01;
                                                
                                                freq_reg2=freq_reg2+freq_reg1;
                                                vrms_adc_zero_cross_ctr++;
                                                freq_reg1=0;
                                        }
                                }
                                freq_reg1++;
                                
                                vrms_adc=vrms_adc>>2; //3;//2;	
                                
                                for(i=0;i<2;i++)
                                {
				    if(irms_adc[i]>=0)
                                    {
                                      irms_reg1[i]=irms_reg1[i]+irms_adc[i];
                                      irms_adc_zero_cross[i]=0;
                                    }
                                    else
                                    {
                                      irms_reg1[i]=irms_reg1[i]-irms_adc[i];
                                      if(irms_adc_zero_cross[i]==0)
                                      {
                                        irms_adc_zero_cross[i]=1;
                                      }  
                                    }    
                                    
                                    tmp_long=(irms_adc[i] - prev_irms_adc[i]);
                                    tmp_long =((tmp_long*iph_val[i])>>10);///1000);
                                    prev_irms_adc[i]=irms_adc[i];
                                    irms_adc[i]=irms_adc[i]-tmp_long;				    
                                    
                                    if((nmiss_stat==0)&&(nd_stat==0))
                                    {
                                            tmp_long=vrms_adc;
                                            tmp_long=tmp_long*irms_adc[i];
                                    }
                                    else
                                    {
                                            tmp_long=irms_adc[i]*n_cfden_val[i];
                                            if(tmp_long<0)
                                              tmp_long=((~tmp_long)+1)&0x7fffffff;
                                    }

                                    tmp_long=tmp_long>>4;
                            
                                    kw_reg1[i]=kw_reg1[i]+tmp_long; //(tmp_long>>2);  //(tmp_long>>10);
                                      
                                    if(kw_reg2[i] < 500)//500->10mA 250->5mA
                                        tmp_long=0;	
                                
                                    if((mag_pulse_stat==1)||(mag_imax_stat==1))
                                    {
                                      if(i==channel)
                                      {
                                        if(mag_imax_stat==1)
                                            pulse_reg0=pulse_reg0+2816000; //2800355;//Anil        //2814428;//2955150;//2970000;//1968000;
                                        else
                                            pulse_reg0=pulse_reg0+mag_pulse_val;//2970000;//23770000;
                                      }
                                    }
                                    else 
                                    {
                                      if(nmiss_stat==0)//&&(nd_stat==0))
                                      {
                                        if(i==channel)
                                          pulse_reg0=pulse_reg0+tmp_long;
                                      }
                                      else
                                      {
                                          pulse_reg0=pulse_reg0+tmp_long;
                                      }
                                    }  
                                }

                                if((P1IN & 0x04) == 0x00)                                    
                                        mag_on_ctr++;
                                else
                                        mag_off_ctr++;
                                
                                //if(adc_data_ctr<50)
                                //    adc_data[adc_data_ctr++]=irms_adc[1];
                               
				pulse_reg1=pulse_reg_rmndr;
                                if(pulse_reg0<0)
                                        pulse_reg1=pulse_reg1+((~pulse_reg0)+1);
                                else
                                        pulse_reg1=pulse_reg1+pulse_reg0;
                        
				if((irms_reg3[0]<7200)&&(irms_reg3[1]<7200))                        
				{
                                  if(led_off==1)
				  {
					if(pulse_reg1>pulse_fact)
					{
                                            sbi(P1OUT,PULSE1_PIN);
                                            load_ctr[0]++;
                                            pulse_reg_rmndr=pulse_reg1-pulse_fact;
                                            pulse_reg0=0;
                                            pulse_on_ctr=0;
                                            led_off=0;	
					}
				  }				
				  else
				  {
                                      if(pulse_on_ctr>=40)  //20 ms
                                      {
                                          cbi(P1OUT,PULSE1_PIN);
                                          led_off=1;
                                      }
                                      else
                                          pulse_on_ctr++;
				  }

                                  //if((magnetic_stat==1)||(mag_imax_stat==1))
                                  if((mag_imax_stat==1)||(pulse_va==0))
                                    pulse_va_reg1=pulse_reg1;
                                  else
                                    pulse_va_reg1=pulse_va_reg1+pulse_va;
                                  
                                  if(led_va_off==1)
                                  {
                                      if(pulse_va_reg1>pulse_fact)
                                      {
                                          load_ctr[1]++;
                                          pulse_va_reg1=pulse_va_reg1-pulse_fact;
                                          
                                          if(kva_pulse_type==0)
                                          {  
                                            //cbi(P2OUT,PULSE2_PIN);
                                            pulse_va_on_ctr=0;
                                            led_va_off=0;	
                                          }  
                                      }
                                  }				
                                  else
                                  {
                                      if(pulse_va_on_ctr>=40)  //20 ms
                                      {
                                          //sbi(P2OUT,PULSE2_PIN);
                                          led_va_off=1;
                                      }
                                      else
                                          pulse_va_on_ctr++;
                                  }
                                }
                                else
                                {
                                  if(led_off==0)
                                  {
                                    pulse_reg1=0;
                                    pulse_reg0=0;
                                    pulse_va_reg1=0;
                                    pulse_on_ctr=0;
                                    cbi(P1OUT,PULSE1_PIN);
                                    led_off=1;
                                  }  
                                }
                                
                                
			 	sample++;		

                                /*
				if((relay_delay_ctr[0]>100))		
				{
					cbi(P4OUT,R_OFF);
					cbi(P4OUT,R_ON);
				}
				else
					relay_delay_ctr[0]++;
			
				if((relay_delay_ctr[1]>100))		
				{
					cbi(P4OUT,Y_OFF);
					cbi(P4OUT,Y_ON);
				}
				else
					relay_delay_ctr[1]++;
			
				if((relay_delay_ctr[2]>100))		
				{
					cbi(P5OUT,B_OFF);
					cbi(P5OUT,B_ON);
				}
				else
					relay_delay_ctr[2]++;
                                */
			}
			break;
	}
        //sbi(P1OUT,PULSE1_PIN);
        
	return ;
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
      unsigned char i;//, j;
      //unsigned long int tmp_long, tmp_long1;
	
      //cbi(P1OUT,PULSE1_PIN);
      
      /*
      if(demo_led_off==1)
      {
        demo_led_off=0;
        cbi(P1OUT,PULSE1_PIN);
      }
      else
      {
        sbi(P1OUT,PULSE1_PIN);
        demo_led_off=1;
      }
      */
      
      mag_on_ctr1=mag_on_ctr;
      mag_on_ctr=0;	
      
      mag_off_ctr1=mag_off_ctr;
      mag_off_ctr=0;	
	
      nmi_err_ctr=0;
      
      if(co_disable_ctr<10)//300)//600
          co_disable_ctr++;

      if(mains_stat==1)
      {
          if(mains_pulse_ctr==0)
            mains_stat=0;
      }
      else
      {
          if(mains_pulse_ctr>20)
            mains_stat=1;
      }
      
      mains_pulse_ctr=mains_pulse_ctr1;
        
      //if((P1IN&0x02)==0x00)
      if(mains_stat==0)
      {  
          //mains_stat=0;
          AUX3CHCTL = AUXCHKEY;// | AUXCHC_3 | AUXCHV_1 | AUXCHEN;  // Enable Charger for AUX3 to enable RTC
          
          if(init_stat==0)
          {
                  if(comm_delay_ctr>=20)
                  {
                          if((P1IN & 0x10)==0x00) //pushb   
                          {
                                  scrl_b++;
                                  if(scrl_b>=3)
                                  {
                                          cbi(P1IE,PUSHDN_B);
                                          
                                          _BIC_SR_IRQ(LPM3_bits);
                  
                                          sbi(P2DIR,I2C_DATA);
                                          sbi(P2DIR,EEPROM_CLOCK);
                                          sbi(P6DIR,COMM_CTRL_PIN);cbi(P6OUT,COMM_CTRL_PIN);
                                          
                                          //RX_OPT P2.2, TX_OPT P2.3, RX_EXT P2.4, TX_EXT P2.5
        
                                          // Setup P1.2 UCA0RXD, P1.3 UCA0TXD
                                          P2SEL |= BIT2 | BIT3| BIT4 | BIT5;                   // Set P3.3, P3.4 to non-IO
                                          //P2DIR |= BIT2 | BIT3| BIT4 | BIT5;                   //output
                                          P2DIR |= BIT3| BIT5;                   //output

                                          init_dlms();
                                          
                                          /*
                                          for(i=0;i<10;i++)
                                          LCDMEM[i]=0x00;                                    
                                          
                                          LCDMEM[1]=0xEC;//d
                                          LCDMEM[2]=0x60;//l
                                          LCDMEM[3]=0xCC;//o
                                          LCDMEM[4]=0xEC;//d
                                          */
                                          
                                          WDTCTL=WDT_ARST_1000;  
                                          recv_ctr=0;
                                          comm_stat=0;
                                          enable_recv_buf=1;
                                          batt_disp_stat=0;
                                          comm_delay_ctr=0;
                                          batt_push_ctr=10;
                                          scrl_b=0;
                                          batt_mode_comm_en_stat=1;
                                  }
                          }
                                  
                          if(batt_disp_stat==1)
                          {
                                  if(batt_push_ctr==0)
                                  {
                                          read_time_date();  
			                  select_disp_index();
                                          switch_index();
                                  }
                                  else
                                  {
                                          batt_disp_stat=0;
                                          WDTCTL = WDTPW+WDTSSEL__ACLK+WDTHOLD;                 // Stop watchdog timer                          
                                          LCDCVCTL &= (~LCDCPEN);
                                          LCDCMEMCTL = LCDCLRM;
                                          LCDCCTL0 &= (~LCDON);
                                          //rtc values can not be  read after SVS is disabled.
                                          PMMCTL0_H=PMMPW_H;   
                                          SVSMHCTL &= ~(SVMHE | SVSHE);
                                          SVSMLCTL &= ~(SVMLE | SVSLE);
                                          PMMCTL0_H=0;   
                                  }	
                          }
                  }	
          }

          /*
          if(init_stat==0)
          {
                if((P2IN & 0x02)==0x00)
                {
                    select_disp_index();
                    switch_index();
                    WDTCTL = WDTPW+WDTSSEL__ACLK+WDTHOLD;                 // Stop watchdog timer                          
                    button_release_ctr=0;
                }
                else
                {
                    if(button_release_ctr>=9)
                    {  
                      LCDCCTL0=0;				
                      button_release_ctr=0;
                    }
                    else
                      button_release_ctr++;         
                }    
          }
          */
      }
      else
      {
          AUX3CHCTL = AUXCHKEY | AUXCHC_3 | AUXCHV_1 | AUXCHEN;  // Enable Charger for AUX3 to enable RTC
          //mains_stat=1;
          batt_push_ctr=10;

          if(init_stat==0)
          {
                  _BIC_SR_IRQ(LPM3_bits);
                  
                  init_io_param();

                  //RX_OPT P2.2, TX_OPT P2.3, RX_EXT P2.4, TX_EXT P2.5
                  
                  // Setup P1.2 UCA0RXD, P1.3 UCA0TXD
                  P2SEL |= BIT2 | BIT3| BIT4 | BIT5;                   // Set P3.3, P3.4 to non-IO
                  //P2DIR |= BIT2 | BIT3| BIT4 | BIT5;                   //output
                  P2DIR |= BIT3| BIT5;                   //output

                  init_dlms();
                  disp_mode_stat=1;
                  
                  disp_index=VER;//LCDCHK; //NONE;		    
                  //disp_ver();
                    
                  disp_sub_ctr=0;
                  disp_ctr=8;
                  disp_cnt=10;	
                  //repeat_data_stat=0;
                  //bkp_disp_index=0;
  
                  /*
                  SD16CTL = SD16REFON + SD16SSEL_1 + SD16DIV_3;//+SD16LP;          // 1.2V ref, SMCLK , SMCLK = 1.04 MHz
                  //SD16CTL = SD16REFON + SD16SSEL_2 + SD16DIV_0;    
                      
                  SD16CCTL0 = SD16OSR_1024 | SD16XOSR | SD16DF | SD16GRP;// | SD16IE;//SD16LSBTOG |  SD16DF| SD16GRP // 256 Over sampling ratio,data format,
                  SD16CCTL1 = SD16OSR_1024 | SD16XOSR | SD16DF | SD16GRP;// | SD16IE;//SD16LSBTOG | SD16DF| SD16GRP // 256 Over sampling ratio,data format,
                  SD16CCTL2 = SD16OSR_1024 | SD16XOSR | SD16DF | SD16GRP;	//| SD16GRP;// | SD16IE;//SD16LSBTOG |  SD16DF| SD16GRP // 256 Over sampling ratio,data format,
                  SD16CCTL3 = SD16OSR_1024 | SD16XOSR | SD16DF | SD16GRP;// | SD16IE;//SD16LSBTOG | SD16DF| SD16GRP // 256 Over sampling ratio,data format,
                  SD16CCTL4 = SD16OSR_1024 | SD16XOSR | SD16DF | SD16GRP;// | SD16IE;//SD16LSBTOG |  SD16DF| SD16GRP // 256 Over sampling ratio,data format,
                  SD16CCTL5 = SD16OSR_1024 | SD16XOSR | SD16DF | SD16GRP;// | SD16IE;//SD16LSBTOG | SD16DF| SD16GRP // 256 Over sampling ratio,data format,
                  SD16CCTL6 = SD16OSR_1024 | SD16XOSR | SD16DF | SD16IE;// | SD16IE;//SD16LSBTOG | SD16DF| SD16GRP // 256 Over sampling ratio,data format,
                  
                  SD16INCTL0 = SD16INCH_0 | SD16GAIN_1;//VR   	
                  SD16INCTL1 = SD16INCH_0 | SD16GAIN_1;//VY   	
                  SD16INCTL2 = SD16INCH_0 | SD16GAIN_1;//VB   	
                  SD16INCTL3 = SD16INCH_0 | SD16GAIN_1;//IR   	
                  SD16INCTL4 = SD16INCH_0 | SD16GAIN_1;//IY   	
                  SD16INCTL5 = SD16INCH_0 | SD16GAIN_1;//IB   	
                  SD16INCTL6 = SD16INCH_0 | SD16GAIN_1;//IN   	
                  SD16CCTL6 |= SD16SC;                 // Set bit to start conversion
                  */
            
                  // Configure ADC
                  SD24_Init();    
                  //init_usart();
                  metering_data_ready=1;            
          }
          
          if(adc_delay_ctr>3)
          {
            init_stat=0;
            adc_delay_ctr=0;
          }
          else
            adc_delay_ctr++;
          
          
          //channel switching
          if(channel==0) 
          {
                  //if(irms2_reg2>((irms1_reg2*115)/100))  // > 10%	
                  if(irms_reg2[1]>irms_reg2[0])  // > 10%	
                          channel=1;	//neutral
          }
          else
          {
                  //if(irms1_reg2>((irms2_reg2*115)/100))  // > 10%
                  if(irms_reg2[0]>irms_reg2[1])  // > 10%
                          channel=0;   //mains	
          }
          
          //if((irms1_reg2<30)&&(irms2_reg2<30))
          //	channel=0;	
      }		
	
      if((sample>0)&&(mains_stat==1))
      {	
          vrms_offset_adc=(vrms_cumm_offset_adc/sample);
          vrms_reg2=vrms_reg1/sample;

          for(i=0;i<2;i++)
          {
                  irms_offset_adc[i]=(irms_cumm_offset_adc[i]/sample);
                  irms_reg2[i]=irms_reg1[i]/sample;
                  
                  //if(vrms_adc_zero_cross_ctr>0)
                  //    kw_reg2[i]=kw_reg1[i]/vrms_adc_zero_cross_ctr;
                  //else
                  //    kw_reg2[i]=0;
                  
                  
                  kw_reg2[i]=(long int)(kw_reg1[i]/sample); //sample//80000
		
                  if(kw_reg2[i]<0)
                  {
                          kw_reg2[i]=(~kw_reg2[i])+1;
                          rev_stat[i]=1;
                  }
                  else
                          rev_stat[i]=0;
          
                  if((irms_reg2[i]<150)||(vrms_reg2<900)||(nd_stat==1)||(magnetic_stat==1))
                          rev_stat[i]=2;
                  
                  irms_cumm_offset_adc[i]=0;
                  irms_reg1[i]=0;
                  kw_reg1[i]=0;
                  
          }
          
          if((vrms_adc_zero_cross_ctr>0)&&(vrms_reg2>500))
          {
                  freq_reg2=(freq_reg2*10)/vrms_adc_zero_cross_ctr;
                  freq_reg3=204500/freq_reg2;//204500 //102000
          }
          else
                  freq_reg3=500;//0

          vrms_cumm_offset_adc=0;
          vrms_reg1=0;
          freq_reg2=0;
          vrms_adc_zero_cross_ctr=0;
  
          bkp_sample=sample;
          sample=0;
          //adc_data_ctr=0;
      }		
      
      mains_pulse_ctr=0;
      
      if(comm_delay_ctr<20)
              comm_delay_ctr++;
      delay_sec=1;
      rd_time++;
      
      //sbi(P1OUT,PULSE1_PIN);
      
      return;
}

/*
#pragma vector=NMI_VECTOR
__interrupt void nmi_ (void)
{
 //_BIC_SR_IRQ(LPM3_bits);               // exit LPM3
  IFG1=0;
  WDTCTL=WDTPW+WDTSSEL__ACLK+WDTCNTCL;
  nmi_err_ctr++;
  //if(nmi_err_ctr>10)
    while(1);
}
*/

/*
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR (void)
{	
	if((enable_recv_buf==1)&&(recv_ctr<11))
	{
			recv_buf[recv_ctr]=UCA0RXBUF;
			recv_ctr++;
	}
	UCA0STAT = 0x00;//UCLISTEN;
	IFG2=0x00;
}
*/


/*
#pragma vector=PORT1_VECTOR
__interrupt void port1_interrupt_ISR(void)
{
	//SVSCTL=SVSCTL&0xFE;
	if(((P1IFG & 0x02)==0x02)||(((P1IFG & 0x01)==0x01)&&(cuopen_stat!=1))) //pushb
	{		
              if((mains_stat==0)&&(batt_push_ctr==10))
              {
                  cbi(P1IE,PUSHDN_B);
                  
                  _BIC_SR_IRQ(LPM3_bits);
                  WDTCTL=WDT_ARST_1000;//WDTPW+WDTSSEL__ACLK+WDTCNTCL;
                  LCDCCTL0 = LCDDIV0 + LCDPRE1 + LCDSON;
                  LCDCVCTL = LCDCPEN + VLCD_2_96; //Internal voltage 3.26V
                  LCDCMEMCTL = LCDCLRM;
                  LCDCCTL0 |= LCD4MUX + LCDON;
                  
                  LCDCPCTL0 = 0xFFFF;
                  LCDCPCTL1 = 0x00FF;
                  LCDCPCTL2 = 0x0000;
        
                  disp_index=KWH; //LCDCHK;
                  
                  sbi(P2DIR,I2C_DATA);
                  sbi(P2DIR,EEPROM_CLOCK);
                  sbi(P3DIR,COMM_CTRL_PIN);sbi(P3OUT,COMM_CTRL_PIN);
                  
                  //TX_EXT P3.3, RX_EXT P3.4, TX_OPT P3.5, RX_OPT P3.6
        
                  // Setup P1.2 UCA0RXD, P1.3 UCA0TXD
                  P3SEL |= BIT3 | BIT4| BIT5 | BIT6;                   // Set P3.3, P3.4 to non-IO
                  P3DIR |= BIT3 | BIT4| BIT5 | BIT6;                   //output
        
                  init_dlms();
                  
                  batt_push_ctr=0;//10;//0;
                  disp_mode_stat=1;
                  comm_delay_ctr=20;
                  //repeat_data_stat=0;
                  disp_sub_ctr=0;
                  disp_ctr=0;//1;
                  disp_cnt=10;//15;
                  hist_ctr=0;
              }	    
	}
  	P1IFG=0;	
	return;
}
*/

#pragma vector=PORT1_VECTOR
__interrupt void port1_interrupt_ISR(void)
{
	//SVSCTL=SVSCTL&0xFE;
        if((P1IFG & 0x02)==0x02)
        {
          mains_pulse_ctr++;
        }   
        
	if((((P1IFG & 0x10)==0x10)&&(batt_disp_stat==0)&&(batt_push_ctr==10))||(((P1IFG & 0x01)==0x01)&&(cuopen_stat!=1))) //pushb
	{		
              if((mains_stat==0))
              {                  
                  //enable SVS and SVM
                  PMMCTL0_H=PMMPW_H;   
                  SVSMHCTL |= SVMHE | SVSHE;
                  SVSMLCTL |= SVMLE | SVSLE;
                  PMMCTL0_H=0;   
    
                  //WDTCTL=WDT_ARST_1000;//WDTPW+WDTSSEL__ACLK+WDTCNTCL;
                  
                  LCDCVCTL |= LCDCPEN;
                  LCDCCTL0 |= LCDON;
                  
                  if((P1IFG & 0x10)==0x10)
                  {  
                      batt_push_ctr=0;//10;//0;
                      batt_disp_stat=1;
                      comm_delay_ctr=20;
                  }
                  else
                  {  
                      cbi(P1IE,PUSHDN_B);
                      _BIC_SR_IRQ(LPM3_bits);
                      
                      sbi(P2DIR,I2C_DATA);
                      sbi(P2DIR,EEPROM_CLOCK);
                      sbi(P6DIR,COMM_CTRL_PIN);cbi(P6OUT,COMM_CTRL_PIN);
                      
                      //RX_OPT P2.2, TX_OPT P2.3, RX_EXT P2.4, TX_EXT P2.5
        
                      // Setup P1.2 UCA0RXD, P1.3 UCA0TXD
                      P2SEL |= BIT2 | BIT3| BIT4 | BIT5;                   // Set P3.3, P3.4 to non-IO
                      //P2DIR |= BIT2 | BIT3| BIT4 | BIT5;                   //output
                      P2DIR |= BIT3| BIT5;                   //output
                      init_dlms();
                      
                      recv_ctr=0;
                      comm_stat=0;
                      enable_recv_buf=1;
                      batt_push_ctr=10;
                      batt_disp_stat=1;
                      comm_delay_ctr=17;
                  }

                  disp_index=VER;//LCDCHK//KWH; //
                  
                  disp_mode_stat=1;
                  disp_sub_ctr=0;
                  disp_ctr=8; //8
                  disp_cnt=10;//15;
                  hist_ctr=0;
              }	    
	}
        P1IFG=0;	
	return;
}


/*
void init_usart(void)
{
	P2SEL |= 0x30;
	UCA0CTL1 = UCSSEL1;// + UCSWRST;
	UCA0CTL0 = 0x00;

//8 mhz
	//4800b
	UCA0BR0 = 0xC7;//0x82;		
	UCA0BR1 = 0x06;//0x06;				

	
	UCA0MCTL = 0x06;
	UCA0STAT = 0x00;//UCLISTEN;
	
	//UCA0IRTCTL =  UCIRTXPL2 + UCIRTXPL0 +UCIRTXCLK + UCIREN;//
	UCA0CTL1 &= ~UCSWRST;
	IE2|=UCA0RXIE;
	return;
}
*/

void transmit_char(unsigned char send)
{
	UCA2TXBUF=send;
	while((UCA2IFG & 0x02)!=0x02);
        
	UCA0TXBUF=send;
	while((UCA0IFG & 0x02)!=0x02);
	
        //comm_checksum=comm_checksum+send;
	delay1ms(2); //non ir
	comm_delay_ctr=0;
	return;
}

void serial_comm(void)
{
    unsigned char cmd, j;
    unsigned int i, tmp_int;
	
  	tmp_int=recv_ctr;
  	i=0;
	while(i<5) //50 ms delay
	{
  		delay1ms(10);
  		if(tmp_int!=recv_ctr)
  		{
  			tmp_int=recv_ctr;
  			i=0;
  		}
  		else
  			i++;
 		if(recv_ctr>=11)
  			break;
	}
  	
  	if(recv_ctr>=2)
  	{
            //comm_stat=1;
            
            enable_recv_buf=0;

            comm_checksum=recv_buf[0];
            for(i=1;i<(recv_ctr-1);i++)
                    comm_checksum=comm_checksum +recv_buf[i];
            comm_checksum=comm_checksum & 0xff;
            
            if(recv_buf[recv_ctr-1]==comm_checksum)
            {
              cmd=recv_buf[0];
              if((meter_no==0)||(meter_no==0xFFFFFF)||(cmd==UNLOCK_CMD))//||(load_val[0]<lock_kwh_val)||(cmd==K_NO_CMD)
              {
                if(cmd!=UNLOCK_CMD)
                 transmit_char(cmd+48);
                
                    switch(cmd)
	            {
		            case TIME_CMD:
		            case DATE_CMD:
		            case MTR_NO_CMD:
		            case CAL_CMD:
                            case DISP_MD_TYPE_CMD:
                            case INTGRTIME_CMD:
                            case TOD_CMD:
                            case DLOAD_CMD:
                            case LOCK_CMD:
                                if((cmd==TIME_CMD)||(cmd==DATE_CMD))
                                {

                                    disp_all_lcd(0x00);
                                    LCDMEM[11]=seg_7_lookup('C');
                                    LCDMEM[10]=seg_7_lookup('L');
                                    LCDMEM[8]=seg_7_lookup('o');
                                    LCDMEM[7]=seg_7_lookup('c');
                                    
                                    d_day=convt_bcd_to_byte(recv_buf[1]);
                                    d_mnth=convt_bcd_to_byte(recv_buf[2]);
                                    d_yr=convt_bcd_to_byte(recv_buf[3]);
                                    t_hr=convt_bcd_to_byte(recv_buf[4]);
                                    t_min=convt_bcd_to_byte(recv_buf[5]);
                                    t_sec=convt_bcd_to_byte(recv_buf[6]);
                                    
                                    RTCCTL0_H = RTCKEY_H;                           
                                    RTCCTL13 = RTCHOLD+RTCMODE;            
                                    RTCCTL0_L &= ~RTCOFIFG_L;                     

                                    RTCSEC   = t_sec;
                                    RTCMIN   = t_min;
                                    RTCHOUR  = t_hr; 
                                    
                                    RTCDAY   = d_day;  
                                    RTCMON   = d_mnth;  
                                    RTCYEAR  = d_yr; 
                                    RTCCTL13 &= ~RTCHOLD;
                                    RTCCTL13 |= RTCLOCK;
                                    RTCCTL0_H = 0;                 

                                    //set_rtc_date(recv_buf[1],recv_buf[2],recv_buf[3]);
                                    //set_rtc_time(recv_buf[4],recv_buf[5],recv_buf[6]);
                                }
	            		else if(cmd==MTR_NO_CMD)
		                {
		                  
                                    disp_all_lcd(0x00);
                                    LCDMEM[11]=seg_7_lookup('S');
                                    LCDMEM[10]=seg_7_lookup('L');
                                    LCDMEM[8]=seg_7_lookup('n');
                                    LCDMEM[7]=seg_7_lookup('o');
                                                                
                                    meter_no=0;
		                    for(i=0;i<7;i++)
		                    {
                                      meter_no=(meter_no*10)+recv_buf[i+2];   
                                      Meter_Sr_No[i+2]=recv_buf[i+2]+48;
                                    }
                                    
                                    to_eeprom(MTRNO_LOC,meter_no,3);
		                    manfact_date=d_mnth;
				    manfact_date=(manfact_date*100)+d_yr;
				    to_eeprom(MANUFACT_DATE_LOC,manfact_date,2);
                                    Year_of_Manufacture=2000+d_yr;    
                                }
                                /*
		                else if(cmd==TOD_CMD)
                                {
                                    for(i=0;i<8;i++)
                                    {
                                            j=(recv_buf[i+1]>>6)&0x03;
                                            j=j*15;
                                            
                                            t_zone[i]=(recv_buf[i+1])&0x3f;
                                        
                                        
                                        tod_time[i][0]=t_zone[i];
                                        tod_time[i][1]=j;
                                        
                                        t_zone[i]=(t_zone[i]*100)+j;
                                        to_eeprom(TOD_LOC+(i*2),t_zone[i],2);
                                    }
                                 
                                    for(i=TOD_SIZE;i>0;i--)
                                    {
                                            if(t_zone[i-1]>0)
                                                    break;
                                    }	
                                    
                                    num_of_zone=i;
                                    if((num_of_zone<2)||(num_of_zone>TOD_SIZE))
                                            num_of_zone=TOD_SIZE;	
                                    write_eeprom(NUM_ZONE_LOC,num_of_zone);
                                    
                                    //md_reset_b=1;
                                }
                                */
                                else 
                                {
                                    tmp_int=0;
                                    for(i=0;i<8;i++)
                                        tmp_int=(tmp_int*10)+recv_buf[i+1];
		                    
                                    if(cmd==LOCK_CMD)
                                    {
                                            if(tmp_int==12345)
                                                    lock_stat=1;
                                            else
                                                    lock_stat=0;
                                            write_eeprom(LOCK_STAT_LOC, lock_stat);		
                                    }
		                    else if(cmd==DISP_MD_TYPE_CMD)
                                    {
                                            disp_md_type=tmp_int;
                                            write_eeprom(DISP_MD_TYPE_LOC, disp_md_type);
                                    }
                                    /*
                                    else if(cmd==INTGRTIME_CMD)
                                    {
                                      if(((tmp_int%300)!=0)&&(tmp_int!=0)&&(tmp_int<=1800))
                                      {
                                            Max_Demand_Capture_Period=Load_Profile_Capture_Period=tmp_int;
                                            md_intgr_val=survey_intgr_val=Load_Profile_Capture_Period/60;
                                            i=t_min/md_intgr_val;
                                            write_eeprom(PREV_MIN_SLOT_LOC,i);
                                            write_eeprom(PREV_SURVEY_MIN_SLOT_LOC,i);
                                            for(i=0;i<2;i++)
                                            {
                                                    last_demand_load_val[i]=load_val[i];
                                                    to_eeprom(LAST_DEMAND_KWH_LOC+(i*4),last_demand_load_val[i],4);	
                                            }			
                                            //write_eeprom(MD_INTGR_LOC,md_intgr_val);        
                                            to_eeprom(MD_INTGR_LOC,Max_Demand_Capture_Period,2);        
                                            to_eeprom(SURVEY_INTGR_LOC,Load_Profile_Capture_Period,2);        
                                      }
                                    }
                                    */
                                    else if(cmd==CAL_CMD)
                                    {   
                                            disp_all_lcd(0x00);
                                            LCDMEM[11]=seg_7_lookup('C');
                                            LCDMEM[10]=seg_7_lookup('A');
                                            LCDMEM[8]=seg_7_lookup('L');
                                            
                                            get_avg_val();
                                            if(pf_reg3>900) 	
                                                calib_func(tmp_int);
                                            else
                                                ph_calib_func(tmp_int);

		                	    store_cal_data();
                                            i=get_cal_crc();
                                            write_eeprom(CAL_CHKSUM_LOC,i);
		                    }
                                    else if(cmd==DLOAD_CMD)
                                    {      		
                                        //LCDMEM[9]=LCDMEM[9]|0xC0;
                                        //LCDMEM[12]=LCDMEM[12]|0xF0;
	      		
		                      	if(tmp_int==TIME_LOC)	
                                        {
                                            store_energy_in_zone();
                                            send_inst_param();
                                        }	  	
                                        for(i=tmp_int;i<tmp_int+2000;i=i+16)
		                       	{
		                           	read_page_eeprom(i,0,16);
		                           	for(j=0;j<16;j++)
		                           		transmit_char(read_data_arr[0][j]);
		                       	}
		                    }
	                    }
	                  	break;
	         		
                            //case MDRESET_CMD:
                            //md_reset_b=1;
                            //break;

                            case UNLOCK_CMD:
                              if((recv_buf[1]=='a')&&(recv_buf[2]=='s')&&(recv_buf[3]=='d')&&(recv_buf[4]=='a')&&(recv_buf[5]=='q')&&(recv_buf[6]=='s')&&(recv_buf[7]=='y')&&(recv_buf[8]=='s'))
                              {
                                      pass_step=1;
                                      pass_ctr=0;
                              }
                              if((recv_buf[1]=='b')&&(recv_buf[2]=='t')&&(recv_buf[3]=='e')&&(recv_buf[4]=='b')&&(recv_buf[5]=='r')&&(recv_buf[6]=='t')&&(recv_buf[7]=='z')&&(recv_buf[8]=='t')&&(pass_step==1)&&(pass_ctr<5))
                              {
                                      disp_all_lcd(0x00);
                                      LCDMEM[11]=seg_7_lookup('U');
                                      LCDMEM[10]=seg_7_lookup('n');
                                      LCDMEM[8]=seg_7_lookup('L');
                                      LCDMEM[7]=seg_7_lookup('o');
                                      LCDMEM[6]=seg_7_lookup('c');
                            
                                      pass_step=0;
                                      pass_ctr=0;
                                      meter_no=0;
                                      to_eeprom(MTRNO_LOC,meter_no,3);
                                      
                                      for(i=0;i<7;i++)
                                      {
                                          Meter_Sr_No[i+2]=48;
                                      }
                              }
                              break;
		         
                            case CUOPEN_CMD:
                              
                              disp_all_lcd(0x00);
                              LCDMEM[11]=seg_7_lookup('C');
                              LCDMEM[10]=seg_7_lookup('L');
                              LCDMEM[8]=seg_7_lookup('r');
                              LCDMEM[7]=seg_7_lookup('C');
                            
	                      if(cuopen_stat==1)
                              {  
                                 cuopen_stat=0;
                                 write_eeprom(CUOPEN_LOC,0);
                                 cuopen_date=0;
                                 cuopen_time=0;
                                 to_eeprom(NOROLL_EVENT_LOC,0,2);
                                 to_eeprom(NOROLL_EVENT_LOC+2,0,3);
                                 to_eeprom(NOROLL_EVENT_LOC+5,0,3);
                                 //if(all_tamper_cnt>0)
                                 //  all_tamper_cnt=all_tamper_cnt-1;
                                 
                                 event_cnt[NOROLL_EVENT]=0;
                                 Tamper_Profile_Entries_In_Use[NOROLL_EVENT]=0;
                                 to_eeprom(EVENTCNT_LOC+10,event_cnt[NOROLL_EVENT],2);
                              }
                              co_disable_ctr=0;
                            break;
	                
                            case IR_INST_CMD:
                            //LCDMEM[9]=LCDMEM[9]|0xC0;
                            //LCDMEM[12]=LCDMEM[12]|0xF0;
                            old_style_send_inst_param();
                            //send_inst_param();    
                            break;
	           

                            case CLR1_CMD:
                            disp_all_lcd(0x00);
                            LCDMEM[11]=seg_7_lookup('C');
                            LCDMEM[10]=seg_7_lookup('L');
                            LCDMEM[8]=seg_7_lookup('r');
                            LCDMEM[7]=seg_7_lookup('1');
                                              
                            default_eeprom(1);
                            break;
						
                            case CLR2_CMD:
                            disp_all_lcd(0x00);
                            LCDMEM[11]=seg_7_lookup('C');
                            LCDMEM[10]=seg_7_lookup('L');
                            LCDMEM[8]=seg_7_lookup('r');
                            LCDMEM[7]=seg_7_lookup('2');
                            
                            default_eeprom(2);
                            break;
                       }
        	}
            }
	    for(i=0;i<11;i++)
               recv_buf[i]=0;	
	    recv_ctr=0;
	    enable_recv_buf=1;
  	}
        
 	return;
}

unsigned char convt_bcd_to_byte(unsigned char bcd_data)
{
	unsigned char byte_data;
        
        
        byte_data=(bcd_data & 0xf0)>>4;
	byte_data=(byte_data*10)+(bcd_data & 0x0f);
	
	return byte_data;
}

void send_inst_param(void)
{
  unsigned char i;
  
  send_data(real_date,3);
  send_data(real_time,3);
  send_data(meter_no,4);
  send_data(vrms_reg3,4);//12
  for(i=0;i<2;i++)
  	send_data(irms_reg3[i],4);//12
  send_data(pf_reg3/10,2);//6
  send_data(Mains_Freq,2);
  send_data(kw_reg3,2);//6
  send_data(kva_reg3,2);//6
  send_data(kvar_reg3,2);//6

  send_data(load_val[0],4);//4
  send_data(load_val[1],4);//4
  send_data(kwmd_val,2);
  send_data(kvamd_val,2);
  send_data(Cum_Power_Off_Count,4);
  send_data(Cum_Power_Off_Dur,4); 
  send_data(Cum_Power_On_Dur,4); 
  send_data(all_tamper_cnt,4);
  send_data(md_reset_cnt,2);
  send_data(Cum_Prog_Count,4);
}

void send_data(unsigned long int ldata, unsigned char size)
{
  unsigned char tmp_byte;

  while(size>0)
  {
    size--;
    tmp_byte=(unsigned char)((ldata>>(8*size))&0xff);
    transmit_char(tmp_byte);
    delay1ms(2);
  }
}

void old_style_send_inst_param(void)
{
  unsigned long int tmp_long;
  //unsigned char i;

	  
  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_MTR_NO,3);
  old_style_send_data(mag_on_ctr1,8); //meter_no
  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_DATE,3);
  old_style_send_data(real_date,8);
  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_TIME,3);
  old_style_send_data(real_time,8);
  
  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_KWH,3);
  old_style_send_data(load_val[0],8);
  
  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_KVAH,3);
  old_style_send_data(load_val[1],8);

  if(vrms_offset_adc>0)
         tmp_long=vrms_offset_adc;
  else
         tmp_long=((~vrms_offset_adc)+1)&0x7fff;
	                    
  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_VRMS,3);
  old_style_send_data(tmp_long,8); //vrms_reg3
  
  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_IRMS,3);
  old_style_send_data(irms_reg2[0],8);
  
  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_IRMSN,3);
  old_style_send_data(irms_reg2[1],8);
  
  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_KW,3);
  old_style_send_data(kw_reg3/10,8);

  transmit_char(';');
  delay1ms(2);
  old_style_send_data(I_KVA,3);
  old_style_send_data(kva_reg3/10,8);  
}

void old_style_send_data(unsigned long int ldata, unsigned char size)
{
  unsigned char tmp_byte;

  while(size>0)
  {
    size--;
    tmp_byte=(unsigned char)(ldata%10);
    ldata=(ldata-tmp_byte)/10;
    transmit_char(tmp_byte+48);
    delay1ms(5);
  }
}

/***** calibration functions********************************/

void get_avg_val(void)
{
   unsigned char i;
			
   scratch=0;//avg_kw	   
   scratch1=0;//avg_vrms	   
   for(i=0;i<4;i++)
   {
   		scratch=scratch+kw_reg2[channel];
   		scratch1=scratch1+vrms_reg2;
   		delay_sec=0;
   		while(delay_sec!=1)
			delay1ms(10);
   }
   scratch=scratch/i;
   scratch=(scratch*10)/3900; //3910 //reduce 5.6% 4135;//2065;  //(2070 -.2%  1=.048%)for 1600     (//4135; //4140 -.1%  1=.024%) for 3200

   scratch1=scratch1/i;
   return;
}

void calib_func(int offset)
{
   //unsigned char i;
   int err_val, tmp_int;

   err_val=(int)((scratch*100)/24);	//240,10,1
   err_val=err_val-10000;
   err_val=err_val+offset;	
   
   if((err_val<10000)&&(err_val>-10000))
   {
          tmp_int=igain_val[channel];
          igain_val[channel]=(unsigned int)(((unsigned long int)igain_val[channel]*10000)/(10000+err_val));
          n_cfden_val[channel]=(unsigned int)((scratch1*1000)/3220); //3270); //3240 // Anil 3235  //3220 +.6% //3250 -0.9%(acc)
          amp_fact[channel]=(irms_reg2[channel]*igain_val[channel])/tmp_int;
          amp_fact[channel]=amp_fact[channel]/10;
          volt_fact=(scratch1*10)/24;
   }
}

void ph_calib_func(int offset)
{
   int err_val;
   
   err_val=(int)((scratch*100)/12);	//240,10,.5
   err_val=err_val-10000;
   err_val=err_val+offset;	
   
   if((err_val<10000)&&(err_val>-10000))
   {
	  err_val=(err_val*3)/8; //10;
          iph_val[channel]=iph_val[channel]+err_val;
   }
}


/***** tender functions *****************************/

unsigned char cyclic(unsigned char val, unsigned char max)
{
	val=val+1;
        val=val%max;
	//while(val>=max)
	//	val=val-max;

	return val;
}

void default_eeprom(unsigned char stat)
{
      unsigned int i, j;
      unsigned long int location;

	  metering_data_ready=0;
	  cbi(P1OUT,PULSE1_PIN);
	  led_off=1;
	  led_va_off=1;
          led_var_off=1;

	  if(stat==1)	
	  {
  		for(location=0;location<(LOAD_SURVEY_LOC+1024);location=location+128) //clears eeprom upto tamper location //MEM_END
	            write_page_eeprom(location,0);                 
          }
	  else
	  {
	        for(location=128;location<(LOAD_SURVEY_LOC+1024);location=location+128) //clears eeprom upto tamper location //MEM_END
		      write_page_eeprom(location,0);
          } 	
          
          for(location=DAILY_SURVEY_LOC;location<=MEM_END;location=location+128) //clears eeprom upto tamper location //MEM_END
              write_page_eeprom(location,0); 
          
	  to_eeprom(DATE_LOC,real_date,3);
	  to_eeprom(TIME_LOC,real_time,3);
	  write_eeprom(LOCK_STAT_LOC, 1);
	  

          bill_day[3]=1;
          write_eeprom(BILL_DAY_LOC,bill_day[3]);

	
          last_bill_date=d_yr;
          last_bill_date=(last_bill_date*100)+d_mnth;
          last_bill_date=(last_bill_date*100)+bill_day[3];

          //last bill time and current bill time is 0
  
          to_eeprom(LAST_BILL_DATE_LOC,last_bill_date,3);

	  /*
          for(i=0;i<48;i++)
	    	write_eeprom(LOAD_SURVEY_LOC+(i*21), 0xff);
	  to_eeprom(LOAD_DATE_LOC,real_date,3);
	  */
          
	  //if(t_min>=30)
          //	write_eeprom(HR_LOC,(t_hr|0x80));
          //else
          
          
          //      write_eeprom(HR_LOC,t_hr);	
	
          clock_time_zone=330;      
          to_eeprom(IST_DIFF_LOC, clock_time_zone, 2);
                
	  i=t_min/md_intgr_val;
	  write_eeprom(PREV_MIN_SLOT_LOC,i);
          
          //i=t_min/survey_intgr_val;
	  //write_eeprom(PREV_SURVEY_MIN_SLOT_LOC,i);

          to_eeprom(TOD_LOC,0,2);
          to_eeprom(TOD_LOC+2,600,2);
          to_eeprom(TOD_LOC+4,1000,2);
          to_eeprom(TOD_LOC+6,1700,2);
          to_eeprom(TOD_LOC+8,2100,2);

	  /*
          to_eeprom(TOD_LOC+10,600,2);
          to_eeprom(TOD_LOC+12,800,2);
          to_eeprom(TOD_LOC+14,1100,2);
	  */

          to_eeprom(TOD_LOC+(8*2),0,2);
          to_eeprom(TOD_LOC+(8*2)+2,500,2);
          to_eeprom(TOD_LOC+(8*2)+4,900,2);
          to_eeprom(TOD_LOC+(8*2)+6,1800,2);
          to_eeprom(TOD_LOC+(8*2)+8,2200,2);

	  /*
          to_eeprom(TOD_LOC+(8*2)+10,600,2);
          to_eeprom(TOD_LOC+(8*2)+12,800,2);
          to_eeprom(TOD_LOC+(8*2)+14,1100,2);
	  */

          to_eeprom(PASSIVE_TOD_LOC,0,2);
          to_eeprom(PASSIVE_TOD_LOC+2,600,2);
          to_eeprom(PASSIVE_TOD_LOC+4,1000,2);
          to_eeprom(PASSIVE_TOD_LOC+6,1700,2);
          to_eeprom(PASSIVE_TOD_LOC+8,2100,2);

          /*
          to_eeprom(PASSIVE_TOD_LOC+10,600,2);
          to_eeprom(PASSIVE_TOD_LOC+12,800,2);
          to_eeprom(PASSIVE_TOD_LOC+14,1100,2);
	  */

          to_eeprom(PASSIVE_TOD_LOC+(8*2),0,2);
          to_eeprom(PASSIVE_TOD_LOC+(8*2)+2,500,2);
          to_eeprom(PASSIVE_TOD_LOC+(8*2)+4,900,2);
          to_eeprom(PASSIVE_TOD_LOC+(8*2)+6,1800,2);
          to_eeprom(PASSIVE_TOD_LOC+(8*2)+8,2200,2);

          /*
          to_eeprom(PASSIVE_TOD_LOC+(8*2)+10,600,2);
          to_eeprom(PASSIVE_TOD_LOC+(8*2)+12,800,2);
          to_eeprom(PASSIVE_TOD_LOC+(8*2)+14,1100,2);
	  */

	  write_eeprom(TOD_ID_LOC,1);
          write_eeprom(TOD_ID_LOC+1,2);
          write_eeprom(TOD_ID_LOC+2,3);
          write_eeprom(TOD_ID_LOC+3,4);
          write_eeprom(TOD_ID_LOC+4,1);
	  write_eeprom(TOD_ID_LOC+5,0);
          write_eeprom(TOD_ID_LOC+6,0);
          write_eeprom(TOD_ID_LOC+7,0);
          
          write_eeprom(TOD_ID_LOC+8,1);
          write_eeprom(TOD_ID_LOC+9,2);
          write_eeprom(TOD_ID_LOC+10,3);
          write_eeprom(TOD_ID_LOC+11,4);
          write_eeprom(TOD_ID_LOC+12,1);
          write_eeprom(TOD_ID_LOC+13,0);
          write_eeprom(TOD_ID_LOC+14,0);
          write_eeprom(TOD_ID_LOC+15,0);
          
          write_eeprom(PASSIVE_TOD_ID_LOC,1);
          write_eeprom(PASSIVE_TOD_ID_LOC+1,2);
          write_eeprom(PASSIVE_TOD_ID_LOC+2,3);
          write_eeprom(PASSIVE_TOD_ID_LOC+3,4);
          write_eeprom(PASSIVE_TOD_ID_LOC+4,1);
          write_eeprom(PASSIVE_TOD_ID_LOC+5,0);
          write_eeprom(PASSIVE_TOD_ID_LOC+6,0);
          write_eeprom(PASSIVE_TOD_ID_LOC+7,0);
          
          write_eeprom(PASSIVE_TOD_ID_LOC+8,1);
          write_eeprom(PASSIVE_TOD_ID_LOC+9,2);
          write_eeprom(PASSIVE_TOD_ID_LOC+10,3);
          write_eeprom(PASSIVE_TOD_ID_LOC+11,4);
          write_eeprom(PASSIVE_TOD_ID_LOC+12,1);
          write_eeprom(PASSIVE_TOD_ID_LOC+13,0);
          write_eeprom(PASSIVE_TOD_ID_LOC+14,0);
          write_eeprom(PASSIVE_TOD_ID_LOC+15,0);
          
	  write_eeprom(NUM_ZONE_LOC,5); //8
	  
          
          write_eeprom(SEASON_NAME_ACTIVE_LOC+0,'s');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+1,'e');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+2,'a');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+3,'s');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+4,'o');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+5,'n');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+6,'1');
          for(i=7;i<10;i++)
          write_eeprom(SEASON_NAME_ACTIVE_LOC+i,'\0');
          
          write_eeprom(SEASON_NAME_ACTIVE_LOC+10,'s');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+11,'e');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+12,'a');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+13,'s');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+14,'o');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+15,'n');
          write_eeprom(SEASON_NAME_ACTIVE_LOC+16,'2');
          for(i=7;i<10;i++)
          write_eeprom(SEASON_NAME_ACTIVE_LOC+10+i,'\0');
          
          for(i=0;i<20;i++)
          {
            j=read_eeprom(SEASON_NAME_ACTIVE_LOC+i);
            write_eeprom(SEASON_NAME_PASSIVE_LOC+i,j);
          }  
          
          write_eeprom(SEASON_DATE_ACTIVE_LOC+0,10);//month
          write_eeprom(SEASON_DATE_ACTIVE_LOC+1,1);//day
          write_eeprom(SEASON_DATE_ACTIVE_LOC+2,0);//hr
          write_eeprom(SEASON_DATE_ACTIVE_LOC+3,0);//min
          
          write_eeprom(SEASON_DATE_ACTIVE_LOC+4,4);//month
          write_eeprom(SEASON_DATE_ACTIVE_LOC+5,1);//day
          write_eeprom(SEASON_DATE_ACTIVE_LOC+6,0);//hr
          write_eeprom(SEASON_DATE_ACTIVE_LOC+7,0);//min
          
          for(i=0;i<8;i++)
          {
            j=read_eeprom(SEASON_DATE_ACTIVE_LOC+i);
            write_eeprom(SEASON_DATE_PASSIVE_LOC+i,j);
          }  
          
          write_eeprom(CAL_NAME_ACTIVE_LOC+0,'c');
          write_eeprom(CAL_NAME_ACTIVE_LOC+1,'a');
          write_eeprom(CAL_NAME_ACTIVE_LOC+2,'l');
          write_eeprom(CAL_NAME_ACTIVE_LOC+3,'e');
          write_eeprom(CAL_NAME_ACTIVE_LOC+4,'n');
          write_eeprom(CAL_NAME_ACTIVE_LOC+5,'d');
          write_eeprom(CAL_NAME_ACTIVE_LOC+6,'a');
          write_eeprom(CAL_NAME_ACTIVE_LOC+7,'r');
          write_eeprom(CAL_NAME_ACTIVE_LOC+8,'1');
          
          for(i=9;i<16;i++)
            write_eeprom(CAL_NAME_ACTIVE_LOC+i,'\0');
            
          write_eeprom(CAL_NAME_PASSIVE_LOC+0,'c');
          write_eeprom(CAL_NAME_PASSIVE_LOC+1,'a');
          write_eeprom(CAL_NAME_PASSIVE_LOC+2,'l');
          write_eeprom(CAL_NAME_PASSIVE_LOC+3,'e');
          write_eeprom(CAL_NAME_PASSIVE_LOC+4,'n');
          write_eeprom(CAL_NAME_PASSIVE_LOC+5,'d');
          write_eeprom(CAL_NAME_PASSIVE_LOC+6,'a');
          write_eeprom(CAL_NAME_PASSIVE_LOC+7,'r');
          write_eeprom(CAL_NAME_PASSIVE_LOC+8,'2');
          
          for(i=9;i<16;i++)
            write_eeprom(CAL_NAME_PASSIVE_LOC+i,'\0');
          
          
          //load_control_stat=1;
          //write_eeprom(LOAD_CONTROL_STAT_LOC,load_control_stat);
          
	  ep_clear_stat=1;
          program_reset=0;
  return;
}

void init_eeprom(void)
{
	unsigned char tmp_byte;
	unsigned int i, j, tmp_int, index;
	//read kwh, kvah, rkvah_lead,rkvah_lag from memory
	unsigned char checksum_fail_ctr, prev_checksum_fail_ctr;
	unsigned long int location;
        
	for(index=0;index<2;index++)		
	{
		prev_checksum_fail_ctr=0;
		while(1)
		{
			checksum_fail_ctr=0;
			load_index[index]=0;
			load_val[index]=0;
			for(tmp_int=0;tmp_int<KWH_ARR;tmp_int++)
			{
				scratch=from_eeprom((KWH_LOC+(index*200)+(tmp_int*4)),4);
				checksum=read_eeprom(KWH_CHKSUM_LOC+(index*50)+tmp_int);
				checksum_calc=crc8(scratch);
				if(checksum_calc==checksum)
				{
					if(load_val[index]<scratch)
					{
		              load_val[index]=scratch;
	                  load_index[index]=tmp_int;
					}    
				}
				else
					checksum_fail_ctr++;		  						
			}
			
			if(checksum_fail_ctr!=prev_checksum_fail_ctr)
				prev_checksum_fail_ctr=checksum_fail_ctr;
			else
				break;
		}
	}
    
    //last_kwh_val=load_val[0];
    lock_stat=read_eeprom(LOCK_STAT_LOC);
    meter_no=from_eeprom(MTRNO_LOC,3);
    
    scratch=meter_no;
    for(i=0;i<7;i++)
    {
      tmp_byte=scratch%10; 
      scratch=(scratch-tmp_byte)/10;
      Meter_Sr_No[8-i]=tmp_byte+48;
    }
    
    bill_day[0]=0xff;
    bill_day[1]=0xff;
    bill_day[2]=0xff;  
    bill_day[3]=read_eeprom(BILL_DAY_LOC);//day
    bill_day[4]=0xff;  
    
    bill_time[0]=read_eeprom(BILL_TIME_LOC);//hr
    bill_time[1]=read_eeprom(BILL_TIME_LOC+1);//min
    bill_time[2]=0x00;
    bill_time[3]=0xff;
    
    for(i=0;i<4;i++)  
      scheduled_bill_day[i]=read_eeprom(SCHEDULED_BILL_DAY_LOC+i);
    scheduled_bill_day[4]=0xff;  
    
    for(i=0;i<2;i++)  
      scheduled_bill_time[i]=read_eeprom(SCHEDULED_BILL_TIME_LOC+i);
    scheduled_bill_time[2]=0xff;
    scheduled_bill_time[3]=0xff;
    
    tmp_int=scheduled_bill_day[1];
    tmp_int=(tmp_int*256)+scheduled_bill_day[0];
    
    if((tmp_int<2017)||(tmp_int>2099)) //year
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
    
    //last_bill_day=read_eeprom(LAST_BILL_DAY_LOC);
    //if((last_bill_day==0)||(last_bill_day>31))
	//	last_bill_day=billing_day;
	last_bill_time=from_eeprom(LAST_BILL_TIME_LOC,2);
	last_bill_date=from_eeprom(LAST_BILL_DATE_LOC,3);


    mnth_pos=read_eeprom(MNTHPOS_LOC);
    md_reset_cnt=from_eeprom(MD_RESET_LOC,2);  
    if(md_reset_cnt<(Billing_Profile_Entries-1))
      Billing_Profile_Entries_In_Use=md_reset_cnt+1;
    else
      Billing_Profile_Entries_In_Use=Billing_Profile_Entries;
    
    day_pos=read_eeprom(DAYPOS_LOC);
    day_hr_pos=from_eeprom(HRPOS_LOC,2);
    Load_Profile_Entries_In_Use=from_eeprom(HRCNT_LOC,2);
    if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
      Load_Profile_Entries_In_Use=Load_Profile_Entries;


    Daily_Load_Profile_Entries_In_Use=read_eeprom(DAYCNT_LOC);
    if(Daily_Load_Profile_Entries_In_Use>Daily_Load_Profile_Entries)
      Daily_Load_Profile_Entries_In_Use=Daily_Load_Profile_Entries;
    
    
    kwcd_val=from_eeprom(KWCD_LOC,4);
    manfact_date=from_eeprom(MANUFACT_DATE_LOC,2);
    Year_of_Manufacture=2000+(manfact_date%100);
			
    reset_on_time=from_eeprom(LST_MNTH_DATA_LOC,2);
            
    for(i=0;i<TOT_EVENT_TYPE;i++)
    {
      Last_Tamper_Id[i]=from_eeprom(LAST_TAMPER_ID_LOC+(i*2),2);
      
      event_pos[i]=from_eeprom(EVENTPOS_LOC+(i*2),2);
      event_cnt[i]=from_eeprom(EVENTCNT_LOC+(i*2),2);
      if(event_cnt[i]<Tamper_Profile_Entries[i])
        Tamper_Profile_Entries_In_Use[i]=event_cnt[i];
      else
        Tamper_Profile_Entries_In_Use[i]=Tamper_Profile_Entries[i];
    }

    last_tamper_occ_type=read_eeprom(LSTTAMPER_OCC_LOC);
    last_tamper_res_type=read_eeprom(LSTTAMPER_RES_LOC);
    cuopen_stat=read_eeprom(CUOPEN_LOC);
    cuopen_date=from_eeprom(CUOPEN_LOC+1,3);
    cuopen_time=from_eeprom(CUOPEN_LOC+4,3);
                      
                      
    //poff_pos=from_eeprom(POFFPOS_LOC,2);
    Cum_Power_Off_Count=from_eeprom(POFF_CNT_LOC,4);
    Cum_Power_Off_Dur=from_eeprom(POFF_DUR_LOC,4);
    Cum_Power_On_Dur=from_eeprom(PON_DUR_LOC,4);
   
    store_tamper_stat=from_eeprom(ALLTAMPER_LOC,3);
    if(store_tamper_stat==0xffffff)
	store_tamper_stat=0;
    //mag_imax_stat=(store_tamper_stat>>2)&0x000001;
    all_tamper_cnt=0;
    for(i=0;i<TAMPER_TYPE;i++)
    {
      //occur_stat[i]=read_eeprom(INST_TAMPER_LOC+(i*26));
      tamper_cnt[i]=from_eeprom(TAMPERCNTARR_LOC+(i*2),2);  
      all_tamper_cnt=all_tamper_cnt+tamper_cnt[i];
    }

    //if(cuopen_stat==1)
    //  all_tamper_cnt=all_tamper_cnt+1;

    Max_Demand_Capture_Period=from_eeprom(MD_INTGR_LOC,2);        
    Load_Profile_Capture_Period=from_eeprom(SURVEY_INTGR_LOC,2);        

    if((Max_Demand_Capture_Period<300)||(Max_Demand_Capture_Period>1800))
      Max_Demand_Capture_Period=1800;//900;

    if((Load_Profile_Capture_Period<300)||(Load_Profile_Capture_Period>1800))
      Load_Profile_Capture_Period=1800;//900;

    
    md_intgr_val=Max_Demand_Capture_Period/60;
    survey_intgr_val=Load_Profile_Capture_Period/60;

    //md_intgr_val=read_eeprom(MD_INTGR_LOC);
    if(((survey_intgr_val%5)!=0)||(survey_intgr_val==0)||(survey_intgr_val>30))
	survey_intgr_val=30;//15;
    
    if(((md_intgr_val%5)!=0)||(md_intgr_val==0)||(md_intgr_val>30))
	md_intgr_val=30;//15;
    
    location=KWMD_LOC+(mnth_pos*19);
    kwmd_val=from_eeprom(location+9,2);
    
    kwmd_date=from_eeprom(location+11,3);
    kwmd_time=from_eeprom(location+14,2);
    get_time_data(kwmd_date, (kwmd_time*100));
    for(i=0;i<12;i++)
      KWMD_DT[i+2]=tmp_time_string[i];

    location=KVAMD_LOC+(mnth_pos*11);
    kvamd_val=from_eeprom(location+4,2);
    
    kvamd_date=from_eeprom(location+6,3);
    kvamd_time=from_eeprom(location+9,2);
    get_time_data(kvamd_date, (kvamd_time*100));
    for(i=0;i<12;i++)
      KVAMD_DT[i+2]=tmp_time_string[i];
    
    
    tmp_int=cyclic(mnth_pos+11-hist_ctr, HISTORY_SIZE); //12 month history
    location=KWMD_LOC+(tmp_int*19);
    
    scratch=from_eeprom(location,3);
    scratch1=from_eeprom(location+3,2);
    get_time_data(scratch, (scratch1*100));
    for(i=0;i<12;i++)
      Last_MD_Rst_DT[i+2]=tmp_time_string[i];

    bkp_kwh_val=from_eeprom(location+5,4);
    bkp_kwmd_val=from_eeprom(location+9,2);
    bkp_kwmd_date=from_eeprom(location+11,3);
    bkp_kwmd_time=from_eeprom(location+14,2);
    bkp_pf_val=read_eeprom(location+18);
    
    location=KVAMD_LOC+(tmp_int*11);
    bkp_kvah_val=from_eeprom(location,4);
    bkp_kvamd_val=from_eeprom(location+4,2);
    bkp_kvamd_date=from_eeprom(location+6,3);
    bkp_kvamd_time=from_eeprom(location+9,2);

    disp_md_type=read_eeprom(DISP_MD_TYPE_LOC);
	
    for(i=0;i<2;i++)
    	last_hr_load_val[i]=from_eeprom(LAST_HR_KWH_LOC+(i*4),4); 

    for(i=0;i<2;i++)
    	last_demand_load_val[i]=from_eeprom(LAST_DEMAND_KWH_LOC+(i*4),4);			
    
    
    location=(day_hr_pos+Load_Profile_Entries-1)%Load_Profile_Entries;
    location=LOAD_SURVEY_LOC+(location*12);
    read_page_eeprom(location,0,12);
    
    for(i=0;i<2;i++)
      block_load_val[i]=from_data_arr(4+(i*2),0,2);
    block_vrms=from_data_arr(8,0,2);
    block_irms=from_data_arr(10,0,2);
    
    if(block_load_val[0]>=0xff00)
    {
        block_load_val[0]=0;
        block_load_val[1]=0;
        block_vrms=0;
        block_irms=0;
    }

    /*  
    Over_Current_Val=from_eeprom(OVER_CURRENT_VAL_LOC,2);
    Over_Load_Val=from_eeprom(OVER_LOAD_VAL_LOC,2);
    Conn_Time_Interval=from_eeprom(CONN_TIME_INTERVAL_LOC,2);
    Conn_Lockout_Time=from_eeprom(CONN_LOCKOUT_TIME_LOC,2);
    Conn_Time_Repeat=read_eeprom(CONN_TIME_REPEAT_LOC);
    
    if((Over_Current_Val<100)||(Over_Current_Val>9000))
      Over_Current_Val=9000;
    if((Over_Load_Val<1200)||(Over_Load_Val>60000))
      Over_Load_Val=60000;
    if((Conn_Time_Interval<60)||(Conn_Time_Interval>3600))
      Conn_Time_Interval=300;
    if((Conn_Lockout_Time<60)||(Conn_Lockout_Time>60000))
      Conn_Lockout_Time=1800;
    if((Conn_Time_Repeat<1)||(Conn_Time_Repeat>100))
      Conn_Time_Repeat=3;
    
    over_limit_stat=read_eeprom(OLOAD_STAT_LOC);  
    over_limit_stat=over_limit_stat&0x01;
    
    load_control_stat=read_eeprom(LOAD_CONTROL_STAT_LOC);
    load_control_stat=load_control_stat&0x01;
    if(load_control_stat==0)
        load_conn_stat=0;
    */
    
    Tamper_Occ_Time=from_eeprom(TAMPER_OCC_TIME_LOC,2);
    Tamper_Res_Time=from_eeprom(TAMPER_RES_TIME_LOC,2);
    
    if((Tamper_Occ_Time<15)||(Tamper_Occ_Time>1800))
      Tamper_Occ_Time=15;//900;

    if((Tamper_Res_Time<15)||(Tamper_Res_Time>1800))
      Tamper_Res_Time=15;//300;
    
    
    Cum_Prog_Count=from_eeprom(PROGRAMCNT_LOC,4);
    
    clock_time_zone=from_eeprom(IST_DIFF_LOC, 2);

    //if(clock_time_zone>1440)
    //      clock_time_zone=330;

    j=0xAA;
    for(i=0;i<8;i++)
    {
         auth1[2+i]=read_eeprom(AUTH1_LOC+i);
         j=j+auth1[2+i];
    }
    j=j&0xff;
    
    i=read_eeprom(AUTH1_LOC+9);
    
    if(i!=j)
    {
      for(i=0;i<8;i++)
           auth1[2+i]=65+i;//'0';
      
    }  
    
    j=0xAA;
    for(i=0;i<16;i++)
    {
         auth2[2+i]=read_eeprom(AUTH2_LOC+i);
         j=j+auth2[2+i];
    }
    j=j&0xff;

    i=read_eeprom(AUTH2_LOC+19);
    
    if(i!=j)
    {
      //for(i=0;i<16;i++)
      //     auth2[2+i]='a';
      auth2[2]='R';auth2[3]='E';auth2[4]='N';auth2[5]='E';auth2[6]='S';auth2[7]='A';auth2[8]='S';
      auth2[9]='_';auth2[10]='P';auth2[11]='6';auth2[12]='w';auth2[13]='R';auth2[14]='J';auth2[15]='2';
      auth2[16]='1';auth2[17]='F';
    }  

    return;
}

void load_func(void)
{
	unsigned int index;
	unsigned char tmp_load_ctr[2];
	
        for(index=0;index<2;index++)
	{
		tmp_load_ctr[index]=load_ctr[index];
		load_ctr[index]=0;
                //dial_pulse[index]=dial_pulse[index]+tmp_load_ctr[index];      	
	}	
	
	for(index=0;index<2;index++)
        {
            tmp_load_ctr[index]=tmp_load_ctr[index]+load_rmndr[index];
	    load_rmndr[index]=(unsigned char)(tmp_load_ctr[index]%32); // change here from 32
	    tmp_load_ctr[index]=(tmp_load_ctr[index]-load_rmndr[index])/32;        // change here from 32
	    inst_load[index]=inst_load[index]+tmp_load_ctr[index];        
	    
	    if (inst_load[index]>=1) //10 watt*ctr
	    {	
	        load_val[index]=load_val[index]+inst_load[index];
	        load_index[index]=cyclic(load_index[index],KWH_ARR);
	        to_eeprom(KWH_LOC+(index*200)+(load_index[index]*4),load_val[index],4);
	        checksum=crc8(load_val[index]);
	        write_eeprom(KWH_CHKSUM_LOC+(index*50)+load_index[index],checksum);
	        inst_load[index]=0;
	    }
    }    
    return;
}

unsigned char crc8(unsigned long int long_val)
{
	unsigned char crc, i, j;
		
	crc=0;
	for(i=0;i<8;i++)
	{	
		j=(unsigned char)((long_val>>(i*4))&0xf);
		j=j*(i+1);
		crc=crc+j;
	}	
	return crc;
}

void store_md_data(unsigned int time_val)
{
    unsigned char i;
    unsigned int j;
    unsigned long int location;
    
    for(i=0;i<2;i++)
    {
          //scratch=from_eeprom(LAST_DEMAND_KWH_LOC+(i*4),4);
          if(load_val[i]>last_demand_load_val[i])
              scratch=load_val[i]-last_demand_load_val[i];
          else
              scratch=0;
            
          scratch=(scratch*60)/md_intgr_val;
            
          if(i==0)
          {
                location=KWMD_LOC+(mnth_pos*19);
                //j=from_eeprom(location+9,2);
                if(scratch>kwmd_val)
                {
                      kwmd_val=scratch;
                      to_eeprom(location+9,scratch,2);

                      scratch1=time_val;
                      
                      if(time_val==0)
                      {
                        to_eeprom(location+11,real_date,3);
                        get_time_data(real_date, (scratch1*100));
                        kwmd_date=real_date;
                      }
                      else
                      {
                        to_eeprom(location+11,prev_date,3);
                        get_time_data(prev_date, (scratch1*100));
                        kwmd_date=prev_date;
                      }
                      to_eeprom(location+14,time_val,2);
                      kwmd_time=time_val;
                      
                      for(j=0;j<12;j++)
                        KWMD_DT[j+2]=tmp_time_string[j];
                }
                
                location=TOD_KWMD_LOC+(mnth_pos*88)+(t_prev_zone*11);
                j=from_eeprom(location+4,2);
                if(scratch>j)
                {        
                      to_eeprom(location+4,scratch,2);
                      if(time_val==0)
                          to_eeprom(location+6,real_date,3);
                      else
                          to_eeprom(location+6,prev_date,3);
                      to_eeprom(location+9,time_val,2);	
                }
          }
          else if(i==1)
          {
              location=KVAMD_LOC+(mnth_pos*11);
              //j=from_eeprom(location+4,2);
              if(scratch>kvamd_val)
              {
                kvamd_val=scratch;
                to_eeprom(location+4,scratch,2);

                scratch1=time_val;
                      
                if(time_val==0)
                {
                  to_eeprom(location+6,real_date,3);
                  get_time_data(real_date, (scratch1*100));
                  kvamd_date=real_date;
                }
                else
                {
                  to_eeprom(location+6,prev_date,3);
                  get_time_data(prev_date, (scratch1*100));
                  kvamd_date=prev_date;
                }
                to_eeprom(location+9,time_val,2);
                kvamd_time=time_val;
                
                for(j=0;j<12;j++)
                  KVAMD_DT[j+2]=tmp_time_string[j];                
              }
              
              location=TOD_KVAMD_LOC+(mnth_pos*88)+(t_prev_zone*11);
              j=from_eeprom(location+4,2);
              if(scratch>j)
              {        
                to_eeprom(location+4,scratch,2);
                if(time_val==0)
                    to_eeprom(location+6,real_date,3);//6
                else
                    to_eeprom(location+6,prev_date,3);//6
                to_eeprom(location+9,time_val,2);	
              }
        }
        last_demand_load_val[i]=load_val[i];
        to_eeprom(LAST_DEMAND_KWH_LOC+(i*4),last_demand_load_val[i],4);			
    }
}

//13,12,yr_h,yr_l,mnth,day,weekday,hr,min,sec,hundredth of sec, dev_h,dev_l,stat
//13,12,INJECT16(2002), DECEMBER, 4, WEDNESDAY, 10, 6, 11, 255, INJECT16(120), 0
void get_time_data(unsigned long int date_val,unsigned long int time_val) 
{
  unsigned int tmp_int;
  
  tmp_int=2000+(date_val%100);
  
  //tmp_time_string[0]=13;
  //tmp_time_string[1]=12;
  tmp_time_string[0]=(tmp_int>>8)&0xff; //YR{H)
  tmp_time_string[1]=tmp_int&0xff;      //YR(L) 
  tmp_time_string[2]=(date_val/100)%100;//MNTH 
  tmp_time_string[3]=(date_val/10000);  //DAY
  tmp_time_string[4]=0xFF;                 //WEEKDAY Not Specified
  tmp_time_string[5]=(time_val/10000);  //HR 
  tmp_time_string[6]=(time_val/100)%100;//MIN
  tmp_time_string[7]=time_val%100;      //SEC
  tmp_time_string[8]=0xFF;                //HUNDREDTH of SEC  Not Specified
  tmp_time_string[9]=0x80;                //DEV(H)   Not Specified
  tmp_time_string[10]=0x00;                //DEV(L) 
  tmp_time_string[11]=0;                //STAT 

  
  if(date_val==0)
  {
    tmp_time_string[0]=0xff;
    tmp_time_string[1]=0xff;
    tmp_time_string[2]=0xff;
    tmp_time_string[3]=0xff;
  }
  
  /*
  if(time_val==0)
  {
    tmp_time_string[5]=0xff;
    tmp_time_string[6]=0xff;
    tmp_time_string[7]=0xff;
  }
  */
  
  return;  
}

void get_bill_data(unsigned int index) 
{
      unsigned int i,j;
      unsigned long int location, location1;//, location2;
      
      /*
      unsigned char Bill_Date[12];
      unsigned long Sys_Power_Factor;
      unsigned long long Cumm_Energy_KWh;
      unsigned long Cumm_Energy_KWh_TZ[8];
      unsigned long long Cumm_Energy_KVarh_Lag;  
      unsigned long long Cumm_Energy_KVarh_Lead;
      unsigned long long Cumm_Energy_KVAh;
      unsigned long Cumm_Energy_KVAh_TZ[8];
      unsigned long MD_KW;
      unsigned char MD_KW_DT[12];
      unsigned long MD_KW_TZ[8];  
      unsigned char MD_KW_TZ_DT[8][12];
      unsigned long MD_KVA;
      unsigned char MD_KVA_DT[12];
      unsigned long MD_KVA_TZ[8];  
      unsigned char MD_KVA_TZ_DT[8][12];


      
      #define KWMD_LOC                1200 //(3+2+4+2+3+2)*13 =(16*13)= 208
                                     //bdate,btime,kwh,kwmd,date,time//,pf,ontime

      #define KVAMD_LOC               1450 //1500 //(4+4+4+2+3+2)*13 =(19*13)= 247
                                       	    //kvah,kvamd,date,time

      #define TOD_KWMD_LOC            1717    //(4+2+3+2)*8*13=88*13=1144    
      #define TOD_KVAMD_LOC           2861    //(4+2+3+2)*8*13=88*13=1144    

      */
      
      location=(mnth_pos+HISTORY_SIZE-index+1) % HISTORY_SIZE;  //cyclic(mnth_pos+13-index, HISTORY_SIZE); //(index 1 to 13) //12 month history
      //location2=KWMD_EXTRA_LOC+(location*2);//+(disp_zone_ctr*11);
      location=KWMD_LOC+(location*19);
      
      location1=(mnth_pos+HISTORY_SIZE-index+1) % HISTORY_SIZE; //cyclic(mnth_pos+13-index, HISTORY_SIZE); //12 month history
      location1=KVAMD_LOC+(location1*11);//11

      read_page_eeprom(location,0,19);
      read_page_eeprom(location1,1,11);

      if(index==1)
      {
        get_time_data(real_date, real_time);//(0,0);
        memcpy(stBilling_Profile.Bill_Date,tmp_time_string,12);
        stBilling_Profile.Cumm_Energy_KWh=load_val[0];
        stBilling_Profile.Cumm_Energy_KVAh=load_val[1];
        //stBilling_Profile.Cumm_Energy_KVarh_Lag=load_val[2];
        //stBilling_Profile.Cumm_Energy_KVarh_Lead=load_val[3];
        stBilling_Profile.Power_On_Time=reset_on_time;
        stBilling_Profile.Sys_Power_Factor=avg_pf;
      }
      else
      { 
        
        scratch=from_data_arr(0,0,3);
        scratch1=from_data_arr(3,0,2);
        get_time_data(scratch, (scratch1*100));
        memcpy(stBilling_Profile.Bill_Date,tmp_time_string,12);
        stBilling_Profile.Cumm_Energy_KWh=from_data_arr(5,0,4);
        stBilling_Profile.Cumm_Energy_KVAh=from_data_arr(0,1,4);
        //stBilling_Profile.Cumm_Energy_KVarh_Lag=from_data_arr(4,1,4);  
        //stBilling_Profile.Cumm_Energy_KVarh_Lead=from_data_arr(8,1,4);
        stBilling_Profile.Power_On_Time=from_data_arr(16,0,2);
        stBilling_Profile.Sys_Power_Factor=from_data_arr(18,0,1);
      }
      
      stBilling_Profile.MD_KW=from_data_arr(9,0,2);
      
      scratch=from_data_arr(11,0,3);
      scratch1=from_data_arr(14,0,2);
      get_time_data(scratch, (scratch1*100));
      memcpy(stBilling_Profile.MD_KW_DT,tmp_time_string,12);
      
      stBilling_Profile.MD_KVA=from_data_arr(4,1,2);
      
      scratch=from_data_arr(6,1,3);
      scratch1=from_data_arr(9,1,2);
      get_time_data(scratch, (scratch1*100));
      memcpy(stBilling_Profile.MD_KVA_DT,tmp_time_string,12);
      
      location=(mnth_pos+HISTORY_SIZE-index+1) % HISTORY_SIZE; //cyclic(mnth_pos+13-index, HISTORY_SIZE); //12 month history
      location=TOD_KWMD_LOC+(location*88);//+(disp_zone_ctr*11);
      
      location1=(mnth_pos+HISTORY_SIZE-index+1) % HISTORY_SIZE; //cyclic(mnth_pos+13-index, HISTORY_SIZE); //12 month history
      location1=TOD_KVAMD_LOC+(location1*88);//+(disp_zone_ctr*11);

      /*
      for(i=0;i<8;i++)
      {        
        read_page_eeprom(location+(i*11),0,11);
        read_page_eeprom(location1+(i*11),1,11);

        scratch=from_data_arr(0,0,4);
        scratch1=from_data_arr(0,1,4);
        if((index==1)&&(i==t_prev_zone))
        {
            if(load_val[0]>last_stored_tod_load_val[0])
              scratch=scratch+(load_val[0]-last_stored_tod_load_val[0]);
            if(load_val[1]>last_stored_tod_load_val[1])
              scratch1=scratch1+(load_val[1]-last_stored_tod_load_val[1]);
        }   
        stBilling_Profile.Cumm_Energy_KWh_TZ[i]=scratch;
        stBilling_Profile.Cumm_Energy_KVAh_TZ[i]=scratch1;
        
        stBilling_Profile.MD_KW_TZ[i]=from_data_arr(4,0,2);  
        
        scratch=from_data_arr(6,0,3);
        scratch1=from_data_arr(9,0,2);
        get_time_data(scratch, (scratch1*100));
        memcpy(stBilling_Profile.MD_KW_TZ_DT[i],tmp_time_string,12);
        
        stBilling_Profile.MD_KVA_TZ[i]=from_data_arr(4,1,2);  
        
        scratch=from_data_arr(6,1,3);
        scratch1=from_data_arr(9,1,2);
        get_time_data(scratch, (scratch1*100));
        memcpy(stBilling_Profile.MD_KVA_TZ_DT[i],tmp_time_string,12);
      }
      */

      for(i=0;i<8;i++)
      { 
        stBilling_Profile.Cumm_Energy_KWh_TZ[i]=0;
        stBilling_Profile.Cumm_Energy_KVAh_TZ[i]=0;
        stBilling_Profile.MD_KW_TZ[i]=0;  
        stBilling_Profile.MD_KVA_TZ[i]=0;
        get_time_data(0,0);
        memcpy(stBilling_Profile.MD_KW_TZ_DT[i],tmp_time_string,12);
        memcpy(stBilling_Profile.MD_KVA_TZ_DT[i],tmp_time_string,12);        
        
        for(j=0;j<8;j++)
        {       
            if(tariff_id[prev_season][j]==(i+1))
            {          
                read_page_eeprom(location+(j*11),0,11);
                read_page_eeprom(location1+(j*11),1,11);

                scratch=from_data_arr(0,0,4);
                scratch1=from_data_arr(0,1,4);
                if((index==1)&&(j==t_prev_zone))
                {
                    if(load_val[0]>last_stored_tod_load_val[0])
                      scratch=scratch+(load_val[0]-last_stored_tod_load_val[0]);
                    if(load_val[1]>last_stored_tod_load_val[1])
                      scratch1=scratch1+(load_val[1]-last_stored_tod_load_val[1]);
                }   
                stBilling_Profile.Cumm_Energy_KWh_TZ[i]=stBilling_Profile.Cumm_Energy_KWh_TZ[i]+scratch;
                stBilling_Profile.Cumm_Energy_KVAh_TZ[i]=stBilling_Profile.Cumm_Energy_KVAh_TZ[i]+scratch1;
                
                scratch=from_data_arr(4,0,2);  
                if(scratch>stBilling_Profile.MD_KW_TZ[i])
                {
                  stBilling_Profile.MD_KW_TZ[i]=scratch;
                  scratch=from_data_arr(6,0,3);
                  scratch1=from_data_arr(9,0,2);
                  get_time_data(scratch, (scratch1*100));
                  memcpy(stBilling_Profile.MD_KW_TZ_DT[i],tmp_time_string,12);                
                }  
                
                scratch=from_data_arr(4,1,2);  
                if(scratch>stBilling_Profile.MD_KVA_TZ[i])
                {  
                  scratch=from_data_arr(6,1,3);
                  scratch1=from_data_arr(9,1,2);
                  get_time_data(scratch, (scratch1*100));
                  memcpy(stBilling_Profile.MD_KVA_TZ_DT[i],tmp_time_string,12);
                }  
            }
        }
      }
      
      return;
}  

unsigned long int get_present_tod_data(unsigned char type, unsigned char index)
{
      unsigned long int location;
      
      location=mnth_pos;
      
      if(type==0)
        location=TOD_KWMD_LOC+(location*88);//+(disp_zone_ctr*11);
      else
        location=TOD_KVAMD_LOC+(location*88);//+(disp_zone_ctr*11);

      read_page_eeprom(location+(index*11),0,4);
      scratch=from_data_arr(0,0,4);
      if(index==t_prev_zone)
      {
          if(load_val[type]>last_stored_tod_load_val[type])
            scratch=scratch+(load_val[type]-last_stored_tod_load_val[type]);
      }
      
      return scratch;
}

void find_num_between_lp_entries()
{
  unsigned long int tmp_long, location;
  unsigned int f_index, l_index, m_index;
  
  scratch=SA_Range[0].Year & 0xff;
  scratch=(scratch<<8)+((SA_Range[0].Year>>8)&0xff);
  
  if(scratch>=2000)
    scratch=scratch-2000;
  else
    scratch=0;

  scratch=(scratch*13)+SA_Range[0].Month;
  scratch=(scratch*32)+SA_Range[0].Date;
  scratch=(scratch*25)+SA_Range[0].Hr;
  scratch=(scratch*60)+SA_Range[0].Min;

  scratch1=SA_Range[1].Year & 0xff;
  scratch1=(scratch1<<8)+((SA_Range[1].Year>>8)&0xff);
  
  if(scratch1>=2000)
    scratch1=scratch1-2000;
  else
    scratch1=0;

  scratch1=(scratch1*13)+SA_Range[1].Month;
  scratch1=(scratch1*32)+SA_Range[1].Date;
  scratch1=(scratch1*25)+SA_Range[1].Hr;
  scratch1=(scratch1*60)+SA_Range[1].Min;

  //scratch=from date, scratch1=to date
  if(scratch<scratch1)
  {
    tmp_long=scratch;
    scratch=scratch1;
    scratch1=tmp_long;
  }

  f_index=1;
  l_index=Load_Profile_Entries_In_Use;
  m_index=(f_index + l_index)/2;
  
  while (f_index <= l_index) 
  {
      tmp_long=(day_hr_pos+Load_Profile_Entries-m_index)%Load_Profile_Entries;
      location=LOAD_SURVEY_LOC+(tmp_long*12);
      tmp_long=from_eeprom(location,4);
      tmp_long=tmp_long&0xfffffff;  
  
      if(tmp_long < scratch)
          l_index = m_index-1;    
      else if(tmp_long > scratch)
          f_index = m_index+1;    
      else
      {
          f_index=m_index;
          break;
      }
 
      m_index = (f_index + l_index)/2;
   }

  range_start_entry=f_index;
  range_num_entries=0;

  f_index=1;
  l_index=Load_Profile_Entries_In_Use;
  m_index=(f_index + l_index)/2;
  
  while (f_index <= l_index) 
  {
      tmp_long=(day_hr_pos+Load_Profile_Entries-m_index)%Load_Profile_Entries;
      location=LOAD_SURVEY_LOC+(tmp_long*12);
      tmp_long=from_eeprom(location,4);
      tmp_long=tmp_long&0xfffffff;  
  
      if(tmp_long < scratch1)
          l_index = m_index-1;    
      else if(tmp_long > scratch1)
          f_index = m_index+1;    
      else
      {
          l_index=m_index;
          break;
      }
 
      m_index = (f_index + l_index)/2;
   }
  
   if((range_start_entry>Load_Profile_Entries_In_Use)||(l_index<1))
   {
        range_start_entry=0;
        range_num_entries=0;
   }
   else
   {
      if(l_index > range_start_entry)
        range_num_entries=(l_index - range_start_entry)+1;
      else
        range_num_entries=1;
   }  

  return;
}

void find_num_between_dlp_entries()
{ 
  unsigned long int tmp_long, location;
  unsigned int f_index, l_index, m_index;

  scratch=SA_Range[0].Year & 0xff;
  scratch=(scratch<<8)+((SA_Range[0].Year>>8)&0xff);
  
  if(scratch>=2000)
    scratch=scratch-2000;
  else
    scratch=0;

  scratch=(scratch*13)+SA_Range[0].Month;
  scratch=(scratch*32)+SA_Range[0].Date;

  scratch1=SA_Range[1].Year & 0xff;
  scratch1=(scratch1<<8)+((SA_Range[1].Year>>8)&0xff);
  
  if(scratch1>=2000)
    scratch1=scratch1-2000;
  else
    scratch1=0;

  scratch1=(scratch1*13)+SA_Range[1].Month;
  scratch1=(scratch1*32)+SA_Range[1].Date;

  //scratch=from date, scratch1=to date
  if(scratch<scratch1)
  {
    tmp_long=scratch;
    scratch=scratch1;
    scratch1=tmp_long;
  }
    
  f_index=1;
  l_index=Daily_Load_Profile_Entries_In_Use;
  m_index=(f_index + l_index)/2;
  
  while (f_index <= l_index) 
  {
      tmp_long=(day_pos+Daily_Load_Profile_Entries-m_index)%Daily_Load_Profile_Entries;
      location=DAILY_SURVEY_LOC+(tmp_long*11);
      tmp_long=from_eeprom(location,3);

      if(tmp_long < scratch)
          l_index = m_index-1;    
      else if(tmp_long > scratch)
          f_index = m_index+1;    
      else
      {
          f_index=m_index;
          break;
      }
 
      m_index = (f_index + l_index)/2;
   }

  range_start_entry=f_index;
  range_num_entries=0;

  f_index=1;
  l_index=Daily_Load_Profile_Entries_In_Use;
  m_index=(f_index + l_index)/2;
  
  while (f_index <= l_index) 
  {
      tmp_long=(day_pos+Daily_Load_Profile_Entries-m_index)%Daily_Load_Profile_Entries;
      location=DAILY_SURVEY_LOC+(tmp_long*11);
      tmp_long=from_eeprom(location,3);
  
      if(tmp_long < scratch1)
          l_index = m_index-1;    
      else if(tmp_long > scratch1)
          f_index = m_index+1;    
      else
      {
          l_index=m_index;
          break;
      }
 
      m_index = (f_index + l_index)/2;
   }
  
   if((range_start_entry>Daily_Load_Profile_Entries_In_Use)||(l_index<1))
   {
        range_start_entry=0;
        range_num_entries=0;
   }
   else
   {
      if(l_index > range_start_entry)
        range_num_entries=(l_index - range_start_entry)+1;
      else
        range_num_entries=1;
   }  
  return;
}

void get_dlp_data(unsigned int index) 
{
      //unsigned int i;//, ld_multi;//, hr_no;
      unsigned long int location, tmp_long;//, day_no;//, location1;
      
      tmp_long=(day_pos+Daily_Load_Profile_Entries-index)%Daily_Load_Profile_Entries;
      location=DAILY_SURVEY_LOC+(tmp_long*11);
      
      read_page_eeprom(location,0,11);

      tmp_long=from_data_arr(0,0,3);
      
      scratch=tmp_long%32;//day
      tmp_long=tmp_long/32;
      scratch=(scratch*100)+(tmp_long%13);//day+mnth
      tmp_long=tmp_long/13;
      scratch=(scratch*100)+tmp_long;//day+mnth+yr
      
      get_time_data(scratch,0);
      memcpy(stLoad_Profile.Load_Date,tmp_time_string,12);
      stLoad_Profile.E_Active=from_data_arr(3,0,4);
      stLoad_Profile.E_Apparent=from_data_arr(7,0,4);
      
      return;

}  

void get_lp_data(unsigned int index) 
{
      unsigned int i;//, ld_multi;//, hr_no;
      unsigned long int location, tmp_long;//, day_no;//, location1;
      
/*
  INJECT16(2002), DECEMBER, 4, WEDNESDAY, 10, 6, 11, 0xFF, INJECT16(120), 0x00, 
  INJECT32X(10001), // Ir
  INJECT32X(10002), // Iy
  INJECT32X(10003), // Ib
  INJECT16X(2411),  // Vr
  INJECT16X(2412),  // Vy
  INJECT16X(2413),  // Vb
  INJECT32X(111111), // KW
  INJECT32X(111112), // KVAr Lag
  INJECT32X(111113), // KVAr Lead
  INJECT32X(111114), // KVA
*/
      //hr_no=index%96;
      //day_no=(index-hr_no)/96;
      //day_no=cyclic(day_hr_pos+Daily_Load_Profile_Entries-1-day_no,Daily_Load_Profile_Entries);
      
      tmp_long=(day_hr_pos+Load_Profile_Entries-index)%Load_Profile_Entries;
      
      location=LOAD_SURVEY_LOC+(tmp_long*12);
      
      read_page_eeprom(location,0,12);
      
      //3+1(date and time)
      tmp_long=from_data_arr(0,0,4);
      
      //ld_multi=(tmp_long>>28)&0xf;
      tmp_long=tmp_long&0xfffffff;  
      
      i=tmp_long%60;//min
      tmp_long=tmp_long/60;
      scratch1=tmp_long%25;//hr
      tmp_long=tmp_long/25;
      scratch1=(scratch1*100)+i;//hr+min
      
      
      scratch=tmp_long%32;//day
      tmp_long=tmp_long/32;
      scratch=(scratch*100)+(tmp_long%13);//day+mnth
      tmp_long=tmp_long/13;
      scratch=(scratch*100)+tmp_long;//day+mnth+yr
      
      get_time_data(scratch,(scratch1*100));
      memcpy(stLoad_Profile.Load_Date,tmp_time_string,12);
      
      stLoad_Profile.E_Active=from_data_arr(4,0,2);
      stLoad_Profile.E_Apparent=from_data_arr(6,0,2);
      stLoad_Profile.Vrms=from_data_arr(8,0,2);
      stLoad_Profile.Irms=from_data_arr(10,0,2);
      
      if(stLoad_Profile.E_Active>=0xff00)
      {
          stLoad_Profile.E_Active=0;
          stLoad_Profile.E_Apparent=0;
          stLoad_Profile.Vrms=0;
          stLoad_Profile.Irms=0;
      }

      return;
}  

void get_tamper_data(unsigned char event_type, unsigned int index) 
{
     unsigned int tmp_int, max_events; //i, 
     unsigned long int location;//, location1;

    if(event_type==5)
       max_events=1;
    else
    {  
       max_events=EVENT_SIZE;             
    }

    if(event_type==0)
        location=VOLT_EVENT_LOC;
    else if(event_type==1)
        location=AMP_EVENT_LOC;
    else if(event_type==2)
        location=PFAIL_EVENT_LOC;
    else if(event_type==3)
        location=TRANSACT_EVENT_LOC;
    else if(event_type==4)
        location=OTHER_EVENT_LOC;
    else if(event_type==5)
        location=NOROLL_EVENT_LOC;
    else if(event_type==6)
        location=CTRL_EVENT_LOC;
      
    tmp_int=(event_pos[event_type]+max_events-index+1) % max_events;
    
    if((event_type==0)|| (event_type==1)||(event_type==4))
    {  
        tmp_int=(tmp_int*17);
        location=location+tmp_int;
        
        read_page_eeprom(location,0,17);
      
        stTamper_Profile.Tamper_ID=from_data_arr(0,0,2);

        scratch=from_data_arr(2,0,3);
        scratch1=from_data_arr(5,0,3);
        get_time_data(scratch, scratch1);
        memcpy(stTamper_Profile.Tamper_Date,tmp_time_string,12);
        
        stTamper_Profile.Vrms=from_data_arr(8,0,2);
        stTamper_Profile.Irms=from_data_arr(10,0,2);
        stTamper_Profile.PF=from_data_arr(12,0,1);
        stTamper_Profile.E_Active=from_data_arr(13,0,4);
     }
     else if(event_type==2)
     {
        tmp_int=(tmp_int*8);
        location=location+tmp_int;

        read_page_eeprom(location,0,8);
      
        stTamper_Profile.Tamper_ID=from_data_arr(0,0,2);
          
        scratch=from_data_arr(2,0,3);
        scratch1=from_data_arr(5,0,3);
        get_time_data(scratch, scratch1);
        memcpy(stTamper_Profile.Tamper_Date,tmp_time_string,12);
     }
     else
     {
        tmp_int=(tmp_int*8);
        location=location+tmp_int;

        read_page_eeprom(location,0,8);
      
        stTamper_Profile.Tamper_ID=from_data_arr(0,0,2);
        
        scratch=from_data_arr(2,0,3);
        scratch1=from_data_arr(5,0,3);
        get_time_data(scratch, scratch1);
        memcpy(stTamper_Profile.Tamper_Date,tmp_time_string,12);
     }
}  

void get_active_season_data(unsigned int index)
{
  unsigned int tmp_int;
  //unsigned char tmp_buf[5];
  
  if(prev_season>1)
    index=0;
  else
    index=prev_season;
  
  memcpy(stSeason_Profile.Season_Name, Active_Season_Name[index],10); 
  //tmp_buf[0]='w';tmp_buf[1]='e';tmp_buf[2]='e';tmp_buf[3]='k';tmp_buf[4]=49+index;
  //memcpy(stWeek_Profile.Week_Name, tmp_buf,5); 
  
  tmp_int=2000+d_yr;
  stSeason_Profile.Season_Date[0]=(tmp_int>>8)&0xff; //YR{H)
  stSeason_Profile.Season_Date[1]=tmp_int&0xff;      //YR(L) 
  stSeason_Profile.Season_Date[2]=active_season_date[index][0];
  stSeason_Profile.Season_Date[3]=active_season_date[index][1];
  stSeason_Profile.Season_Date[4]=0xFF;                 //WEEKDAY Not Specified
  stSeason_Profile.Season_Date[5]=active_season_date[index][2];
  stSeason_Profile.Season_Date[6]=active_season_date[index][3];
  stSeason_Profile.Season_Date[7]=0x00;      //SEC
  stSeason_Profile.Season_Date[8]=0xFF;                //HUNDREDTH of SEC  Not Specified
  stSeason_Profile.Season_Date[9]=0x80;                //DEV(H)   Not Specified
  stSeason_Profile.Season_Date[10]=0x00;                //DEV(L) 
  stSeason_Profile.Season_Date[11]=0;                //STAT 
  return;  
}  

void get_passive_season_data(unsigned int index)
{
  unsigned int tmp_int;
  //unsigned char tmp_buf[5];
  
  memcpy(stSeason_Profile.Season_Name, Passive_Season_Name[index],10); 
  //tmp_buf[0]='w';tmp_buf[1]='e';tmp_buf[2]='e';tmp_buf[3]='k';tmp_buf[4]=49+index;
  //memcpy(stWeek_Profile.Week_Name, tmp_buf,5); 
  
  tmp_int=2000+d_yr;
  stSeason_Profile.Season_Date[0]=(tmp_int>>8)&0xff; //YR{H)
  stSeason_Profile.Season_Date[1]=tmp_int&0xff;      //YR(L) 
  stSeason_Profile.Season_Date[2]=passive_season_date[index][0];
  stSeason_Profile.Season_Date[3]=passive_season_date[index][1];
  stSeason_Profile.Season_Date[4]=0xFF;                 //WEEKDAY Not Specified
  stSeason_Profile.Season_Date[5]=passive_season_date[index][2];
  stSeason_Profile.Season_Date[6]=passive_season_date[index][3];
  stSeason_Profile.Season_Date[7]=0x00;      //SEC
  stSeason_Profile.Season_Date[8]=0xFF;                //HUNDREDTH of SEC  Not Specified
  stSeason_Profile.Season_Date[9]=0x80;                //DEV(H)   Not Specified
  stSeason_Profile.Season_Date[10]=0x00;                //DEV(L) 
  stSeason_Profile.Season_Date[11]=0;                //STAT 
  return;  
}


void get_active_week_data(unsigned int index)
{
  unsigned int i;//, j;
  unsigned char tmp_buf[5];
  
  if(prev_season>1)
    index=0;
  else
    index=prev_season;
  
  tmp_buf[0]='w';tmp_buf[1]='e';tmp_buf[2]='e';tmp_buf[3]='k';tmp_buf[4]=49+index;
  memcpy(stWeek_Profile.Week_Name, tmp_buf,5); 
    
  for(i=0;i<7;i++)
    stWeek_Profile.Week_Id[i]=index+1;  
  return;  
}

void get_passive_week_data(unsigned int index)
{
  unsigned int i;//, j;
  unsigned char tmp_buf[5];
  
  tmp_buf[0]='w';tmp_buf[1]='e';tmp_buf[2]='e';tmp_buf[3]='k';tmp_buf[4]=49+index;
  memcpy(stWeek_Profile.Week_Name, tmp_buf,5); 
    
  for(i=0;i<7;i++)
    stWeek_Profile.Week_Id[i]=index+1;  
  return;  
}

void get_active_day_data(unsigned int index)
{
  unsigned int i, j;
  
  if(prev_season>1)
    index=0;
  else
    index=prev_season;
  
  stDay_Profile.day_id=index+1;
  
  for(i=0;i<8;i++)
  {  
    for(j=0;j<4;j++)
      stDay_Profile.tod_time[i][j]=tod_time[(index*8)+i][j];
    stDay_Profile.tariff_id[i]=tariff_id[index][i];
  }

  return;  
}

void get_passive_day_data(unsigned int index)
{
  unsigned int i, j;
  
  stDay_Profile.day_id=index+1;
  
  for(i=0;i<8;i++)
  {  
    for(j=0;j<4;j++)
      stDay_Profile.tod_time[i][j]=passive_tod_time[(index*8)+i][j];
    stDay_Profile.tariff_id[i]=passive_tariff_id[index][i];
  }  
  return;  
}

void history_func(void)
{
        unsigned char i, k, tmp_byte, intgr_ctr[3]; 
	unsigned int j, tmp_int, prev_slot, curr_slot, curr_time, curr_bill_time, single_bill_time;
        unsigned long int tmp_long, location, location1, curr_date, curr_bill_date, single_bill_date, prev_bill_date;
        
        curr_time=t_hr;
        curr_time=(curr_time*100)+t_min;
        
        curr_date=d_yr;
        curr_date=(curr_date*100)+d_mnth;
	curr_date=(curr_date*100)+d_day;
	
        curr_bill_time=bill_time[0];
        curr_bill_time=(curr_bill_time*100)+bill_time[1];
        
        curr_bill_date=d_yr;
        curr_bill_date=(curr_bill_date*100)+d_mnth;
        curr_bill_date=(curr_bill_date*100)+bill_day[3];

        prev_bill_date=prev_yr;
        prev_bill_date=(prev_bill_date*100)+prev_mnth;
        prev_bill_date=(prev_bill_date*100)+bill_day[3];

        single_bill_time=scheduled_bill_time[0];
        single_bill_time=(single_bill_time*100)+scheduled_bill_time[1];

	i=scheduled_bill_day[0];
        i=(i*256)+scheduled_bill_day[1];
                  
        single_bill_date=i%100;
        single_bill_date=(single_bill_date*100)+scheduled_bill_day[2];
	single_bill_date=(single_bill_date*100)+scheduled_bill_day[3];

	if((d_day!=prev_day)||(d_mnth!=prev_mnth)||(t_hr!=(prev_hr&0x7f))||(t_min!=prev_min))
        {
            //switch passive tod to active tod
         
            if(activate_date!=0)
            {  
                    scratch=d_yr;
                    scratch=(scratch*13)+d_mnth;
                    scratch=(scratch*32)+d_day;
                    scratch=(scratch*25)+t_hr;
                    scratch=(scratch*60)+t_min;
         
                    scratch1=activate_date%100;
                    i=(activate_date/100)%100;
                    scratch1=(scratch1*13)+i;
                    i=(activate_date/10000);
                    scratch1=(scratch1*32)+i;
                    i=(activate_time/100)%100;
                    scratch1=(scratch1*25)+i;
                    i=activate_time%100;
                    scratch1=(scratch1*60)+i;
                    
                    if(scratch>=scratch1)
                    {
                            tod_passive_to_active();
                            md_reset_b=1;
                    }
            }            

            //#define LST_MNTH_DATA_LOC       489	//2481 //2,3,3,2,1  //power_on, tamper cnt, pf_cnt, pf_ctr, lvolt
	    
            reset_on_min_ctr++;
            if(reset_on_min_ctr>=3) // power on time after md reset
            {	      
      		reset_on_time=reset_on_time+reset_on_min_ctr;
                to_eeprom(LST_MNTH_DATA_LOC,reset_on_time,2);
                Cum_Power_On_Dur=Cum_Power_On_Dur+reset_on_min_ctr;
                to_eeprom(PON_DUR_LOC, Cum_Power_On_Dur,4);
                reset_on_min_ctr=0;
            }
            
            
            if(ep_clear_stat==0)
            {    
              write_eeprom(KWMD_MIN_FLAG_LOC+prev_min,prev_hr&0x7f);
              to_eeprom(VOLT_MIN_LOC+(prev_min*2),vrms_reg3,2);
              to_eeprom(AMP_MIN_LOC+(prev_min*2),irms_reg3[channel],2);
              //write_eeprom(PF_MIN_LOC+prev_min,pf_reg3/10);
            }    
            else
              ep_clear_stat=0;
            
            prev_min=t_min;
	    
            
            prev_slot=read_eeprom(PREV_MIN_SLOT_LOC);
            curr_slot=t_min/md_intgr_val;
            if((prev_slot!=curr_slot)||(d_day!=prev_day)||(d_mnth!=prev_mnth)||(t_hr!=prev_hr))
            {
                    scratch1=(prev_hr&0x7f);
                    scratch1=(scratch1*100);
                    k=(prev_slot+1)*md_intgr_val;
                    
                    //md calculation
                    if(k<60)
                        scratch1=scratch1+k;
                    else
                    {
                      scratch1=scratch1+100;
                      if(scratch1==2400)//to make 00 hrs with present date
                        scratch1=0;
                    }

                    store_md_data(scratch1);		
                    
                    write_eeprom(PREV_MIN_SLOT_LOC,curr_slot);
                    rd_time=0;
            }
            
            prev_slot=read_eeprom(PREV_SURVEY_MIN_SLOT_LOC);
            curr_slot=t_min/survey_intgr_val;
            if((prev_slot!=curr_slot)||(d_day!=prev_day)||(d_mnth!=prev_mnth)||(t_hr!=prev_hr))
            {
                    //load survey calculation
                    /*
                    k=(i+1)*survey_intgr_val;
                    if(k>60)
                        i=(60/survey_intgr_val)-1;
                    
                    k=(60/survey_intgr_val);
                    
                    tmp_byte=prev_hr&0x7f;
                    tmp_byte=(tmp_byte*k)+i;
                    
                    */
              
                    tmp_byte=(prev_hr&0x7f);
                    k=(prev_slot+1)*survey_intgr_val;
                    
                    if(k>=60)
                    {
                      tmp_byte=tmp_byte+1;
                      k=0;
                      prev_slot=(60/survey_intgr_val)-1;

                      //if(tmp_byte==24)//to make 00 hrs with present date
                      //    tmp_byte=0;
                    } 
                    
                    if(tmp_byte==24)
                    {
                      //present date
                      //scratch=d_yr;
                      //scratch=(scratch*13)+d_mnth;
                      //scratch=(scratch*32)+d_day;
                      
                      tmp_long=find_next_date(prev_date);
                      tmp_byte=0;
                      k=0;
                    }
                    else
                    {  
                      //prev date
                      tmp_long=prev_yr;
                      tmp_long=(tmp_long*13)+prev_mnth;
                      tmp_long=(tmp_long*32)+prev_day;
                    }
                    
                    tmp_long=(tmp_long*25)+tmp_byte;
                    tmp_long=(tmp_long*60)+k;
                    
                    scratch1=(60/survey_intgr_val);
                    tmp_long=tmp_long+(scratch1<<28); //contains date,time and intgr period
        
                    k=(prev_slot*survey_intgr_val);
                    
                    demand_volt=0;
                    demand_amp=0;
                    demand_pf=0;
                    
                    for(i=0;i<3;i++)
                      intgr_ctr[i]=0;
                    
                    for(i=0;i<survey_intgr_val;i++)
                    {
                        tmp_byte=read_eeprom(KWMD_MIN_FLAG_LOC+k+i);
                        if(tmp_byte==(prev_hr&0x7f))
                        {
                            tmp_int=from_eeprom(VOLT_MIN_LOC+((k+i)*2),2);
                            if(tmp_int>500)
                            {  
                              demand_volt=demand_volt+tmp_int;
                              intgr_ctr[0]++;
                            }
                            
                            tmp_int=from_eeprom(AMP_MIN_LOC+((k+i)*2),2);
                            if(tmp_int>20)
                            {
                              demand_amp=demand_amp+tmp_int;
                              intgr_ctr[1]++;
                            }
  
                            /*
                            tmp_byte=read_eeprom(PF_MIN_LOC+k+i);
                            if(tmp_byte>20)
                            {
                              demand_pf[j]=demand_pf[j]+tmp_byte;
                              intgr_ctr[2]++;
                            } 
                            */
                        }
                    }
                    
                    if(intgr_ctr[0]>0)
                      demand_volt=demand_volt/intgr_ctr[0];
                    if(intgr_ctr[1]>0)
                      demand_amp=demand_amp/intgr_ctr[1];
                    //if(intgr_ctr[2]>0)
                    //  demand_pf=demand_pf/intgr_ctr[2];
                  
                      
                    k=0; //adjustment value of kvah to match with kwh value in load survey
                    if(day_hr_pos<Load_Profile_Entries)
                    {
                      location=LOAD_SURVEY_LOC+(day_hr_pos*12);
                      
                      //write date and time
                      //to_eeprom(location,prev_date,3);
                      //write_eeprom(location+3,tmp_byte);
                      to_eeprom(location,tmp_long,4);
                      
                      
                      for(i=0;i<2;i++)
                      {  
                        if(load_val[i]>=last_hr_load_val[i])
                          scratch=load_val[i]-last_hr_load_val[i];
                        else
                          scratch=0;
                        
                        if(i==0)
                          scratch1=scratch;
                        else
                        {
                          if(scratch<scratch1)
                          {
                            k=scratch1-scratch;
                            scratch=scratch1;
                          }  
                        }  
                        to_eeprom(location+4+(i*2),scratch,2);
                        block_load_val[i]=scratch;
                      }
                      to_eeprom(location+8,demand_volt,2);
                      block_vrms=demand_volt;
                      to_eeprom(location+10,demand_amp,2);
                      block_irms=demand_amp;
                    }

                    day_hr_pos=day_hr_pos+1;
                    if(day_hr_pos>=Load_Profile_Entries)
                      day_hr_pos=0;

                    to_eeprom(HRPOS_LOC,day_hr_pos,2);

                    if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
                    {
                      Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+1;
                      to_eeprom(HRCNT_LOC,Load_Profile_Entries_In_Use,2);
                    }
                    
                    fill_load_survey_entries(tmp_long, prev_slot, curr_slot);
                    
                    for(i=0;i<2;i++)
                    {
                      
                            if(i==0)
                              last_hr_load_val[i]=load_val[i];
                            else
                              last_hr_load_val[i]=load_val[i]+k;
                              
                            to_eeprom(LAST_HR_KWH_LOC+(i*4),load_val[i],4);
                    }
                    
                    write_eeprom(PREV_SURVEY_MIN_SLOT_LOC,curr_slot);
            }
        }

        if((d_day!=prev_day)||(d_mnth!=prev_mnth)||(t_hr!=prev_hr))
	{
              write_eeprom(HR_LOC,t_hr);	  
	}

        if((d_day!=prev_day)||(d_mnth!=prev_mnth))
	{
              if(day_pos<Daily_Load_Profile_Entries)//day_pos
              {
                  location=DAILY_SURVEY_LOC+(day_pos*11);//day_pos
                  //if(load_val[0]>last_day_kwh_val)
                  //	scratch=load_val[0]-last_day_kwh_val;
                  //else
                  //	scratch=0;
                  //to_eeprom(location,scratch,2);
                  scratch=d_yr;
                  scratch=(scratch*13)+d_mnth;
                  scratch=(scratch*32)+d_day;

                  to_eeprom(location,scratch,3);//real_date
                  if(load_val[0]>load_val[1])
                    to_eeprom(location+3,load_val[1],4);
                  else
                    to_eeprom(location+3,load_val[0],4);
                  
                  to_eeprom(location+7,load_val[1],4);
              }
              
              /*
              day_hr_pos=day_hr_pos+1;
              if(day_hr_pos>=Daily_Load_Profile_Entries)
                day_hr_pos=0;
              */
              
              day_pos=day_pos+1;
              if(day_pos>=Daily_Load_Profile_Entries)
                day_pos=0;
              
              //for(j=0;j<96;j++)
              //  write_eeprom(LOAD_SURVEY_LOC+(day_hr_pos*2016)+(j*21), 0xff);
              //to_eeprom(LOAD_DATE_LOC+(day_hr_pos*3),real_date,3);

              
              if(Daily_Load_Profile_Entries_In_Use<Daily_Load_Profile_Entries)
              {
                Daily_Load_Profile_Entries_In_Use=Daily_Load_Profile_Entries_In_Use+1;
                write_eeprom(DAYCNT_LOC,Daily_Load_Profile_Entries_In_Use);
              }
                    
              write_eeprom(DAYPOS_LOC,day_pos);
              //write_eeprom(HRPOS_LOC,day_hr_pos);
              to_eeprom(DATE_LOC, real_date, 3);
              
 	}

        if((last_bill_date<prev_bill_date)&&(prev_bill_date<curr_bill_date))
            md_reset_b=2;
        
        if((curr_date>=single_bill_date)&&(curr_time>=single_bill_time)&&(scheduled_bill_day[3]>0))
            md_reset_b=3;
        
        if
        (
          (md_reset_b==1)||(md_reset_b==2)||(md_reset_b==3)||
          (
	          ((d_day>bill_day[3])||((d_day==bill_day[3])&&(curr_time>=curr_bill_time)))&&
	          ((last_bill_date<curr_bill_date)||((last_bill_date==curr_bill_date)&&(last_bill_time!=curr_bill_time)))
          )
        )
        {
                /*                      
		disp_all_lcd(0x00);	                                  
                data[0]=0x60;//r
		data[1]=0xE9;//e
		data[2]=0xCB;//s
		data[3]=0xE9;//e
		data[4]=0xE1;//t
		write_data_in_lcd();
                */
          
                reset_on_time=reset_on_time+reset_on_min_ctr;
                to_eeprom(LST_MNTH_DATA_LOC,reset_on_time,2);
                Cum_Power_On_Dur=Cum_Power_On_Dur+reset_on_min_ctr;
                to_eeprom(PON_DUR_LOC, Cum_Power_On_Dur,4);
                reset_on_min_ctr=0; 
                
                 scratch=from_eeprom(LST_MNTH_DATA_LOC+6,3);
		if(all_tamper_cnt>scratch)	
                  scratch=all_tamper_cnt-scratch;
		else
                  scratch=0;
		to_eeprom(BILLPT_LOC+6,scratch,2);
		
                location=KWMD_LOC+(mnth_pos*19);
		to_eeprom(location,real_date,3);
		to_eeprom(location+3,real_time/100,2);
		to_eeprom(location+5,load_val[0],4);
                to_eeprom(location+16,reset_on_time,2);
                write_eeprom(location+18,avg_pf);//pf
		
                get_time_data(real_date, ((real_time/100)*100));
                for(i=0;i<12;i++)
                  Last_MD_Rst_DT[i+2]=tmp_time_string[i];
		
		scratch=from_eeprom(location+9,2);
		kwcd_val=kwcd_val+scratch;
		to_eeprom(KWCD_LOC,kwcd_val,4);

		location=KVAMD_LOC+(mnth_pos*11);
		to_eeprom(location,load_val[1],4);
		
                bkp_kwh_val=load_val[0];
                bkp_kwmd_val=kwmd_val;
                bkp_kwmd_date=kwmd_date;
                bkp_kwmd_time=kwmd_time;
                bkp_pf_val=avg_pf;
                
                bkp_kvah_val=load_val[1];
    	        bkp_kvamd_val=kvamd_val;
                bkp_kvamd_date=kvamd_date;
                bkp_kvamd_time=kvamd_time;
		
		store_energy_in_zone();

		location=TOD_KWMD_LOC+(mnth_pos*88);
                j=cyclic(mnth_pos,HISTORY_SIZE); //13 months including current month	
                location1=TOD_KWMD_LOC+(j*88);
        
                for(tmp_long=location1;tmp_long<location1+88;tmp_long++)
                  write_eeprom(tmp_long,0);
                for(j=0;j<TOD_SIZE;j++)//TOD_SIZE
                {
                  scratch=from_eeprom(location+(j*11),4);
                  to_eeprom(location1+(j*11),scratch,4);
                }
                
                location=TOD_KVAMD_LOC+(mnth_pos*88);
                j=cyclic(mnth_pos,HISTORY_SIZE); //13 months including current month	
                location1=TOD_KVAMD_LOC+(j*88);
                for(tmp_long=location1;tmp_long<location1+88;tmp_long++)
                  write_eeprom(tmp_long,0);
                for(j=0;j<TOD_SIZE;j++)//TOD_SIZE
                {
                  scratch=from_eeprom(location+(j*11),4);
                  to_eeprom(location1+(j*11),scratch,4);
                }
                
                mnth_pos=cyclic(mnth_pos,HISTORY_SIZE); //13 months including current month	
        	
		location=KWMD_LOC+(mnth_pos*19);
                for(tmp_long=location;tmp_long<location+19;tmp_long++)
                  write_eeprom(tmp_long,0);
                
                location=KVAMD_LOC+(mnth_pos*11);
                for(tmp_long=location;tmp_long<location+11;tmp_long++)
                  write_eeprom(tmp_long,0);
		
                kwmd_val=0;
                kwmd_date=0;
                kwmd_time=0;
                
                kvamd_val=0;
                kvamd_date=0;
                kvamd_time=0;
                
                get_time_data(0,0);
                for(i=0;i<12;i++)
                {
                  KWMD_DT[i+2]=tmp_time_string[i];
                  KVAMD_DT[i+2]=tmp_time_string[i];
                }
          

		//for(j=0;j<60;j++)
		//	write_eeprom(KWMD_MIN_LOC+j,0);
		
		for(i=0;i<3;i++)
			to_eeprom(LST_MNTH_DATA_LOC+(i*2),0,2);//100
		reset_on_time=0;
                
		to_eeprom(LST_MNTH_DATA_LOC+6,all_tamper_cnt,3);
		to_eeprom(LST_MNTH_DATA_LOC+9,load_val[0],4);
		to_eeprom(LST_MNTH_DATA_LOC+13,load_val[1],4);
		
		md_reset_cnt=md_reset_cnt+1;
		to_eeprom(MD_RESET_LOC,md_reset_cnt,2);  
		write_eeprom(MNTHPOS_LOC,mnth_pos);        

                if(md_reset_cnt<(Billing_Profile_Entries-1))
                  Billing_Profile_Entries_In_Use=md_reset_cnt+1;
                else
                  Billing_Profile_Entries_In_Use=Billing_Profile_Entries;

		
		if(md_reset_b!=1)
		{
			if(md_reset_b==2)
			        last_bill_date=prev_bill_date;	
			//else if(md_reset_b==3)
				//last_bill_date=curr_date;
			else if(md_reset_b!=3)
			        last_bill_date=curr_bill_date;
			
                        if(md_reset_b==3)
			{
                              for(i=0;i<4;i++)  
                              {
                                scheduled_bill_day[i]=0;
                                write_eeprom(SCHEDULED_BILL_DAY_LOC+i,scheduled_bill_day[i]);
                              }
                              
                              for(i=0;i<2;i++)  
                              {
                                scheduled_bill_time[i]=0;
                                write_eeprom(SCHEDULED_BILL_TIME_LOC+i,scheduled_bill_time[i]);
                              }
                       }	
                        
			//last_bill_day=billing_day;
                        //if(md_reset_b==3)
			//	last_bill_time=curr_time;
			//else
	                if(md_reset_b!=3)
                          last_bill_time=curr_bill_time;
                        
                        //write_eeprom(LAST_BILL_DAY_LOC,last_bill_day);
                        to_eeprom(LAST_BILL_TIME_LOC,last_bill_time,2);
			to_eeprom(LAST_BILL_DATE_LOC,last_bill_date,3);
		}
		
                //if(md_reset_b==0)
                //  store_event_data(TRANSACT_EVENT, 154,0);//occ     
                
                //last_kwh_val=load_val[0];
		md_reset_b=0;
	}

    return;
}


void reset_ls_data(void)
{
	unsigned char curr_slot,num_slots;
	unsigned char i,j,tmp_byte;
	unsigned int tmp_int;
	unsigned int location;
	//unsigned int block_load_val[2],block_vrms=0;
	
	curr_slot=t_min/survey_intgr_val;
        
        day_hr_pos=0;
        Load_Profile_Entries_In_Use=0;
        to_eeprom(HRCNT_LOC,Load_Profile_Entries_In_Use,2);

	num_slots=(60/survey_intgr_val);//num slots in an hour

	if(curr_slot>=num_slots)
		j=0;
	else
		j=curr_slot;
	//find total slots to fill for present day
	if(t_hr>=24)
		tmp_byte=0;
	else
		tmp_byte=t_hr;
	j=j+(num_slots*tmp_byte);
        
        if(j>0)
        {

                scratch=d_yr;
                scratch=(scratch*13)+d_mnth;
                scratch=(scratch*32)+d_day;
                scratch=(scratch*25)+0;
                scratch=(scratch*60)+0;

                for(i=0;i<j;i++)
                {
                        tmp_int=survey_intgr_val;
                //tmp_int=tmp_int*(i+1);
                        scratch=scratch+tmp_int;

                        location=LOAD_SURVEY_LOC+(day_hr_pos*12);

                        //write date and time
                        //to_eeprom(location,prev_date,3);
                        //write_eeprom(location+3,tmp_byte);
                        to_eeprom(location,scratch,4);
                        write_eeprom(location+4,0xff);
                        block_load_val[0]=0;block_load_val[1]=0;block_vrms=0;block_irms=0;
                          
                        day_hr_pos=day_hr_pos+1;
                        if(day_hr_pos>=Load_Profile_Entries)//3456
                          day_hr_pos=0;
                }

                to_eeprom(HRPOS_LOC,day_hr_pos,2);
                if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
                {
                  Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+j;
                  if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
                    Load_Profile_Entries_In_Use=Load_Profile_Entries;
                  to_eeprom(HRCNT_LOC,Load_Profile_Entries_In_Use,2);
                }
        }									

	for(i=0;i<2;i++)
	{
		last_hr_load_val[i]=load_val[i];
		to_eeprom(LAST_HR_KWH_LOC+(i*4),load_val[i],4);
	}
	write_eeprom(PREV_SURVEY_MIN_SLOT_LOC,curr_slot);
}


void fill_load_survey_entries(unsigned long int last_time, unsigned char prev_slot, unsigned char curr_slot)
{
		unsigned char i, tmp_byte, num_slots; //k, 
		unsigned int j, tmp_int;
		unsigned long int location;//,tmp_long, location1;

		num_slots=(60/survey_intgr_val);//num slots in an hour
		
		if((d_day!=prev_day)||(d_mnth!=prev_mnth))
		{
			if((prev_slot+1)>=num_slots)
				j=0;
			else
				j=num_slots-(prev_slot+1);
	
			tmp_byte=(prev_hr&0x7f);
			tmp_byte=tmp_byte+1;
			
			//find total slots to fill for prev day
			if(tmp_byte>=24)
				tmp_byte=0;
			else
				tmp_byte=24-tmp_byte;
			
			j=j+(num_slots*tmp_byte);
			
			if(j>0)
			{
				for(i=0;i<j;i++)
				{
					tmp_int=survey_intgr_val;
	          		//tmp_int=tmp_int*(i+1);
					last_time=last_time+tmp_int;
					if(i==(j-1))
					{
						last_time=find_next_date(prev_date);
						last_time=(last_time*25)+0;
						last_time=(last_time*60)+0;
					}
	
					location=LOAD_SURVEY_LOC+(day_hr_pos*12);
	
					//write date and time
					//to_eeprom(location,prev_date,3);
					//write_eeprom(location+3,tmp_byte);
					to_eeprom(location,last_time,4);
					write_eeprom(location+4,0xff);
                                        block_load_val[0]=0;block_load_val[1]=0;block_vrms=0;block_irms=0;
                                        
                                        day_hr_pos=day_hr_pos+1;
                                        if(day_hr_pos>=Load_Profile_Entries)//3456
                                          day_hr_pos=0;
		
				}
	
                                to_eeprom(HRPOS_LOC,day_hr_pos,2);
                                if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
                                {
                                  Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+j;
                                  if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
                                    Load_Profile_Entries_In_Use=Load_Profile_Entries;
                                  to_eeprom(HRCNT_LOC,Load_Profile_Entries_In_Use,2);
                                }
			}									
					
	
			if(curr_slot>=num_slots)
				j=0;
			else
				j=curr_slot;
	
			//find total slots to fill for present day
			if(t_hr>=24)
				tmp_byte=0;
			else
				tmp_byte=t_hr;
			
			j=j+(num_slots*tmp_byte);
			
			if(j>0)
			{
	
                                scratch=d_yr;
                                scratch=(scratch*13)+d_mnth;
                                scratch=(scratch*32)+d_day;
				scratch=(scratch*25)+0;
				scratch=(scratch*60)+0;
	
				for(i=0;i<j;i++)
				{
					tmp_int=survey_intgr_val;
	          		//tmp_int=tmp_int*(i+1);
					scratch=scratch+tmp_int;
	
					location=LOAD_SURVEY_LOC+(day_hr_pos*12);
	
					//write date and time
					//to_eeprom(location,prev_date,3);
					//write_eeprom(location+3,tmp_byte);
					to_eeprom(location,scratch,4);
					write_eeprom(location+4,0xff);
                                        block_load_val[0]=0;block_load_val[1]=0;block_vrms=0;block_irms=0;
                                          
                                        day_hr_pos=day_hr_pos+1;
                                        if(day_hr_pos>=Load_Profile_Entries)//3456
                                          day_hr_pos=0;
				}

                                to_eeprom(HRPOS_LOC,day_hr_pos,2);
                                if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
                                {
                                  Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+j;
                                  if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
                                    Load_Profile_Entries_In_Use=Load_Profile_Entries;
                                  to_eeprom(HRCNT_LOC,Load_Profile_Entries_In_Use,2);
                                }
			}									
		}
		else
		{
				
			tmp_byte=(prev_hr&0x7f);
			
			if(tmp_byte>=t_hr)
			{
				if((prev_slot+1)>=curr_slot)
					j=0;
				else
					j=curr_slot-(prev_slot+1);
					
				if(j>=4)
					j=0;
				
				tmp_byte=0;		
			}
			else
			{
				if((prev_slot+1)>=num_slots)
					j=0;
				else
					j=num_slots-(prev_slot+1);
		
				if(curr_slot<num_slots)
					j=j+curr_slot;

				tmp_byte=tmp_byte+1;					
				tmp_byte=t_hr-tmp_byte;
			}
			
			j=j+(num_slots*tmp_byte);
			
			if(j>0)
			{
				for(i=0;i<j;i++)
				{
					tmp_int=survey_intgr_val;
	          		//tmp_int=tmp_int*(i+1);
					last_time=last_time+tmp_int;
	
					location=LOAD_SURVEY_LOC+(day_hr_pos*12);
	
					//write date and time
					//to_eeprom(location,prev_date,3);
					//write_eeprom(location+3,tmp_byte);
					to_eeprom(location,last_time,4);
					write_eeprom(location+4,0xff);
                                        block_load_val[0]=0;block_load_val[1]=0;block_vrms=0;block_irms=0;
                                            
                                        day_hr_pos=day_hr_pos+1;
                                        if(day_hr_pos>=Load_Profile_Entries)//3456
                                          day_hr_pos=0;
		
				}
	
                                to_eeprom(HRPOS_LOC,day_hr_pos,2);
                                if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
                                {
                                  Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+j;
                                  if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
                                    Load_Profile_Entries_In_Use=Load_Profile_Entries;
                                  to_eeprom(HRCNT_LOC,Load_Profile_Entries_In_Use,2);
                                }
			}									
		}
}

unsigned long int find_next_date(unsigned long int curr_date)
{
	unsigned long int next_date;
	unsigned char i,j,tmp_day, tmp_mnth, tmp_yr;
	
	tmp_yr=curr_date%100;
	tmp_mnth=(curr_date/100)%100;
	tmp_day=(curr_date/10000);
	
	if(tmp_yr>99)
		tmp_yr=0;
	
	if((tmp_mnth<1)||(tmp_mnth>12))
		tmp_mnth=1;
	
	if((tmp_day<1)||(tmp_day>31))
		tmp_day=1;
		
	j=months[tmp_mnth-1];
	if(tmp_mnth==2)
	{
	  i=(2000+tmp_yr)%4;
	  if(i==0)
	  	j=29;	
	}	

	tmp_day=tmp_day+1;
	if(tmp_day>j)
	{
		tmp_day=1;
		tmp_mnth++;
	
		if(tmp_mnth>12)
		{
			tmp_mnth=1;
			tmp_yr++;
			
			if(tmp_yr>99)
				tmp_yr=0;
		}	
	}
	
    next_date=tmp_yr;
    next_date=(next_date*13)+tmp_mnth;
    next_date=(next_date*32)+tmp_day;
	return next_date;
}

/*
void find_nd_tamper(void)
{
  
  
}
*/
//1 Phase
//rev(0),el(1),mag(2),nd(3),nm(4)    
//current 	Reverse(51-52),EL(69-70)
//other events	magnet(201,202), nd(203,204), NM(207,208)
//const unsigned char event_ids[10]={51,69,201,203,207};


void tamper_func(void)
{

    unsigned char tmp_event_type, tmp_event_id, indv_tamper_stat; //, curr_tamper_stat,tmp_byte, 
    unsigned int i, tamper_persist_ctr;//, j, k
    unsigned long int tmp_tamper_stat;
    //int tmp_int;
    //long int tmp_long;
    //unsigned long int location, location1;
    
    for(i=0;i<TAMPER_TYPE;i++)
    {
        tmpr_time_on_ctr[i]=tmpr_time_on_ctr[i]+1;
        tmpr_time_off_ctr[i]=tmpr_time_off_ctr[i]+1;
    }

    tmp_tamper_stat=0;//store_tamper_stat;

    
    
//rev(0),el(1)
//current 	Reverse(51-52),EL(69-70)

    //reverse
    indv_tamper_stat=(store_tamper_stat>>0)&0x000001; //stored tamper stat
    //all voltages > 180 volts ie 75% of VREF and current > 1A ie 10% of IB and PF > .2
    if((rev_stat[0]==1)||(rev_stat[1]==1))
      tmp_tamper_stat=(tmp_tamper_stat|((0x000001)<<0));
    //else if(((rev_stat[i]==1)||(irms_reg3[i]<100)||(pf_reg3[i]<200))&&(indv_tamper_stat==1))  
    else if(((rev_stat[0]==2)&&(rev_stat[1]==2))&&(indv_tamper_stat==1))  	
      tmp_tamper_stat=(tmp_tamper_stat|((0x000001)<<0));
  
    //earth load
    indv_tamper_stat=(store_tamper_stat>>1)&0x000001; //stored tamper stat
    //all voltages > 180 volts ie 75% of VREF and current > 1A ie 10% of IB and PF > .2
    if(eload_stat==1)//&&(nd_stat==0))
      tmp_tamper_stat=(tmp_tamper_stat|((0x000001)<<1));
    else if((eload_stat==2)&&(indv_tamper_stat==1))  
      tmp_tamper_stat=(tmp_tamper_stat|((0x000001)<<1));//(2^)7,8,9
    
    
//mag(2),nd(3),nm(4)    
//other events	magnet(201,202), nd(203,204), NM(207,208)
    
    /*
    //magnetic tamper
    if((P1IN & 0x04) == 0x00)
    {
      tmp_tamper_stat=(tmp_tamper_stat|((unsigned long int)(0x000001)<<2));//current unbalance
      magnetic_stat=1;
    }
    else
      magnetic_stat=0;
    */
    
    if(mag_imax_stat==1)
      tmp_tamper_stat=(tmp_tamper_stat|((unsigned long int)(0x000001)<<2));//current unbalance
      
    indv_tamper_stat=(store_tamper_stat>>3)&0x000001; //stored tamper stat
    if(nd_stat==1)
      tmp_tamper_stat=(tmp_tamper_stat|((unsigned long int)(0x000001)<<3));//nd tamper
    else if((vrms_reg3<1800)&&(indv_tamper_stat==1))
      tmp_tamper_stat=(tmp_tamper_stat|((unsigned long int)(0x000001)<<3));//nd tamper

    indv_tamper_stat=(store_tamper_stat>>4)&0x000001; //stored tamper stat
    if(nmiss_stat==1)
      tmp_tamper_stat=(tmp_tamper_stat|((unsigned long int)(0x000001)<<4));//nm tamper
    else if((vrms_reg3<1800)&&(indv_tamper_stat==1))
      tmp_tamper_stat=(tmp_tamper_stat|((unsigned long int)(0x000001)<<4));//nm tamper

    /*
    if(pf_reg3<200)
    {  
        indv_tamper_stat=(store_tamper_stat>>17)&0x000001; //stored tamper stat
        if((pf_reg3<200)&&((irms_reg3[0]>100)||(irms_reg3[1]>100)))
          tmp_tamper_stat=(tmp_tamper_stat|((unsigned long int)(0x000001)<<17));//low pf
        else if((pf_reg3<300)&&(indv_tamper_stat==1))
          tmp_tamper_stat=(tmp_tamper_stat|((unsigned long int)(0x000001)<<17));//low pf
    }    
    */
    
    for(i=0;i<TAMPER_TYPE;i++)
    {
        //if(i<=5) //voltage tamper
        //  tmp_event_type=VOLT_EVENT;
        if(i<=1) //current tamper
          tmp_event_type=AMP_EVENT;
        else if(i<=4) //other tamper
          tmp_event_type=OTHER_EVENT;
        tmp_event_id=event_ids[i];

        //location=INST_TAMPER_OCC_LOC+(i*16);
        //location1=INST_TAMPER_RES_LOC+(i*16);
        
        //j=read_eeprom(location);//occ registered
        //k=read_eeprom(location1);//res registered
        
        if((tmp_tamper_stat & ((unsigned long int)(0x000001)<<i))==((unsigned long int)(0x000001)<<i)) //tamper on
	{
            /*
            if(k==1)
                write_eeprom(location1,0); //res registered
            else if((j==0)&&(k==0)&&(tmpr_time_on_ctr[i]>=3))//10
            {
                write_eeprom(location,1);//occ registered
                to_eeprom(location+1,real_date,3);
                to_eeprom(location+4,real_time,3);
                to_eeprom(location+7,vrms_reg3,2);
                to_eeprom(location+9,irms_reg3[channel],2);
                write_eeprom(location+11,pf_reg3/10);
                to_eeprom(location+12,load_val[0],4);
            }
            */
          
            if(i==2)//magnet
              tamper_persist_ctr=15;//10;//28;//182;//15;
            else
              tamper_persist_ctr=60;//182;//Tamper_Occ_Time;//TAMPER_DELAY;
            
            if(tmpr_time_on_ctr[i]>=tamper_persist_ctr)
            {
                if((store_tamper_stat&((unsigned long int)(0x000001)<<i))==0x000000)
                {
                    store_tamper_stat=store_tamper_stat | ((unsigned long int)(0x000001)<<i);

                    //if(i==2)
                    //  mag_imax_stat=1;
                    
                    tamper_snap_index=i;
                    //write_eeprom(location,0);//occ registered
                    store_event_data(tmp_event_type, tmp_event_id,0);
        
                    last_tamper_occ_type=i+1;    
                    write_eeprom(LSTTAMPER_OCC_LOC,last_tamper_occ_type);

                    /*
                    scratch=from_eeprom(location+1,3);
                    to_eeprom(LSTTAMPER_OCC_LOC+1,scratch,3);//real_date

                    scratch=from_eeprom(location+4,3);
                    to_eeprom(LSTTAMPER_OCC_LOC+4,scratch,3);//real_time
                    */
                    
                    to_eeprom(LSTTAMPER_OCC_LOC+1,real_date,3);//real_date
                    to_eeprom(LSTTAMPER_OCC_LOC+4,real_time,3);//real_time

                    to_eeprom(ALLTAMPER_LOC,store_tamper_stat,3);
                    all_tamper_cnt=all_tamper_cnt+1;
                    tamper_cnt[i]=tamper_cnt[i]+1;
                    to_eeprom(TAMPERCNTARR_LOC+(i*2),tamper_cnt[i],2);
                }
		tmpr_time_on_ctr[i]=0;
            }		
            tmpr_time_off_ctr[i]=0;		
	}
	else        //tamper off
	{
            /*
            if(j==1)
                write_eeprom(location,0); //occ registered
            else if((j==0)&&(k==0)&&(tmpr_time_off_ctr[i]>=3))  //20
            {
                write_eeprom(location1,1); //res registered
                to_eeprom(location1+1,real_date,3);
                to_eeprom(location1+4,real_time,3);
                to_eeprom(location1+7,vrms_reg3,2);
                to_eeprom(location1+9,irms_reg3[channel],2);
                write_eeprom(location1+11,pf_reg3/10);
                to_eeprom(location1+12,load_val[0],4);
            }
            */
          
            if(i==2)//magnet
              tamper_persist_ctr=30;//15; //10;//28;//15;
            else
              tamper_persist_ctr=60; //28;//60;//Tamper_Res_Time; //TAMPER_RESTORE_DELAY;
        
            if(tmpr_time_off_ctr[i]>=tamper_persist_ctr)
            {
                if((store_tamper_stat&((unsigned long int)(0x000001)<<i))==((unsigned long int)(0x000001)<<i))
                {                  
                    store_tamper_stat=store_tamper_stat & (~((unsigned long int)(0x000001)<<i));
                    //if(i==2)
                    //  mag_imax_stat=0;

                    tamper_snap_index=i;
                    //write_eeprom(location1,0); //res registered
                    store_event_data(tmp_event_type, tmp_event_id+1,1);

                    last_tamper_res_type=i+1;    
                    write_eeprom(LSTTAMPER_RES_LOC,last_tamper_res_type);
                    /*
                    scratch=from_eeprom(location1+1,3);
                    to_eeprom(LSTTAMPER_RES_LOC+1,scratch,3);//real_date

                    scratch=from_eeprom(location1+4,3);
                    to_eeprom(LSTTAMPER_RES_LOC+4,scratch,3);//real_time
                    */
                    to_eeprom(LSTTAMPER_RES_LOC+1,real_date,3);//real_date
                    to_eeprom(LSTTAMPER_RES_LOC+4,real_time,3);//real_time

                    to_eeprom(ALLTAMPER_LOC,store_tamper_stat,3);                                  
                }
                tmpr_time_off_ctr[i]=0;
            }		
            tmpr_time_on_ctr[i]=0;
	}
    }
    return;
}


/*
//1 Phase
//const unsigned char event_ids[10]={51,69,201,203,207};

//rev(0),el(1)    
//current 	Reverse(51-52),EL(69-70)

power fail 	Power Fail(101,102)

transaction	rtc date and time(151)
		demand integration period(152)
		Profile capture period(153)
		Single action schedule for billing dates(154)
		Activity calander for timezones(155)

mag(2),nd(3),nm(4)
other events	magnet(201,202), nd(203,204), NM(207,208)
      
Non Roll Events	cover open(251)
Control Events  load discon(301), load con(302)
*/      

/*
#define VOLT_EVENT_LOC          3200	//(2+3+3+2+2+1+4)*60 =17*60=1020     
#define AMP_EVENT_LOC           3300	//(2+3+3+2+2+1+4)*60 =17*60=1020
#define PFAIL_EVENT_LOC         4400	//(2+3+3)*60 =8*60=480
#define TRANSACT_EVENT_LOC      5000	//(2+3+3)*60 =8*60=480
#define OTHER_EVENT_LOC         5500	//(2+3+3+2+2+1+4)*60 =17*60=1020       
#define NOROLL_EVENT_LOC        6600	//(2+3+3)*1  =7
#define CTRL_EVENT_LOC          6610	//(2+3+3)*60 =8*60=480
*/

void store_event_data(unsigned char event_type, unsigned int event_id, unsigned char stat) 
{
   //unsigned char tmp_byte;
   unsigned int tmp_int;//, i;
   unsigned long int location;//, location1;
    
   
   Last_Tamper_Id[event_type]=event_id;
   to_eeprom(LAST_TAMPER_ID_LOC+((unsigned long int)event_type*2),Last_Tamper_Id[event_type],2);
      
   if(event_type==5)
      event_pos[event_type]=0;
   else
   {  
     event_pos[event_type]=event_pos[event_type]+1;
     if(event_pos[event_type]>=EVENT_SIZE)             
       event_pos[event_type]=0;
   }
    
   if(event_type==0)
      location=VOLT_EVENT_LOC;
   else if(event_type==1)
      location=AMP_EVENT_LOC;
   else if(event_type==2)
      location=PFAIL_EVENT_LOC;
   else if(event_type==3)
   {
     location=TRANSACT_EVENT_LOC;
     Cum_Prog_Count++;
     to_eeprom(PROGRAMCNT_LOC,Cum_Prog_Count,4);
   }
   else if(event_type==4)
      location=OTHER_EVENT_LOC;
   else if(event_type==5)
      location=NOROLL_EVENT_LOC;
   else if(event_type==6)
      location=CTRL_EVENT_LOC;
    
   if((event_type==0)|| (event_type==1)||(event_type==4))
   {  
       tmp_int=(event_pos[event_type]*17);
       location=location+tmp_int;
       
       /*
       if(stat==0)
        location1=INST_TAMPER_OCC_LOC+(tamper_snap_index*16);
       else
        location1=INST_TAMPER_RES_LOC+(tamper_snap_index*16);
                
       to_eeprom(location,event_id,2);
       
       for(i=0;i<15;i++)
       {
         tmp_byte=read_eeprom(location1+1+i);
         write_eeprom(location+2+i,tmp_byte);
       }
       */
       
       to_eeprom(location,event_id,2);
       to_eeprom(location+2,real_date,3);
       to_eeprom(location+5,real_time,3);
       to_eeprom(location+8,vrms_reg3,2);
       to_eeprom(location+10,irms_reg3[channel],2);
       write_eeprom(location+12,pf_reg3/10);
       to_eeprom(location+13,load_val[0],4);
       
       /*
       scratch=from_eeprom(location1+1,3);
       to_eeprom(location+2,scratch,3);//real_date
       scratch=from_eeprom(location1+4,3);
       to_eeprom(location+5,scratch,3);//real_time
       
       to_eeprom(location+8,vrms_reg3,2);
       to_eeprom(location+10,irms_reg3[channel],2);
       write_eeprom(location+12,pf_reg3/10);
       scratch=from_eeprom(location1+7,4);
       to_eeprom(location+13,scratch,4);//load_val[0]
       */
       
   }
   else if(event_type==2)
   {
       tmp_int=(event_pos[event_type]*8);
       location=location+tmp_int;

       to_eeprom(location,event_id,2);
       if(event_id==101)
       {  
        scratch=from_eeprom(DATE_LOC,3);
        to_eeprom(location+2,scratch,3);
        scratch=from_eeprom(TIME_LOC,3);
        to_eeprom(location+5,scratch,3);
       }
       else
       {  
        to_eeprom(location+2,real_date,3);
        to_eeprom(location+5,real_time,3);
       } 
   }
   else
   {
       tmp_int=(event_pos[event_type]*8);
       location=location+tmp_int;

       to_eeprom(location,event_id,2);
       to_eeprom(location+2,real_date,3);
       to_eeprom(location+5,real_time,3);
   }

   event_cnt[event_type]=event_cnt[event_type]+1;
   if(event_cnt[event_type]<Tamper_Profile_Entries[event_type])
    Tamper_Profile_Entries_In_Use[event_type]=event_cnt[event_type];
   else
    Tamper_Profile_Entries_In_Use[event_type]=Tamper_Profile_Entries[event_type];

   to_eeprom(EVENTCNT_LOC+(event_type*2),event_cnt[event_type],2);
   to_eeprom(EVENTPOS_LOC+(event_type*2),event_pos[event_type],2);

   return;
}

void power_fail_func(void)
{
    unsigned long int tmp_long;//, location;
    unsigned int tmp_int;
    unsigned char i,j;
	
    //fower fail check
    tmp_long=from_eeprom(DATE_LOC,3);

    tmp_int=tmp_long%100;
    scratch=tmp_int;//yr
    tmp_int=(tmp_long/100)%100;
    //scratch=(scratch*365)+tmp_int;//mnth
    scratch=(scratch*365);
    for(i=1;i<tmp_int;i++)
    {
            j=days_in_month(i-1);
            /*
            if(tmp_int==2)
            {
                    i=(2000+d_yr)%4;
                    if(i==0)
                            j=29;	
            }
            */
            scratch=scratch+j;	
    }			
    tmp_int=(tmp_long/10000);
    scratch=scratch+tmp_int;
    //scratch=(scratch*30)+tmp_int;//day
	
    tmp_long=from_eeprom(TIME_LOC,3);
    tmp_int=(tmp_long/10000);
    scratch=(scratch*24)+tmp_int;//hr
    tmp_int=(tmp_long/100)%100;
    scratch=(scratch*60)+tmp_int;//min

    scratch1=d_yr;
    scratch1=(scratch1*365);
    for(i=1;i<d_mnth;i++)
    {
            j=days_in_month(i-1);
            /*
            if(tmp_int==2)
            {
                    i=(2000+d_yr)%4;
                    if(i==0)
                            j=29;	
            }
            */
            scratch1=scratch1+j;	
    }			
    
    //scratch1=(scratch1*365)+d_mnth;
    //scratch1=(scratch1*30)+d_day;
    scratch1=scratch1+d_day;
    scratch1=(scratch1*24)+t_hr;
    scratch1=(scratch1*60)+t_min;

    if(scratch1>scratch)
        scratch1=scratch1-scratch;
    else
        scratch1=0;
    
    if(scratch1>=5)//30)//15)//35
    {
       store_event_data(PFAIL_EVENT, 101,0);//occ 
       store_event_data(PFAIL_EVENT, 102,1);//res 
       Cum_Power_Off_Count=Cum_Power_Off_Count+1;
       Cum_Power_Off_Dur=Cum_Power_Off_Dur+scratch1;
       to_eeprom(POFF_CNT_LOC, Cum_Power_Off_Count,4);
       to_eeprom(POFF_DUR_LOC, Cum_Power_Off_Dur,4);
    }
    to_eeprom(TIME_LOC,real_time,3);
}

unsigned char days_in_month(unsigned char index)
{
	if((index==0)||(index==2)||(index==4)||(index==6)||(index==7)||(index==9)||(index==11))
		return 31;
	if((index==3)||(index==5)||(index==8)||(index==10))
		return 30;
	else
		return 28;
}

/*
unsigned char rtc_log_func(unsigned char cyear)
{
	unsigned char i, tmp_year;
	
	i=(cyear & 0xf0)>>4;
	i=(i*10)+(cyear & 0x0f);
	cyear=i;
	for(i=0;i<15;i++)
	{
		scratch=from_eeprom(RTCLOG_LOC+(i*6),3);
		tmp_year=(unsigned char)(scratch%100);
		if((scratch==0xFFFFFF)||(scratch==0))
			break;	
		else if(tmp_year==cyear)
		{
			i=15;
			break;
		}
	}
	return i;	
}
*/

void bstart(unsigned char device)
{
    cbi(P2OUT,EEPROM_CLOCK);	
    delay10us(2);

    sbi(P2OUT,I2C_DATA);
    delay10us(2);
    sbi(P2OUT,EEPROM_CLOCK);
	delay10us(2);
    cbi(P2OUT,I2C_DATA);
    delay10us(2);
    
    cbi(P2OUT,EEPROM_CLOCK);
	return ;
}

void bstop(unsigned char device)
{
    cbi(P2OUT,EEPROM_CLOCK);
    delay10us(2);

    cbi(P2OUT,I2C_DATA);
    delay10us(2);
    sbi(P2OUT,EEPROM_CLOCK);
	delay10us(2);
    sbi(P2OUT,I2C_DATA);
	delay10us(2);
        
    cbi(P2OUT,EEPROM_CLOCK);
	return ;
}

void tx_data(unsigned char data, unsigned char device)
{
    unsigned char j;

    //sbi(P1OUT,PULSE1_PIN);
    
    cbi(P2OUT,EEPROM_CLOCK);
            
    for(j=0;j<8;j++)
    {
        if((data & 0x80) == 0x80)
        {
          sbi(P2OUT,I2C_DATA);
          //cbi(P1OUT,PULSE1_PIN);
        }
        else
        {
          cbi(P2OUT,I2C_DATA);
          //sbi(P1OUT,PULSE1_PIN);
        }
        
        delay10us(1);
        sbi(P2OUT,EEPROM_CLOCK);
        delay10us(1);
        cbi(P2OUT,EEPROM_CLOCK);
        delay10us(1);
        data = (data << 1);		
    }
    sbi(P2OUT,I2C_DATA);
    cbi(P2DIR,I2C_DATA);  //input
    delay10us(1);

    sbi(P2OUT,EEPROM_CLOCK);
    delay10us(1);
    //I2C_DATA
    cbi(P2OUT,EEPROM_CLOCK);
    sbi(P2DIR,I2C_DATA);  //output
    delay10us(1);

    //sbi(P1OUT,PULSE1_PIN);

    return;
}

unsigned char rx_data(unsigned char ack, unsigned char device)
{
    unsigned char j;
    unsigned char read_data;
    read_data = 0;

    cbi(P2DIR,I2C_DATA); //input	
    cbi(P2OUT,EEPROM_CLOCK);
    delay10us(1);

    for(j=0;j<8;j++)
    {       
        while(1)
        {
              sbi(P2OUT,EEPROM_CLOCK);
              delay10us(1);    
              if((P2OUT& 0x80)==0x80)
                break;
        }
        delay10us(1);
        read_data = (read_data << 1);
        if((P2IN & 0x40)==0x40)
          read_data = (read_data | 0x01);
    
        cbi(P2OUT,EEPROM_CLOCK);
        delay10us(1);
    }        
    sbi(P2DIR,I2C_DATA);
    delay10us(1);

    if(ack)
        sbi(P2OUT,I2C_DATA);
    else
        cbi(P2OUT,I2C_DATA);

    sbi(P2OUT,EEPROM_CLOCK);
    delay10us(1);
    cbi(P2OUT,EEPROM_CLOCK);
    return read_data;	
}

void write_eeprom(unsigned long int naddress,unsigned char cData)
{
    unsigned int addrH, addrL;
    
    addrH = (naddress >> 8 & 0xFF);
    addrL = (naddress & 0xFF);
    
    bstart(EEPROM);
    tx_data(0xA0,EEPROM);
    tx_data(addrH,EEPROM);
    tx_data(addrL,EEPROM);
    tx_data(cData,EEPROM);
    bstop(EEPROM);	
    delay1ms(6);//10

    return ;

}

void write_page_eeprom(unsigned long int naddress,unsigned char cData)
{
    unsigned int addrH, addrL, i;
    
    addrH = (naddress >> 8 & 0xFF);
    addrL = (naddress & 0xFF);
    
    bstart(EEPROM);
    tx_data(0xA0,EEPROM);
    tx_data(addrH,EEPROM);
    tx_data(addrL,EEPROM);
    
    for(i=0;i<128;i++)
        tx_data(cData,EEPROM);
    
    bstop(EEPROM);	
    delay1ms(6); //10
    return ;
}

unsigned char read_eeprom(unsigned long int naddress)
{
    unsigned char read_data;
    unsigned int addrH, addrL;
    
    addrH = (naddress >> 8 & 0xFF);
    addrL = (naddress & 0xFF);
	
    bstart(EEPROM);
    tx_data(0xA0,EEPROM);
    tx_data(addrH,EEPROM);	
    tx_data(addrL,EEPROM);	

    bstart(EEPROM);
    tx_data(0xA1,EEPROM);	
    read_data = rx_data(1,EEPROM);
    bstop(EEPROM);
    delay1ms(1);	

    return read_data;
}

void read_page_eeprom(unsigned long int naddress, unsigned char buf_index, unsigned char size)
{
    unsigned int addrH, addrL, i;
    
    addrH = (naddress >> 8 & 0xFF);
    addrL = (naddress & 0xFF);
        
    bstart(EEPROM);
    tx_data(0xA0,EEPROM);
    tx_data(addrH,EEPROM);	
    tx_data(addrL,EEPROM);	

    bstart(EEPROM);
    tx_data(0xA1,EEPROM);	
    for(i=0;i<(size-1);i++)
            read_data_arr[buf_index][i]= rx_data(0,EEPROM);
    read_data_arr[buf_index][size-1]= rx_data(1,EEPROM);
    bstop(EEPROM);
    delay10us(6); //10
    
    return;
}

void to_eeprom(unsigned long int nAddr, unsigned long int ldata, unsigned char size)
{
	unsigned char j,k;
	
	for(j=0;j<size;j++)
	{	
		k=((ldata>>((size-j-1)*8))&0xff);	
		write_eeprom(nAddr+j,k);
	}

	return;
		
}

unsigned long int from_eeprom(unsigned long int nAddr,unsigned char size)
{
    unsigned long int ldata;
    unsigned char j,k;

    ldata=0;
    for(j=0;j<size;j++)
    {	
        k=read_eeprom(nAddr+j);
        ldata=(ldata<<8)+k;
    }
    
    return ldata;	
		
}

unsigned long int from_data_arr(unsigned long int nAddr,unsigned char buf_index, unsigned char size)
{
    unsigned long int ldata;
    unsigned char j,k;

    ldata=0;
    for(j=0;j<size;j++)
    {	
        k=read_data_arr[buf_index][nAddr+j];
        ldata=(ldata<<8)+k;
    }

    return ldata;		
}

unsigned char convt_byte_to_bcd(unsigned char byte_data)
{
    unsigned char bcd_data;
    bcd_data=byte_data/10;
    bcd_data=(bcd_data<<4)+(byte_data % 10);
    return bcd_data;
}

void mem_check(void)
{
	unsigned char i,j;
	unsigned int tmp_int;

	i=0;
	j=0;
	while(1)
	{
		tmp_int=from_eeprom(MEM_END,2);
		//lcd_data_cnt=0;
                if(tmp_int==12345)
		{	
			//LCDMEM[13]=LCDMEM[13]|0x10; //send_lcd(0xFF);	 
                        j=0;
			i++;
			if(i>3)
				break;
		}
		else
		{
			LCDMEM[13]=LCDMEM[13]|0x10;//send_lcd(0x00);	 
                        i=0;
			j++;
			if(j>3)
				to_eeprom(MEM_END,12345,2);
		}
		delay1ms(50);
	}

        /*
	if(tmp_int==12345)
		mem_stat=1;
	else
		mem_stat=0;
        */
        
	tmp_int=0;	
	while(1)
	{
            volt_fact=from_eeprom(VOLT_FACT_LOC,2); 

            for(i=0;i<2;i++)
	    {
              amp_fact[i]=from_eeprom(AMP_FACT_LOC+(i*2),2); 
              iph_val[i] = from_eeprom(IPH_LOC+(i*2),2);
              igain_val[i]=from_eeprom(IGAIN_LOC+(i*2),2);
              n_cfden_val[i]=from_eeprom(NCFDEN_LOC+(i*2),2);
            }
            
            i=get_cal_crc();
            j=read_eeprom(CAL_CHKSUM_LOC);
            if(i==j)
            {
                //LCDMEM[13]=LCDMEM[13]|0x10;	 
                break;
            }
            else
                tmp_int++;		
	
            if(tmp_int>=4)
            {
                volt_fact=VOLT_CONST; 
                for(i=0;i<2;i++)
                {
                  amp_fact[i]=AMP_CONST; 
                  //iph_val[i] = IPH_CONST;
                  //igain_val[i]=IGAIN_CONST;
                  n_cfden_val[i]=NCF1DEN;
                }
                iph_val[0] = IPH1_CONST;
                iph_val[1] = IPH2_CONST;
                igain_val[0]=IGAIN1_CONST;
                igain_val[1]=IGAIN2_CONST;
                
                LCDMEM[13]=LCDMEM[13]|0x10;	 
                break;
            }
            delay1ms(250);
	}

	init_stat=0;
	//nmiss_init_stat=0;
	//mains_init_stat=0;	
}


unsigned char get_cal_crc(void)
{
	unsigned char i;
	scratch=0xAA;
	scratch=scratch+volt_fact;
	for(i=0;i<2;i++)
        {  
          scratch=scratch+amp_fact[i];
          scratch=scratch+iph_val[i];
          scratch=scratch+igain_val[i];
          scratch=scratch+n_cfden_val[i];
        } 
         i=crc8(scratch);
	
	return i;
}

void store_cal_data(void)
{
  unsigned char i;
  
    to_eeprom(VOLT_FACT_LOC,volt_fact,2); 
    for(i=0;i<2;i++)
    {  
      to_eeprom(AMP_FACT_LOC+(i*2),amp_fact[i],2); 
      to_eeprom(IPH_LOC+(i*2),iph_val[i],2);
      to_eeprom(IGAIN_LOC+(i*2),igain_val[i],2);
      to_eeprom(NCFDEN_LOC+(i*2),n_cfden_val[i],2);
    }  
}

void clear_bstat(void)
{
	scrl_b=0;
	return;
}

void delay1ms(unsigned char ms)
{
	//unsigned char i;
        unsigned int delay_cnt;
	
	u10_delay_ctr=0;
	delay_cnt=ms*100;
	while(u10_delay_ctr<delay_cnt)
	{
          delay10us(5);
          //WDTCTL=WDT_ARST_1000;//WDTPW+WDTSSEL__ACLK+WDTCNTCL;
        }          
        return;
}

void delay10us(unsigned char us)
{
    unsigned char j, k; //i,
    
    for(k=0;k<us;k++) 
    {
        j=48;//24;
        while(j>0) 
                j--;
        u10_delay_ctr++;
    }	  	
	
    WDTCTL=WDT_ARST_1000;//WDTPW+WDTSSEL__ACLK+WDTCNTCL;
    return;
}

/*
void dflt_tod_zone(void)
{
    unsigned char i;//,j;
    unsigned int tmp_int;

    for(i=0;i<TOD_SIZE;i++)
    {
      tmp_int=((24*i)/TOD_SIZE);
      //tmp_int=tmp_int*100;
      write_eeprom(TOD_LOC+i,tmp_int);
    }
    write_eeprom(NUM_ZONE_LOC,TOD_SIZE);        
}	
*/
	
void init_tod(void)//unsigned int pos, unsigned long int e_val)
{
	unsigned int i,j,k;//,tmp_int;
	unsigned long int location, location1;
        
	//----TOD------------------------------
	
        for(j=0;j<2;j++)
        {  
          for(i=0;i<TOD_SIZE;i++)//TOD_SIZE
          {
            t_zone[(j*8)+i]=from_eeprom(TOD_LOC+(j*16)+(i*2),2);//read_eeprom(TOD_LOC+i);
            tod_time[(j*8)+i][0]=t_zone[(j*8)+i]/100;
            tod_time[(j*8)+i][1]=t_zone[(j*8)+i]%100;
            tod_time[(j*8)+i][2]=0;
            tod_time[(j*8)+i][3]=0xff;
            tariff_id[j][i]=read_eeprom(TOD_ID_LOC+(j*8)+i);
          }
        }

	//num_of_zone=read_eeprom(NUM_ZONE_LOC);
        //if((num_of_zone<2)||(num_of_zone>TOD_SIZE))
	//	num_of_zone=TOD_SIZE;	
	 
	t_prev_zone=read_eeprom(T_ZONE_LOC);
	for(i=0;i<2;i++)
		last_stored_tod_load_val[i]=from_eeprom(LAST_STORE_TOD_KWH_LOC+(i*4),4);
        
        
        location=TOD_KWMD_LOC+(mnth_pos*88);
        location1=TOD_KVAMD_LOC+(mnth_pos*88);

        for(i=0;i<TOD_SIZE;i++)
        { 
          scratch=from_eeprom(location+(i*11),4);
          scratch1=from_eeprom(location1+(i*11),4);
          
          if(i==t_prev_zone)
          {
              if(load_val[0]>last_stored_tod_load_val[0])
                scratch=scratch+(load_val[0]-last_stored_tod_load_val[0]);
              if(load_val[1]>last_stored_tod_load_val[1])
                scratch1=scratch1+(load_val[1]-last_stored_tod_load_val[1]);
          }   
          tod_kwh_val[i]=scratch;
          tod_kvah_val[i]=scratch1;
        }
        
        activate_date=from_eeprom(ACTIVATE_DATE_LOC,3);
        activate_time=from_eeprom(ACTIVATE_TIME_LOC,2);
        
        get_time_data(activate_date,(activate_time*100));
        for(i=0;i<12;i++)
          Activate_Passive_Calendar_Time[i+2]=tmp_time_string[i];

        //if(activate_date!=0)
        //{
          for(j=0;j<2;j++)
          {  
            for(i=0;i<TOD_SIZE;i++)//TOD_SIZE
            {
              k=from_eeprom(PASSIVE_TOD_LOC+(j*16)+(i*2),2);//read_eeprom(TOD_LOC+i);
              passive_tod_time[(j*8)+i][0]=k/100;
              passive_tod_time[(j*8)+i][1]=k%100;
              passive_tod_time[(j*8)+i][2]=0;
              passive_tod_time[(j*8)+i][3]=0xff;
              passive_tariff_id[j][i]=read_eeprom(PASSIVE_TOD_ID_LOC+(j*8)+i);
            }
          }
        
        //}
        
        /*
        else
        {
          for(i=0;i<TOD_SIZE;i++)//TOD_SIZE
          {
            passive_tod_time[i][0]=0;
            passive_tod_time[i][1]=0;
            passive_tod_time[i][2]=0;
            passive_tod_time[i][3]=0xff;
            passive_tariff_id[i]=0;
          }
        }  
        */
          
        for(i=0;i<16;i++)
        {
          Calendar_Name_Passive[2+i]=read_eeprom(CAL_NAME_PASSIVE_LOC+i);
          Calendar_Name_Active[2+i]=read_eeprom(CAL_NAME_ACTIVE_LOC+i);
        }
        
        for(j=0;j<2;j++)
        {  
          for(i=0;i<10;i++)
          {
            Active_Season_Name[j][i]=read_eeprom(SEASON_NAME_ACTIVE_LOC+(j*10)+i);
            Passive_Season_Name[j][i]=read_eeprom(SEASON_NAME_PASSIVE_LOC+(j*10)+i);
          }
          for(i=0;i<4;i++)
          {
            active_season_date[j][i]=read_eeprom(SEASON_DATE_ACTIVE_LOC+(j*4)+i);
            passive_season_date[j][i]=read_eeprom(SEASON_DATE_PASSIVE_LOC+(j*4)+i);
          }
        }
        
        prev_season=255;
        select_season();
        //update_tod_data(pos, e_val);
}

void tod_passive_to_active(void)
{
	unsigned int i, j, k;
	//----TOD------------------------------
	
        for(j=0;j<2;j++)
        {  
          for(i=0;i<TOD_SIZE;i++)//TOD_SIZE
          {
            k=from_eeprom(PASSIVE_TOD_LOC+(j*16)+(i*2),2);
            to_eeprom(TOD_LOC+(j*16)+(i*2),k,2);
            t_zone[(j*8)+i]=k;
            tod_time[(j*8)+i][0]=t_zone[(j*8)+i]/100;
            tod_time[(j*8)+i][1]=t_zone[(j*8)+i]%100;
            tod_time[(j*8)+i][2]=0;
            tod_time[(j*8)+i][3]=0xff;
            
            k=read_eeprom(PASSIVE_TOD_ID_LOC+(j*8)+i);
            write_eeprom(TOD_ID_LOC+(j*8)+i,k);
            tariff_id[j][i]=k;
            
            /*
            passive_tod_time[i][0]=0;
            passive_tod_time[i][1]=0;
            passive_tod_time[i][2]=0;
            passive_tod_time[i][3]=0xff;
            passive_tariff_id[i]=0;
            */
          }
        }
        
        for(i=0;i<16;i++)
        {
          Calendar_Name_Active[2+i]=Calendar_Name_Passive[2+i];
          write_eeprom(CAL_NAME_ACTIVE_LOC+i,Calendar_Name_Active[2+i]);
        }
        
        for(j=0;j<2;j++)
        {  
          for(i=0;i<10;i++)
          {
            Active_Season_Name[j][i]=Passive_Season_Name[j][i];
            write_eeprom(SEASON_NAME_ACTIVE_LOC+(j*10)+i,Active_Season_Name[j][i]);
          }
          
          for(i=0;i<4;i++)
          {
            active_season_date[j][i]=passive_season_date[j][i];
            write_eeprom(SEASON_DATE_ACTIVE_LOC+(j*4)+i,active_season_date[j][i]);
          }  
        }
        
        
	activate_date=0;
        activate_time=0;
        to_eeprom(ACTIVATE_DATE_LOC,0,3);
        to_eeprom(ACTIVATE_TIME_LOC,0,2);
        
        get_time_data(activate_date,(activate_time*100));
        for(i=0;i<12;i++)
          Activate_Passive_Calendar_Time[i+2]=tmp_time_string[i];
        
        prev_season=255;
        select_season();
}

void select_season(void)
{
    unsigned long int tmp_long;
    unsigned char i, curr_season;
    
    //scratch=d_yr;
    tmp_long=d_mnth; //(scratch*13)+d_mnth;
    tmp_long=(tmp_long*32)+d_day;
    tmp_long=(tmp_long*25)+t_hr;
    tmp_long=(tmp_long*60)+t_min;
    
    for(i=0;i<2;i++)
    {  
      scratch1=active_season_date[i][0];
      scratch1=(scratch1*32)+active_season_date[i][1];
      scratch1=(scratch1*25)+active_season_date[i][2];
      scratch1=(scratch1*60)+active_season_date[i][3];
      
      if(i==0)
        scratch=scratch1;
    }
    
    if(scratch<=scratch1)
    {  
        if((tmp_long>=scratch)&&(tmp_long<scratch1))
          curr_season=0;
        else
          curr_season=1;
    }
    else
    {
        if((tmp_long>=scratch1)&&(tmp_long<scratch))
          curr_season=1;
        else
          curr_season=0;
    }  
    
    if(prev_season!=curr_season)
    {
      prev_season=curr_season;
        
      for(i=TOD_SIZE;i>0;i--)
      {
              if(t_zone[(prev_season*8)+(i-1)]>0)
                      break;
      }	
                      
      num_of_zone=i;
      if((num_of_zone<2)||(num_of_zone>TOD_SIZE))
              num_of_zone=TOD_SIZE;	
      //write_eeprom(NUM_ZONE_LOC,num_of_zone);
    }    
    
  return;
}


void update_tod_data(void)
{
    unsigned int tmp_int;
    unsigned char i, t_curr_zone;
    
      //finding TOD Zone
        tmp_int=t_zone[(prev_season*8)+0];
        t_curr_zone=0;
        for(i=1;i<num_of_zone;i++)//TOD_SIZE
        {
          if(tmp_int>t_zone[(prev_season*8)+i]) //>=
          {
             t_curr_zone=i;
             tmp_int=t_zone[(prev_season*8)+i];
          }
        }    
        //1400        
        tmp_int=t_hr;
        tmp_int=(tmp_int*100)+t_min;
        for(i=0;i<num_of_zone;i++)//TOD_SIZE										   
        {
          if(tmp_int<t_zone[(prev_season*8)+t_curr_zone]) //<=
               break;
          t_curr_zone=cyclic(t_curr_zone,num_of_zone); //TOD_SIZE
        }
        t_curr_zone=cyclic(t_curr_zone+num_of_zone-2,num_of_zone); //zone+1+6 %8= zone - 1  
        
        if(t_prev_zone!=t_curr_zone)
        {  
          store_energy_in_zone();	
          write_eeprom(T_ZONE_LOC,t_curr_zone);
          t_prev_zone=t_curr_zone;
        }    
    return;
}

void store_energy_in_zone()
{
	unsigned int i;
	
	for(i=0;i<2;i++)
	{
          if(load_val[i]>last_stored_tod_load_val[i])
          { 
            scratch=load_val[i]-last_stored_tod_load_val[i];
            if(i==0)
            {  
              scratch1=from_eeprom(TOD_KWMD_LOC+(t_prev_zone*11)+(mnth_pos*88),4);
              scratch=scratch+scratch1;
              to_eeprom(TOD_KWMD_LOC+(t_prev_zone*11)+(mnth_pos*88),scratch,4);
              tod_kwh_val[t_prev_zone]=scratch;
            }
            else
            {
              scratch1=from_eeprom(TOD_KVAMD_LOC+(t_prev_zone*11)+(mnth_pos*88),4);
              scratch=scratch+scratch1;
              to_eeprom(TOD_KVAMD_LOC+(t_prev_zone*11)+(mnth_pos*88),scratch,4);
              tod_kvah_val[t_prev_zone]=scratch;
            }
          }
          last_stored_tod_load_val[i]=load_val[i];
          to_eeprom(LAST_STORE_TOD_KWH_LOC+(i*4),load_val[i],4);
	}
}

