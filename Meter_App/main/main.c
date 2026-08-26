/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Main routine.
 *
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * $Id: main.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "options.h"  // Define the IC, CE code and other options.
#include "afe.h"	  // To read the AFE.
#include "bat.h"	  // To read the batteries.
#include "cli.h"	  // Command line interface.
#include "delay.h"	  // A calibrated delay.
#include "gpio.h"	  // GPIO functions.
#include "hash_str.h" // Makes a number from a string.
#include "irq.h"	  // Interrupt management.
#include "meter.h"	  // Meter API and data structures.
#include "nvram.h"	  // Read and write nonvolatile RAM.
#include "pwrmode.h"  // To use battery mode code.
#include "sspi.h"	  // Slave SPI initialization
#include "stm.h"	  // Software timers
#include "systick.h"  // Systick driver
#include "tamper.h"	  // Tamper detection
#include "time.h"	  // Clock and calendar.
#include "tmp.h"	  // To read the temperature.
#include "wd.h"		  // Watchdog driver.
#include "main.h"	  // Check prototypes
#include "timer0.h"

#include "ser_common.h"
#include "ser2.h"

#include "timer1.h"
#include "Defines.h"
#include "dlms_eeprom.h"
#include "dlms_stack.h"
#include "asdaq_variables.h"
#include "flash.h"
extern void TaskAutoScroll(void);
extern void lcd_init_1(void);
void invole_BL(void);

extern void lcd_update(void);

#define WD_MONITOR_MS 10000		  // 10 seconds before watchdog failure.
#define WD_TIMER_MS 300			  // 1/3 seconds per timer
#define SAVE_DELAY_SECONDS 2	  // Seconds before saving billing data again.
#define USER_TIMEOUT_SECONDS 1800 // 1/2hr after CLI command it's unattended.

// Global data definitions.
// Defined in globals.h, included by options.h
Global_t global;

// Nonvolatile data definitions are in battery-backed CMOS RAM.
// Defined in globals.h, included by options.h
// For debug, see the nonvolatile memory structure.
Nvm_t nvm; // Non-volatile RAM (NVRAM)

// Select a default CE.
const ce_t *ce_ptr = &default_ce; // Pointer to CE structure.

// Internal data definitions.
volatile bool reset_mode;		 // Reset occurred.
volatile bool lcd_data;			 // LCD data is available.
volatile bool wfi_enable = true; // true = CPU may idle its clock
static int user_timer;			 // If nonzero, a user is present (they typed).
volatile int wd_timer;			 // If zero, the self-tests failed.
uint8_t CalDisplayVar = 0;

static void main_mission_mode(void);	// Do mission mode.
static void main_timing_tasks(void);	// Do shared timing tasks.
static void eeprom_vcc_enable(bool en); // Enable/disable power supply EEPROM_VCC.

unsigned char delay_sec;
// struct tm tmT; // time structure.

void lcdmem(unsigned char, unsigned char);
void disp_all_lcd(unsigned char);

unsigned char get_digit(unsigned char);
int ser2_tx_ch(int c);

void serial_comm(void);
void transmit_char2(unsigned char);

extern int eeprom_erase(unsigned char);
void lcd_put_flash_str(int32_t i, const char *str);
/****************************************************************************
 * Description:
 *       Handle faults and spurious interrupts.
 *       The problem is that these can happen at any time at all:
 *       during initialization, before the register data structure is valid,
 *       etc.
 * Parameters:
 *   Input:          An event number to log.
 *   Output:         The log is updated.
 * Return Status:    None
 ******************************************************************************/
static void Fault_Handler(Ev_t ev)
{
	// This flag tells which of the two register copies is current.
	extern volatile bool save_global_reg;

	// Get the currently valid data into nvm.reg.
	if (save_global_reg)
		nvm.reg = global.reg;
	// else the nvm reg copy is current.

	// A fault report is not worth trashing the meter's billing registers.
	if (0 != nvm.reg.data_ok_reg) // A nonzero CRC? Probably valid.
	{
		log_event(ev); // Log the event into the reg structure.

		reg_crc(); // Make the result valid.

		// Write it to NVRAM. If the NVRAM is enabled.
		nvram_write(PCB_NVRAM_ADR_NVM, (uint8_t *)&nvm, sizeof(Nvm_t));
	}

	nvram_disable();
	NVIC_SystemReset(); // Recover by reinitializing the meter.
}

// Handles a hard fault. Called from the vector table. Enables NVRAM.
void HardFault_Handler(void)
{
	nvram_enable();
	Fault_Handler(EV_HARDFAULT);
} // HardFault_Handler

// Handles a spurious interrupt. Called from the vector table.
void Default_Handler(void)
{
	nvram_enable();
	Fault_Handler(EV_HARDFAULT);
} // Default_Handler

// Handles a crystal failure. (E.g. from condensation.)
void main_xtal_alarm(void)
{
	log_state(EV_RTC_UNSET, true); // Clock is bad.
	log_state(EV_HARDWARE, true);
} // main_xtal_alarm

/*****************************************************************************
 * Description:
 *        Enable power supply EEPROM_VCC.
 * Parameters:
 *   Input:
 *         en: true - enable the EEPROM_VCC.
 *         		false - disable the EEPROM_VCC.
 *   Output:
 *         None.
 * Return Status:
 *         None
 *****************************************************************************/

static void eeprom_vcc_enable(bool en)
{
	gpio_alt_off(VCC_EEPROM);
	gpio_dir_out(VCC_EEPROM);
	if (true == en)
	{
		gpio_set_state(VCC_EEPROM, true);
	}
	else
	{
		gpio_set_state(VCC_EEPROM, false);
	}
}

static void pulse_vcc_enable(bool en)
{
	gpio_alt_off(PULSE_VCC);
	gpio_dir_out(PULSE_VCC);
	if (true == en)
	{
		// Demo board has DIO_40 as VCC_EEPROM
		// When VCC_EEPROM is output and set high then SDA and SCL will have power
		gpio_dir_out(PULSE_VCC);
		gpio_set_state(PULSE_VCC, true);
	}
	else
	{

		gpio_set_state(PULSE_VCC, false);
	}
}

static void optical_power_enable(bool en)
{
	gpio_alt_off(OPTICAL_POWER);
	if (true == en)
	{

		gpio_dir_out(OPTICAL_POWER);
		gpio_set_state(OPTICAL_POWER, true);
	}
	else
	{
		gpio_dir_out(OPTICAL_POWER);
		gpio_set_state(OPTICAL_POWER, false);
	}
}

/****************************************************************************
* Description:
*       Poll the meter's pushbutton
* Parameters:
*   Input:
*         The push-button's GPIO bit.
*   Output:
* Return Status:
*         Returns true if the button is pushed, false otherwise.
0******************************************************************************/
int pb_read(void)
{
	int ret_val = false;
#ifdef PCB_BUTTON_SEG
	ret_val |= (PCB_BUTTON_PUSHED == gpio_get_state(PCB_BUTTON_SEG));
#endif
#ifdef PCB_SCROLL_SEG
	ret_val |= (PCB_SCROLL_PUSHED == gpio_get_state(PCB_SCROLL_SEG));
#endif
	return ret_val;
} // pb_read

void ReadCalConstants(void)
{

	//*************Meter 1************************

	ce_data.cal_v0 = from_eeprom(VOLT_FACT_LOC, 2);
	ce_data.cal_i0 = from_eeprom(AMP_FACT_LOC, 2);
	ce_data.phadj_0 = (long int)from_eeprom(IPH_LOC, 2);
	ce_data.cal_i1 = from_eeprom(AMP_FACT_LOC + 2, 2);
	ce_data.phadj_1 = (long int)from_eeprom(IPH_LOC + 2, 2);

	if (ce_data.phadj_0 > 30000)
		ce_data.phadj_0 = ce_data.phadj_0 - 65535;

	if (ce_data.phadj_1 > 30000)
		ce_data.phadj_1 = ce_data.phadj_1 - 65535;

	if (ce_data.cal_v0 < 5000 || ce_data.cal_v0 > 30000)
	{
		ce_data.cal_v0 = 16350; // 16050;
		ce_data.phadj_0 = -580; // 0;
		ce_data.cal_i0 = 27566; // 13500;

		ce_data.phadj_1 = 1090; // 0;
		ce_data.cal_i1 = 17693; // 16384;
	}

	if (NM_CT_Detected)
	{
		ce_data.cal_i0 = 1.121 * ce_data.cal_i0;
		;
	} //*/
	//******************************************

	if (NM_CT_Detected == 0)
		ce_data.wrate = WRATE;
	else
		ce_data.wrate = WRATE_NM; // WRATE_NM;
}

void DefaultCalConstants(void)
{
	//	ce_data.cal_v0 = 16384;//
	//	ce_data.cal_i0 = 16384;
	//	ce_data.phadj_0 = 0;
	//
	//	ce_data.cal_i1 = 16384;
	//	ce_data.phadj_1 =0;
}

uint16_t WakeFromReason;
#define WAKE_FROM_MAINS 100
#define WAKE_FROM_PB 101
#define WAKE_FROM_UNKOWN 110
extern void lcd_clear(void);
void Sleep(void)
{
	int i, j, k = 1;

	lcd_clear();

	if (NM_CT_Detected == 1)
		k = 1;

	if (WakeFromReason == WAKE_FROM_MAINS)
	{
		for (i = 0; i < k; i++)
		{
			for (j = 0; j < 40000; j++)
			{
			};
			wd_reset();
		}
	}
	// Wait for the 32KHz state machine to switch to sleep.
	while (true)
	{

		SYS->MOD_CNTL = 1;	// Sleep
		delay(DELAY_MS(1)); // Wait for the switch, then retry.
	}
}

uint32_t NVRAM_Push_Buton_Disp; // NVRAM var should be 32bit
void PushButtonWakeDisplay(void);
void WakePushButtonFunction(void)
{
	nvram_enable();
	nvram_read((uint8_t *)&NVRAM_Push_Buton_Disp, 1, PCB_NVRAM_ADR_NVM + sizeof(Nvm_t));

	NVRAM_Push_Buton_Disp++;

	if (NVRAM_Push_Buton_Disp > 5) // parameters to display in push button mode
		NVRAM_Push_Buton_Disp = 1;

	nvram_write(PCB_NVRAM_ADR_NVM + sizeof(Nvm_t), (uint8_t *)&NVRAM_Push_Buton_Disp, 1);
	nvram_disable();

	rtc_read(&global.reg.tm);
	eeprom_vcc_enable(true);
	PushButton_init_eeprom();

	lcd_init_1();
	PushButtonWakeDisplay();
	SetWakeSources();

	// Wait for the 32KHz state machine to switch to sleep.
	while (true)
	{

		SYS->MOD_CNTL = 3;	// Sleep
		delay(DELAY_MS(1)); // Wait for the switch, then retry.
	}
}
uint8_t PushButtonCommMode = 0;
void CheckWakeSource(void)
{
	uint32_t i, j = 0;

	if (SYS->WAKE_SRC_b.ws_vsys || SYS->WAKE_SRC_b.ws_wdof || SYS->WAKE_SRC_b.ws_rstbit || SYS->WAKE_SRC_b.ws_rst || SYS->WAKE_SRC_b.ws_cstart)
	{
		WakeFromReason = WAKE_FROM_MAINS;
	}
	else if (SYS->WAKE_SRC_b.ws_pb)
	{
		WakeFromReason = WAKE_FROM_PB;
		WakeFromPushButtonFlag = 1;

		gpio_dir_in(PCB_BUTTON_SEG);
		while (gpio_get_state(PCB_BUTTON_SEG))
		{
			for (i = 0; i < 1000; i++)
			{
			}
			wd_reset();
			j++;

			if (j > 100)
			{
				PushButtonCommMode = 1;
				break;
			}
		}

		if (PushButtonCommMode == 0) // Jump to brownout mode for communication
			WakePushButtonFunction();
	}
	else if (SYS->WAKE_SRC_b.ws_tmr)
	{
		WakeFromPushButtonFlag = 0; // donot set timer wake again

		NVRAM_Push_Buton_Disp = 0; // reset pushbuttun variable

		nvram_enable();
		nvram_write(PCB_NVRAM_ADR_NVM + sizeof(Nvm_t), (uint8_t *)&NVRAM_Push_Buton_Disp, 1);
		nvram_disable();
		SetWakeSources();
		Sleep();
	}
	else
		WakeFromReason = WAKE_FROM_UNKOWN;
}
uint8_t WakeFromPushButtonFlag = 0;
void SetWakeSources(void)
{
	sys_shadow_busy_wait();		   // Wait till the RTC is free
	SYS->WAKE_SRC = SHADOW_UNLOCK; // Freeze the shadow registers.
	SYS->WAKE_b.ew_pb = 1;

	if (WakeFromPushButtonFlag)
	{
		SYS->WAKE_b.ew_tmr = 1;
		SYS->WAKE_b.wake_tmr = 8 * 8; // 0.125*8*8(Seconds required)
	}

	SYS->PWR_MOD_b.brn_en = 1; // Enable battery modes by default.//Push Button Wake
	SYS->WAKE_SRC = 0;		   // Unfreeze the shadow registers.
	sys_shadow_busy_wait();
} // pwrmode_set_rtcwake

void Stabilization_Check(void)
{
	long temp_long_i;
	///...stabilization check
	// if(WakeFromReason==WAKEFROMMAINS)
	{
		if (!(SYS->STAT_b.v3a_nok))
		{
			for (temp_long_i = 0; temp_long_i < 6500; temp_long_i++)
			{
				wd_reset();
				if (SYS->STAT_b.v3a_nok)
				{
					while (1)
					{
						// sleep_variable();
						Sleep();
					}
				}
			}
		}
	}
}

void Initialize_Metering(void)
{

	afe_init(&default_ce);
	cal_get();
	wd_reset();
	meter_start(&default_ce);

	if (NM_CT_Detected)
	{
		ce_data.ceconfig &= ~BIT1; // PULSE_FAST=1
		ce_data.ceconfig &= ~BIT0; // PULSE_SLOW=0
	}

	ReadCalConstants();
	wd_reset();

	if (NM_CT_Detected == 1)
	{
		SYS->MPU_CLK_b.mpu_spd = 0; // Update mpu_spd
		ce_data.ceconfig |= BIT29;	//==>VLS=1
	}
	else
	{
		SYS->MPU_CLK_b.mpu_spd = 3; // Update mpu_spd	//RAJIV
	}

	sys_set_mpuclk(SYS->MPU_CLK_b.mpu_spd, ADC->CNTL_b.adc_vls); // 851kHz with VLS=0; 786KHz VLS=1
}

void ReadShuntKW(void);
void ReadShuntKVA(void);
void CalibrateShunt(void);
void ReadctKW(void);
void ReadctKVA(void);
void CalibrateCT(void);
extern int afe_busy;

// unsigned long tmp_arr;
uint8_t NM_CT_Detected = 0;

extern uint32_t Temp_inst_voltage;
uint8_t PowerOnSec = 0, Mains_Supply_DIO = 0, forcefully_set_rtc = 0xFF;
uint16_t Last_Communication_Enable_Counter;
uint32_t WakeFromRegisterCopy;
int main(void)
{
	wd_reset(); // Reset the watchdog.

	WakeFromRegisterCopy = (SYS->WAKE_SRC >> 8) & 0x0FFFFF; // bits 8 to 27 to fit in LCD for testing
	CheckWakeSource();										// Check reason of meter reset/power on
	SetWakeSources();										// sleep; Wake for V3P3SYS
	wd_reset();												// push off the hardware watchdog
	SystemInit();											// Initialize the SoC to defaults.

	if (WakeFromReason != WAKE_FROM_PB)
		if (SYS->STAT_b.v3a_nok)
			Sleep();

	gpio_dir_in(NM_DETECT_PIN);
	gpio_dir_in(MAGNET_PIN);
	lcd_init_1();
	Display_Version();
	wd_reset(); // push off the hardware watchdog

	if (WakeFromReason == WAKE_FROM_MAINS)
	{
		Stabilization_Check();
	}

	// 0,0==>10.223616MHz   	//0,1==>786.432kHz
	// 1,0==>5.11808MHz      //1,1==>393.216kHz
	// 2,0==>2.555904MHz     //2,1==>196.608kHz
	// 3,0==>851.968kHz      //3,1==>196.608kHz
	// 4,0==>425.984kHz      //4,1==>196.608kHz
	sys_set_mpuclk(3, 0); // 851kHz

	eeprom_vcc_enable(true);

	if (WakeFromReason != WAKE_FROM_PB)
		pulse_vcc_enable(true);

	if (WakeFromReason == WAKE_FROM_MAINS)
		NM_CT_Detected = !(gpio_get_state(NM_DETECT_PIN)); // NM Pin

	if (NM_CT_Detected == 0) // not required in NM mode
	{
		optical_power_enable(false);
	}

	init_io_param();

	rtc_read(&global.reg.tm);

	//**************************************clear NVRAM_Push_Buton_Disp during mission and comm mode
	nvram_enable();
	nvram_read((uint8_t *)&NVRAM_Push_Buton_Disp, 1, PCB_NVRAM_ADR_NVM + sizeof(Nvm_t));

	NVRAM_Push_Buton_Disp = 0;

	nvram_write(PCB_NVRAM_ADR_NVM + sizeof(Nvm_t), (uint8_t *)&NVRAM_Push_Buton_Disp, 1);
	nvram_disable();
	//**************************************

	init_eeprom();
	wd_reset(); // Reset the watchdog.
	global.cal.cfg = CLEAR_REG;

	init_tod();

	if (WakeFromReason == WAKE_FROM_MAINS)
	{
		TaskAutoScroll();
	}
	else // if wake from push button comm mode
	{
		NoOfSeconds = 2;
		LCD_DisplayParm = 1;
		PushButtonWakeDisplay();
	}

	wd_reset();
	if (WakeFromReason != WAKE_FROM_PB)
	{
		if (WakeFromReason == WAKE_FROM_MAINS)
		{
			Initialize_Metering();
		}
	}

	if (NM_CT_Detected == 0)
	{
		// timer0_start(100, dlmsif_timer0); // DLMS Timer
		// hdlc_handle = dlms_process_init();
		ser2_init(9600, NULL, NULL, NULL, NULL);
	}

	timer1_start(TIMER1_MSEC, Timer1_Interrupt); // General timer for housekeeping

	// Set a timer to monitor the main loop and CE.
	global.misc.wd_want = CE_WD;
	global.misc.wd_set = 0;
	stm_start(&wd_timer, STM_SEC(10));
	wd_reset(); // Reset the watchdog.

	lcd_data = false;

	IRQ_GLOBAL_ENABLE(); // Enable interrupts

	if (forcefully_set_rtc == 1)
	{
		default_eeprom(0);
		/*s_time.tm_mday = 08;
			s_time.tm_mon = 12-1;
			s_time.tm_year = 25;
			s_time.tm_wday = MON;
			s_time.tm_hour = 19;
			s_time.tm_min = 15;
			s_time.tm_sec = 0;
			s_time.tm_subsec = 0;//*/
		rtc_write(&s_time);
	}

	while (true)
	{
		if (SYS->STAT_b.v3a_nok) // if V3P3A is below its threshold
		{
			if (PushButtonCommMode == 0)
			{
				afe_disable();
				meter_save_data(); // Save the meter data.
			}

			while (1)
			{
				if (SYS->STAT_b.v3a_nok)
				{
					if (WakeFromReason == WAKE_FROM_PB)
					{
						_1_SecFunction();

						//*****************
						if (PushButtonCommMode == 1)
						{
							while ((Display_Complete == 0) || (Communication_Enable_Counter < 10))
							{
								_1_SecFunction();
								// dlms_server_process(hdlc_handle);
								if (!(SYS->STAT_b.v3a_nok)) // if power comes in between, reset meter
									break;
							}

							SYS->MOD_CNTL |= BIT31;
							while (1)
								;
						}
					}
					else
					{
						Sleep();
					}
				}
				else // Power restored
				{
					SYS->MOD_CNTL |= BIT31;
					while (1)
						;
				}
			}
		}
		else
		{
			// UART2->DATA =2;
			main_mission_mode();

			if (NM_CT_Detected == 0)
			{
				// dlms_server_process(hdlc_handle);
			}

			/* --- THE DLMS BRAIN --- */
			DLMS_HDLC_ProcessFrame();

			serial_comm();
			_1_SecFunction();
			// ser2_tx_ch(0xaa);

			if ((NM_CT_Detected == 1) && (Mains_Supply_DIO == 1) && (inst_voltage > 1000) && (inst_voltage != 2400)) // Reset in case power resumes during NM
			{

				SYS->MOD_CNTL |= BIT31;
				while (1)
					;
			}
			else
			{
				if ((NM_CT_Detected == 0) && (Temp_inst_voltage < 4) && (PowerOnSec > 2)) // No NM but voltage removed without removing load. With Diode tamper voltage read is approx 4V-6V
				{
					SYS->MOD_CNTL |= BIT31;
					while (1)
						;
				}
			}
		}
	}
} // main

/***************************************************************************
 * Description:
 *       This mode meters from normal AC power.
 *   Input:
 *       It reads the analog front end and meters from it.
 *       It reads the serial port.
 *       It reads the power supply's state.
 * Parameters:
 *       None.
 *   Output:
 *       It produces billing data.
 *       It leaves this mode when the power fails.
 * Return Status:
 *       none
 ***************************************************************************/
static void main_mission_mode(void)
{
	// unsigned int tmp_int;

	// if(wfi_enable) // Controlled by cli "w" command.
	//    __WFI();  // Shut down CPU clock until the next interrupt.

	// Run the software timers.
	stm_run();

	// Are all the needed events done? (Their bits set?)
	if (global.misc.wd_want == (global.misc.wd_set & global.misc.wd_want))
	{
		// Good, detect the events again.
		global.misc.wd_set = 0;
		stm_start(&wd_timer, STM_SEC(10));
	}
	//   if(0 != wd_timer)
	wd_reset(); // Reset the watchdog.

	// Detect tamper, by polling or resetting interrupts.
	//  tamper_run();

	// Run metering logic
	lcd_data = meter_run(); // returns true for new data.

	// Things to do once per second, and at start.
	if (RTC->TIME != global.misc.last_sec)
	{
		global.misc.last_sec = RTC->TIME;

		// rtc_read(&global.reg.tm);       // Read the clock once per second.

		main_timing_tasks();
	} // end once per second.

	// Run the command line interface.
	if (cli_run()) // if typing...
	{
		log_state(EV_COM, true); // Communication.
		// Detect an engineer.
		if (stm_start(&user_timer, STM_SEC(USER_TIMEOUT_SECONDS)))
			log_software();
	}

	if (lcd_data)
	{
		lcd_data = false;

		global.misc.wd_set |= CE_WD; // Note that the CE ran.
	}
	reset_mode = false;
}

/***************************************************************************
 * Description:
 *       Does timed tasks.
 *   Input:
 *       It's called once per second.
 * Parameters:
 *       None.
 *   Output:
 *       This collects many small timed activities:
 *       timers are updated and rolled-over as needed.
 *       The Vref is compensated.
 *       The once-per-minute demand task is performed.
 *       The battery is measured.
 *       Once per hour the billing registers are saved.
 *       Any errors are recorded.
 * Return Status:
 *         None
 ***************************************************************************/
static void main_timing_tasks(void)
{
	// Restart crystal failure sensing.
	if (!(SYS->STAT_b.xtal_fail))
	{
		// Detect a crystal failure.
		sys_int_enable(CRYSTAL_FAIL_INT, main_xtal_alarm);
	}

	// Seconds of operation.
	++global.reg.operating_sec; // Count operating seconds.
	/* turn over the counter at 100,000 hours, 11.4 years */
	if (MAX_OPERATING_SECS <= global.reg.operating_sec)
	{
		global.reg.operating_sec -= MAX_OPERATING_SECS;
	}

	// Count seconds of sag.
	if (meter_state & SAG_MASK)
	{
		++global.reg.sag_sec;
		// turn over the counter, format like operating secs.
		if (MAX_OPERATING_SECS <= global.reg.sag_sec)
		{
			global.reg.sag_sec -= MAX_OPERATING_SECS;
		}
	}

	// Count seconds of tamper.
	if (global.reg.log.bits & LOG_TAMPER_MASK)
	{
		++global.reg.tamper_sec;
		/* turn over the counter, format like operating secs */
		if (MAX_OPERATING_SECS <= global.reg.tamper_sec)
		{
			global.reg.tamper_sec -= MAX_OPERATING_SECS;
		}
	}

	// Clear an RTC alarm after about a minute.
	if (59 == RTC->TIME_b.rtc_sec && log_active(EV_RTC_ALARM))
	{
		log_clear_one(EV_RTC_ALARM);
	}

	// Check for things to do once per minute, and at start.
	if (global.misc.last_min != global.reg.tm.tm_min)
	{
		global.misc.last_min = global.reg.tm.tm_min;

		// The user timer detects an engineer.
		if (0 == user_timer)
		{								  // If the meter is unattended,
			global.misc.wd_want |= CE_WD; // enable the watchdog.
		}

		// This is a good place for time-of-use logic.

		// Demand interval done?
		if (0 == (global.reg.tm.tm_min % global.cal.interval))
		{
			meter_demand();
		}

		// Check for things to do once per hour, and at start.
		if (global.misc.last_hr != global.reg.tm.tm_hour)
		{

			global.misc.last_hr = global.reg.tm.tm_hour;

// Measure batteries once per hour, report failure.
#if PCB_HAS_VBAT // PCB_* symbols are from the PCB map file.
			{
				int tmp = MIN_LOGIC_VOLTS > bat_volts();
#if PCB_HAS_VBAT_RTC
				tmp |= MIN_LOGIC_VOLTS > bat_volts_rtc();
#endif
				log_state(EV_BAT_BAD, tmp);
			}
#endif

			// Change to and from daylight savings time.
			if (time_to_dst(&global.reg.tm)) // If DST or PST changed...
				time_write(&global.reg.tm);	 // Write it to RTC.

			// Save the meter billing data once per hour.
			meter_save_data();
		} // end once per hour.
	} // end once per minute.
} // main_timing_tasks()

int SwitchCurrentState = 0, SwitchLastState = 0;
uint8_t PowerFailDataSaveFlag = 0;
int8_t WatchdogResetVar = 0, PowerOnCount;
int16_t WDResetCounter_5Sec, _1_SecCount_Timer1 = 1000 / TIMER1_MSEC;
uint16_t TestTimer_1;
uint8_t OneSecondCounter = 0, _1_SecFlag = 0;
#define WDRESET_TIME_SEC 5
#define POWER_STABLE_DURATION 5
void Timer1_Interrupt(void)
{
	TestTimer_1++;

	if ((PowerFailDataSaveFlag == 0) && (PowerOnCount > POWER_STABLE_DURATION))
	{
		if (SYS->STAT_b.v3a_nok)
		{
			if (SYS->WAKE_SRC_b.ws_vsys) // Save Data
			{
				// eeprom_write (RTC_PowerfailAddr,(uint8_t *)&RTC_Array, 6);
				//	EnergyUpdate();
				PowerFailDataSaveFlag = 1;
				PowerOnCount = 0; // incase supply restores here
			}
		}
	}

	if (TestTimer_1 >= _1_SecCount_Timer1)
	{
		_1_SecFlag = 1;
		delay_sec = 1;
		TestTimer_1 = 0;

		if (PowerOnCount <= POWER_STABLE_DURATION)
			PowerOnCount++;
	}

	//******************Check Push Button************************* ,PushButtonDisplay,LCD_Auto_Parm;
	if (PushButtonCommMode == 0) // Not required in push button comm mode
	{
		SwitchCurrentState = pb_read();
		if (SwitchLastState != SwitchCurrentState)
			if (SwitchCurrentState)
			{
				LCD_PushButton_Parm++;
				NoOfSeconds = 0;
				PushButtonDisplayFlag = 1;
				PushButtonTimeOut = 0;
				TaskAutoScroll();
			}
		SwitchLastState = SwitchCurrentState;
	}
	//************************************************************

	//*************Watch dog clear**************************
	if (WatchdogResetVar == 1)
	{
		WatchdogResetVar = 0;
		WDResetCounter_5Sec = 0;
		wd_reset();
	}
	else if ((WatchdogResetVar == 0) && (WDResetCounter_5Sec < WDRESET_TIME_SEC)) // about 5000mSec
	{
		WDResetCounter_5Sec++;
		wd_reset();
	}
	//************************************************************
}

uint16_t Communication_Enable_Counter = 0;
void _1_SecFunction(void)
{
	if (_1_SecFlag == 1)
	{
		_1_SecFlag = 0;
		WatchdogResetVar = 1;
		rtc_read(&global.reg.tm);

		DLMS_HDLC_Tick();

		if (PushButtonCommMode == 1)
		{
			Communication_Enable_Counter++;
			PushButtonWakeDisplay();
			return;
		}

		TaskAutoScroll();

		if (NM_CT_Detected == 1)
			Mains_Supply_DIO = (gpio_get_state(NM_DETECT_PIN)); // NM Pin

		if (PowerOnSec < 10)
			PowerOnSec++;

		prev_date = from_eeprom(DATE_LOC, 3);
		prev_yr = prev_date % 100;
		prev_mnth = (prev_date / 100) % 100;
		prev_day = (prev_date / 10000);
		prev_hr = read_eeprom(HR_LOC);

		read_time_date();
		dlms_actions_func();
		// inst_param_func();
		load_func();

		// if((rtc_status==1)&&(ep_status==1))
		//{
		select_season();
		history_func();
		update_tod_data();
		tamper_func(); //*/
					   //}
	}
}
#define INVOKE_BL_CMD 99
#define CAL_Default_Constants 101
#define CAL_Shunt_Read_UPF_Power 102
#define CAL_Shunt_Read_Lag_Power 103
#define CAL_CT_Read_UPF_Power 104
#define CAL_CT_Read_Lag_Power 105
extern void meter_sum_data(void); // Adds up the billing data.;

void serial_comm(void)
{
	unsigned int cmd;
	unsigned int i, tmp_int;
	unsigned long int tmp_long;

	/* If a DLMS frame flag is detected, flush it to prevent lockout */
	if (recv_buf[0] == 0x7E)
	{
		for (i = 0; i < 25; i++)
			recv_buf[i] = 0;
		recv_ctr = 0;
		return;
	}

	/* 1. If buffer is empty, exit immediately */
	if (recv_ctr < 2)
		return;

	/* 2. Wait for full frame arrival */
	tmp_int = recv_ctr;
	i = 0;
	while ((i < 5) && (recv_buf[0] != 0x7E))
	{
		if (tmp_int != recv_ctr)
		{
			tmp_int = recv_ctr;
			i = 0;
		}
		else
		{
			i++;
		}
		delay(5000);
		delay(5000);
		if (recv_ctr >= 11)
			break;
	}

	enable_recv_buf = 0;

	if ((recv_ctr >= 2) && ((recv_buf[0] != 0x7E) && (recv_buf[recv_ctr - 1] != 0x7E)))
	{
		/* 3. Validate ASDAQ Checksum */
		comm_checksum = recv_buf[0];
		for (i = 1; i < (recv_ctr - 1); i++)
			comm_checksum = comm_checksum + recv_buf[i];
		comm_checksum = comm_checksum & 0xff;

		if (recv_buf[recv_ctr - 1] == comm_checksum)
		{
			cmd = recv_buf[0];
			transmit_char2(cmd + 48);

			/* --- ORIGINAL gate: unlocked meter, or MTR_NO/UNLOCK commands --- */
			if ((meter_no == 0) || (meter_no == 0xFFFFFF) || (cmd == MTR_NO_CMD) || (cmd == UNLOCK_CMD))
			{
				switch (cmd)
				{
				case TIME_CMD:
				case DATE_CMD:
				case VI_CAL_CMD:
				case MTR_NO_CMD:
				case CAL_CMD:
				case TOD_CMD:
				case NUM_ZONE_CMD:
				case INTGRTIME_CMD:
				case DLOAD_CMD:
				case BILL_DAY_CMD:
				case RTC_CALIB_CMD:
					SerialDisplayTimeOut = 1; // --- Hold display for 1 seconds ---
					if ((cmd == TIME_CMD) || (cmd == DATE_CMD))
					{
						disp_all_lcd(0x00);
						LCD->DIO[7] = get_digit('C');
						LCD->DIO[6] = get_digit('L');
						LCD->DIO[5] = get_digit('o');
						LCD->DIO[4] = get_digit('c');

						d_day = convt_bcd_to_byte(recv_buf[1]);
						d_mnth = convt_bcd_to_byte(recv_buf[2]);
						d_yr = convt_bcd_to_byte(recv_buf[3]);
						t_hr = convt_bcd_to_byte(recv_buf[4]);
						t_min = convt_bcd_to_byte(recv_buf[5]);
						t_sec = convt_bcd_to_byte(recv_buf[6]);

						s_time.tm_subsec = 0;
						s_time.tm_mday = d_day;
						if (d_mnth > 0)
							s_time.tm_mon = d_mnth - 1;
						else
							s_time.tm_mon = 0;
						s_time.tm_year = d_yr;
						s_time.tm_wday = SUN;
						s_time.tm_hour = t_hr;
						s_time.tm_min = t_min;
						s_time.tm_sec = t_sec;
						s_time.tm_subsec = 0;

						rtc_write(&s_time);
					}
					else
					{
						tmp_long = 0;
						for (i = 0; i < 8; i++)
							tmp_long = (tmp_long * 10) + recv_buf[i + 1];

						if (cmd == MTR_NO_CMD)
						{
							CalDisplayVar = SLNO;
							CalDisplay();
							meter_no = tmp_long;
							to_eeprom(MTRNO_LOC, meter_no, 3);
							manfact_date = d_mnth;
							manfact_date = (manfact_date * 100) + d_yr;
							to_eeprom(MANUFACT_DATE_LOC, manfact_date, 2);
							dlms_manufacture_year = 2000 + (manfact_date % 100);
						}
						else if (cmd == CAL_CMD)
						{
							CalDisplayVar = CAL;
							CalDisplay();

							if (inst_pf < 80)
							{
								if (channel == 0)
								{
									ce_data.cal_v0 = 16384;
									ce_data.cal_i0 = 16384;
									ce_data.phadj_0 = 0;

									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);

									meter_sum_data();
									ReadShuntKVA();
									CalibrateShunt();
								}
								else
								{
									ce_data.cal_i1 = 16384;
									ce_data.phadj_1 = 0;

									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);
									delay1ms(250);

									meter_sum_data();
									ReadctKVA();
									CalibrateCT();
								}
							}

							to_eeprom(VOLT_FACT_LOC, ce_data.cal_v0, 2);
							to_eeprom(AMP_FACT_LOC, ce_data.cal_i0, 2);
							to_eeprom(IPH_LOC, ce_data.phadj_0, 2);
							to_eeprom(AMP_FACT_LOC + 2, ce_data.cal_i1, 2);
							to_eeprom(IPH_LOC + 2, ce_data.phadj_1, 2);
						}
						/* RTC_CALIB_CMD body stays disabled - it called Renesas-specific
						 * R_RTC_Compensate()/RADJ, which no longer applies to your new
						 * RTC driver. Leave commented until/unless you reimplement it. */
					}
					break;

				case MDRESET_CMD:
					SerialDisplayTimeOut = 1;
					CalDisplayVar = RESET;
					CalDisplay();
					break;

				case CUOPEN_CMD:
					SerialDisplayTimeOut = 1;
					CalDisplayVar = CLRC;
					CalDisplay();
					for (i = 0; i < 7; i++)
						to_eeprom(CUOPEN_LOC + i, 0, 1);
					cuopen_stat = 0;
					break;

				case UNLOCK_CMD:
					if ((recv_buf[1] == 'a') && (recv_buf[2] == 'a') && (recv_buf[3] == 'm') &&
						(recv_buf[4] == 't') && (recv_buf[5] == 'p') && (recv_buf[6] == 'l') &&
						(recv_buf[7] == 'a') && (recv_buf[8] == 'a'))
					{
						pass_step = 1;
						pass_ctr = 0;
					}
					if ((recv_buf[1] == 'b') && (recv_buf[2] == 'b') && (recv_buf[3] == 'n') &&
						(recv_buf[4] == 'u') && (recv_buf[5] == 'q') && (recv_buf[6] == 'm') &&
						(recv_buf[7] == 'b') && (recv_buf[8] == 'b') && (pass_step == 1) && (pass_ctr < 5))
					{
						pass_step = 0;
						pass_ctr = 0;
						SerialDisplayTimeOut = 1;
						CalDisplayVar = UNLOCK;
						CalDisplay();
						meter_no = 0;
						to_eeprom(MTRNO_LOC, meter_no, 3);
					}
					break;

				case IR_INST_CMD:
					/* old_style_send_inst_param(); -- still not present anywhere in your
					 * uploaded files; leave disabled unless you reimplement it */
					break;

				case CLR1_CMD:
					SerialDisplayTimeOut = 1;
					CalDisplayVar = CLR1;
					CalDisplay();
					default_eeprom(1);
					{
						SYS->MOD_CNTL |= BIT31;
						while (1)
							;
					}
					break;

				case CLR2_CMD:
				{
					SerialDisplayTimeOut = 1;
					disp_all_lcd(0x00);
					CalDisplayVar = CLR2;
					CalDisplay();
					default_eeprom(2);
					SYS->MOD_CNTL |= BIT31;
					while (1)
						;
				}
				break;

				case CLR_TAMPER_CMD:
					SerialDisplayTimeOut = 1;
					disp_all_lcd(0x00);
					CalDisplayVar = CLR3;
					CalDisplay();
					DefaultCalConstants();
					break;

				case INVOKE_BL_CMD:
					invole_BL();
					break;

				case CAL_Default_Constants:
					DefaultCalConstants();
					break;

				case CAL_Shunt_Read_UPF_Power:
					ReadShuntKW();
					break;

				case CAL_Shunt_Read_Lag_Power:
					CalibrateShunt();
					break;

				case CAL_CT_Read_UPF_Power:
					ReadctKW();
					break;

				case CAL_CT_Read_Lag_Power:
					CalibrateCT();
					break;
				}
			}
		}
	}

	/* 4. Reset Buffer */
	for (i = 0; i < 25; i++)
		recv_buf[i] = 0;
	recv_ctr = 0;
	enable_recv_buf = 1;
}

/*
unsigned char convt_bcd_to_byte(unsigned char bcd_data)
{
	unsigned char byte_data;
		byte_data=(bcd_data & 0xf0)>>4;
	byte_data=(byte_data*10)+(bcd_data & 0x0f);

	return byte_data;
}

unsigned char convt_byte_to_bcd(unsigned char byte_data)
{
	unsigned char bcd_data;

		bcd_data=(byte_data)/10;
	bcd_data=((bcd_data&0x0f)<<4)+(byte_data % 10);

	return bcd_data;
}
*/

unsigned char get_digit(unsigned char val)
{
	switch (val)
	{
	case 'A':
		return 0x7E;
	case 'b':
	case 'B':
		return 0xF4;
	case 'C':
		return 0xA6;
	case 'c':
		return 0xB0;
	case 'd':
		return 0xF8;
	case 'E':
		return 0xB6;
	case 'F':
		return 0x36;
	case 'g':
		return 0xDE;
	case 'H':
		return 0x7C;
	case 'h':
		return 0x74;
	case 'i':
		return 0x40;
	case 'I':
		return 0x48;
	case 'L':
		return 0xA4;
	case 'm':
	case 'M':
		return 0x72;
	case 'n':
		return 0x70;
	case 'N':
		return 0x6E;
	case 'o':
		return 0xF0;
	case 'O':
		return 0xEE;
	case 'P':
		return 0x3E;
	case 'r':
		return 0x30;
	case 'S':
		return 0xD6;
	case 't':
		return 0xB4;
	case 'u':
		return 0xE0;
	case 'U':
	case 'V':
		return 0xEC;
	case '-':
		return 0x10;
	case '_':
		return 0x80;

	case '0':
	case 0:
		return 0xEE;
	case '1':
	case 1:
		return 0x48;
	case '2':
	case 2:
		return 0xBA;
	case '3':
	case 3:
		return 0xDA;
	case '4':
	case 4:
		return 0x5C;
	case '5':
	case 5:
		return 0xD6;
	case '6':
	case 6:
		return 0xF6;
	case '7':
	case 7:
		return 0x4A;
	case '8':
	case 8:
		return 0xFE;
	case '9':
	case 9:
		return 0xDE;
	}
	return val;
}

void transmit_char2(unsigned char txd)
{
	ser2_tx_ch(txd);
}

void lcdmem(unsigned char address, unsigned char data)
{
	LCD->DIO[address] = data;
}

void disp_all_lcd(unsigned char data)
{
	unsigned char i;
	for (i = 0; i < 10; i++)
		LCD->DIO[i] = data;
}

// Flash page size.
#define FLASH_PAGE_SZ (0x200)
#define FLASH_SIZE (0x20000) // SY7T_M11 is 128K
// Flash checkcode at last word of the second last page
#define CHKCODE_ADDR (FLASH_SIZE - FLASH_PAGE_SZ - 4)
extern int flash_passcode;
const uint32_t check_code __attribute__((at(CHKCODE_ADDR))) = 0x875A875A;
void invole_BL(void)
{
	flash_passcode = FLASH_UNLOCK;
	flash_page_erase(CHKCODE_ADDR);

	// jump to vector 0 to enter bootloader mode
	SYS->MOD_CNTL_b.reset = 1;
	while (true)
		; // Watchdog reset
}

int32_t PhasePowerat0, NeutralPowerat0, NeutralPowerat60, PhasePowerat60;
float UPFErrorAtCT, LAGErrorAtCT, UPFErrorAtShunt, LAGErrorAtShunt;
#define REF_VOLTAGE 240
#define CURRENT_FOR_CALIB 5										// in Amps e.g 5A
#define POWER_FOR_CALIB ((REF_VOLTAGE * CURRENT_FOR_CALIB) / 2) // power at 0.5Lag--e.g 240V*5A*0.5lag=1200W
#define VDP 10

void ReadShuntKW(void)
{
	PhasePowerat0 = labs(inst_kw_p);
	UPFErrorAtShunt = ((float)(PhasePowerat0 - (REF_VOLTAGE * CURRENT_FOR_CALIB)) / (REF_VOLTAGE * CURRENT_FOR_CALIB)); //-0.1225
}

void CalibrateShunt(void)
{

	uint16_t Voltage_Applied = REF_VOLTAGE * VDP;

	float E27;
	float H20, H21, H22, H25, H26;
	float L27, M27;
	float J21, J22, J23, J24, J25, J26;
	long xxxx;

	PhasePowerat60 = labs(inst_kw_p);
	LAGErrorAtShunt = (((float)((PhasePowerat60 * 100) - (POWER_FOR_CALIB * 100))) / POWER_FOR_CALIB);
	LAGErrorAtShunt = LAGErrorAtShunt / 100;

	//***********************************************/	 //Cal_v0
	// ROUND(L26/H24,0)//L26==>old constant H24=>Error +1
	xxxx = (inst_voltage - Voltage_Applied);
	E27 = (float)xxxx / Voltage_Applied; // Voltage error
	ce_data.cal_v0 = (float)16384 / (E27 + 1);

	//	E25=UPFErrorAtShunt;
	//	E26=LAGErrorAtShunt; //Error Energy reading at +60�
	H25 = (LAGErrorAtShunt - UPFErrorAtShunt) / ((UPFErrorAtShunt + 1) * sqrt(3)); // H25=(E26-E25)/((E25+1)*SQRT(3))

	H20 = 0.015625;							 // 2^-6;
	H21 = 0.99804688;						 //(1-(2^-9));
	H22 = 0.99706007;						 // cos(2*3.14*50/4096);//H22=COS(2*PI()*D20/D22) D20==>AC Freq D22==>Sample freq
	L27 = 0;								 // default pahaj adj
	J21 = 1 + (H21 * H21) - (2 * H21 * H22); // J21=1+H21^2-2*H21*H22
	J22 = 0.076474;							 //(1-2^-9)*sin(2*3.14*50/4096);//J22=H21*H23
	J23 = 1 - H21 * H22;
	J24 = J21 * H25 / (J22 - J23 * H25) / H20;											   // J24=$J$21*H25/($J$22-$J$23*H25)/$H$20
	M27 = (16384 * J24) + L27;															   // PHADJ_0
	J26 = sqrt(1 + H20 * (M27 / 16384) * (2 + H20 * (M27 / 16384) - 2 * H21 * H22) / J21); // J26= SQRT(1 + $H$20*(M27/2^14)*(2+$H$20*(M27/2^14)-2*$H$21*$H$22)/$J$21)
	J25 = sqrt(1 + H20 * (L27 / 16384) * (2 + H20 * (L27 / 16384) - 2 * H21 * H22) / J21); // J25=SQRT(1+$H$20*(L27/2^14)*(2+$H$20*(L27/2^14)-2*$H$21*$H$22)/$J$21)
	H26 = (UPFErrorAtShunt + 1) / ((E27 + 1) * cos(atan(H25))) * (J26 / J25);			   // H26=(E25+1)/(H24*COS(ATAN(H25)))*(J26/J25)

	ce_data.cal_i0 = (float)16384 / H26;
	ce_data.phadj_0 = M27;
}

void ReadctKW(void)
{
	NeutralPowerat0 = (inst_kw_n);
	UPFErrorAtCT = ((float)(NeutralPowerat0 - (REF_VOLTAGE * CURRENT_FOR_CALIB)) / (REF_VOLTAGE * CURRENT_FOR_CALIB));
}

void CalibrateCT(void)
{
	float E33, H20, H21, H22, H23, H32, H33, H34, L35, J21, J22, J23, J32, J33, J34, M35;

	NeutralPowerat60 = (inst_kw_n);
	LAGErrorAtCT = (((float)((NeutralPowerat60 - POWER_FOR_CALIB))) / POWER_FOR_CALIB);

	E33 = UPFErrorAtCT; // Energy reading at 0�
	// H32=E35+1;
	H32 = 0 + 1; // E25=>Voltage error at 0�..consider this as 0 % error as already calibrated in Shunt
	// H33=(E34-E33)/((E33+1)*SQRT(3))
	H33 = (LAGErrorAtCT - UPFErrorAtCT) / ((UPFErrorAtCT + 1) * sqrt(3)); // E34=LAGErrorAtCT //Energy reading at +60�
	H20 = 0.015625;														  // 2^-6;
	H21 = 0.99804688;													  //(1-(2^-9));
	H22 = 0.99706007;													  // cos(2*3.14*50/4096);//H22=COS(2*PI()*D20/D22) D20==>AC Freq D22==>Sample freq
	L35 = 0;															  // PHADJ_1 default value
	// H23=SIN(2*PI()*D20/D22)
	H23 = sin(2 * 3.14 * 50 / 4096); // D20 Ac Freq D22=>Sampling Freq
	J21 = 1 + (H21 * H21) - (2 * H21 * H22);
	J22 = H21 * H23;
	J23 = 1 - H21 * H22;
	// J33=SQRT(1 + $H$20*(L35/2^14)*(2+$H$20*(L35/2^14)-2*$H$21*$H$22)/$J$21)
	J33 = sqrt(1 + H20 * (L35 / 16384) * (2 + H20 * (L35 / 16384) - 2 * H21 * H22) / J21);
	// J32=$J$21*H33/($J$22-$J$23*H33)/$H$20

	J32 = J21 * H33 / (J22 - J23 * H33);
	J32 = J32 / H20;
	M35 = (16384 * J32) + L35; // PHADJ_1

	// J34=SQRT(1 + $H$20*(M35/2^14)*(2+$H$20*(M35/2^14)-2*$H$21*$H$22)/$J$21)
	J34 = sqrt(1 + H20 * (M35 / 16384) * (2 + H20 * (M35 / 16384) - 2 * H21 * H22) / J21);

	H34 = (E33 + 1) / (H32 * cos(atan(H33))) * (J34 / J33);

	ce_data.cal_i1 = (16384 / H34);
	ce_data.phadj_1 = M35;

	///....Normalisation
	// ce_data.i1_scale = 16384 * IMAXB/(IMAXA/GAIN);
}

void ReadShuntKVA(void)
{
	PhasePowerat0 = inst_kva;																							// labs(inst_kw_p);
	UPFErrorAtShunt = ((float)(PhasePowerat0 - (REF_VOLTAGE * CURRENT_FOR_CALIB)) / (REF_VOLTAGE * CURRENT_FOR_CALIB)); //-0.1225
}

void ReadctKVA(void)
{
	NeutralPowerat0 = inst_kva;
	UPFErrorAtCT = ((float)(NeutralPowerat0 - (REF_VOLTAGE * CURRENT_FOR_CALIB)) / (REF_VOLTAGE * CURRENT_FOR_CALIB));
}

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
