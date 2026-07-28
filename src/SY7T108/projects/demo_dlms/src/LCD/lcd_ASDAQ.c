
#include "options.h"    // System options for compiler, hardware, etc.
#include <math.h>       // for fabs()
#include "afe.h"        // For meter data to display.
#include "bat.h"        // For battery readings to display.
#include "pwrmode.h"    // To read the battery mode.
#include <ctype.h>
//#include "LCD_BQ14053.h"
#include "lcd_ASDAQ.h"// Display labels.

#include "dlms_eeprom.h"   
#include "dlms_variables.h"
#include "asdaq_variables.h"
#include "Defines.h"
#include "wd.h"         // Watchdog driver.
void  PushButtonDisplay(void);
void Display_Version(void);
void PushButtonWakeDisplay(void);
void TamperIcons(void);

 uint8_t LCD_DisplayParm;

uint32_t eepromtestvar;
/***************************************************************************   
* Description:  
*   Write a bit to the LCD map.
* Parameters:
*   Input:
*         pin_idx is an LCD pin number, 0.. max.
*         val is a value to write to a map register bit.
*   Output: 
*         LCD pins are enabled or disabled.
* Return Status: 
*         None
***************************************************************************/
void lcd_map(int pin_idx, int val)
{
    int bit_idx, sfr_idx, tmp, mask;

    if (LCD_PIN_CNT <= pin_idx) /* the pin is invalid */
        return;

    bit_idx = 0x1F & pin_idx;
    sfr_idx = pin_idx >> 5; 
    switch(sfr_idx) {
        case 0: tmp = LCD->MAPL; break; // Controls SEG0..31
        case 1: tmp = LCD->MAPH; break; // Controls SEG32..35
        default: tmp = 0; break;
    }
    mask = 1UL << bit_idx;
    if(0 == val)
    {
        tmp &= ~mask;
    } else {
        tmp |= mask;
    }
    switch(sfr_idx) {
        case 0: LCD->MAPL = tmp; break; // Controls SEG0..31
        case 1: LCD->MAPH = tmp; break; // Controls SEG32..35
        default: break;
    }
} // lcd_map()

uint32_t LCD_Buffer[10]={0};
static void lcd_set_seg(uint32_t seg_idx, uint32_t seg)
{
	// LCD->DIO[seg_idx] |= seg;
	LCD_Buffer[seg_idx]  |= seg;
}
void Put_Data_On_LCD(void)
{
 	int8_t i;
	for (i=0;i<10;i++)
	{
		LCD->DIO[i] =0;
		LCD->DIO[i] =LCD_Buffer[i];
	}
	
	for (i=0;i<10;i++)//Clear buffer once data on Seg is loaded
	{
		LCD_Buffer[i]=0;;
	}
}

void lcd_put_icon (int32_t icon_idx)
{
    lcd_set_seg((uint32_t)icon_seg_8COM[icon_idx], 
        (uint32_t)icon_common_8COM[icon_idx]);	  
}

void put_ch_LCD (int l_idx, int f_idx)
{
   if(0 >= l_idx || DIGIT_CNT < l_idx)
        return;

			lcd_set_seg((uint32_t)char_seg_8COM[l_idx], 
                (uint32_t)font_8COM[f_idx]);
}

void lcd_put_char (int32_t chr_idx, char ch)
{
 int chr = (int)ch;
    // Convert an ISO character code to an index into the font.
    // Write only known characters.
    // Leave a blank for a space or any character with no font entry.
    if( isdigit(chr) )
    {
        chr -= '0';         // Numerals in the font start at 0.
    }
    else 
    {
        if( isalpha(chr) )
        {
            chr = toupper(chr); // Accept lower case alphas.          
            chr -= ('A'-10);    // Index the alphas after the numerals.                      
        }
        else 
        {
            if( '-' == chr )
            {
                chr = LCD_MINUS;
            }
            else 
            {
                if( '.' == chr )
                {
                    // Write '.' if it exists.
                    if( LCD_FIRST_PERIOD <= chr_idx && chr_idx <= LCD_LAST_PERIOD )
                    {
                        lcd_put_icon(chr_idx - LCD_FIRST_PERIOD);
                    }
                    chr = -1;             // '.' is written, so don't write more data.
                }
                else 
                {
                    chr = -1;             // Not in font, so write nothing.
                }
            }
        }
    }
    if( 0 <= chr )
    {
        put_ch_LCD (chr_idx, chr);
    }
} 

void lcd_put_flash_str( int32_t i , const char * str )
{
    uint8_t chr;                // The character.

    for(;;)
    {
        chr = *str++;           // Get the next character. 

    // Exit the loop at the end of the string or at the end of the display.
    if (0 == chr || DIGIT_CNT < i) break; 

			lcd_put_char(i, chr);
        // In a string, '.' is between chars.
        if ('.' == chr)
            --i;
				else
					i++;

        
			}
       
}

const  char error_str[] = "Error";
void lcd_put_num(int32_t chr_idx, int32_t digit_cnt, int32_t num)
{
    int32_t chr;
    int32_t i;

    if (num < 0)
    {
return;
    }

    // The digit is made starting from the right side to the left.
    for (i = chr_idx + digit_cnt - 1; i >= chr_idx; --i)
    {
        chr = num % 10;
        num /= 10;

        // Set the display's segments from commons data in the font arrays.
        lcd_put_char(i, chr + '0');

    }

}

void lcd_clear (void)
{
	LCD->MODE_b.rst = 1; // Toggle the reset to clear the segments.
	LCD->MODE_b.rst = 0;
	LCD->MODE_b.blank = 0; // Enable the segments.
	LCD->MODE_b.on = 0; // Clear test as well
	
} // void lcd_clear (void)

void lcd_test (void)
{
  LCD->MODE_b.on = 1;
} // void lcd_test (void)

void lcd_init_1(void)
{	
	LCD->MAPL = LCD_MAPL_DEF;
	LCD->MAPH = LCD_MAPH_DEF;
	LCD->MODE = LCD_MODE_DEF;
	LCD->CNTL = 0x00000004;//LCD->MODE, LCD DAC Bit 6 is set. So, lcd voltage will be (2.65V + 2.65V * 4/31)=2.9919V
	lcd_clear();
}
uint8_t LCD_PushButton_Parm,PushButtonTimeOut,PushButtonDisplayFlag;
uint32_t EEPROM_Test =4567,EEPROM_Test_1;
#define EEPROM_Test_Addr 2345
	long templong;
uint16_t MD_Year;
uint8_t NoOfSeconds;
void TaskAutoScroll(void)
{
	if((PushButtonDisplayFlag==1)&&(PushButtonTimeOut<10))
		{
			PushButtonDisplay();
			PushButtonTimeOut++;
			return;
		}
		else
		{	
      PushButtonDisplayFlag=0;
			PushButtonTimeOut=0;	
			LCD_PushButton_Parm=0;			
			if( NoOfSeconds++>=10)
			{
				NoOfSeconds = 0;
				LCD_DisplayParm++;  
			}
		}

				/*EEPROM_Test++;
				to_eeprom(1234,EEPROM_Test,4);			
				EEPROM_Test_1=from_eeprom(1234,4);//*/
	
   switch(LCD_DisplayParm)
  {
      case DISP_V://Voltage
					
					if(nmiss_stat==1)
					lcd_put_num(3,4,2400 );//  dec place
					else
          lcd_put_num(3,4,inst_voltage );// 1 dec place
					
           V_Icon
					 lcd_put_icon(P3);
          break;

      case DISP_I_S://Shunt Current
          lcd_put_icon(P2);
			if(inst_phase_current>8)
          lcd_put_num(3,5,inst_phase_current);// 2 dec place
			else
				 lcd_put_num(3,5,0);// 2 dec place
			
          lcd_put_flash_str(1,test_str[4]);
          A_Icon;
          break;

      case DISP_I_N://Neutral Current
          lcd_put_icon(P2);
				
			if(inst_neutral_current>7)
          lcd_put_num(3,5,inst_neutral_current);// 2 dec place
			else
				 lcd_put_num(3,5,0);// 2 dec place
			
          lcd_put_flash_str(1,test_str[5]);
			    A_Icon;
          break;
			
			case DISP_KW_S://Shunt Power
          lcd_put_icon(P2);
          lcd_put_num(4,4,inst_kw );//  dec place
			    KW_Icon;
          break;
			
			case DISP_KWH://Active energy
          lcd_put_icon(P3);
          lcd_put_num(1,7, load_val[0] );//  dec place //load_val[0]
			    KWh_Icon;
          break;
   
      case DISP_FREQ://Frequency
           lcd_put_num(4,3,inst_freq);
					 lcd_put_icon(P3);
			lcd_put_flash_str(1,test_str[20]);
          break;
			
      case DISP_PF:// pf
            lcd_put_icon(P2);
            lcd_put_num(4,3,abs(inst_pf));         
          lcd_put_flash_str(1,test_str[0]);
            break;

		
		 case DISP_TIME://Current Time
          lcd_put_num(5,2,SEC);//sec
          lcd_put_num(3,2,MIN);//min
          lcd_put_num(1,2,HOUR);//hr
          lcd_put_icon(COL);
          lcd_put_icon(TIME_Icon);
          break;
   
     case DISP_DATE://Current Date
          lcd_put_num(5,2,YEAR );//Year
          lcd_put_num(3,2,MONTH );//Month
          lcd_put_num(1,2,DATE );//Date
          lcd_put_icon(COL);
          lcd_put_icon(DATE_Icon);
          break;    

	case DISP_MD://MD
          lcd_put_icon(P2);
          lcd_put_num(4,3,kwmd_val );//  dec place
			    KW_Icon;
					lcd_put_icon(MD);
          break;

	 case MD_TIME://MD Time
          lcd_put_num(5,2,KWMD_DT[7]);//sec
          lcd_put_num(3,2,KWMD_DT[6]);//min
          lcd_put_num(1,2,KWMD_DT[5]);//hr
          lcd_put_icon(COL);
          lcd_put_icon(TIME_Icon);
					lcd_put_icon(MD);
          break;
   
     case MD_DATE://MD Date
					MD_Year= (KWMD_DT[0]<<8 | KWMD_DT[1])%2000;
          lcd_put_num(5,2,MD_Year );//Year
          lcd_put_num(3,2,KWMD_DT[2] );//Month
          lcd_put_num(1,2,KWMD_DT[3] );//Date
          lcd_put_icon(COL);
          lcd_put_icon(DATE_Icon);
					lcd_put_icon(MD);
          break;    

     
      
    default :
					lcd_put_num(5,2,YEAR );//Year
          lcd_put_num(3,2,MONTH );//Month
          lcd_put_num(1,2,DATE );//Date
          lcd_put_icon(COL);
          lcd_put_icon(DATE_Icon);
					LCD_DisplayParm = 0;	
          break;
  }
	
	
//	if(LCD_DisplayParm>9)
//	lcd_put_icon(T19);
//	lcd_put_num(8,1,(LCD_DisplayParm)%10);
	
	if(PowerOnSec>3)// show after 3 secs
	TamperIcons();
	
	Put_Data_On_LCD();// Lcd clear and seg writing is done here

}

uint8_t PBTempChar;
void PushButtonDisplay(void)
{
	#if 1

		if(LCD_PushButton_Parm>0)
		PBTempChar=LCD_PushButton_Parm-1;
//		lcd_clear();
	
	//LCD_DisplayParm=DISP_EE_TEST;
   switch(PBTempChar)
  {
			case DISP_V://Voltage
					
					if(nmiss_stat==1)
					lcd_put_num(3,4,2400 );//  dec place
					else
          lcd_put_num(3,4,inst_voltage );// 1 dec place
					
           V_Icon
					 lcd_put_icon(P3);
          break;

      case DISP_I_S://Shunt Current
          lcd_put_icon(P2);
			if(inst_phase_current>8)
          lcd_put_num(3,5,inst_phase_current);// 2 dec place
			else
				 lcd_put_num(3,5,0);// 2 dec place
			
          lcd_put_flash_str(1,test_str[4]);
          A_Icon;
          break;

      case DISP_I_N://Neutral Current
          lcd_put_icon(P2);
				
			if(inst_neutral_current>8)
          lcd_put_num(3,5,inst_neutral_current);// 2 dec place
			else
				 lcd_put_num(3,5,0);// 2 dec place
			
          lcd_put_flash_str(1,test_str[5]);
			    A_Icon;
          break;
			
			case DISP_KW_S://Shunt Power
          lcd_put_icon(P2);
          lcd_put_num(4,4,inst_kw );//  dec place
			    KW_Icon;
          break;
			
			case DISP_KWH://Active energy
          lcd_put_icon(P3);
          lcd_put_num(1,7, load_val[0] );//  dec place //load_val[0]
			    KWh_Icon;
          break;
   
      case DISP_FREQ://Frequency
           lcd_put_num(4,3,inst_freq);
					 lcd_put_icon(P3);
			lcd_put_flash_str(1,test_str[20]);
          break;
			
      case DISP_PF:// pf
            lcd_put_icon(P2);
            lcd_put_num(4,3,abs(inst_pf));         
          lcd_put_flash_str(1,test_str[0]);
            break;

		
		 case DISP_TIME://Current Time
          lcd_put_num(5,2,SEC);//sec
          lcd_put_num(3,2,MIN);//min
          lcd_put_num(1,2,HOUR);//hr
          lcd_put_icon(COL);
          lcd_put_icon(DATE_Icon);
          break;
   
     case DISP_DATE://Current Date
          lcd_put_num(5,2,YEAR );//Year
          lcd_put_num(3,2,MONTH );//Month
          lcd_put_num(1,2,DATE );//Date
          lcd_put_icon(COL);
          lcd_put_icon(DATE_Icon);
          break;    

	case DISP_MD://MD
          lcd_put_icon(P2);
          lcd_put_num(4,3,kwmd_val );//  dec place
			    KW_Icon;
					lcd_put_icon(MD);
          break;

	 case MD_TIME://MD Time
          lcd_put_num(5,2,KWMD_DT[7]);//sec
          lcd_put_num(3,2,KWMD_DT[6]);//min
          lcd_put_num(1,2,KWMD_DT[5]);//hr
          lcd_put_icon(COL);
          lcd_put_icon(TIME_Icon);
					lcd_put_icon(MD);
          break;
   
     case MD_DATE://MD Date
					MD_Year= (KWMD_DT[0]<<8 | KWMD_DT[1])%2000;
          lcd_put_num(5,2,MD_Year );//Year
          lcd_put_num(3,2,KWMD_DT[2] );//Month
          lcd_put_num(1,2,KWMD_DT[3] );//Date
          lcd_put_icon(COL);
          lcd_put_icon(DATE_Icon);
					lcd_put_icon(MD);
          break;    

    default :
					lcd_put_num(5,2,YEAR );//Year
					lcd_put_num(3,2,MONTH );//Month
					lcd_put_num(1,2,DATE );//Date
					lcd_put_icon(COL);
					lcd_put_icon(DATE_Icon);			
					PushButtonDisplayFlag=0;
					LCD_PushButton_Parm=0;
		
          break;
  }
	//lcd_put_num(7,2,LCD_PushButton_Parm/4);
	if(PowerOnSec>3)// show after 3 secs
	TamperIcons();
	
	Put_Data_On_LCD();// Lcd clear and seg writing is done here
	#endif
}
int8_t Display_Complete=0;
void PushButtonWakeDisplay(void)
{
		//lcd_clear();
	
			if(PushButtonCommMode==1)
			{
				//PushButtonDisplayFlag=0;
				//PushButtonTimeOut=0;	
				//LCD_PushButton_Parm=0;			
				if(NoOfSeconds++>=5)
				{
					NoOfSeconds = 0;
					LCD_DisplayParm++;  
					//NVRAM_Push_Buton_Disp=LCD_DisplayParm;
				}
			
				if(LCD_DisplayParm >=6)
				{
					Display_Complete=1; 
					LCD_DisplayParm=6;
				}
				else
				Communication_Enable_Counter=0;
			
				NVRAM_Push_Buton_Disp=LCD_DisplayParm;
			}
			
			

			switch(NVRAM_Push_Buton_Disp)
			{
							
				case 1://Current Date
				lcd_put_num(5,2,YEAR );//Year
				lcd_put_num(3,2,MONTH );//Month
				lcd_put_num(1,2,DATE );//Date
				lcd_put_icon(COL);
				lcd_put_icon(DATE_Icon);
				break;    
				
				case 2://Current Time
				lcd_put_num(5,2,SEC);//sec
				lcd_put_num(3,2,MIN);//min
				lcd_put_num(1,2,HOUR);//hr
				lcd_put_icon(COL);
				lcd_put_icon(TIME_Icon);
				break;

				case 3://MD
				lcd_put_icon(P2);
				lcd_put_num(4,3,kwmd_val );//  dec place
				KW_Icon;
				lcd_put_icon(MD);
				break;

				case 4://MD Time
				lcd_put_num(5,2,KWMD_DT[7]);//sec
				lcd_put_num(3,2,KWMD_DT[6]);//min
				lcd_put_num(1,2,KWMD_DT[5]);//hr
				lcd_put_icon(COL);
				lcd_put_icon(TIME_Icon);
				lcd_put_icon(MD);
				break;
 
				case 5://MD Date
				MD_Year= (KWMD_DT[0]<<8 | KWMD_DT[1])%2000;
        lcd_put_num(5,2,MD_Year );//Year
				lcd_put_num(3,2,KWMD_DT[2] );//Month
				lcd_put_num(1,2,KWMD_DT[3] );//Date
				lcd_put_icon(COL);
				lcd_put_icon(DATE_Icon);
				lcd_put_icon(MD);
				break;
				
				case 6:
				lcd_put_num(4,4,Communication_Enable_Counter );
				break;
				
				default :
			  NVRAM_Push_Buton_Disp=0;
				LCD_PushButton_Parm=0;	
				break;				
										
			}//*/
			
			Put_Data_On_LCD();
}

extern uint8_t THD_10V_40V_inject_flag;
void TamperIcons(void)
{
	#if 0
if(gFlags.EarthTamper)
lcd_put_icon(Earth);	

if(gFlags.MagTamper)
lcd_put_icon(Mag);

if(gFlags.ReverseTamper)
lcd_put_icon(Rev);

//if(gFlags.ReverseTamper)
//lcd_put_icon(NM);
	#endif
	
	if(rev_stat==1)
		lcd_put_icon(REV);
		if(cuopen_stat==1)
		lcd_put_icon(COPEN);
	if(magnetic_stat==1)
		lcd_put_icon(MAG);
		
	if((NM_CT_Detected==1)||(nd_stat==1))
		lcd_put_icon(NM);
	if(((eload_stat==1)||(nd_stat==1))&& (magnetic_stat==0))
		lcd_put_icon(EARTH);
	
	if(THD_10V_40V_inject_flag)
	lcd_put_icon(OK);
	
		if(NM_CT_Detected==0)// not required in NM mode 
	if(!(SYS->STAT_b.v3a_nok))//Power ON Icon
	lcd_put_icon(ON);
}

#if 0
void WakeFromSourceDisplay(void)
{
	long i;
	//lcd_clear();
	lcd_put_num(1,6,WakeFromReason);
	for(i=0;i<50000;i++);wd_reset();
	for(i=0;i<50000;i++);wd_reset();
	for(i=0;i<50000;i++);wd_reset();
	for(i=0;i<50000;i++);wd_reset();
	for(i=0;i<50000;i++);wd_reset();
	Put_Data_On_LCD();// Lcd clear and seg writing is done here
}

void NMDisplay(void)
{
	#if 0
	//lcd_clear();
//	lcd_put_icon(P1);
//	lcd_put_num(2,5,gMeter.Current[PHASE]);
//	A_Icon;
	
	lcd_put_icon(P1);
	lcd_put_num(3,4,gMeter.ActivePower[PHASE]);
	KW_Icon
Put_Data_On_LCD();// Lcd clear and seg writing is done here
//	gMeter.ActivePower[PHASE]=240*NM_Phase_Current/1000;
//	
//  gCeData.w0sum_x=gMeter.ActivePower[PHASE]/(VMAX * (IMAXA/GAIN) * (CP/SUMSAMPS));
	#endif
}

void Display_Version(void)
{
	long i;
	char j;
	//lcd_clear();
	
	lcd_put_num(1,6,Version_no);
	Put_Data_On_LCD();// Lcd clear and seg writing is done here
	//lcd_put_num(1,8,TestVar);
	//for(j=0;j<10;j++)
	{
		for(i=0;i<50000;i++){}
		wd_reset();
	}
}
#endif
void Display_Version(void)
{

	//lcd_clear();
	
	if(PushButtonCommMode==1)
		 lcd_put_flash_str(1,test_str[11]);
	else	
		lcd_put_flash_str(1,test_str[10]);
	//*/
	
//lcd_put_num(1,7,WakeFromRegisterCopy );
	Put_Data_On_LCD();// Lcd clear and seg writing is done here
}

void CalDisplay(void)
{
if(CalDisplayVar==CLR1)
	lcd_put_flash_str(1,test_str[14]);
else if(CalDisplayVar==CLR2)
	lcd_put_flash_str(1,test_str[12]);
else if(CalDisplayVar==CLR3)
	lcd_put_flash_str(1,test_str[13]);
else if(CalDisplayVar==UNLOCK)
	lcd_put_flash_str(1,test_str[15]);
else if(CalDisplayVar==CLRC)
	lcd_put_flash_str(1,test_str[16]);
else if(CalDisplayVar==RESET)
	lcd_put_flash_str(1,test_str[17]);
else if(CalDisplayVar==SLNO)
	lcd_put_flash_str(1,test_str[18]);
else if(CalDisplayVar==CAL)
	lcd_put_flash_str(1,test_str[19]);
Put_Data_On_LCD();
}



