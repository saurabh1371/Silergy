#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "options.h"    // Define the IC, CE code and other options.
#include "afe.h"        // To read the AFE.
#include "bat.h"        // To read the batteries.
#include "cli.h"        // Command line interface.
#include "delay.h"      // A calibrated delay.
#include "gpio.h"       // GPIO functions.
#include "hash_str.h"	  // Makes a number from a string.
#include "irq.h"        // Interrupt management.
#include "meter.h"	    // Meter API and data structures.
#include "nvram.h"      // Read and write nonvolatile RAM.
#include "pwrmode.h"    // To use battery mode code.
#include "sspi.h"       // Slave SPI initialization
#include "stm.h"        // Software timers
#include "systick.h"    // Systick driver
#include "tamper.h"     // Tamper detection
#include "time.h"       // Clock and calendar.
#include "tmp.h"        // To read the temperature.
#include "wd.h"         // Watchdog driver.
#include "main.h"       // Check prototypes
#include "timer0.h"
#include "Defines.h"

#include "silergy_hdlc_lib.h"
#include "silergy_dlms_lib.h"
#include "dlmsif.h"
#include "dlmsif_timer.h"
#include "timer1.h"
#include "Defines.h"
#include "dlms_variables.h"
//#include "eeprom.h"
#include "dlms_eeprom.h"
#include "dlms_utils.h"
#include "asdaq_variables.h"

unsigned char months[12]={31,28,31,30,31,30,31,31,30,31,30,31};

//1 Phase
//rev(0),el(1),mag(2),nd(3),nm(4)    
//current 	Reverse(51-52),EL(69-70)
//other events	magnet(201,202), nd(203,204), NM(207,208)
const unsigned char event_ids[10]={51,69,201,203,207};

typedef struct tag_date_time_t
{
	uint8_t year_high;                    /* HI and LO of year. Interpreted as long-unsigned,   */
	uint8_t year_low;                     /* Range is 0…0xFFFF-1. 0xFFFF mean not specified.    */
	uint8_t month;                        /* Month. Interpreted as unsigned. 1...12             */
	uint8_t day_of_month;                 /* Day. Interpreted as unsigned. 1...31               */
	uint8_t day_of_week;                  /* Week. Interpreted as unsigned. 1...7               */
	uint8_t hour;                         /* Hour. Interpreted as unsigned. 0...23              */
	uint8_t minute;                       /* Minute. Interpreted as unsigned. 0...59            */
	uint8_t second;                       /* Hour. Interpreted as unsigned. 0...59              */
	uint8_t hundredths;                   /* Hundredths. Interpreted as unsigned. 0...99        */
	uint8_t deviation_high;               /* Interpreted as long,                               */
	uint8_t deviation_low;                /* Range -720…720, in minutes of local time to GMT.   */
	uint8_t clock_status;                 /* The clock status for date time value.              */
} date_time_t;



void default_eeprom(unsigned char stat)
{
      unsigned int i, j;
      unsigned long int location;

	  if(stat==1)	
	  {
  		for(location=0;location<(LOAD_SURVEY_LOC+1024);location=location+EEPROM_PAGE_SIZE) //clears eeprom upto tamper location //MEM_END
	            EPClearPage(location); //write_page_eeprom(location,0);                 
    }
	  else
	  {
	        for(location=128;location<(LOAD_SURVEY_LOC+1024);location=location+EEPROM_PAGE_SIZE) //clears eeprom upto tamper location //MEM_END
		      EPClearPage(location); //write_page_eeprom(location,0);
    }
		
    for(location=DAILY_SURVEY_LOC;location<=MEM_END;location=location+EEPROM_PAGE_SIZE) //clears eeprom upto tamper location //MEM_END
              EPClearPage(location); //write_page_eeprom(location,0); 
        
	  to_eeprom(DATE_LOC,(uint8_t *) &real_date,3);
	  to_eeprom(TIME_LOC,(uint8_t *) &real_time,3);
		scratch=1;
	  to_eeprom(LOCK_STAT_LOC,(uint8_t *) &scratch ,1);
	  

		bill_day[3]=1;
		to_eeprom(BILL_DAY_LOC,(uint8_t *) &bill_day[3],1);


		last_bill_date=d_yr;
		last_bill_date=(last_bill_date*100)+d_mnth;
		last_bill_date=(last_bill_date*100)+bill_day[3];

		//last bill time and current bill time is 0

		to_eeprom(LAST_BILL_DATE_LOC,(uint8_t *) &last_bill_date,3);

	  //for(i=0;i<48;i++)
	  //  	to_eeprom(LOAD_SURVEY_LOC+(i*21), 0xff);
	  //to_eeprom(LOAD_DATE_LOC,real_date,3);
	  
          
	  //if(t_min>=30)
		//	to_eeprom(HR_LOC,(t_hr|0x80));
		//else
		//  to_eeprom(HR_LOC,t_hr);	
	
		clock_time_zone=330;      
		to_eeprom(IST_DIFF_LOC, (uint8_t *) &clock_time_zone, 2);
					
	  i=t_min/md_intgr_val;
	  to_eeprom(PREV_MIN_SLOT_LOC,(uint8_t *) &i,1);
    
		//i=t_min/survey_intgr_val;
	  //to_eeprom(PREV_SURVEY_MIN_SLOT_LOC,i);
		      
		t_zone[0]=0;t_zone[1]=600;t_zone[2]=1000;t_zone[3]=1700;t_zone[4]=2100;
    to_eeprom(TOD_LOC,(uint8_t *) &t_zone[0],2);
		to_eeprom(TOD_LOC+2,(uint8_t *) &t_zone[1],2);
		to_eeprom(TOD_LOC+4,(uint8_t *) &t_zone[2],2);
		to_eeprom(TOD_LOC+6,(uint8_t *) &t_zone[3],2);
		to_eeprom(TOD_LOC+8,(uint8_t *) &t_zone[4],2);


		//to_eeprom(TOD_LOC+10,600,2);
		//to_eeprom(TOD_LOC+12,800,2);
		//to_eeprom(TOD_LOC+14,1100,2);

		t_zone[0]=0;t_zone[1]=500;t_zone[2]=900;t_zone[3]=1800;t_zone[4]=2200;
    to_eeprom(TOD_LOC+(8*2),(uint8_t *) &t_zone[0],2);
		to_eeprom(TOD_LOC+(8*2)+2,(uint8_t *) &t_zone[1],2);
		to_eeprom(TOD_LOC+(8*2)+4,(uint8_t *) &t_zone[2],2);
		to_eeprom(TOD_LOC+(8*2)+6,(uint8_t *) &t_zone[3],2);
		to_eeprom(TOD_LOC+(8*2)+8,(uint8_t *) &t_zone[4],2);


		//to_eeprom(TOD_LOC+(8*2)+10,600,2);
		//to_eeprom(TOD_LOC+(8*2)+12,800,2);
		//to_eeprom(TOD_LOC+(8*2)+14,1100,2);


		t_zone[0]=0;t_zone[1]=600;t_zone[2]=1000;t_zone[3]=1700;t_zone[4]=2100;
    to_eeprom(PASSIVE_TOD_LOC,(uint8_t *) &t_zone[0],2);
		to_eeprom(PASSIVE_TOD_LOC+2,(uint8_t *) &t_zone[1],2);
		to_eeprom(PASSIVE_TOD_LOC+4,(uint8_t *) &t_zone[2],2);
		to_eeprom(PASSIVE_TOD_LOC+6,(uint8_t *) &t_zone[3],2);
		to_eeprom(PASSIVE_TOD_LOC+8,(uint8_t *) &t_zone[4],2);

		//to_eeprom(PASSIVE_TOD_LOC+10,600,2);
		//to_eeprom(PASSIVE_TOD_LOC+12,800,2);
		//to_eeprom(PASSIVE_TOD_LOC+14,1100,2);

		t_zone[0]=0;t_zone[1]=500;t_zone[2]=900;t_zone[3]=1800;t_zone[4]=2200;
    to_eeprom(PASSIVE_TOD_LOC+(8*2),(uint8_t *) &t_zone[0],2);
		to_eeprom(PASSIVE_TOD_LOC+(8*2)+2,(uint8_t *) &t_zone[1],2);
		to_eeprom(PASSIVE_TOD_LOC+(8*2)+4,(uint8_t *) &t_zone[2],2);
		to_eeprom(PASSIVE_TOD_LOC+(8*2)+6,(uint8_t *) &t_zone[3],2);
		to_eeprom(PASSIVE_TOD_LOC+(8*2)+8,(uint8_t *) &t_zone[4],2);
		
		//to_eeprom(PASSIVE_TOD_LOC+(8*2)+10,600,2);
		//to_eeprom(PASSIVE_TOD_LOC+(8*2)+12,800,2);
		//to_eeprom(PASSIVE_TOD_LOC+(8*2)+14,1100,2);


		tariff_id[0][0]=1;tariff_id[0][1]=2;tariff_id[0][2]=3;tariff_id[0][3]=4;
		tariff_id[0][4]=1;tariff_id[0][5]=0;tariff_id[0][6]=0;tariff_id[0][7]=0;
		
		for(i=0;i<8;i++)
		{	
			to_eeprom(TOD_ID_LOC+i,(uint8_t *) &tariff_id[0][i],1);		
			to_eeprom(TOD_ID_LOC+8+i,(uint8_t *) &tariff_id[0][i],1);
			to_eeprom(PASSIVE_TOD_ID_LOC+i,(uint8_t *) &tariff_id[0][i],1);		
			to_eeprom(PASSIVE_TOD_ID_LOC+8+i,(uint8_t *) &tariff_id[0][i],1);
		}
		
		scratch=5;
		to_eeprom(NUM_ZONE_LOC,(uint8_t *) &scratch,1); //8

		
		Active_Season_Name[0][0]='s';Active_Season_Name[0][1]='e';Active_Season_Name[0][2]='a';
		Active_Season_Name[0][3]='s';Active_Season_Name[0][4]='o';Active_Season_Name[0][5]='n';Active_Season_Name[0][6]='1';
		for(i=7;i<10;i++)
			Active_Season_Name[0][i]='\0';
		
		to_eeprom(SEASON_NAME_ACTIVE_LOC,(uint8_t *) &Active_Season_Name[0],10);
		to_eeprom(SEASON_NAME_PASSIVE_LOC,(uint8_t *) &Active_Season_Name[0],10);
		
		Active_Season_Name[0][6]='2';
		to_eeprom(SEASON_NAME_ACTIVE_LOC+10,(uint8_t *) &Active_Season_Name[0],10);
		to_eeprom(SEASON_NAME_PASSIVE_LOC+10,(uint8_t *) &Active_Season_Name[0],10);
		
		//month, day, hr, min
		active_season_date[0][0]=10;active_season_date[0][1]=1;active_season_date[0][2]=0;active_season_date[0][3]=0;
		to_eeprom(SEASON_DATE_ACTIVE_LOC,(uint8_t *) &active_season_date[0],4);
		to_eeprom(SEASON_DATE_PASSIVE_LOC,(uint8_t *) &active_season_date[0],4);
		active_season_date[0][0]=4;active_season_date[0][1]=1;active_season_date[0][2]=0;active_season_date[0][3]=0;
		to_eeprom(SEASON_DATE_ACTIVE_LOC+4,(uint8_t *) &active_season_date[0],4);
		to_eeprom(SEASON_DATE_PASSIVE_LOC+4,(uint8_t *) &active_season_date[0],4);
		
		Calendar_Name_Active[0]='c';Calendar_Name_Active[1]='a';Calendar_Name_Active[2]='l';Calendar_Name_Active[3]='e';
		Calendar_Name_Active[4]='n';Calendar_Name_Active[5]='d';Calendar_Name_Active[6]='a';Calendar_Name_Active[7]='r';
		Calendar_Name_Active[8]='1';
		for(i=9;i<16;i++)
			Calendar_Name_Active[9+i]='\0';
		
		to_eeprom(CAL_NAME_ACTIVE_LOC, (uint8_t *) &Calendar_Name_Active, 16);
		Calendar_Name_Active[8]='2';
		to_eeprom(CAL_NAME_PASSIVE_LOC, (uint8_t *) &Calendar_Name_Active, 16);
		
		//load_control_stat=1;
		//to_eeprom(LOAD_CONTROL_STAT_LOC,load_control_stat);		
	  ep_clear_stat=1;
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
				from_eeprom((KWH_LOC+(index*200)+(tmp_int*4)), (uint8_t *) &scratch, 4);
				from_eeprom(KWH_CHKSUM_LOC+(index*50)+tmp_int, (uint8_t *) &checksum, 1);
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
    //lock_stat=from_eeprom(LOCK_STAT_LOC);
    from_eeprom(MTRNO_LOC, (uint8_t *) &meter_no, 3);
    
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
    from_eeprom(BILL_DAY_LOC, (uint8_t *) &bill_day[3], 1);//day
    bill_day[4]=0xff;  
    
    from_eeprom(BILL_TIME_LOC, (uint8_t *) &bill_time[0], 1);//hr
    from_eeprom(BILL_TIME_LOC+1, (uint8_t *) &bill_time[1], 1);//min
    bill_time[2]=0x00;
    bill_time[3]=0xff;
    
    from_eeprom(SCHEDULED_BILL_DAY_LOC, (uint8_t *) &scheduled_bill_day, 4);
    scheduled_bill_day[4]=0xff;  
    
    from_eeprom(SCHEDULED_BILL_TIME_LOC, (uint8_t *) &scheduled_bill_time, 2);
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
    
    //last_bill_day=from_eeprom(LAST_BILL_DAY_LOC);
    //if((last_bill_day==0)||(last_bill_day>31))
	  //	last_bill_day=billing_day;
	  from_eeprom(LAST_BILL_TIME_LOC,(uint8_t *) &last_bill_time,2);
	  from_eeprom(LAST_BILL_DATE_LOC,(uint8_t *) &last_bill_date,3);


    from_eeprom(MNTHPOS_LOC, (uint8_t *) &mnth_pos,1);
    from_eeprom(MD_RESET_LOC,(uint8_t *) &md_reset_cnt,2);  
    if(md_reset_cnt<(Billing_Profile_Entries-1))
      Billing_Profile_Entries_In_Use=md_reset_cnt+1;
    else
      Billing_Profile_Entries_In_Use=Billing_Profile_Entries;
    
    from_eeprom(DAYPOS_LOC,(uint8_t *) &day_pos,1);
    from_eeprom(HRPOS_LOC,(uint8_t *) &day_hr_pos,2);
    from_eeprom(HRCNT_LOC,(uint8_t *) &Load_Profile_Entries_In_Use,2);
    if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
      Load_Profile_Entries_In_Use=Load_Profile_Entries;


    from_eeprom(DAYCNT_LOC, (uint8_t *) &Daily_Load_Profile_Entries_In_Use,1);
    if(Daily_Load_Profile_Entries_In_Use>Daily_Load_Profile_Entries)
      Daily_Load_Profile_Entries_In_Use=Daily_Load_Profile_Entries;
    
    
    from_eeprom(KWCD_LOC,(uint8_t *) &kwcd_val,4);
    from_eeprom(MANUFACT_DATE_LOC,(uint8_t *) &manfact_date,2);
    Year_of_Manufacture=2000+(manfact_date%100);
			
    from_eeprom(LST_MNTH_DATA_LOC,(uint8_t *) &reset_on_time,2);
            
    for(i=0;i<TOT_EVENT_TYPE;i++)
    {
      from_eeprom(LAST_TAMPER_ID_LOC+(i*2),(uint8_t *) &Last_Tamper_Id[i], 2);
      
      from_eeprom(EVENTPOS_LOC+(i*2), (uint8_t *) &event_pos[i], 2);
      from_eeprom(EVENTCNT_LOC+(i*2), (uint8_t *) &event_cnt[i], 2);
      if(event_cnt[i]<Tamper_Profile_Entries[i])
        Tamper_Profile_Entries_In_Use[i]=event_cnt[i];
      else
        Tamper_Profile_Entries_In_Use[i]=Tamper_Profile_Entries[i];
    }

    from_eeprom(LSTTAMPER_OCC_LOC,(uint8_t *) &last_tamper_occ_type,1);
    from_eeprom(LSTTAMPER_RES_LOC,(uint8_t *) &last_tamper_res_type,1);
    from_eeprom(CUOPEN_LOC,(uint8_t *) &cuopen_stat,1);
    from_eeprom(CUOPEN_LOC+1,(uint8_t *) &cuopen_date,3);
    from_eeprom(CUOPEN_LOC+4,(uint8_t *) &cuopen_time,3);
                      
                      
    //poff_pos=from_eeprom(POFFPOS_LOC,2);
    from_eeprom(POFF_CNT_LOC,(uint8_t *) &Cum_Power_Off_Count,4);
    from_eeprom(POFF_DUR_LOC,(uint8_t *) &Cum_Power_Off_Dur,4);
    from_eeprom(PON_DUR_LOC,(uint8_t *) &Cum_Power_On_Dur,4);
   
    from_eeprom(ALLTAMPER_LOC,(uint8_t *) &store_tamper_stat,3);
    if(store_tamper_stat==0xffffff)
	  store_tamper_stat=0;
    //mag_imax_stat=(store_tamper_stat>>2)&0x000001;
    all_tamper_cnt=0;
    for(i=0;i<TAMPER_TYPE;i++)
    {
      //occur_stat[i]=from_eeprom(INST_TAMPER_LOC+(i*26));
      from_eeprom(TAMPERCNTARR_LOC+(i*2),(uint8_t *) &tamper_cnt[i],2);  
      all_tamper_cnt=all_tamper_cnt+tamper_cnt[i];
    }

    //if(cuopen_stat==1)
    //  all_tamper_cnt=all_tamper_cnt+1;

    from_eeprom(MD_INTGR_LOC,(uint8_t *) &Max_Demand_Capture_Period,2);        
    from_eeprom(SURVEY_INTGR_LOC,(uint8_t *) &Load_Profile_Capture_Period,2);        

    if((Max_Demand_Capture_Period<300)||(Max_Demand_Capture_Period>1800))
      Max_Demand_Capture_Period=1800;//900;

    if((Load_Profile_Capture_Period<300)||(Load_Profile_Capture_Period>1800))
      Load_Profile_Capture_Period=1800;//900;

    
    md_intgr_val=Max_Demand_Capture_Period/60;
    survey_intgr_val=Load_Profile_Capture_Period/60;

    //md_intgr_val=from_eeprom(MD_INTGR_LOC);
    if(((survey_intgr_val%5)!=0)||(survey_intgr_val==0)||(survey_intgr_val>30))
	survey_intgr_val=30;//15;
    
    if(((md_intgr_val%5)!=0)||(md_intgr_val==0)||(md_intgr_val>30))
	md_intgr_val=30;//15;
    
    location=KWMD_LOC+(mnth_pos*19);
    from_eeprom(location+9,(uint8_t *) &kwmd_val,2);
    
    from_eeprom(location+11,(uint8_t *) &kwmd_date,3);
    from_eeprom(location+14,(uint8_t *) &kwmd_time,2);
    get_time_data(kwmd_date, (kwmd_time*100));
    for(i=0;i<12;i++)
      KWMD_DT[i+2]=tmp_time_string[i];

    location=KVAMD_LOC+(mnth_pos*11);
    from_eeprom(location+4,(uint8_t *) &kvamd_val,2);
    
    from_eeprom(location+6,(uint8_t *) &kvamd_date,3);
    from_eeprom(location+9,(uint8_t *) &kvamd_time,2);
    get_time_data(kvamd_date, (kvamd_time*100));
    for(i=0;i<12;i++)
      KVAMD_DT[i+2]=tmp_time_string[i];
    
    
    tmp_int=cyclic(mnth_pos+11-hist_ctr, HISTORY_SIZE); //12 month history
    location=KWMD_LOC+(tmp_int*19);
    
    from_eeprom(location,(uint8_t *) &scratch,3);
    from_eeprom(location+3,(uint8_t *) &scratch1,2);
    get_time_data(scratch, (scratch1*100));
    for(i=0;i<12;i++)
      Last_MD_Rst_DT[i+2]=tmp_time_string[i];

    from_eeprom(location+5,(uint8_t *) &bkp_kwh_val,4);
    from_eeprom(location+9,(uint8_t *) &bkp_kwmd_val,2);
    from_eeprom(location+11,(uint8_t *) &bkp_kwmd_date,3);
    from_eeprom(location+14,(uint8_t *) &bkp_kwmd_time,2);
    from_eeprom(location+18,(uint8_t *) &bkp_pf_val,1);
    
    location=KVAMD_LOC+(tmp_int*11);
    from_eeprom(location,(uint8_t *) &bkp_kvah_val,4);
    from_eeprom(location+4,(uint8_t *) &bkp_kvamd_val,2);
    from_eeprom(location+6,(uint8_t *) &bkp_kvamd_date,3);
    from_eeprom(location+9,(uint8_t *) &bkp_kvamd_time,2);

    from_eeprom(DISP_MD_TYPE_LOC,(uint8_t *) &disp_md_type,1);
	
    for(i=0;i<2;i++)
    	from_eeprom(LAST_HR_KWH_LOC+(i*4),(uint8_t *) &last_hr_load_val[i],4); 

    for(i=0;i<2;i++)
    	from_eeprom(LAST_DEMAND_KWH_LOC+(i*4),(uint8_t *) &last_demand_load_val[i],4);			
    
    
    location=(day_hr_pos+Load_Profile_Entries-1)%Load_Profile_Entries;
    location=LOAD_SURVEY_LOC+(location*12);
    from_eeprom(location,read_data_arr[0],12);
		
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
    Conn_Time_Repeat=from_eeprom(CONN_TIME_REPEAT_LOC);
    
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
    
    over_limit_stat=from_eeprom(OLOAD_STAT_LOC);  
    over_limit_stat=over_limit_stat&0x01;
    
    load_control_stat=from_eeprom(LOAD_CONTROL_STAT_LOC);
    load_control_stat=load_control_stat&0x01;
    if(load_control_stat==0)
        load_conn_stat=0;
    */
    
    from_eeprom(TAMPER_OCC_TIME_LOC,(uint8_t *) &Tamper_Occ_Time,2);
    from_eeprom(TAMPER_RES_TIME_LOC,(uint8_t *) &Tamper_Res_Time,2);
    
    if((Tamper_Occ_Time<15)||(Tamper_Occ_Time>1800))
      Tamper_Occ_Time=15;//900;

    if((Tamper_Res_Time<15)||(Tamper_Res_Time>1800))
      Tamper_Res_Time=15;//300;
    
    
    from_eeprom(PROGRAMCNT_LOC,(uint8_t *) &Cum_Prog_Count,4);
    
    from_eeprom(IST_DIFF_LOC,(uint8_t *) &clock_time_zone, 2);

    //if(clock_time_zone>1440)
    //      clock_time_zone=330;

    j=0xAA;
    for(i=0;i<8;i++)
    {
         from_eeprom(AUTH1_LOC+i,(uint8_t *) &auth1[i],1);
         j=j+auth1[i];
    }
    j=j&0xff;
    
    from_eeprom(AUTH1_LOC+9,(uint8_t *) &j,1);
    
    if(i!=j)
    {
      for(i=0;i<8;i++)
           auth1[i]=65+i;//'0';
      
    }  
    
    j=0xAA;
    for(i=0;i<16;i++)
    {
         from_eeprom(AUTH2_LOC+i, (uint8_t *) &auth2[i],1);
         j=j+auth2[i];
    }
    j=j&0xff;

    from_eeprom(AUTH2_LOC+19, (uint8_t *) &j,1);
    
    if(i!=j)
    {
      //for(i=0;i<16;i++)
      //     auth2[2+i]='a';
      auth2[0]='R';auth2[1]='E';auth2[2]='N';auth2[3]='E';auth2[4]='S';auth2[5]='A';auth2[6]='S';
      auth2[7]='_';auth2[8]='P';auth2[9]='6';auth2[10]='w';auth2[11]='R';auth2[12]='J';auth2[13]='2';
      auth2[14]='1';auth2[15]='F';
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
	        to_eeprom(KWH_LOC+(index*200)+(load_index[index]*4),(uint8_t *) &load_val[index],4);
	        checksum=crc8(load_val[index]);
	        to_eeprom(KWH_CHKSUM_LOC+(index*50)+load_index[index],(uint8_t *) &checksum,1);
	        inst_load[index]=0;
	    }
			//clearnvram(); what is the use of it???
  }    
  return;
}



void inst_data_func(void)
{
	unsigned int i;
	
	float l_kva, l_kvarh_lag, l_kvarh_lead, l_kwh;                      
  unsigned long int load_cnt_val[4];//, vah_val, varh_lead_val, varh_lag_val;
	
	
	dlms_hz = afe_frequency() * 10.0;

	// energy in watt-hours and VAHours
	load_cnt_val[0] = wh(&global.reg.wh_cnt);//afe_get(AFE_WH);
	load_cnt_val[0] += wh(&global.reg.wh_cnt_ex);//afe_get(AFE_WH);

	load_cnt_val[2]  = wh(&global.reg.varh_cnt_ex);//afe_get(AFE_VARHLEAD);//wh(&rpulse_cnt_ex);
	load_cnt_val[3] = wh(&global.reg.varh_cnt);//afe_get(AFE_VARHLAG);


	l_kwh = load_cnt_val[0]/1000.0;
	l_kvarh_lead = load_cnt_val[2]/1000.0;
	l_kvarh_lag = load_cnt_val[3]/1000.0;

	load_cnt_val[1] =1000.0 * sqrtf((float)((l_kwh*l_kwh) + ((l_kvarh_lag + l_kvarh_lead) * (l_kvarh_lag + l_kvarh_lead))));	
	
	
	for(i=0;i<4;i++)
		load_ctr[i]=load_ctr[i]+load_cnt_val[i];
	
	vrms_reg3 = labs((long)(10.0 * afe_vrms(PHASE_A)));//afe_get(AFE_VR);
	irms1_reg3 = labs((long)(1000.0 * afe_arms(PHASE_A)));//afe_get(AFE_IR);
	irms2_reg3 = labs((long)(1000.0 * afe_arms(PHASE_B)));//afe_get(AFE_IY);
	irms_reg3 = MAX(dlms_phase_current,dlms_neutral_current);

	kw_reg3=afe_w(PHASE_A);//afe_get(AFE_WATTS);
	kvar_reg3=afe_var(PHASE_A);//afe_get(AFE_VAR);

	// As per BIS - kvar is +ve for lag, -ve for lead under all conditions 
	// current forward and reverse (current leads reversed)
	// PF       Current Leads    VAR      Watts
	// Lag         Normal        +ve       +ve
	// Lag         Reverse       +ve       -ve
	// Lead        Normal        -ve       +ve
	// Lead        Reverse       -ve       +ve


	l_kva = (kw_reg3 * kw_reg3) + (kvar_reg3 * kvar_reg3);
	kva_reg3 = sqrtf((float)(l_kva));
	
	pf_reg3 = labs((long)(100.0 * afe_power_factor(PHASE_A)));//afe_get(AFE_PF);
	
	//pf_reg4 = dlms_pf;	
	
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
                to_eeprom(LST_MNTH_DATA_LOC,(uint8_t *) &reset_on_time,2);
                Cum_Power_On_Dur=Cum_Power_On_Dur+reset_on_min_ctr;
                to_eeprom(PON_DUR_LOC, (uint8_t *) &Cum_Power_On_Dur,4);
                reset_on_min_ctr=0;
            }
            
            
            if(ep_clear_stat==0)
            {    
							i=prev_hr&0x7f;
              to_eeprom(KWMD_MIN_FLAG_LOC+prev_min,(uint8_t *) &i,1);
              to_eeprom(VOLT_MIN_LOC+(prev_min*2),(uint8_t *) &vrms_reg3,2);
              to_eeprom(AMP_MIN_LOC+(prev_min*2),(uint8_t *) &irms_reg3,2);
              //write_eeprom(PF_MIN_LOC+prev_min,pf_reg3/10);
            }    
            else
              ep_clear_stat=0;
            
            prev_min=t_min;
	    
            
            from_eeprom(PREV_MIN_SLOT_LOC,(uint8_t *) &prev_slot,1);
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
                    
                    to_eeprom(PREV_MIN_SLOT_LOC,(uint8_t *) &curr_slot,1);
                    rd_time=0;
            }
            
            from_eeprom(PREV_SURVEY_MIN_SLOT_LOC,(uint8_t *) &prev_slot,1);
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
                        from_eeprom(KWMD_MIN_FLAG_LOC+k+i,(uint8_t *) &tmp_byte,1);
                        if(tmp_byte==(prev_hr&0x7f))
                        {
                            from_eeprom(VOLT_MIN_LOC+((k+i)*2),(uint8_t *) &tmp_int,2);
                            if(tmp_int>500)
                            {  
                              demand_volt=demand_volt+tmp_int;
                              intgr_ctr[0]++;
                            }
                            
                            from_eeprom(AMP_MIN_LOC+((k+i)*2),(uint8_t *) &tmp_int,2);
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
                      to_eeprom(location,(uint8_t *) &tmp_long,4);
                      
                      
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
                        to_eeprom(location+4+(i*2),(uint8_t *) &scratch,2);
                        block_load_val[i]=scratch;
                      }
                      to_eeprom(location+8,(uint8_t *) &demand_volt,2);
                      block_vrms=demand_volt;
                      to_eeprom(location+10,(uint8_t *) &demand_amp,2);
                      block_irms=demand_amp;
                    }

                    day_hr_pos=day_hr_pos+1;
                    if(day_hr_pos>=Load_Profile_Entries)
                      day_hr_pos=0;

                    to_eeprom(HRPOS_LOC,(uint8_t *) &day_hr_pos,2);

                    if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
                    {
                      Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+1;
                      to_eeprom(HRCNT_LOC,(uint8_t *) &Load_Profile_Entries_In_Use,2);
                    }
                    
                    fill_load_survey_entries(tmp_long, prev_slot, curr_slot);
                    
                    for(i=0;i<2;i++)
                    {
                      
                            if(i==0)
                              last_hr_load_val[i]=load_val[i];
                            else
                              last_hr_load_val[i]=load_val[i]+k;
                              
                            to_eeprom(LAST_HR_KWH_LOC+(i*4),(uint8_t *) &load_val[i],4);
                    }
                    
                    to_eeprom(PREV_SURVEY_MIN_SLOT_LOC,(uint8_t *) &curr_slot,1);
            }
        }

        if((d_day!=prev_day)||(d_mnth!=prev_mnth)||(t_hr!=prev_hr))
				{
              to_eeprom(HR_LOC,(uint8_t *) &t_hr,1);	  
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

                  to_eeprom(location,(uint8_t *) &scratch,3);//real_date
                  if(load_val[0]>load_val[1])
                  {  
										to_eeprom(location+3,(uint8_t *) &load_val[1],4);
                  }
									else
                  {
										to_eeprom(location+3,(uint8_t *) &load_val[0],4);
									}
									
                  to_eeprom(location+7,(uint8_t *) &load_val[1],4);
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
                to_eeprom(DAYCNT_LOC,(uint8_t *) &Daily_Load_Profile_Entries_In_Use,1);
              }
                    
              to_eeprom(DAYPOS_LOC,(uint8_t *) &day_pos,1);
              //write_eeprom(HRPOS_LOC,day_hr_pos);
              to_eeprom(DATE_LOC, (uint8_t *) &real_date, 3);
              
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
                to_eeprom(LST_MNTH_DATA_LOC,(uint8_t *) &reset_on_time,2);
                Cum_Power_On_Dur=Cum_Power_On_Dur+reset_on_min_ctr;
                to_eeprom(PON_DUR_LOC, (uint8_t *) &Cum_Power_On_Dur,4);
                reset_on_min_ctr=0; 
                
                from_eeprom(LST_MNTH_DATA_LOC+6,(uint8_t *) &scratch,3);
								if(all_tamper_cnt>scratch)	
                  scratch=all_tamper_cnt-scratch;
								else
                  scratch=0;
								to_eeprom(BILLPT_LOC+6,(uint8_t *) &scratch,2);
		
                location=KWMD_LOC+(mnth_pos*19);
								to_eeprom(location,(uint8_t *) &real_date,3);
								scratch=real_time/100;
								to_eeprom(location+3,(uint8_t *) &scratch,2);
								to_eeprom(location+5,(uint8_t *) &load_val[0],4);
                to_eeprom(location+16,(uint8_t *) &reset_on_time,2);
                to_eeprom(location+18,(uint8_t *) &avg_pf,1);//pf
		
                get_time_data(real_date, ((real_time/100)*100));
                for(i=0;i<12;i++)
                  Last_MD_Rst_DT[i+2]=tmp_time_string[i];
		
								from_eeprom(location+9,(uint8_t *) &scratch,2);
								kwcd_val=kwcd_val+scratch;
								to_eeprom(KWCD_LOC,(uint8_t *) &kwcd_val,4);

								location=KVAMD_LOC+(mnth_pos*11);
								to_eeprom(location,(uint8_t *) &load_val[1],4);
		
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
                {
									scratch=0;
									to_eeprom(tmp_long,(uint8_t *) &scratch, 1);
								}	
								
                for(j=0;j<TOD_SIZE;j++)//TOD_SIZE
                {
                  from_eeprom(location+(j*11),(uint8_t *) &scratch,4);
                  to_eeprom(location1+(j*11),(uint8_t *) &scratch,4);
                }
                
                location=TOD_KVAMD_LOC+(mnth_pos*88);
                j=cyclic(mnth_pos,HISTORY_SIZE); //13 months including current month	
                location1=TOD_KVAMD_LOC+(j*88);
                for(tmp_long=location1;tmp_long<location1+88;tmp_long++)
                {
									scratch=0;
									to_eeprom(tmp_long,(uint8_t *) &scratch, 1);
								}	
								
                for(j=0;j<TOD_SIZE;j++)//TOD_SIZE
                {
                  from_eeprom(location+(j*11),(uint8_t *) &scratch,4);
                  to_eeprom(location1+(j*11),(uint8_t *) &scratch,4);
                }
                
                mnth_pos=cyclic(mnth_pos,HISTORY_SIZE); //13 months including current month	
        	
								location=KWMD_LOC+(mnth_pos*19);
                for(tmp_long=location;tmp_long<location+19;tmp_long++)
                {
									scratch=0;
									to_eeprom(tmp_long,(uint8_t *) &scratch, 1);
								}	
								
                
                location=KVAMD_LOC+(mnth_pos*11);
                for(tmp_long=location;tmp_long<location+11;tmp_long++)
                {
									scratch=0;
									to_eeprom(tmp_long,(uint8_t *) &scratch, 1);
								}	
								
		
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
														
								to_eeprom(LST_MNTH_DATA_LOC+6,(uint8_t *) &all_tamper_cnt,3);
								to_eeprom(LST_MNTH_DATA_LOC+9,(uint8_t *) &load_val[0],4);
								to_eeprom(LST_MNTH_DATA_LOC+13,(uint8_t *) &load_val[1],4);
								
								md_reset_cnt=md_reset_cnt+1;
								to_eeprom(MD_RESET_LOC,(uint8_t *) &md_reset_cnt,2);  
								to_eeprom(MNTHPOS_LOC,(uint8_t *) &mnth_pos,1);        

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
																scheduled_bill_day[i]=0;
															to_eeprom(SCHEDULED_BILL_DAY_LOC,(uint8_t *) &scheduled_bill_day,4);
															
															for(i=0;i<2;i++)  
																scheduled_bill_time[i]=0;
															to_eeprom(SCHEDULED_BILL_TIME_LOC,(uint8_t *) &scheduled_bill_time,2);
												}	
																		
												//last_bill_day=billing_day;
																					//if(md_reset_b==3)
												//	last_bill_time=curr_time;
												//else
												if(md_reset_b!=3)
																last_bill_time=curr_bill_time;
																		
												//write_eeprom(LAST_BILL_DAY_LOC,last_bill_day);
												to_eeprom(LAST_BILL_TIME_LOC,(uint8_t *) &last_bill_time,2);
												to_eeprom(LAST_BILL_DATE_LOC,(uint8_t *) &last_bill_date,3);
								}
		
                //if(md_reset_b==0)
                //  store_event_data(TRANSACT_EVENT, 154,0);//occ     
                
                //last_kwh_val=load_val[0];
					md_reset_b=0;
				}
				return;
}

void get_time_data(unsigned long int date_val, unsigned long int time_val)
{
	unsigned int tmp_int;

	tmp_int=(unsigned int)(2000+(date_val%100));

	tmp_time_string[0]=(unsigned char)((tmp_int>>8)&0xff); //YR{H)
	tmp_time_string[1]=(unsigned char)(tmp_int&0xff);      //YR(L) 
	tmp_time_string[2]=(unsigned char)((date_val/100)%100);//MNTH 
	tmp_time_string[3]=(unsigned char)((date_val/10000));  //DAY
	tmp_time_string[4]=0xFF;                 //WEEKDAY Not Specified
	tmp_time_string[5]=(unsigned char)(time_val/10000);  //HR 
	tmp_time_string[6]=(unsigned char)((time_val/100)%100);//MIN
	tmp_time_string[7]=(unsigned char)(time_val%100);      //SEC
	tmp_time_string[8]=0xFF;                //HUNDREDTH of SEC  Not Specified
	tmp_time_string[9]=0x80;                //DEV(H)   Not Specified
	tmp_time_string[10]=0x00;                //DEV(L) 
	tmp_time_string[11]=0x00;                //STAT 


	if(date_val==0)
	{
		tmp_time_string[0]=0xff;
		tmp_time_string[1]=0xff;
		tmp_time_string[2]=0xff;
		tmp_time_string[3]=0xff;
		
		if(time_val==0)
		{
			tmp_time_string[5]=0xff;
			tmp_time_string[6]=0xff;
			tmp_time_string[7]=0xff;
		}
		tmp_time_string[11]=0xFF;                //STAT 
	}


	return;  
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
                      to_eeprom(location+9,(uint8_t *) &scratch,2);

                      scratch1=time_val;
                      
                      if(time_val==0)
                      {
                        to_eeprom(location+11,(uint8_t *) &real_date,3);
                        get_time_data(real_date, (scratch1*100));
                        kwmd_date=real_date;
                      }
                      else
                      {
                        to_eeprom(location+11,(uint8_t *) &prev_date,3);
                        get_time_data(prev_date, (scratch1*100));
                        kwmd_date=prev_date;
                      }
                      to_eeprom(location+14,(uint8_t *) &time_val,2);
                      kwmd_time=time_val;
                      
                      for(j=0;j<12;j++)
                        KWMD_DT[j+2]=tmp_time_string[j];
                }
                
                location=TOD_KWMD_LOC+(mnth_pos*88)+(t_prev_zone*11);
                from_eeprom(location+4,(uint8_t *) &j,2);
                if(scratch>j)
                {        
                      to_eeprom(location+4,(uint8_t *) &scratch,2);
                      if(time_val==0)
                      {
												to_eeprom(location+6,(uint8_t *) &real_date,3);
                      }
											else
                      {
												to_eeprom(location+6,(uint8_t *) &prev_date,3);
											}	
                      to_eeprom(location+9,(uint8_t *) &time_val,2);	
                }
          }
          else if(i==1)
          {
              location=KVAMD_LOC+(mnth_pos*11);
              //j=from_eeprom(location+4,2);
              if(scratch>kvamd_val)
              {
                kvamd_val=scratch;
                to_eeprom(location+4,(uint8_t *) &scratch,2);

                scratch1=time_val;
                      
                if(time_val==0)
                {
                  to_eeprom(location+6,(uint8_t *) &real_date,3);
                  get_time_data(real_date, (scratch1*100));
                  kvamd_date=real_date;
                }
                else
                {
                  to_eeprom(location+6,(uint8_t *) &prev_date,3);
                  get_time_data(prev_date, (scratch1*100));
                  kvamd_date=prev_date;
                }
                to_eeprom(location+9,(uint8_t *) &time_val,2);
                kvamd_time=time_val;
                
                for(j=0;j<12;j++)
                  KVAMD_DT[j+2]=tmp_time_string[j];                
              }
              
              location=TOD_KVAMD_LOC+(mnth_pos*88)+(t_prev_zone*11);
              from_eeprom(location+4,(uint8_t *) &j, 2);
              if(scratch>j)
              {        
                to_eeprom(location+4,(uint8_t *) &scratch,2);
                if(time_val==0)
                {
									to_eeprom(location+6,(uint8_t *) &real_date,3);//6
                }
								else
                {
									to_eeprom(location+6,(uint8_t *) &prev_date,3);//6
								}	
                to_eeprom(location+9,(uint8_t *) &time_val,2);	
              }
        }
        last_demand_load_val[i]=load_val[i];
        to_eeprom(LAST_DEMAND_KWH_LOC+(i*4),(uint8_t *) &last_demand_load_val[i],4);			
    }
}


void clearnvram(void)
{
	memset((uint8_t*)&nvm.reg, 0, sizeof(Reg_t));
	global.reg = nvm.reg;
}

unsigned int crc8(unsigned long int long_val)
{
	unsigned int crc, i, j;
		
	crc=0;
	for(i=0;i<8;i++)
	{	
		j=(unsigned int)((long_val>>(i*4))&0xf);
		j=j*(i+1);
		crc=crc+j;
	}	
	return crc;
}

/**********************************************
cyclic works like a round-robin scheduler
**********************************************/

unsigned int cyclic(unsigned int val, unsigned int max)
{
	val=val+1;
	while(val>=max)
		val=val-max;

	return val;
}


void reset_ls_data(void)
{
	unsigned char curr_slot,num_slots;
	unsigned char i,j,k,tmp_byte;
	unsigned int tmp_int;
	unsigned int location;
	//unsigned int block_load_val[2],block_vrms=0;
	
	curr_slot=t_min/survey_intgr_val;
        
	day_hr_pos=0;
	Load_Profile_Entries_In_Use=0;
	to_eeprom(HRCNT_LOC,(uint8_t *) &Load_Profile_Entries_In_Use,2);

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
									to_eeprom(location,(uint8_t *) &scratch,4);
									k=0xff;
									to_eeprom(location+4,(uint8_t *) &k,1);
									block_load_val[0]=0;block_load_val[1]=0;block_vrms=0;block_irms=0;
										
									day_hr_pos=day_hr_pos+1;
									if(day_hr_pos>=Load_Profile_Entries)//3456
										day_hr_pos=0;
					}

					to_eeprom(HRPOS_LOC,(uint8_t *) &day_hr_pos,2);
					if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
					{
						Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+j;
						if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
							Load_Profile_Entries_In_Use=Load_Profile_Entries;
						to_eeprom(HRCNT_LOC,(uint8_t *) &Load_Profile_Entries_In_Use,2);
					}
	}									

	for(i=0;i<2;i++)
	{
		last_hr_load_val[i]=load_val[i];
		to_eeprom(LAST_HR_KWH_LOC+(i*4),(uint8_t *) &load_val[i],4);
	}
	to_eeprom(PREV_SURVEY_MIN_SLOT_LOC,(uint8_t *) &curr_slot,1);
}


void fill_load_survey_entries(unsigned long int last_time, unsigned char prev_slot, unsigned char curr_slot)
{
		unsigned char i, k, tmp_byte, num_slots; //k, 
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
					to_eeprom(location,(uint8_t *) &last_time,4);
					k=0xff;
					to_eeprom(location+4,(uint8_t *) &k,1);
					block_load_val[0]=0;block_load_val[1]=0;block_vrms=0;block_irms=0;
					
					day_hr_pos=day_hr_pos+1;
					if(day_hr_pos>=Load_Profile_Entries)//3456
						day_hr_pos=0;
		
				}
	
				to_eeprom(HRPOS_LOC,(uint8_t *) &day_hr_pos,2);
				if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
				{
					Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+j;
					if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
						Load_Profile_Entries_In_Use=Load_Profile_Entries;
					to_eeprom(HRCNT_LOC,(uint8_t *) &Load_Profile_Entries_In_Use,2);
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
					to_eeprom(location,(uint8_t *) &scratch,4);
					k=0xff;
					to_eeprom(location+4,(uint8_t *) &k,1);
					block_load_val[0]=0;block_load_val[1]=0;block_vrms=0;block_irms=0;
						
					day_hr_pos=day_hr_pos+1;
					if(day_hr_pos>=Load_Profile_Entries)//3456
						day_hr_pos=0;
				}

					to_eeprom(HRPOS_LOC,(uint8_t *) &day_hr_pos,2);
					if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
					{
						Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+j;
						if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
							Load_Profile_Entries_In_Use=Load_Profile_Entries;
						to_eeprom(HRCNT_LOC,(uint8_t *) &Load_Profile_Entries_In_Use,2);
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
					to_eeprom(location,(uint8_t *) &last_time,4);
					k=0xff;
					to_eeprom(location+4,(uint8_t *) &k,1);
					block_load_val[0]=0;block_load_val[1]=0;block_vrms=0;block_irms=0;
							
					day_hr_pos=day_hr_pos+1;
					if(day_hr_pos>=Load_Profile_Entries)//3456
						day_hr_pos=0;
		
				}
	
				to_eeprom(HRPOS_LOC,(uint8_t *) &day_hr_pos,2);
				if(Load_Profile_Entries_In_Use<Load_Profile_Entries)
				{
					Load_Profile_Entries_In_Use=Load_Profile_Entries_In_Use+j;
					if(Load_Profile_Entries_In_Use>Load_Profile_Entries)
						Load_Profile_Entries_In_Use=Load_Profile_Entries;
					to_eeprom(HRCNT_LOC,(uint8_t *) &Load_Profile_Entries_In_Use,2);
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
    if(rev_stat==1)
      tmp_tamper_stat=(tmp_tamper_stat|((0x000001)<<0));
    //else if(((rev_stat[i]==1)||(irms_reg3[i]<100)||(pf_reg3[i]<200))&&(indv_tamper_stat==1))  
    else if((rev_stat==2)&&(indv_tamper_stat==1))  	
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
                    to_eeprom(LSTTAMPER_OCC_LOC,(uint8_t *) &last_tamper_occ_type,1);

                    /*
                    scratch=from_eeprom(location+1,3);
                    to_eeprom(LSTTAMPER_OCC_LOC+1,scratch,3);//real_date

                    scratch=from_eeprom(location+4,3);
                    to_eeprom(LSTTAMPER_OCC_LOC+4,scratch,3);//real_time
                    */
                    
                    to_eeprom(LSTTAMPER_OCC_LOC+1,(uint8_t *) &real_date,3);//real_date
                    to_eeprom(LSTTAMPER_OCC_LOC+4,(uint8_t *) &real_time,3);//real_time

                    to_eeprom(ALLTAMPER_LOC,(uint8_t *) &store_tamper_stat,3);
                    all_tamper_cnt=all_tamper_cnt+1;
                    tamper_cnt[i]=tamper_cnt[i]+1;
                    to_eeprom(TAMPERCNTARR_LOC+(i*2),(uint8_t *) &tamper_cnt[i],2);
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
                    to_eeprom(LSTTAMPER_RES_LOC,(uint8_t *) &last_tamper_res_type,1);
                    /*
                    scratch=from_eeprom(location1+1,3);
                    to_eeprom(LSTTAMPER_RES_LOC+1,scratch,3);//real_date

                    scratch=from_eeprom(location1+4,3);
                    to_eeprom(LSTTAMPER_RES_LOC+4,scratch,3);//real_time
                    */
                    to_eeprom(LSTTAMPER_RES_LOC+1,(uint8_t *) &real_date,3);//real_date
                    to_eeprom(LSTTAMPER_RES_LOC+4,(uint8_t *) &real_time,3);//real_time

                    to_eeprom(ALLTAMPER_LOC,(uint8_t *) &store_tamper_stat,3);                                  
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
   to_eeprom(LAST_TAMPER_ID_LOC+((unsigned long int)event_type*2),(uint8_t *) &Last_Tamper_Id[event_type],2);
      
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
     to_eeprom(PROGRAMCNT_LOC,(uint8_t *) &Cum_Prog_Count,4);
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
       
       to_eeprom(location,(uint8_t *) &event_id,2);
       to_eeprom(location+2,(uint8_t *) &real_date,3);
       to_eeprom(location+5,(uint8_t *) &real_time,3);
       to_eeprom(location+8,(uint8_t *) &vrms_reg3,2);
       to_eeprom(location+10,(uint8_t *) &irms_reg3,2);
			 scratch=pf_reg3/10;
       to_eeprom(location+12,(uint8_t *) &scratch,1);
       to_eeprom(location+13,(uint8_t *) &load_val[0],4);
       
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

       to_eeprom(location,(uint8_t *) &event_id,2);
       if(event_id==101)
       {  
        from_eeprom(DATE_LOC,(uint8_t *) &scratch,3);
        to_eeprom(location+2,(uint8_t *) &scratch,3);
        from_eeprom(TIME_LOC,(uint8_t *) &scratch,3);
        to_eeprom(location+5,(uint8_t *) &scratch,3);
       }
       else
       {  
        to_eeprom(location+2,(uint8_t *) &real_date,3);
        to_eeprom(location+5,(uint8_t *) &real_time,3);
       } 
   }
   else
   {
       tmp_int=(event_pos[event_type]*8);
       location=location+tmp_int;

       to_eeprom(location,(uint8_t *) &event_id,2);
       to_eeprom(location+2,(uint8_t *) &real_date,3);
       to_eeprom(location+5,(uint8_t *) &real_time,3);
   }

   event_cnt[event_type]=event_cnt[event_type]+1;
   if(event_cnt[event_type]<Tamper_Profile_Entries[event_type])
    Tamper_Profile_Entries_In_Use[event_type]=event_cnt[event_type];
   else
    Tamper_Profile_Entries_In_Use[event_type]=Tamper_Profile_Entries[event_type];

   to_eeprom(EVENTCNT_LOC+(event_type*2),(uint8_t *) &event_cnt[event_type],2);
   to_eeprom(EVENTPOS_LOC+(event_type*2),(uint8_t *) &event_pos[event_type],2);

   return;
}

void power_fail_func(void)
{
    unsigned long int tmp_long;//, location;
    unsigned int tmp_int;
    unsigned char i,j;
	
    //fower fail check
    from_eeprom(DATE_LOC,(uint8_t *) &tmp_long,3);

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
	
    from_eeprom(TIME_LOC,(uint8_t *) &tmp_long,3);
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
       to_eeprom(POFF_CNT_LOC, (uint8_t *) &Cum_Power_Off_Count,4);
       to_eeprom(POFF_DUR_LOC, (uint8_t *) &Cum_Power_Off_Dur,4);
    }
    to_eeprom(TIME_LOC,(uint8_t *) &real_time,3);
}

void init_tod(void)//unsigned int pos, unsigned long int e_val)
{
	unsigned int i,j,k;//,tmp_int;
	unsigned long int location, location1;
        
	//----TOD------------------------------
	
        for(j=0;j<2;j++)
        {  
          for(i=0;i<TOD_SIZE;i++)//TOD_SIZE
          {
            from_eeprom(TOD_LOC+(j*16)+(i*2),(uint8_t *) &t_zone[(j*8)+i],2);//read_eeprom(TOD_LOC+i);
            tod_time[(j*8)+i][0]=t_zone[(j*8)+i]/100;
            tod_time[(j*8)+i][1]=t_zone[(j*8)+i]%100;
            tod_time[(j*8)+i][2]=0;
            tod_time[(j*8)+i][3]=0xff;
            from_eeprom(TOD_ID_LOC+(j*8)+i,(uint8_t *) &tariff_id[j][i],1);
          }
        }

	//num_of_zone=read_eeprom(NUM_ZONE_LOC);
        //if((num_of_zone<2)||(num_of_zone>TOD_SIZE))
	//	num_of_zone=TOD_SIZE;	
	 
	from_eeprom(T_ZONE_LOC, (uint8_t *) &t_prev_zone,1);
	for(i=0;i<2;i++)
		from_eeprom(LAST_STORE_TOD_KWH_LOC+(i*4),(uint8_t *) &last_stored_tod_load_val[i],4);
        
        
        location=TOD_KWMD_LOC+(mnth_pos*88);
        location1=TOD_KVAMD_LOC+(mnth_pos*88);

        for(i=0;i<TOD_SIZE;i++)
        { 
          from_eeprom(location+(i*11),(uint8_t *) &scratch,4);
          from_eeprom(location1+(i*11),(uint8_t *) &scratch1,4);
          
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
        
        from_eeprom(ACTIVATE_DATE_LOC,(uint8_t *) &activate_date,3);
        from_eeprom(ACTIVATE_TIME_LOC,(uint8_t *) &activate_time,2);
        
        get_time_data(activate_date,(activate_time*100));
        for(i=0;i<12;i++)
          Activate_Passive_Calendar_Time[i+2]=tmp_time_string[i];

        //if(activate_date!=0)
        //{
          for(j=0;j<2;j++)
          {  
            for(i=0;i<TOD_SIZE;i++)//TOD_SIZE
            {
              from_eeprom(PASSIVE_TOD_LOC+(j*16)+(i*2),(uint8_t *) &k,2);//read_eeprom(TOD_LOC+i);
              passive_tod_time[(j*8)+i][0]=k/100;
              passive_tod_time[(j*8)+i][1]=k%100;
              passive_tod_time[(j*8)+i][2]=0;
              passive_tod_time[(j*8)+i][3]=0xff;
              from_eeprom(PASSIVE_TOD_ID_LOC+(j*8)+i,(uint8_t *) &passive_tariff_id[j][i],1);
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
          
				from_eeprom(CAL_NAME_ACTIVE_LOC,Calendar_Name_Active,16);
				from_eeprom(CAL_NAME_PASSIVE_LOC,Calendar_Name_Passive,16);
        
        for(j=0;j<2;j++)
        {  
					from_eeprom(SEASON_NAME_ACTIVE_LOC+(j*10),(uint8_t *) &Calendar_Name_Active[j],10);
					from_eeprom(SEASON_NAME_PASSIVE_LOC+(j*10),(uint8_t *) &Calendar_Name_Passive[j],10);
					from_eeprom(SEASON_DATE_ACTIVE_LOC+(j*4),(uint8_t *) &active_season_date[j],4);
					from_eeprom(SEASON_DATE_PASSIVE_LOC+(j*4),(uint8_t *) &passive_season_date[j],4);
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
            from_eeprom(PASSIVE_TOD_LOC+(j*16)+(i*2),(uint8_t *) &k,2);
            to_eeprom(TOD_LOC+(j*16)+(i*2),(uint8_t *) &k,2);
            t_zone[(j*8)+i]=k;
            tod_time[(j*8)+i][0]=t_zone[(j*8)+i]/100;
            tod_time[(j*8)+i][1]=t_zone[(j*8)+i]%100;
            tod_time[(j*8)+i][2]=0;
            tod_time[(j*8)+i][3]=0xff;
            
            from_eeprom(PASSIVE_TOD_ID_LOC+(j*8)+i,(uint8_t *) &k,1);
            to_eeprom(TOD_ID_LOC+(j*8)+i,(uint8_t *) &k,1);
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
          Calendar_Name_Active[i]=Calendar_Name_Passive[i];
        to_eeprom(CAL_NAME_ACTIVE_LOC,Calendar_Name_Active,16);
				
        for(j=0;j<2;j++)
        {  
          for(i=0;i<10;i++)
            Active_Season_Name[j][i]=Passive_Season_Name[j][i];
          for(i=0;i<4;i++)
            active_season_date[j][i]=passive_season_date[j][i];
        	to_eeprom(SEASON_NAME_ACTIVE_LOC+(j*10),(uint8_t *) &Calendar_Name_Active[j],10);
					to_eeprom(SEASON_DATE_ACTIVE_LOC+(j*4),(uint8_t *) &active_season_date[j],4);
				}	
        
				activate_date=0;
        activate_time=0;
        to_eeprom(ACTIVATE_DATE_LOC,(uint8_t *) &activate_date,3);
        to_eeprom(ACTIVATE_TIME_LOC,(uint8_t *) &activate_time,2);
        
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
          to_eeprom(T_ZONE_LOC,(uint8_t *) &t_curr_zone,1);
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
              from_eeprom(TOD_KWMD_LOC+(t_prev_zone*11)+(mnth_pos*88),(uint8_t *) &scratch1,4);
              scratch=scratch+scratch1;
              to_eeprom(TOD_KWMD_LOC+(t_prev_zone*11)+(mnth_pos*88),(uint8_t *) &scratch,4);
              tod_kwh_val[t_prev_zone]=scratch;
            }
            else
            {
              from_eeprom(TOD_KVAMD_LOC+(t_prev_zone*11)+(mnth_pos*88),(uint8_t *) &scratch1,4);
              scratch=scratch+scratch1;
              to_eeprom(TOD_KVAMD_LOC+(t_prev_zone*11)+(mnth_pos*88),(uint8_t *) &scratch,4);
              tod_kvah_val[t_prev_zone]=scratch;
            }
          }
          last_stored_tod_load_val[i]=load_val[i];
          to_eeprom(LAST_STORE_TOD_KWH_LOC+(i*4),(uint8_t *) &load_val[i],4);
	}
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

      from_eeprom(location,read_data_arr[0],19);
			from_eeprom(location,read_data_arr[1],11);
			

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
      
      stBilling_Profile.md_kw.kw=from_data_arr(9,0,2);
      
      scratch=from_data_arr(11,0,3);
      scratch1=from_data_arr(14,0,2);
      get_time_data(scratch, (scratch1*100));
      memcpy(stBilling_Profile.md_kw.date,tmp_time_string,12);
      
      stBilling_Profile.md_kva.kva=from_data_arr(4,1,2);
      
      scratch=from_data_arr(6,1,3);
      scratch1=from_data_arr(9,1,2);
      get_time_data(scratch, (scratch1*100));
      memcpy(stBilling_Profile.md_kva.date,tmp_time_string,12);
      
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
        stBilling_Profile.tz_kwh[i]=0;
        stBilling_Profile.tz_kvah[i]=0;
        stBilling_Profile.tz_md_kw[i].kw=0;  
        stBilling_Profile.tz_md_kva[i].kva=0;
        get_time_data(0,0);
        memcpy(stBilling_Profile.tz_md_kw[i].date,tmp_time_string,12);
        memcpy(stBilling_Profile.tz_md_kva[i].date,tmp_time_string,12);        
        
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
                stBilling_Profile.tz_kwh[i]=stBilling_Profile.tz_kwh[i]+scratch;
                stBilling_Profile.tz_kvah[i]=stBilling_Profile.tz_kvah[i]+scratch1;
                
                scratch=from_data_arr(4,0,2);  
                if(scratch>stBilling_Profile.tz_md_kw[i].kw)
                {
                  stBilling_Profile.tz_md_kw[i].kw=scratch;
                  scratch=from_data_arr(6,0,3);
                  scratch1=from_data_arr(9,0,2);
                  get_time_data(scratch, (scratch1*100));
                  memcpy(stBilling_Profile.tz_md_kw[i].date,tmp_time_string,12);                
                }  
                
                scratch=from_data_arr(4,1,2);  
                if(scratch>stBilling_Profile.tz_md_kva[i].kva)
                {  
                  stBilling_Profile.tz_md_kva[i].kva=scratch;
                  scratch=from_data_arr(6,1,3);
                  scratch1=from_data_arr(9,1,2);
                  get_time_data(scratch, (scratch1*100));
                  memcpy(stBilling_Profile.tz_md_kva[i].date,tmp_time_string,12);
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

void dlms_actions_func(void)
{
	unsigned int i, j, k;
	
		if(set_auth_data!=0)
		{
				if(set_auth_data==1)
				{
					j=0xAA;
					for(i=0;i<8;i++)
					{
							 to_eeprom(AUTH1_LOC+i,(uint8_t *) &auth1[i],1);
							 j=j+auth1[i];
					}
					j=j&0xff;
					to_eeprom(AUTH1_LOC+9,(uint8_t *) &j,1);
				}
				else if(set_auth_data==2)
				{
					j=0xAA;
					for(i=0;i<16;i++)
					{
							 to_eeprom(AUTH2_LOC+i,(uint8_t *) &auth2[i],1);
							 j=j+auth2[i];
					}
					j=j&0xff;
					to_eeprom(AUTH2_LOC+19,(uint8_t *) &j,1);
				}
				set_auth_data=0;
		}

		if(set_cal_name_data==1)
		{				
				to_eeprom(CAL_NAME_PASSIVE_LOC,(uint8_t *) &Calendar_Name_Passive,16);
				set_cal_name_data=0;
		}

		if(set_season_profile==1)
		{
				for(j=0;j<2;j++)
				{  
						to_eeprom(SEASON_NAME_PASSIVE_LOC+(j*10), (uint8_t *) &Passive_Season_Name[j],4);
						to_eeprom(SEASON_DATE_PASSIVE_LOC+(j*4), (uint8_t *) &passive_season_date[j],1);
				}
				set_season_profile=0;
		}
		

		if(set_clock_time_zone==1)
		{
			to_eeprom(IST_DIFF_LOC, (uint8_t *) &clock_time_zone,2);
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
			
			/*
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
			*/
			
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

			to_eeprom(MD_INTGR_LOC,(uint8_t *) &Max_Demand_Capture_Period,2);        
			md_intgr_val=Max_Demand_Capture_Period/60;
			if(((md_intgr_val%5)!=0)||(md_intgr_val==0)||(md_intgr_val>30))
					md_intgr_val=30;//15;
		
			
			i=t_min/md_intgr_val;
			to_eeprom(PREV_MIN_SLOT_LOC,(uint8_t *) &i,1);

			store_event_data(TRANSACT_EVENT, 152,0);//occ     
		
			set_demand_intgr_period=0;
		}
		
		if(set_capture_period==1)
		{
			//if((Load_Profile_Capture_Period<300)||(Load_Profile_Capture_Period>1800))
			if((Load_Profile_Capture_Period!=900)&&(Load_Profile_Capture_Period!=1800))
				Load_Profile_Capture_Period=1800;//900;

			to_eeprom(SURVEY_INTGR_LOC,(uint8_t *) &Load_Profile_Capture_Period,2); 
			survey_intgr_val=Load_Profile_Capture_Period/60;
			if(((survey_intgr_val%5)!=0)||(survey_intgr_val==0)||(survey_intgr_val>30))
					survey_intgr_val=30;//15;

			i=t_min/survey_intgr_val;
			to_eeprom(PREV_SURVEY_MIN_SLOT_LOC,(uint8_t *) &i,1);
		
			store_event_data(TRANSACT_EVENT, 153,0);//occ     
			reset_ls_data();
			set_capture_period=0;
		}

		
		if(set_bill_date==1)
		{
			//to_eeprom(BILL_DAY_LOC,bill_day[3]);
			//to_eeprom(BILL_TIME_LOC,bill_time[0]);
			//to_eeprom(BILL_TIME_LOC+1,bill_time[1]);
			
			if((scheduled_bill_day[0]==0)&&(scheduled_bill_day[1]==0)&&(scheduled_bill_day[2]==0))	
			{
							bill_day[3]=scheduled_bill_day[3];
							bill_time[0]=scheduled_bill_time[0];
							bill_time[1]=scheduled_bill_time[1];
							to_eeprom(BILL_DAY_LOC,(uint8_t *) &bill_day[3],1);
							for(i=0;i<2;i++)  
								to_eeprom(BILL_TIME_LOC+i,(uint8_t *) &bill_time[i],1);
							
							scheduled_bill_day[3]=0;
							
			}
			else
			{
							for(i=0;i<4;i++)  
								to_eeprom(SCHEDULED_BILL_DAY_LOC+i,(uint8_t *) &scheduled_bill_day[i],1);
							
							for(i=0;i<2;i++)  
								to_eeprom(SCHEDULED_BILL_TIME_LOC+i,(uint8_t *) &scheduled_bill_time[i],1);
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
					to_eeprom(TOD_ID_LOC+(j*8)+i,tariff_id[j][i]);

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
			to_eeprom(NUM_ZONE_LOC,num_of_zone);
			
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
					to_eeprom(PASSIVE_TOD_ID_LOC+(j*8)+i,(uint8_t *) &passive_tariff_id[j][i],1);
					k=passive_tod_time[(j*8)+i][0];
					k=(k*100)+passive_tod_time[(j*8)+i][1];
					to_eeprom(PASSIVE_TOD_LOC+(j*16)+(i*2),(uint8_t *) &k,2);
				}
			}
			
			//to_eeprom(ACTIVATE_DATE_LOC,activate_date,3);
			//to_eeprom(ACTIVATE_TIME_LOC,activate_time,2);

			//store_event_data(TRANSACT_EVENT, 155,0);//occ     
			set_day_profile=0;
		}
		
		if(set_activation_date==1)
		{
			to_eeprom(ACTIVATE_DATE_LOC,(uint8_t *) &activate_date,3);
			to_eeprom(ACTIVATE_TIME_LOC,(uint8_t *) &activate_time,2);
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
			to_eeprom(CONN_TIME_REPEAT_LOC,Conn_Time_Repeat);
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
						to_eeprom(OLOAD_STAT_LOC, over_limit_stat);  
					}
			}
			else
			{
					unset_all_phase();
					load_conn_stat=0;
					over_limit_time_ctr=0;
					load_break_time_ctr=0;
			}
			to_eeprom(LOAD_CONTROL_STAT_LOC,load_control_stat);
			set_load_control_stat=0;
		}  
		*/
		
		/*
		if(set_tamper_time==1)
		{

			if((Tamper_Occ_Time<15)||(Tamper_Occ_Time>1800))
				Tamper_Occ_Time=15;//900;

			if((Tamper_Res_Time<15)||(Tamper_Res_Time>1800))
				Tamper_Res_Time=15;//300;

			to_eeprom(TAMPER_OCC_TIME_LOC,(uint8_t *) &Tamper_Occ_Time,2);
			to_eeprom(TAMPER_RES_TIME_LOC,(uint8_t *) &Tamper_Res_Time,2);
			set_tamper_time=0;
		}
		*/
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
