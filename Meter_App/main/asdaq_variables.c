#include <stdbool.h>
#include "Defines.h"
#include "time.h" // Clock and calendar.
#include "asdaq_variables.h"

// unsigned char d_yr,d_mnth,d_day,t_hr,t_min,t_sec;
// unsigned long int real_time, real_date;//,prev_real_date;
// unsigned int vrms_reg3, vrms_reg3_actual, irms_reg3, irms1_reg3, irms2_reg3, kw_reg3, kva_reg3, kvar_reg3,  pf_reg3, temp_reg3, freq_reg3;
unsigned char channel;
// int pf_reg4, kw_reg4;

unsigned long int load_val[2], disp_load_val[2], last_load_val[2], last_stored_tod_load_val[2], last_day_load_val[2], last_hr_load_val[2];
unsigned long int last_demand_load_val[2];
unsigned int load_ctr[2], load_index[2], inst_load[2], load_rmndr[2];

unsigned long int kwh_bkp, kvah_bkp, reset_on_time, bkp_kwh_val, bkp_kvah_val;
unsigned int kwmd_val, kwmd_bkp, kvamd_val, kvamd_bkp, ontime_bkp, tamper_cnt_bkp;
unsigned long int kwmd_date, kvamd_date;
unsigned long int kwmd_time, kvamd_time;
unsigned int bkp_kwmd_val;
unsigned int bkp_kvamd_val;
unsigned int bkp_kwmd_date, bkp_kwmd_time;
unsigned int bkp_kvamd_date, bkp_kvamd_time;
unsigned char bkp_pf_val;

unsigned long int cum_on_time;
unsigned long int cum_kwmd_val;
unsigned long int kwcd_val;

unsigned int hist_pf;
unsigned int bkp_pf; // avg_pf,

unsigned char prev_min;
unsigned char ep_clr_stat;
unsigned int md_intgr_val, survey_intgr_val;
unsigned long int demand_volt, demand_amp, demand_pf;

unsigned long int scratch, scratch1, scratch2;

unsigned char reset_on_min_ctr;
unsigned long int power_on_time;
unsigned long int cum_on_time;

unsigned char num_of_zone;

unsigned char billing_day, billing_hr, billing_min;
unsigned char single_billing_day, single_billing_month, single_billing_year, single_billing_hr, single_billing_min;

unsigned int last_bill_time;
unsigned long int last_bill_date;

unsigned long int md_reset_cnt;

unsigned int prev_mnth, prev_day, prev_hr, prev_yr;
unsigned long int prev_date;

unsigned char md_reset_b;

unsigned char set_cal_name_data;
unsigned char set_auth_data;
unsigned char set_rtc_data;
unsigned char set_demand_intgr_period;
unsigned char set_capture_period;
unsigned char set_bill_date;
unsigned char set_day_profile;
unsigned char set_season_profile;
unsigned char set_activation_date;

/*
unsigned char set_load_control_stat;
unsigned char set_over_current_val;
unsigned char set_over_load_val;
unsigned char set_conn_time_interval;
unsigned char set_conn_lockout_time;
unsigned char set_conn_time_repeat;
unsigned char set_tamper_time;
*/

unsigned char set_clock_time_zone;
unsigned int clock_time_zone;
uint8_t SerialDisplayTimeOut = 0;

unsigned char activate_tod_stat;

unsigned long int Over_Current_Val;
unsigned long int Over_Load_Val;
unsigned long int Conn_Time_Interval;
unsigned long int Conn_Lockout_Time;
unsigned long int Conn_Time_Repeat;
unsigned long int Tamper_Occ_Time;
unsigned long int Tamper_Res_Time;

unsigned long int Load_Profile_Capture_Period = CLASS07_BLOCKLOAD_INTERVAL;
unsigned long int Load_Profile_Entries_In_Use;
unsigned long int Load_Profile_Entries = CLASS07_BLOCKLOAD_MAX_ENTRY;

unsigned long int Daily_Load_Profile_Capture_Period = CLASS07_DAILYLOAD_INTERVAL;
unsigned long int Daily_Load_Profile_Entries_In_Use;
unsigned long int Daily_Load_Profile_Entries = CLASS07_DAILYLOAD_MAX_ENTRY;

unsigned long int Max_Demand_Capture_Period;
unsigned long int Billing_Profile_Entries_In_Use;
unsigned long int Billing_Profile_Entries = CLASS07_BILLING_MAX_ENTRY;

/*
unsigned int t_zone[8], t_prev_zone;//, passive_script_id[8];
unsigned int passive_tod_zone_val[8], passive_tod_zone_val2[8];

unsigned long int activate_tod_date;
unsigned long int activate_tod_time;

unsigned long int season_activate_date, season_activate_date2;
unsigned long int season_activate_time, season_activate_time2;
unsigned char active_season;
unsigned char active_tod_name[10];
unsigned char passive_tod_name[10],passive_tod_name2[10];
unsigned char season_name[10],season_name2[10];
*/
unsigned char bill_day[5];
unsigned char bill_time[4];
unsigned char scheduled_bill_day[5];
unsigned char scheduled_bill_time[4];

unsigned int t_zone[16], t_prev_zone;
unsigned long int tod_kwh_val[8], tod_kvah_val[8];

unsigned char Calendar_Name_Active[18];
unsigned char Calendar_Name_Passive[18];
unsigned char Active_Season_Name[2][10];
unsigned char Passive_Season_Name[2][10];
unsigned char active_season_date[2][4];
unsigned char passive_season_date[2][4];
unsigned char tod_time[16][4];         //[8*2][4]
unsigned char passive_tod_time[16][4]; //[8*2][4]
unsigned char tariff_id[2][8];
unsigned char passive_tariff_id[2][8];
unsigned char Activate_Passive_Calendar_Time[14];
unsigned char prev_season;

unsigned long int activate_date;
unsigned long int activate_time;

unsigned int mnth_pos, day_pos;
unsigned int tmpr_time_on_ctr[TAMPER_TYPE], tmpr_time_off_ctr[TAMPER_TYPE];
// unsigned char occur_stat[TAMPER_TYPE];
unsigned int event_pos[TOT_EVENT_TYPE];
unsigned long int store_tamper_stat; //,tamper_stat;
unsigned long int day_hr_pos, tamper_cnt[TAMPER_TYPE];
unsigned long int event_cnt[TOT_EVENT_TYPE];
unsigned long int Tamper_Profile_Entries_In_Use[TOT_EVENT_TYPE];
unsigned long int Tamper_Profile_Entries[TOT_EVENT_TYPE] = {60, 60, 60, 60, 60, 1, 60};
unsigned long int all_tamper_cnt;
// unsigned long int all_tamper_cnt, bill_tamper_cnt, tamper_cnt_array[TAMPER_TYPE];

unsigned long int block_load_val[2], block_vrms, block_irms;

unsigned char checksum, checksum_calc;
unsigned int volt_fact, ampp_fact, ampn_fact, pf_fact, freq_fact, kw_fact, n_cfdenn_val, n_cfdenp_val, m_cfden_val, iagain_val, ibgain_val;
int ph_val, iaph_val, ibph_val;

unsigned long int meter_no;

unsigned char pass_step, pass_ctr;

unsigned char recv_buf[25], recv_ctr, recv_ovf;
unsigned char enable_recv_buf;
unsigned char transmit_complete;
unsigned char comm_delay_ctr;
unsigned long int comm_checksum;

unsigned int bkp_kwmd_val;
unsigned int bkp_kvamd_val;
unsigned int bkp_kwmd_date, bkp_kwmd_time;
unsigned int bkp_kvamd_date, bkp_kvamd_time;

unsigned long int Cum_Power_Off_Count;
unsigned long int Cum_Power_Off_Dur;
unsigned long int Cum_Power_On_Dur;

unsigned char ep_clear_stat;
unsigned int rd_time;

unsigned char time_string[12];
unsigned char Last_MD_Rst_DT[12];
unsigned char KWMD_DT[12];
unsigned char KVAMD_DT[12];
unsigned char tmp_time_string[12]; //={0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char last_tamper_occ_type, last_tamper_res_type;
unsigned int tamper_snap_index;

unsigned char magnetic_stat, eload_stat, rev_stat, ctn_stat, nd_stat, v440_stat, cbyp_stat, nmiss_stat;

unsigned char mag_imax_stat, mag_restore_ctr;
unsigned char mag_chk_delay;

unsigned int Last_Tamper_Id[7];
unsigned char Last_Tamper_Date_Time[7][12];
unsigned long int Cum_Prog_Count;

unsigned long int manfact_date;
unsigned char cuopen_stat;
unsigned long int cuopen_date, cuopen_time;
unsigned char hist_ctr;
unsigned char disp_md_type;
unsigned char read_data_arr[2][30];
unsigned int u10_delay_ctr;
tm_t s_time; // time structure.

uint32_t inst_voltage, inst_phase_current, inst_neutral_current, inst_measured_current, inst_freq, inst_kva;
int32_t inst_kw, inst_kvar, inst_pf, avg_pf;
uint16_t dlms_manufacture_year;
uint8_t SEC, MIN, HOUR, DATE, MONTH, YEAR;

uint8_t execution_billing_date[12][12];
uint8_t dlms_billing_date[12];
stBilling_Profile_t stBilling_Profile;
SA_Range_t SA_Range[2];
uint32_t range_start_entry;
uint32_t range_num_entries;
stLoad_Profile_t stLoad_Profile;
stTamper_Profile_t stTamper_Profile;
stSeason_Profile_t stSeason_Profile[2];
stWeek_Profile_t stWeek_Profile[2];
stDay_Profile_t stDay_Profile[2];
uint8_t dlms_lls_password[16];
uint8_t dlms_hls_password[16];

/* --- DLMS STUB ALLOCATIONS AND HELPER BRIDGES --- */
class07_blockload_entry_t g_Class07_BlockLoadBuffer;

void convt_utc(unsigned long tmp)
{
    (void)tmp;
}

void set_rtc(int bcd_sec, int bcd_min, int bcd_hr, int bcd_day, int bcd_mo, int bcd_yr)
{
    s_time.tm_sec = ((bcd_sec >> 4) * 10) + (bcd_sec & 0x0F);
    s_time.tm_min = ((bcd_min >> 4) * 10) + (bcd_min & 0x0F);
    s_time.tm_hour = ((bcd_hr >> 4) * 10) + (bcd_hr & 0x0F);
    s_time.tm_mday = ((bcd_day >> 4) * 10) + (bcd_day & 0x0F);
    s_time.tm_mon = (((bcd_mo >> 4) * 10) + (bcd_mo & 0x0F)) - 1; // rtc.h: tm_mon is 0-11
    s_time.tm_year = ((bcd_yr >> 4) * 10) + (bcd_yr & 0x0F);      // driver expects 0-99 (years since 2000)
    s_time.tm_wday = SUN;                                         // day-of-week not sent by DLMS write; recompute if needed
    s_time.tm_subsec = 0;

    rtc_write(&s_time);
}

void store_log_dt(int a)
{
    (void)a;
}

void log_config_change_event(int a)
{
    (void)a;
}
/*
uint8_t auth0[2];
uint8_t auth1[10];
uint8_t auth2[18];

uint8_t auth3[10];
*/

// uint8_t Meter_Sr_No[9];
// uint16_t Year_of_Manufacture;
