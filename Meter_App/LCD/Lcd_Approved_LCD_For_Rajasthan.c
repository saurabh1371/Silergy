#include "options.h" // System options for compiler, hardware, etc.
#include <math.h>	 // for fabs()
#include "afe.h"	 // For meter data to display.
#include "bat.h"	 // For battery readings to display.
#include "pwrmode.h" // To read the battery mode.
#include <ctype.h>
// #include "LCD_BQ14053.h"
// #include "lcd_ASDAQ.h"// Display labels.
#include "Lcd_Approved_LCD_For_Rajasthan.h" // Display labels.
#include "dlms_eeprom.h"
// #include "dlms_variables.h"
#include "asdaq_variables.h"
#include "Defines.h"
#include "wd.h"	 // Watchdog driver.
#include "irq.h" // Global interrupt enable/disable macros

void PushButtonDisplay(void);
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
	switch (sfr_idx)
	{
	case 0:
		tmp = LCD->MAPL;
		break; // Controls SEG0..31
	case 1:
		tmp = LCD->MAPH;
		break; // Controls SEG32..35
	default:
		tmp = 0;
		break;
	}
	mask = 1UL << bit_idx;
	if (0 == val)
	{
		tmp &= ~mask;
	}
	else
	{
		tmp |= mask;
	}
	switch (sfr_idx)
	{
	case 0:
		LCD->MAPL = tmp;
		break; // Controls SEG0..31
	case 1:
		LCD->MAPH = tmp;
		break; // Controls SEG32..35
	default:
		break;
	}
} // lcd_map()

uint32_t LCD_Buffer[10] = {0};
static void lcd_set_seg(uint32_t seg_idx, uint32_t seg)
{
	// LCD->DIO[seg_idx] |= seg;
	LCD_Buffer[seg_idx] |= seg;
}
void Put_Data_On_LCD(void)
{
	int8_t i;
	for (i = 0; i < 10; i++)
	{
		LCD->DIO[i] = 0;
		LCD->DIO[i] = LCD_Buffer[i];
	}

	for (i = 0; i < 10; i++) // Clear buffer once data on Seg is loaded
	{
		LCD_Buffer[i] = 0;
		;
	}
}

void lcd_put_icon(int32_t icon_idx)
{
	lcd_set_seg((uint32_t)icon_seg_8COM[icon_idx],
				(uint32_t)icon_common_8COM[icon_idx]);
}

void put_ch_LCD(int l_idx, int f_idx)
{
	if (0 >= l_idx || DIGIT_CNT < l_idx)
		return;

	lcd_set_seg((uint32_t)char_seg_8COM[l_idx],
				(uint32_t)font_8COM[f_idx]);
}

void lcd_put_char(int32_t chr_idx, char ch)
{
	int chr = (int)ch;
	// Convert an ISO character code to an index into the font.
	// Write only known characters.
	// Leave a blank for a space or any character with no font entry.
	if (isdigit(chr))
	{
		chr -= '0'; // Numerals in the font start at 0.
	}
	else
	{
		if (isalpha(chr))
		{
			chr = toupper(chr); // Accept lower case alphas.
			chr -= ('A' - 10);	// Index the alphas after the numerals.
		}
		else
		{
			if ('-' == chr)
			{
				chr = LCD_MINUS;
			}
			else
			{
				if ('.' == chr)
				{
					// Write '.' if it exists.
					if (LCD_FIRST_PERIOD <= chr_idx && chr_idx <= LCD_LAST_PERIOD)
					{
						lcd_put_icon(chr_idx - LCD_FIRST_PERIOD);
					}
					chr = -1; // '.' is written, so don't write more data.
				}
				else
				{
					chr = -1; // Not in font, so write nothing.
				}
			}
		}
	}
	if (0 <= chr)
	{
		put_ch_LCD(chr_idx, chr);
	}
}

void lcd_put_flash_str(int32_t i, const char *str)
{
	uint8_t chr; // The character.

	for (;;)
	{
		chr = *str++; // Get the next character.

		// Exit the loop at the end of the string or at the end of the display.
		if (0 == chr || DIGIT_CNT < i)
			break;

		lcd_put_char(i, chr);
		// In a string, '.' is between chars.
		if ('.' == chr)
			--i;
		else
			i++;
	}
}

const char error_str[] = "Error";
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

void lcd_clear(void)
{
	LCD->MODE_b.rst = 1; // Toggle the reset to clear the segments.
	LCD->MODE_b.rst = 0;
	LCD->MODE_b.blank = 0; // Enable the segments.
	LCD->MODE_b.on = 0;	   // Clear test as well

} // void lcd_clear (void)

void lcd_test(void)
{
	LCD->MODE_b.on = 1;
} // void lcd_test (void)

void lcd_init_1(void)
{
	LCD->MAPL = LCD_MAPL_DEF;
	LCD->MAPH = LCD_MAPH_DEF;
	LCD->MODE = LCD_MODE_DEF;
	LCD->CNTL = 0x00000004; // LCD->MODE, LCD DAC Bit 6 is set. So, lcd voltage will be (2.65V + 2.65V * 4/31)=2.9919V
	lcd_clear();
}

void lcd_init_no_clear(void)
{
    // Initializes the LCD registers without triggering a hardware blank/reset
    LCD->MAPL = LCD_MAPL_DEF;
    LCD->MAPH = LCD_MAPH_DEF;
    LCD->MODE = LCD_MODE_DEF;
    LCD->CNTL = 0x00000004; 
}

uint8_t LCD_PushButton_Parm, PushButtonTimeOut, PushButtonDisplayFlag;
uint32_t EEPROM_Test = 4567, EEPROM_Test_1;
#define EEPROM_Test_Addr 2345
long templong;
uint16_t MD_Year;
uint8_t NoOfSeconds;

/* Forward declaration: avoids re-reading the same EEPROM billing history
 * entry on every 1 s tick while a history screen is held (definition below,
 * shared by Auto Mode, Push Button Mode, and Battery Mode). */
static void PB_LoadBillHistory(uint8_t idx);

/* --- Mode-entry banners ("AUTO" / "PUSH"), held for MODE_BANNER_DURATION_SEC
 * seconds before the corresponding scroll cycle starts (see display spec). */
static uint8_t s_push_banner_ctr;
static uint8_t s_push_banner_done;
static uint8_t s_auto_banner_ctr;
static uint8_t s_auto_banner_done; // 0 = show "AUTO" before the next cycle (incl. at power-up)

void TaskAutoScroll(void)
{
	// --- Hold the display for serial commands ---
	if (SerialDisplayTimeOut > 0)
	{
		SerialDisplayTimeOut--;
		return; // Skip the auto-scroll update while the timer is active
	}
	if ((PushButtonDisplayFlag == 1) && (LCD_PushButton_Parm < PB_TOTAL_SCREENS))
	{
		if (!s_push_banner_done) // show "PUSH" before the very first screen of this cycle
		{
			LCD->MODE_b.on = 0;
			lcd_put_flash_str(1, test_str[31]); // "PUSH"
			Put_Data_On_LCD();
			if (++s_push_banner_ctr >= MODE_BANNER_DURATION_SEC)
			{
				s_push_banner_ctr = 0;
				s_push_banner_done = 1;
			}
			return;
		}

		PushButtonDisplay();
		if (++PushButtonTimeOut >= 10) // hold each screen 10 s
		{
			PushButtonTimeOut = 0;
			LCD_PushButton_Parm++; // next screen; auto-terminates after one full cycle
		}
		return;
	}
	else
	{
		if (PushButtonDisplayFlag == 1) // we were still in Push mode last tick - it just ended
		{
			s_auto_banner_done = 0; // show the "AUTO" banner right away
			LCD_DisplayParm = 0;	// and restart Auto Mode from the top of its list
			NoOfSeconds = 0;
		}
		PushButtonDisplayFlag = 0;
		PushButtonTimeOut = 0;
		LCD_PushButton_Parm = 0;
		s_push_banner_done = 0; // re-arm so the next button press shows "PUSH" again

		// 1. Process the 10-second hold and cycle wrap FIRST
		if (s_auto_banner_done)
		{
			if (NoOfSeconds++ >= 10)
			{
				NoOfSeconds = 0;
				LCD_DisplayParm++;
				if (LCD_DisplayParm >= DISP_AUTO_TOTAL_SCREENS)
				{
					LCD_DisplayParm = 0;
					s_auto_banner_done = 0; // Trigger "AUTO" banner for the next pass
				}
			}
		}

		// 2. Process the banner SECOND. If the cycle just wrapped, this catches it
		// instantly and returns before hitting the switch statement.
		if (!s_auto_banner_done)
		{
			LCD->MODE_b.on = 0;
			lcd_put_flash_str(1, test_str[30]); // "AUTO"
			Put_Data_On_LCD();
			if (++s_auto_banner_ctr >= MODE_BANNER_DURATION_SEC)
			{
				s_auto_banner_ctr = 0;
				s_auto_banner_done = 1;
				LCD_DisplayParm = DISP_AUTO_SEG_CHECK;
				NoOfSeconds = 0;
			}
			return; // Safely exit here so the LCD isn't overwritten
		}
	}

	LCD->MODE_b.on = 0; // make sure the segment-test screen isn't left on from the previous pass
	switch (LCD_DisplayParm)
	{
	case DISP_AUTO_SEG_CHECK: // 1. LCD segment check (lamp test)
		LCD->MODE_b.on = 1;
		break;

	case DISP_KWH: // 2. KWh (6+1)
		lcd_put_icon(P3);
		lcd_put_num(1, 7, ENERGY_TENTHS(load_val[0]));
		KWh_Icon;
		break;

	case DISP_KVAH: // 3. KVAh (6+1)
		lcd_put_icon(P3);
		lcd_put_num(1, 7, ENERGY_TENTHS(load_val[1])); // (6+1) - fills all 8 digits, same layout as DISP_KWH
		KVAh_Icon;
		break;

	case DISP_MD: // 4. Current month MD in KW (3 decimal)
		lcd_put_icon(P1);
		lcd_put_num(2, 4, MD_THOUSANDTHS(kwmd_val)); // X.XXX - digits 2-5
		KW_Icon;
		lcd_put_icon(T12);
		break;

	case DISP_MD_KVA: // 5. Current month MD in KVA (3 decimal)
		lcd_put_icon(P1);
		lcd_put_num(2, 4, MD_THOUSANDTHS(kvamd_val)); // X.XXX - digits 2-5, mirrors DISP_MD
		KVA_Icon;
		lcd_put_icon(T12); // "MD" icon (shared with the KW-MD screen)
		break;

	case DISP_IL_KW: // 6. Instant Load in KW (3 decimal)
		lcd_put_icon(P1);
		lcd_put_num(2, 4, inst_kw); // real-time AFE power reading, X.XXX - digits 2-5
		KW_Icon;
		lcd_put_char(8, 'L');
		lcd_put_icon(T19);
		break;

	case DISP_IL_KVA: // 7. Instant Load in KVA (3 decimal)
		lcd_put_icon(P1);
		lcd_put_num(2, 4, inst_kva); // real-time AFE apparent-power reading, X.XXX - digits 2-5
		KVA_Icon;
		lcd_put_char(8, 'L');
		lcd_put_icon(T19);
		break;

	case DISP_PM_KWH: // 8. Previous month KWh (5+1, H1, Bill)
		PB_LoadBillHistory(1);
		lcd_put_num(1, 6, ENERGY_TENTHS(stBilling_Profile.Cumm_Energy_KWh)); // (5+1), digits 1-6
		KWh_Icon;
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		lcd_put_icon(T1);
		lcd_put_icon(T13);
		break;

	case DISP_PM_KVAH: // 9. Previous month KVAh (5+1, H1, Bill)
		PB_LoadBillHistory(1);
		lcd_put_num(1, 6, ENERGY_TENTHS(stBilling_Profile.Cumm_Energy_KVAh)); // (5+1), digits 1-6
		KVAh_Icon;
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		lcd_put_icon(T1);
		lcd_put_icon(T13); // BP
		break;

	case DISP_PM_MD_KW: // 10. Previous month MD in KW (3 decimal, H1, Bill)
		PB_LoadBillHistory(1);
		lcd_put_icon(P1);
		lcd_put_num(2, 4, MD_THOUSANDTHS(stBilling_Profile.MD_KW)); // X.XXX - digits 2-5
		KW_Icon;
		lcd_put_icon(T12); // "MD" icon
		lcd_put_icon(T13); // BP
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		break;

	case DISP_PM_MD_KVA: // 11. Previous month MD in KVA (3 decimal, H1, Bill)
		PB_LoadBillHistory(1);
		lcd_put_icon(P1);
		lcd_put_num(2, 4, MD_THOUSANDTHS(stBilling_Profile.MD_KVA)); // X.XXX - digits 2-5
		KVA_Icon;
		lcd_put_icon(T12);
		lcd_put_icon(T13); // BP
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		break;

	case DISP_PM_ONHR: // 12. Previous month power-on hours (H1, Bill)
	{
		uint32_t hrs;

		PB_LoadBillHistory(1);
		hrs = stBilling_Profile.Power_On_Time / 60;

		if (hrs > 999)
			lcd_put_num(3, 4, hrs);
		else if (hrs > 99)
			lcd_put_num(4, 3, hrs);
		else if (hrs > 9)
			lcd_put_num(5, 2, hrs);
		else
			lcd_put_num(6, 1, hrs);

		lcd_put_flash_str(1, test_str[27]); // "ON" tag
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		lcd_put_icon(T13); // BP
		lcd_put_icon(T17); // h
		break;
	}

	default: // shouldn't be reached - the wrap check above keeps LCD_DisplayParm < DISP_AUTO_TOTAL_SCREENS
		LCD_DisplayParm = 0;
		s_auto_banner_done = 0;
		break;
	}

	//	if(LCD_DisplayParm>9)
	//	lcd_put_icon(T19);
	//	lcd_put_num(8,1,(LCD_DisplayParm)%10);

	if (PowerOnSec > 1) // show after 1 secs
		TamperIcons();

	Put_Data_On_LCD(); // Lcd clear and seg writing is done here
}

uint8_t PBTempChar;

/* Avoids re-reading the same EEPROM billing history entry on every 1 s tick
 * while a history screen is held (each screen is shown for 10 ticks). */
static void PB_LoadBillHistory(uint8_t idx)
{
	static uint8_t s_bill_idx_loaded = 0xFF;
	if (s_bill_idx_loaded != idx)
	{
		get_bill_data(idx);
		s_bill_idx_loaded = idx;
	}
}

/* Shows one MD (KW or KVA) history sub-screen: value, then time, then date.
 * is_kva selects which billing-profile pair to render; sub selects
 * 0=value,1=time,2=date. hist_idx (1-3) drives the "Hx" tag and the
 * underlying get_bill_data() fetch.
 * IMPORTANT: stBilling_Profile.MD_KW/MD_KVA (and their _DT arrays) are read
 * from *inside* this function, strictly after PB_LoadBillHistory(hist_idx)
 * has refreshed them. Do NOT go back to passing them in as arguments - a
 * caller like PB_ShowMDHistory(2, 0, stBilling_Profile.MD_KW, ...) would
 * evaluate stBilling_Profile.MD_KW at the call site, i.e. BEFORE the
 * refresh below runs, so the first render of a new history screen would
 * show the previous screen's stale value for one tick. */
static void PB_ShowMDHistory(uint8_t hist_idx, uint8_t sub, uint8_t is_kva)
{
	uint16_t md_val;
	const uint8_t *md_dt;

	PB_LoadBillHistory(hist_idx);

	md_val = is_kva ? stBilling_Profile.MD_KVA : stBilling_Profile.MD_KW;
	md_dt = is_kva ? stBilling_Profile.MD_KVA_DT : stBilling_Profile.MD_KW_DT;

	switch (sub)
	{
	case 0: // value
		lcd_put_icon(P1);
		lcd_put_num(2, 4, MD_THOUSANDTHS(md_val)); // X.XXX - digits 2-5
		if (is_kva)
		{
			KVA_Icon;
		}
		else
		{
			KW_Icon;
		}
		lcd_put_icon(T12); // "MD" icon
		lcd_put_icon(T13); // BP
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, hist_idx);
		break;
	case 1: // date
		MD_Year = (md_dt[0] << 8 | md_dt[1]) % 2000;
		lcd_put_num(5, 2, MD_Year);
		lcd_put_num(3, 2, md_dt[2]); // month
		lcd_put_num(1, 2, md_dt[3]); // date
		lcd_put_icon(P4);
		lcd_put_icon(T6);
		lcd_put_icon(T12);
		lcd_put_icon(T13); // BP
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, hist_idx);
		if (is_kva)
		{
			KVA_Icon;
		}
		else
		{
			KW_Icon;
		}
		break;
	case 2:							 // time
		lcd_put_num(5, 2, md_dt[7]); // sec
		lcd_put_num(3, 2, md_dt[6]); // min
		lcd_put_num(1, 2, md_dt[5]); // hr
		lcd_put_icon(P4);
		lcd_put_icon(T7);
		lcd_put_icon(T12);
		lcd_put_icon(T13); // BP
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, hist_idx);
		if (is_kva)
		{
			KVA_Icon;
		}
		else
		{
			KW_Icon;
		}
		break;
	}
}

/* Push Button Mode: full auto-scrolling sequence, one screen per
 * LCD_PushButton_Parm value (see enum PBDispSeq). TaskAutoScroll() holds
 * each screen for 10 s and stops after LCD_PushButton_Parm reaches
 * PB_TOTAL_SCREENS - i.e. one full cycle - per the display spec. */
void PushButtonDisplay(void)
{
	LCD->MODE_b.on = 0; // make sure the segment-test screen isn't left on

	switch (LCD_PushButton_Parm)
	{
	case PB_SEG_CHECK: // LCD segment check
		LCD->MODE_b.on = 1;
		break;

	case PB_DATE: // Real date
		lcd_put_num(5, 2, YEAR);
		lcd_put_num(3, 2, MONTH);
		lcd_put_num(1, 2, DATE);
		lcd_put_icon(P4);
		lcd_put_icon(T6);
		break;

	case PB_TIME: // Real time
		lcd_put_num(5, 2, SEC);
		lcd_put_num(3, 2, MIN);
		lcd_put_num(1, 2, HOUR);
		lcd_put_icon(P4);
		lcd_put_icon(T7);
		break;

	case PB_SERIAL_NO:				 // Serial number, tagged "ID" in the spare digits (same layout as the H1-H6 history tag)
		lcd_put_num(1, 7, meter_no); // 7 digits, digits 1-7
		lcd_put_char(8, 'D');
		lcd_put_icon(T19);
		break;

	case PB_KWH: // KWh (6+1)
		lcd_put_icon(P3);
		lcd_put_num(1, 7, ENERGY_TENTHS(load_val[0]));
		KWh_Icon;
		break;

	case PB_KVAH: // KVAh (6+1)
		lcd_put_icon(P3);
		lcd_put_num(1, 7, ENERGY_TENTHS(load_val[1]));
		KVAh_Icon;
		break;

	/* --- Previous months KWh, H1-H6 (5+1, Bill) --- */
	case PB_H1_KWH:
	case PB_H2_KWH:
	case PB_H3_KWH:
	case PB_H4_KWH:
	case PB_H5_KWH:
	case PB_H6_KWH:
	{
		uint8_t hidx = (LCD_PushButton_Parm - PB_H1_KWH) + 1; // 1..6
		PB_LoadBillHistory(hidx);
		lcd_put_num(1, 6, ENERGY_TENTHS(stBilling_Profile.Cumm_Energy_KWh));
		KWh_Icon;
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, hidx);
		lcd_put_icon(T1);
		lcd_put_icon(T13);
		break;
	}

	/* --- Previous months MD in KW, H1-H3, each with value/time/date --- */
	case PB_H1_MD_KW:
		PB_ShowMDHistory(1, 0, 0);
		break;
	case PB_H1_MD_KW_TIME:
		PB_ShowMDHistory(1, 1, 0);
		break;
	case PB_H1_MD_KW_DATE:
		PB_ShowMDHistory(1, 2, 0);
		break;
	case PB_H2_MD_KW:
		PB_ShowMDHistory(2, 0, 0);
		break;
	case PB_H2_MD_KW_TIME:
		PB_ShowMDHistory(2, 1, 0);
		break;
	case PB_H2_MD_KW_DATE:
		PB_ShowMDHistory(2, 2, 0);
		break;
	case PB_H3_MD_KW:
		PB_ShowMDHistory(3, 0, 0);
		break;
	case PB_H3_MD_KW_TIME:
		PB_ShowMDHistory(3, 1, 0);
		break;
	case PB_H3_MD_KW_DATE:
		PB_ShowMDHistory(3, 2, 0);
		break;

	/* --- Previous months KVAh, H1-H6 (5+1, Bill) --- */
	case PB_H1_KVAH:
	case PB_H2_KVAH:
	case PB_H3_KVAH:
	case PB_H4_KVAH:
	case PB_H5_KVAH:
	case PB_H6_KVAH:
	{
		uint8_t hidx = (LCD_PushButton_Parm - PB_H1_KVAH) + 1; // 1..6
		PB_LoadBillHistory(hidx);
		lcd_put_num(1, 6, ENERGY_TENTHS(stBilling_Profile.Cumm_Energy_KVAh));
		KVAh_Icon;
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, hidx);
		lcd_put_icon(T1);
		lcd_put_icon(T13);
		break;
	}

	/* --- Previous months MD in KVA, H1-H3, each with value/time/date --- */
	case PB_H1_MD_KVA:
		PB_ShowMDHistory(1, 0, 1);
		break;
	case PB_H1_MD_KVA_TIME:
		PB_ShowMDHistory(1, 1, 1);
		break;
	case PB_H1_MD_KVA_DATE:
		PB_ShowMDHistory(1, 2, 1);
		break;
	case PB_H2_MD_KVA:
		PB_ShowMDHistory(2, 0, 1);
		break;
	case PB_H2_MD_KVA_TIME:
		PB_ShowMDHistory(2, 1, 1);
		break;
	case PB_H2_MD_KVA_DATE:
		PB_ShowMDHistory(2, 2, 1);
		break;
	case PB_H3_MD_KVA:
		PB_ShowMDHistory(3, 0, 1);
		break;
	case PB_H3_MD_KVA_TIME:
		PB_ShowMDHistory(3, 1, 1);
		break;
	case PB_H3_MD_KVA_DATE:
		PB_ShowMDHistory(3, 2, 1);
		break;

	case PB_HIRES_KWH: // High resolution KWh (2+4)
	{
		uint32_t l_load_val, l_load_rmndr, l_load_ctr;
		int32_t l_wh_frac, l_wsum_per_cnt;
		uint32_t real_wh;
		uint32_t tenths_of_wh;
		uint32_t real_wh_tenths;

		// Snapshot all sources atomically to prevent metrology race conditions
		IRQ_DISABLE();
		l_load_val = load_val[0];
		l_load_rmndr = load_rmndr[0];
		l_load_ctr = load_ctr[0];
		l_wh_frac = global.misc.wh_frac;
		l_wsum_per_cnt = global.cal.wsum_per_cnt;
		IRQ_GLOBAL_ENABLE();

		// 1. Reconstruct 1 Wh precision from application accumulators
		real_wh = (l_load_val * 10) + l_load_rmndr + l_load_ctr;

		// 2. Extract the sub-1-Wh fractional energy from the metrology core (tenths of a Wh)
		tenths_of_wh = (l_wsum_per_cnt > 0)
						   ? ((uint32_t)l_wh_frac * 10) / (uint32_t)l_wsum_per_cnt
						   : 0;

		// 3. Combine them to get 0.0001 kWh precision
		real_wh_tenths = (real_wh * 10) + tenths_of_wh;

		lcd_put_num(1, 6, real_wh_tenths % 1000000UL);
		lcd_put_icon(P1);
		KWh_Icon;
		lcd_put_char(7, 'H');
		break;
	}

	case PB_HIRES_KVAH: // High resolution KVAh (2+4)
	{
		uint32_t l_load_val, l_load_rmndr, l_load_ctr;
		int32_t l_vah_frac, l_wsum_per_cnt;
		uint32_t real_vah;
		uint32_t tenths_of_vah;
		uint32_t real_vah_tenths;

		IRQ_DISABLE();
		l_load_val = load_val[1];
		l_load_rmndr = load_rmndr[1];
		l_load_ctr = load_ctr[1];
		l_vah_frac = global.misc.vah_frac;
		l_wsum_per_cnt = global.cal.wsum_per_cnt;
		IRQ_GLOBAL_ENABLE();

		// 1. Reconstruct 1 VAh precision from application accumulators
		real_vah = (l_load_val * 10) + l_load_rmndr + l_load_ctr;

		// 2. Extract the sub-1-VAh fractional energy from the metrology core
		tenths_of_vah = (l_wsum_per_cnt > 0)
							? ((uint32_t)l_vah_frac * 10) / (uint32_t)l_wsum_per_cnt
							: 0;

		// 3. Combine them to get 0.0001 kVAh precision
		real_vah_tenths = (real_vah * 10) + tenths_of_vah;

		lcd_put_num(1, 6, real_vah_tenths % 1000000UL);
		lcd_put_icon(P1);
		KVAh_Icon;
		lcd_put_char(7, 'H');
		break;
	}

	case PB_IL_KW: // Instant Load in KW (3 decimal)
		lcd_put_icon(P1);
		lcd_put_num(2, 4, inst_kw); // real-time AFE power reading, X.XXX - digits 2-5
		KW_Icon;
		lcd_put_char(8, 'L');
		lcd_put_icon(T19);
		break;

	case PB_IL_KVA: // Instant Load in KVA (3 decimal)
		lcd_put_icon(P1);
		lcd_put_num(2, 4, inst_kva); // real-time AFE apparent-power reading, X.XXX - digits 2-5
		KVA_Icon;
		lcd_put_char(8, 'L');
		lcd_put_icon(T19);
		break;

	case PB_INST_V: // Instant Voltage
		if (nmiss_stat == 1)
			lcd_put_num(3, 4, 2400);
		else
			lcd_put_num(3, 4, inst_voltage);
		V_Icon
			lcd_put_icon(T1);
		break;

	case PB_INST_I: // Phase Current
		lcd_put_icon(P2);
		if (inst_phase_current > 8)
			lcd_put_num(3, 5, inst_phase_current);
		else
			lcd_put_num(3, 5, 0);
		lcd_put_char(8, 'P'); // "P"
		A_Icon;
		break;

	case PB_INST_IN: // Neutral Current
		lcd_put_icon(P2);
		if (inst_neutral_current > 7)
			lcd_put_num(3, 5, inst_neutral_current);
		else
			lcd_put_num(3, 5, 0);
		lcd_put_char(8, 'N'); // "N"
		A_Icon;
		break;

	case PB_INST_PF: // Instant PF
		lcd_put_icon(P2);
		lcd_put_num(4, 3, abs(inst_pf));
		lcd_put_flash_str(1, test_str[0]); // "PF"
		break;

	case PB_ONHR: // Power-on hours, current month
	{
		uint32_t hrs = reset_on_time / 60;

		if (hrs > 999)
			lcd_put_num(3, 4, hrs);
		else if (hrs > 99)
			lcd_put_num(4, 3, hrs);
		else if (hrs > 9)
			lcd_put_num(5, 2, hrs);
		else
			lcd_put_num(6, 1, hrs);

		lcd_put_flash_str(1, test_str[27]); // "ON"
		lcd_put_icon(T17);					// h
		break;
	}

	case PB_AVG_PF: // Average PF
		lcd_put_icon(P2);
		lcd_put_num(4, 3, abs(avg_pf));
		lcd_put_flash_str(1, test_str[0]); // "PF"
		lcd_put_icon(T8);				   // AV
		break;

	default: // shouldn't be reached - TaskAutoScroll() stops the cycle at PB_TOTAL_SCREENS
		PushButtonDisplayFlag = 0;
		LCD_PushButton_Parm = 0;
		break;
	}

	if (LCD_PushButton_Parm != PB_SEG_CHECK && PowerOnSec > 3) // show after 3 secs
		TamperIcons();

	Put_Data_On_LCD(); // Lcd clear and seg writing is done here
}
int8_t Display_Complete = 0;
void PushButtonWakeDisplay(void)
{
	// lcd_clear();

	if (PushButtonCommMode == 1)
	{
		// PushButtonDisplayFlag=0;
		// PushButtonTimeOut=0;
		// LCD_PushButton_Parm=0;
		if (NoOfSeconds++ >= 5)
		{
			NoOfSeconds = 0;
			LCD_DisplayParm++;
			// NVRAM_Push_Buton_Disp=LCD_DisplayParm;
		}

		if (LCD_DisplayParm >= 6)
		{
			Display_Complete = 1;
			LCD_DisplayParm = 6;
		}
		else
			Communication_Enable_Counter = 0;

		NVRAM_Push_Buton_Disp = LCD_DisplayParm;
	}

	switch (NVRAM_Push_Buton_Disp)
	{

	case 1:						  // Current Date
		lcd_put_num(5, 2, YEAR);  // Year
		lcd_put_num(3, 2, MONTH); // Month
		lcd_put_num(1, 2, DATE);  // Date
		lcd_put_icon(P4);
		lcd_put_icon(T6);
		break;

	case 2:						 // Current Time
		lcd_put_num(5, 2, SEC);	 // sec
		lcd_put_num(3, 2, MIN);	 // min
		lcd_put_num(1, 2, HOUR); // hr
		lcd_put_icon(P4);
		lcd_put_icon(T7);
		break;

	case 3: // MD
		lcd_put_icon(P2);
		lcd_put_num(4, 3, kwmd_val); //  dec place
		KW_Icon;
		lcd_put_icon(T12);
		break;

	case 4:							   // MD Time
		lcd_put_num(5, 2, KWMD_DT[7]); // sec
		lcd_put_num(3, 2, KWMD_DT[6]); // min
		lcd_put_num(1, 2, KWMD_DT[5]); // hr
		lcd_put_icon(P4);
		lcd_put_icon(T7);
		lcd_put_icon(T12);
		break;

	case 5: // MD Date
		MD_Year = (KWMD_DT[0] << 8 | KWMD_DT[1]) % 2000;
		lcd_put_num(5, 2, MD_Year);	   // Year
		lcd_put_num(3, 2, KWMD_DT[2]); // Month
		lcd_put_num(1, 2, KWMD_DT[3]); // Date
		lcd_put_icon(P4);
		lcd_put_icon(T6);
		lcd_put_icon(T12);
		break;

	case 6:
		lcd_put_num(4, 4, Communication_Enable_Counter);
		break;

	default:
		NVRAM_Push_Buton_Disp = 0;
		LCD_PushButton_Parm = 0;
		break;

	} //*/

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

	if (rev_stat == 1)
		lcd_put_icon(T4);
	if (cuopen_stat == 1)
		lcd_put_icon(T11);
	if (magnetic_stat == 1)
		lcd_put_icon(T9);

	if ((NM_CT_Detected == 1) || (nd_stat == 1))
		lcd_put_icon(T5);
	if (((eload_stat == 1) || (nd_stat == 1)) && (magnetic_stat == 0))
		lcd_put_icon(T4);

	//	if(THD_10V_40V_inject_flag)
	//	lcd_put_icon(OK);

	if (NM_CT_Detected == 0)		// not required in NM mode
		if (!(SYS->STAT_b.v3a_nok)) // Power ON Icon
			lcd_put_icon(T2);
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

	// lcd_clear();

	if (PushButtonCommMode == 1)
		lcd_put_flash_str(1, test_str[11]);
	else
		lcd_put_flash_str(1, test_str[10]);
	//*/

	// lcd_put_num(1,7,WakeFromRegisterCopy );
	Put_Data_On_LCD(); // Lcd clear and seg writing is done here
}

void CalDisplay(void)
{
	if (CalDisplayVar == CLR1)
		lcd_put_flash_str(1, test_str[14]);
	else if (CalDisplayVar == CLR2)
		lcd_put_flash_str(1, test_str[12]);
	else if (CalDisplayVar == CLR3)
		lcd_put_flash_str(1, test_str[13]);
	else if (CalDisplayVar == UNLOCK)
		lcd_put_flash_str(1, test_str[15]);
	else if (CalDisplayVar == CLRC)
		lcd_put_flash_str(1, test_str[16]);
	else if (CalDisplayVar == RESET)
		lcd_put_flash_str(1, test_str[17]);
	else if (CalDisplayVar == SLNO)
		lcd_put_flash_str(1, test_str[18]);
	else if (CalDisplayVar == CAL)
		lcd_put_flash_str(1, test_str[19]);
	Put_Data_On_LCD();
}

/* ==========================================================================
 * Battery Mode display (on battery-backup power - see pwrmode.h / bat.h).
 * Shorter, low-power screen set with both auto-scrolling and manual
 * (push-button) scrolling, per the display spec:
 *   KWh, KVAh, current-month MD KW/KVA,
 *   previous-month (H1) KWh/KVAh/MD KW/MD KVA/power-on hours.
 * ========================================================================== */
uint8_t BAT_DisplayParm;
static uint8_t s_bat_sec_ctr;

void BatteryModeRender(void)
{
	switch (BAT_DisplayParm)
	{
	case BAT_KWH: // KWh (6+1)
		lcd_put_icon(P3);
		lcd_put_num(1, 7, ENERGY_TENTHS(load_val[0]));
		KWh_Icon;
		break;

	case BAT_KVAH: // KVAh (6+1)
		lcd_put_icon(P3);
		lcd_put_num(1, 7, ENERGY_TENTHS(load_val[1]));
		KVAh_Icon;
		break;

	case BAT_MD_KW: // Current month MD in KW (3 decimal)
		lcd_put_icon(P1);
		lcd_put_num(2, 4, MD_THOUSANDTHS(kwmd_val)); // X.XXX - digits 2-5
		KW_Icon;
		lcd_put_icon(T12);
		break;

	case BAT_MD_KVA: // Current month MD in KVA (3 decimal)
		lcd_put_icon(P1);
		lcd_put_num(2, 4, MD_THOUSANDTHS(kvamd_val)); // X.XXX - digits 2-5, mirrors DISP_MD
		KVA_Icon;
		lcd_put_icon(T12); // "MD" icon (shared with the KW-MD screen)
		break;

	case BAT_PM_KWH: // Previous month KWh (5+1, H1, Bill)
		PB_LoadBillHistory(1);
		lcd_put_num(1, 6, ENERGY_TENTHS(stBilling_Profile.Cumm_Energy_KWh)); // (5+1), digits 1-6
		KWh_Icon;
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		lcd_put_icon(T1);
		lcd_put_icon(T13);
		break;

	case BAT_PM_KVAH: // Previous month KVAh (5+1, H1, Bill)
		PB_LoadBillHistory(1);
		lcd_put_num(1, 6, ENERGY_TENTHS(stBilling_Profile.Cumm_Energy_KVAh)); // (5+1), digits 1-6
		KVAh_Icon;
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		lcd_put_icon(T1);
		lcd_put_icon(T13); // BP
		break;

	case BAT_PM_MD_KW: // Previous month MD in KW (3 decimal, H1, Bill)
		PB_LoadBillHistory(1);
		lcd_put_icon(P1);
		lcd_put_num(2, 4, MD_THOUSANDTHS(stBilling_Profile.MD_KW)); // X.XXX - digits 2-5
		KW_Icon;
		lcd_put_icon(T12); // "MD" icon
		lcd_put_icon(T13); // BP
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		break;

	case BAT_PM_MD_KVA: // Previous month MD in KVa (3 decimal, H1, Bill)
		PB_LoadBillHistory(1);
		lcd_put_icon(P1);
		lcd_put_num(2, 4, MD_THOUSANDTHS(stBilling_Profile.MD_KVA)); // X.XXX - digits 2-5
		KVA_Icon;
		lcd_put_icon(T12);
		lcd_put_icon(T13); // BP
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		break;

	case BAT_PM_ONHR: // Previous month power-on hours (P, on, Bill)
	{
		uint32_t hrs;

		PB_LoadBillHistory(1);
		hrs = stBilling_Profile.Power_On_Time / 60;

		// Battery mode shifts the digits right by one index
		if (hrs > 999)
			lcd_put_num(3, 4, hrs);
		else if (hrs > 99)
			lcd_put_num(4, 3, hrs);
		else if (hrs > 9)
			lcd_put_num(5, 2, hrs);
		else
			lcd_put_num(6, 1, hrs);

		lcd_put_flash_str(1, test_str[27]); // "ON" tag
		lcd_put_char(7, 'H');
		lcd_put_num(8, 1, 1);
		lcd_put_icon(T13); // BP
		lcd_put_icon(T17); // h
		break;
	}

	default:
		BAT_DisplayParm = 0;
		break;
	}

	Put_Data_On_LCD();
}

/* Call once per second while running on battery backup to auto-scroll
 * through the battery-mode screen set. */
void BatteryModeTask(void)
{
	if (++s_bat_sec_ctr >= BATTERY_DISP_INTERVAL_SEC)
	{
		s_bat_sec_ctr = 0;
		if (++BAT_DisplayParm >= BAT_TOTAL_SCREENS)
			BAT_DisplayParm = 0; // wrap and keep auto-scrolling (battery mode has no "one cycle" timeout in the spec)
	}
	BatteryModeRender();
}

/* Call on a push-button press while on battery backup for manual scrolling.
 * Resets the auto-scroll timer so the screen doesn't jump again right away. */
void BatteryModeManualNext(void)
{
	if (++BAT_DisplayParm >= BAT_TOTAL_SCREENS)
		BAT_DisplayParm = 0;
	s_bat_sec_ctr = 0;
	BatteryModeRender();
}