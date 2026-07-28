#include <stdbool.h>
#include "Defines.h"
#include "time.h"       // Clock and calendar.

//For ASDAQ calibration protocol implementation
// DISPLAY DATA SCREEN NAMES
#define INST_CMD        0
#define IR_INST_CMD     80
#define TIME_CMD        1
#define DATE_CMD        2
#define MTR_NO_CMD      3
#define CLR1_CMD        4
#define CLR2_CMD        85
#define DLOAD_CMD       5
#define CLR_TAMPER_CMD  185
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

#define COMM_CMD				51
#define MAG_PULSE_CMD   52
#define CBYP_CMD        53
#define MDRESET_CMD     54
#define NUM_ZONE_CMD    55
#define BILL_DAY_CMD		56
#define VERSION_CMD			70

#define RTC_CALIB_CMD		60
#define UNLOCK_CMD			170

#define I_MTR_NO  0
#define I_TIME    10
#define I_DATE    20
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

#define I_CNT1 901
#define I_CNT2 902
#define I_CNT3 903
#define I_CNT4 904
#define I_CNT5 905
#define I_CNT6 906
#define I_CNT7 907
#define I_CNT8 908
#define I_CNT9 909
#define I_CNT10 910
#define I_CNT11 911
#define I_CNT12 912
#define I_CNT13 913
#define I_CNT14 914
#define I_CNT15 915
#define I_CNT16 916
#define I_CNT17 917
#define I_CNT18 918
#define I_CNT19 919
#define I_CNT20 920
#define I_CNT21 921

//ADC
#define MCF1DEN	10000
#define NCF1DEN 1200

#define VOLT_CONST	16600	//2800
#define AMPP_CONST	790
#define AMPN_CONST	797

#define IAPH_CONST  	0
#define IBPH_CONST  	786
#define IAGAIN_CONST	18780
#define IBGAIN_CONST 	19950
#define VGAIN_CONST		16600

/*
extern unsigned char recv_buf[25],recv_ctr,recv_ovf;
extern unsigned char enable_recv_buf;
extern unsigned char transmit_complete;
extern unsigned char comm_delay_ctr;
extern unsigned long int comm_checksum;
extern unsigned char checksum, checksum_calc;

extern unsigned long int meter_no;
extern unsigned char pass_step,pass_ctr;
extern unsigned int vrms_reg3, vrms_reg3_actual, irms_reg3, irms1_reg3, irms2_reg3, kw_reg3, kva_reg3, pf_reg3, temp_reg3, freq_reg3;
extern unsigned char channel;
extern unsigned int volt_fact, ampp_fact,ampn_fact, pf_fact, freq_fact, kw_fact,n_cfdenn_val,n_cfdenp_val,m_cfden_val,iagain_val, ibgain_val;
extern int ph_val, iaph_val, ibph_val;

//extern unsigned long real_time, real_date;//,prev_real_date;
//extern unsigned long int scratch, scratch1;


//extern unsigned char d_yr,d_mnth,d_day,t_hr,t_min,t_sec;
//extern unsigned long int real_time, real_date;//,prev_real_date;
*/

//extern unsigned int vrms_reg3, vrms_reg3_actual, irms_reg3, irms1_reg3, irms2_reg3, kw_reg3, kva_reg3, kvar_reg3,  pf_reg3, temp_reg3, freq_reg3;
extern unsigned char channel;
//extern int pf_reg4, kw_reg4;

extern unsigned long int load_val[2], disp_load_val[2], last_load_val[2], last_stored_tod_load_val[2], last_day_load_val[2], last_hr_load_val[2];
extern unsigned long int last_demand_load_val[2];
extern unsigned int load_ctr[2], load_index[2], inst_load[2], load_rmndr[2];

extern unsigned long int kwh_bkp, kvah_bkp, reset_on_time,bkp_kwh_val,bkp_kvah_val;
extern unsigned int kwmd_val, kwmd_bkp, kvamd_val, kvamd_bkp, ontime_bkp, tamper_cnt_bkp;
extern unsigned long int kwmd_date,kvamd_date;
extern unsigned long int kwmd_time,kvamd_time;
extern unsigned int bkp_kwmd_val; 
extern unsigned int bkp_kvamd_val; 
extern unsigned int bkp_kwmd_date, bkp_kwmd_time; 
extern unsigned int bkp_kvamd_date, bkp_kvamd_time; 
extern unsigned char bkp_pf_val;


extern unsigned long int cum_on_time;
extern unsigned long int cum_kwmd_val;
extern unsigned long int kwcd_val;

extern unsigned int hist_pf;
extern unsigned int bkp_pf; //avg_pf, 

extern unsigned char prev_min;
extern unsigned char ep_clr_stat;
extern unsigned int md_intgr_val,survey_intgr_val;
extern unsigned long int demand_volt, demand_amp, demand_pf;  

extern unsigned char reset_on_min_ctr;
extern unsigned long int power_on_time;
extern unsigned long int cum_on_time;

extern unsigned char num_of_zone;

extern unsigned char billing_day, billing_hr, billing_min;
extern unsigned char single_billing_day, single_billing_month, single_billing_year, single_billing_hr, single_billing_min;

extern unsigned int last_bill_time;
extern unsigned long int last_bill_date;

extern unsigned long int md_reset_cnt;

extern unsigned int prev_mnth, prev_day, prev_hr, prev_yr;
extern unsigned long int prev_date;

extern unsigned char md_reset_b;


/*
extern unsigned char set_cal_name_data;
extern unsigned char set_auth_data;
extern unsigned char set_rtc_data;
extern unsigned char set_demand_intgr_period;
extern unsigned char set_capture_period;
extern unsigned char set_bill_date;
extern unsigned char set_day_profile;
extern unsigned char set_season_profile;
extern unsigned char set_activation_date;

extern unsigned char set_load_control_stat;
extern unsigned char set_over_current_val;
extern unsigned char set_over_load_val;
extern unsigned char set_conn_time_interval;
extern unsigned char set_conn_lockout_time;
extern unsigned char set_conn_time_repeat;
extern unsigned char set_tamper_time;
*/

extern unsigned char set_clock_time_zone;
extern unsigned int clock_time_zone;

extern unsigned char activate_tod_stat;

extern unsigned long int Over_Current_Val;
extern unsigned long int Over_Load_Val;
extern unsigned long int Conn_Time_Interval;
extern unsigned long int Conn_Lockout_Time;
extern unsigned long int Conn_Time_Repeat;
extern unsigned long int Tamper_Occ_Time;
extern unsigned long int Tamper_Res_Time;

extern unsigned long int Load_Profile_Capture_Period;
extern unsigned long int Load_Profile_Entries_In_Use;
extern unsigned long int Load_Profile_Entries;

extern unsigned long int Daily_Load_Profile_Capture_Period;
extern unsigned long int Daily_Load_Profile_Entries_In_Use;
extern unsigned long int Daily_Load_Profile_Entries;

extern unsigned long int Max_Demand_Capture_Period;
extern unsigned long int Billing_Profile_Entries_In_Use;
extern unsigned long int Billing_Profile_Entries;


extern unsigned char bill_day[5];
extern unsigned char bill_time[4];
extern unsigned char scheduled_bill_day[5];
extern unsigned char scheduled_bill_time[4];

extern unsigned int t_zone[16], t_prev_zone;
extern unsigned long int tod_kwh_val[8], tod_kvah_val[8];

extern unsigned char Calendar_Name_Active[18];
extern unsigned char Calendar_Name_Passive[18];
extern unsigned char Active_Season_Name[2][10];
extern unsigned char Passive_Season_Name[2][10];
extern unsigned char active_season_date[2][4];
extern unsigned char passive_season_date[2][4];
extern unsigned char tod_time[16][4]; //[8*2][4]
extern unsigned char passive_tod_time[16][4]; //[8*2][4]
extern unsigned char tariff_id[2][8];
extern unsigned char passive_tariff_id[2][8];
extern unsigned char Activate_Passive_Calendar_Time[14];
extern unsigned char prev_season;

extern unsigned long int activate_date;
extern unsigned long int activate_time;

extern unsigned int mnth_pos, day_pos;
extern unsigned int tmpr_time_on_ctr[TAMPER_TYPE], tmpr_time_off_ctr[TAMPER_TYPE];
//unsigned char occur_stat[TAMPER_TYPE];
extern unsigned int event_pos[TOT_EVENT_TYPE];
extern unsigned long int store_tamper_stat; //,tamper_stat;
extern unsigned long int day_hr_pos, tamper_cnt[TAMPER_TYPE];
extern unsigned long int event_cnt[TOT_EVENT_TYPE];
extern unsigned long int Tamper_Profile_Entries_In_Use[TOT_EVENT_TYPE];
extern unsigned long int Tamper_Profile_Entries[TOT_EVENT_TYPE];
extern unsigned long int all_tamper_cnt;
//unsigned long int all_tamper_cnt, bill_tamper_cnt, tamper_cnt_array[TAMPER_TYPE];

extern unsigned long int block_load_val[2], block_vrms, block_irms;

extern unsigned char checksum, checksum_calc;
extern unsigned int volt_fact, ampp_fact,ampn_fact, pf_fact, freq_fact, kw_fact,n_cfdenn_val,n_cfdenp_val,m_cfden_val,iagain_val, ibgain_val;
extern int ph_val, iaph_val, ibph_val;

extern unsigned long int meter_no;

extern unsigned char pass_step,pass_ctr;

extern unsigned char recv_buf[25],recv_ctr,recv_ovf;
extern unsigned char enable_recv_buf;
extern unsigned char transmit_complete;
extern unsigned char comm_delay_ctr;
extern unsigned long int comm_checksum;

extern unsigned int bkp_kwmd_val; 
extern unsigned int bkp_kvamd_val; 
extern unsigned int bkp_kwmd_date, bkp_kwmd_time; 
extern unsigned int bkp_kvamd_date, bkp_kvamd_time; 

extern unsigned long int Cum_Power_Off_Count;
extern unsigned long int Cum_Power_Off_Dur;
extern unsigned long int Cum_Power_On_Dur;

extern unsigned char ep_clear_stat;
extern unsigned int rd_time;

extern unsigned char time_string[12];
extern unsigned char Last_MD_Rst_DT[12];
extern unsigned char KWMD_DT[12];
extern unsigned char KVAMD_DT[12];
extern unsigned char tmp_time_string[12];//={0,0,0,0,0,0,0,0,0,0,0,0};

extern unsigned char last_tamper_occ_type, last_tamper_res_type;
extern unsigned int tamper_snap_index;

extern unsigned char magnetic_stat, eload_stat, rev_stat, ctn_stat, nd_stat, v440_stat, cbyp_stat, nmiss_stat;

extern unsigned char mag_imax_stat, mag_restore_ctr;
extern unsigned char mag_chk_delay;

extern unsigned int Last_Tamper_Id[7];
extern unsigned char Last_Tamper_Date_Time[7][12];
extern unsigned long int Cum_Prog_Count;

extern unsigned long int manfact_date;
extern unsigned char cuopen_stat;
extern unsigned long int cuopen_date, cuopen_time;
extern unsigned char hist_ctr;
extern unsigned char disp_md_type;
extern unsigned char read_data_arr[2][30];
extern unsigned int u10_delay_ctr;
extern tm_t s_time; // time structure.


void get_time_data(unsigned long int, unsigned long int);
unsigned int crc8(unsigned long int);
unsigned int cyclic(unsigned int, unsigned int);
void read_time_date(void);

void default_eeprom(unsigned char);
void write_eeprom(unsigned long int,unsigned char);
void write_page_eeprom(unsigned long int,unsigned char);
unsigned char read_eeprom(unsigned long int);
void read_page_eeprom(unsigned long int, unsigned char, unsigned char);
void to_eeprom(unsigned long int nAddr, unsigned long int ldata, unsigned char size);
unsigned long int from_eeprom(unsigned long int nAddr,unsigned char size);

unsigned long int from_data_arr(unsigned long int,unsigned char, unsigned char);

void delay1ms(unsigned char);
void delay10us(unsigned char);	
unsigned char convt_byte_to_bcd(unsigned char);
unsigned char convt_bcd_to_byte(unsigned char);

void init_io_param(void);
void init_eeprom(void);
void load_func(void);
void inst_param_func(void);

void history_func(void);
void store_md_data(unsigned int);

void init_tod(void);
void dflt_tod_zone(void);
void update_tod_data(void);
void store_energy_in_zone(void);
void select_season(void);
void tod_passive_to_active(void);

void fill_load_survey_entries(unsigned long int, unsigned char, unsigned char);
unsigned long int find_next_date(unsigned long int);

void tamper_func(void);
void store_event_data(unsigned char, unsigned int, unsigned char);
unsigned char days_in_month(unsigned char);
unsigned long int from_data_arr(unsigned long int, unsigned char, unsigned char);
void get_active_season_data(void);//unsigned int); 
void get_passive_season_data(void);//unsigned int); 

void get_active_week_data(void);//unsigned int); 
void get_passive_week_data(void);//unsigned int); 

void get_active_day_data(void);//unsigned int); 
void get_passive_day_data(void);//unsigned int); 
void get_dlp_data(unsigned int); 
void get_tamper_data(unsigned char, unsigned int); 
void get_lp_data(unsigned int); 
void get_bill_data(unsigned int); 
void get_time_data(unsigned long int, unsigned long int);
void find_num_between_lp_entries(void);
void find_num_between_dlp_entries(void);
void dlms_actions_func(void);

void clearnvram(void);
