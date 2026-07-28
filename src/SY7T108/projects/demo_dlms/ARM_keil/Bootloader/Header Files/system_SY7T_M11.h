/**************************************************************************//**
 * @file     system_SY7T_M11.h
 * @brief    CMSIS Cortex-M# Device Peripheral Access Layer Header File for
 *           Device SY7T_M11_H
 * @version  V5.00
 * @date     02. March 2016
 * $Id: system_SY7T_M11.h 25146 2020-05-12 07:00:17Z rvandewa $
 ******************************************************************************/
/*
 * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SYSTEM_SY7T_M11_H   
#define SYSTEM_SY7T_M11_H

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
// This is the default clock rate after reset.
#define SYSTEM_CLOCK_F   (float)10223616.0  /* 10.2 Mhz */
#define XTAL_CLOCK      	(32768U)      /* Oscillator frequency */
#define SYSTEM_CLOCK    	(10223616U)   /* System clock = 10MHz */
#define PLL_CLOCK       	(20447232U)   /* System clock = 20MHz */

// Minimum logic voltage.
#define MIN_LOGIC_VOLTS (2.0) // C

// FLASH SIZE constants are in SoC\Drivers\Include\system_SoC.h
#define FLASH_SIZE (0x20000)        // SY7T_M11 is 128K

// Start of RAM
#define RAM_LOCATOR 0x20000000
#define RAM_LENGTH  0x2000         // 8K

// Size and last cell of NVRAM.
#define NVRAM_SIZE 128    // In bytes
#define NVRAM_LAST_CELL (NVRAM_SIZE/4 - 1)

// LCD RAM
#define LCD_RAM_OFFSET (0x20000000) // Offset of LCD RAM
#define LCD_IMAGE_SIZE (9*4)        // Bytes in an LCD image in RAM
#define LCD_IMAGE_CNT_MAX 15        // Max. number of LCD images.


// Interrupt priorities for SY7T_M11
// The SY7T_M11 has two interrupt bits, thus four levels of priority.
// The NVIC's set-priority function automatically shifts them as needed.
#define HIGHEST_PRIORITY 0
#define HIGH_PRIORITY 1
#define LOW_PRIORITY 2
#define LOWEST_PRIORITY 3

// Last IRQ number in NVIC
#define LAST_IRQ    SSPI_IRQn
	

// Field and mask definitions for portable drivers

// Default temperature measurement period to compensate RTC.
#define TEMP_PER_DEFAULT 0x08   // 2/minute

//Computes VBAT
#define BSENSE_LSB (1.0/42.7)
#define COMPUTE_VBAT(_x) (_x * BSENSE_LSB) 

// Calibration temperature when IC is built.
#define ATE_CAL_TEMP (22.0) // C

// Unlock code for all the shadow/battery-backed registers and RTC
#define SHADOW_UNLOCK (0xA5)

// Computes temperature from STEMP.
#define COMPUTE_TEMP(_x) \
    (21.42 + ((float)_x) * (8.0658e-2F - 1.7578e-6F * ((float)_x)))

// Compute floating point STEMP from temperature; Factored
#define COMPUTE_STEMP(_x) \
    ((_x - 22.0F) * (12.404F + ((_x - 22.00F) * -3.3589e-3F)))

// This is the number of pins with DIO[] entries.
#define LCD_PIN_CNT 45
#define LAST_DIO (LCD_PIN_CNT-1) // Last DIO number

// gpio/seg numbers of alternate device pins on the SY7T_M11.
// These exist so common drivers can operate devices with different pinouts.
// An emumeration is not possible, because there are duplicates.
// Debug reset
#define dbg_rstb_seg 41 // SWD/debug reset segment
// I2C, Master
#define i2c_sda_seg 19
#define i2c_scl_seg 20
// LCD commons
#define com0_seg 36
#define com1_seg 37
#define com2_seg 38
#define com3_seg 39
#define com4_seg 15
#define com5_seg 16
#define com6_seg 17
#define com7_seg 18
// Pulses
#define wpulse_seg 42
#define vpulse_seg 41
#define ypulse_seg 27
#define xpulse_seg 35
// Pulse width modulation
#define pwm1_seg 31
#define pwm0_seg 32
// SPI, Master
// spi_cs_seg, please see the PCB file. Any DIO can be used.
#define spi_mosi_seg 19
#define spi_miso_seg 21
#define spi_scl_seg 20
// SPI, Slave
#define sspi_csb_seg 9
#define sspi_sclk_seg 10
#define sspi_cdin_seg 11
#define sspi_cdout_seg 12
// Test muxes
#define tmux1_seg 35
#define tmux0_seg 27
// UARTs
#define tx0_seg 22
#define rx0_seg 23
#define tx1_seg 31
#define rx1_seg 32
#define tx2_seg 14
#define rx2_seg 13
#define opt_tx_seg 14
#define opt_rx_seg 13
// Wake pins
#define pb_seg 26
#define wake0_seg 25
#define wake1_seg 24

// Macro is nonzero if vpulse interferes with debugger.
#define DEBUGGER_PRESENT (1 == SV->VERSION_b.test) // 1 = TEST_E pin is high
//#define VPULSE_OK 1   // On SoCs with no conflict.
#define VPULSE_OK (0 == SV->VERSION_b.test)

// Wake masks
#define EW_PB  BIT19    // pin PB
#define EW_WK0 BIT20    // pin WAKE0
#define EW_WK1 BIT21    // pin WAKE1
#define EW_RX0 BIT22    // pin RX0
#define EW_TMR BIT23    // Timer
#define EW_TMP BIT24    // Temperature range
#define EW_VLCT BIT26   // Wake on VLCT

// Wake from bits
#define WF_BADVDD_MASK   0x00000100
#define WF_OSCFAIL_MASK  0x00000200
#define WF_WDOF_MASK     0x00000800
#define WF_RSTBIT_MASK   0x00001000
#define WF_RST_MASK      0x00002000
#define WF_CSTART_MASK   0x00004000
#define WF_PB0_MASK      0x00080000
#define WF_WK0_MASK      0x00100000
#define WF_WK1_MASK      0x00200000
#define WF_RX0_MASK      0x00400000
#define WF_TMR_MASK      0x00800000
#define WF_TMP_MASK      0x01000000
#define WF_VSYS_MASK     0x02000000
#define WF_VLCT_MASK     0x04000000


// Millisecs/ Microsecs to tick count
#define MS2TICKS(ms) (SystemCoreClock * ms / 1000)
#define US2TICKS(us) (SystemCoreClock * us / 1000000)

// Test multiplexer signals vary on different models of SoC.
// Test multiplexer 0 signals (TMUX0 pin)
typedef enum  
{
    pg_xpulse       = 0x00,     // default is XPULSE output
    rtm_dat         = 0x01,     // real-time monitor, serial CE test data
    rtc_sec_pulse   = 0x08,
    rtc_qsec_pulse  = 0x09,
    int_vstat       = 0x11,     // Voltage state change interrupt
    int_ext         = 0x12,     // External interrupt
    int_ce          = 0x13,     // CE interrupt
    int_fuse        = 0x1d,     // Fuse parity failure interrupt
    int_rtc         = 0x1e,     // RTC interrupt
    int_ce_busy     = 0x26,     // CE busy signal
    int_xfer_n      = 0x27,     // CE xfer interrupt
    tmux0_low       = 0x40,     // TMUX0 idle, low-power state
    pll_ok          = 0x87,     // Indicates that the PLL has started
    osc_fail        = 0x88,     // Indicates that the crystal oscillator failed
    vref_out        = 0x103     // Voltage reference (enabling can harm accuracy)
}e_tmux0_signal;

// Test multiplexer 1 signals (TMUX1 pin)
typedef enum  
{
    pg_ypulse       = 0x00,     // Default is y pulse output (CE's sag int.)
    mux_sync        = 0x03,     // AFE multiplexer sync, 1/sample
    clk_rtm         = 0x04,     // CE real-time monitor data clock.
    clk_ce          = 0x07,     // CE clock
    clk_mpu         = 0x09,     // MPU clock
    tmux1_low       = 0x40,     // TMUX1 idle, low-power state
    bad_vdd         = 0x91,     // Digital power (Vdd) is out of regulation
    pulse_sec       = 0x92,
    pulse_qsec      = 0x93,
    osc32_x32       = 0x118
}e_tmux1_signal;


#ifdef __cplusplus
extern "C" {
#endif

/*!< System Clock Frequency (Core Clock) */
extern uint32_t SystemCoreClock;
/*!< Divisor from maximum Clock freq. */
extern uint32_t SystemCoreClockDivisor;


/**
  \brief Set up the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
   Initializing the system disables the system interrupts, except fuse fail.
   A default fuse interrupt is enabled when SystemInit() is called.
 */
extern void SystemInit (void);


/**
  \brief  Update SystemCoreClock variable.

   Updates the SystemCoreClock with current core Clock retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

/**
  \brief  Update mpu_spd and VLS setting

   Updates Updates the MPU clock, VLS settings then updates SystemCoreClock.
 */
void sys_set_mpuclk(int32_t mpuspd, bool vls);
	
/**
  \brief  Wait for shadow registers to unlock.

   Waits until the shadow registers are unlocked.
   Return nonzero if it times out.
 */
extern int32_t sys_shadow_busy_wait(void);


/**
  \brief  Wait for temperature measurement to complete.

   Waits until the temperature measurement is done.
   Return nonzero if it times out.
 */
extern int32_t sys_tmp_busy_wait(void);


typedef enum {
    RTC_ALARM_INT=1,    // RTC alarm interrupt bit
    TEMP_RANGE_INT=2,   // Temperature out of range interrupt bit
    VSTAT_INT=4,        // DC power voltage status change interrupt bit
        // Fuses record factory adjustments to the IC; A failure is
        // very serious and can be caused if EMI interferes with a fuse-read. 
        // SystemInit() sets up a default fuse fail interrupt.
    FUSE_FAIL_INT=8,    // Fuse parity failure detection interrupt bit
    CRYSTAL_FAIL_INT=0x10   // Crystal oscillator failure detection bit
} Sys_int_t;


/**
  \brief Enable system interrupts with C routine callbacks.

  int_bit is the interrupt bit defined above.
  fn_ptr is the callback routine, called when the interrupt occurs.
  Enables interrupts for RTC alarm, voltage status change, temperature range,
  crystal failure and fuse failure. Passing NULL disables.
  Note that the system interrupts are a mix of level and edge-sensitive
  interrupts: Level: Alarm, Temp range, Fuse-fail, Crystal_fail.
  Edge: Vstat (voltage status change)
 */
extern void sys_int_enable(Sys_int_t int_bit, void (*fn_ptr)(void) );


/**
  \brief Disable system interrupts.

  int_bit is the interrupt bit defined above.
  Note that the fuse-fail's default interrupt behavior cannot be disabled.
 */
extern void sys_int_disable(Sys_int_t int_bit);


/**
  \brief System interrupt handler.

  Handles interrupts for RTC alarm, voltage status change, temperature range,
  crystal failure and fuse failure.
 */
extern void SYS_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_SY7T_M11_H */
