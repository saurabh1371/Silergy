/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Maps pins in the evaluation PCB for the SoC. 
 *
 * Copyright 2019, Silergy Corp.   All rights reserved.
 *
 * $Id: mapeb11_1.h 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
#ifndef MAP_H
#define MAP_H 1

// These depend on whether the PCB has the battery.
// If there is no VBAT, the meter should not enter brownout mode.
// It also should not utilize the RTC or NVRAM.
#define PCB_HAS_VBAT 1      // 1 = battery exists, 0 = No battery.
#define PCB_HAS_VBAT_RTC 1  // 1 = RTC battery exists, 0 = No battery.

// MPU clock speed set-up, used in system_<SoC name>.c
// The setting of VLS depends on the sample speed of the ADC, which in turn
// depends on the choice of CE and sensor configuration, which depends on the
// analog metering input design of the PCB electronics.
#define PCB_ADC_VLS_MISSION 0   // Fastest PLL mode
// The MPU speed depends on the setting of VLS.
#define PCB_MPU_SPD_MISSION 4   // Fastest MPU divider, 1/48 maximum

#if PCB_HAS_VBAT // Battery modes exist?
// MPU clock speed used in brownout.c
#define PCB_ADC_VLS_BROWNOUT 0 
#define PCB_MPU_SPD_BROWNOUT 4
#endif

// Code exists for all of these serial ports, but they can interfere with 
// the LCD on this demo PCB, so they are disabled.
#define PCB_SER_0     1         // Serial 0 is used on this PCB
//#define PCB_SER_1     1         // Serial 1 is used on this PCB
//#define PCB_SER_2     1         // Serial 2 is used on this PCB
//#define PCB_SSPI      1         // Slave SPI is used on this PCB

// These items depend on the choice of EEPROM, which is
// part of the design of the PCB.
//#define PCB_24C256 1 // Atmel AT24LC256
//#define PCB_24C512 1 // Atmel AT24LC612
#define PCB_24C1024 1 // Atmel AT24LC1024

#define PCB_EEPROM_I2C_ADDR 0xA4 // A2=0, A1=1, A0=0; (address in low nybble)
// EEPROM Address mapping for Billing register, 
// Calibration and Error Data.
#define PCB_EEPROM_REGISTERS    0x0000UL    // Address of preserved energy data.
#define PCB_EEPROM_CAL_DATA     0x0600UL    // Address of calibration data.
#define PCB_EEPROM_ERR_DATA     0x0A00UL    // Address of error data in EEPROM.

// 2 Wire I2C EEPROM Read and Write adddress for 24LC256-E I2C EEPROM
#define I2C_EE_RD_ADDR 0xA5
#define I2C_EE_WR_ADDR 0xA4
#define I2C_EE_WAIT    0x2000

// NVRAM Addresses
#define PCB_NVRAM_ADR_NVM 0     // Address of structure nvm in NVRAM.

// These define the pins of the LCD on a PCB.
// The pins are the pins of the LCD.  The numbers are the segment
// numbers of the IC. In this way, a piece of LCD glass can be
// ported to a different PCB by changing the segment numbers.

#define ncPin (LCD_PIN_CNT+1) // for a pin that is not connected.

#define PIN01 27 //
// not connected on glass
#define PIN02 ncPIN
#define PIN03 ncPIN
#define PIN04 28 //
#define PIN05 29 //
#define PIN06 30 //
#define PIN07 34 // 
#define PIN08 1 //
#define PIN09 3 //
#define PIN10 5 //
#define PIN11 7 //
#define PIN12 9 //
#define PIN13 11 //
#define PIN14 17 //
#define PIN15 21 //
// not connected on glass or coms
#define PIN16 ncPin
#define PIN17 ncPin
#define PIN18 ncPin
// End of pins on one side of the LCD glass
// Start of pins on the other side of the LCD glass
#define PIN19 ncPin
#define PIN20 ncPin
#define PIN21 ncPin
#define PIN22 18 //
#define PIN23 12 //
#define PIN24 10 //
#define PIN25 8 //
#define PIN26 6 //
#define PIN27 4 //
#define PIN28 2 //
#define PIN29 0 //
#define PIN30 33 //
#define PIN31 23 //
#define PIN32 22 //
#define PIN33 32 //
// not connected on glass
#define PIN34 ncPin
#define PIN35 ncPin
#define PIN36 31 //
// End of pins on the other side of the LCD glass

// Demo board DIO_40 -> VCC_EEPROM
#define VCC_EEPROM 40 // DIO40 

// Define the commons.
#define COM(x) (1 << (8 + x))
enum COMMONS { COM0 = COM(0), COM1 = COM(1), COM2 = COM(2), 
               COM3 = COM(3), COM4 = COM(4), COM5 = COM(5) };

#include "LCD_BQ14053.h" // LCD Panel BQ14053 is used on this PCB.

// Set-up values for LCD (derived by running the full LCD init code once.)
#define LCD_MODE_DEF 0xE2000068
#define LCD_MAPL_DEF 0xF8E79FFF
#define LCD_MAPH_DEF 0x000000F7

// Used in meter\mmath.c; These depend on the number of digits
// of the LCD, which depends on the LCD, which is on the PCB.
#define PCB_WHPDISPMAX  1000000L    // Turn over display at this number.
#define PCB_WHPMAX      1000000000L // Turn over a pulse counter at this number.
#define PCB_WH_PLCDCNT  1.0         // 1.0 Wh per LCD count.
#define PCB_PULSE_IDX   0           // Use billable Wh

// Current sensors used with this PCB
// These depend on the electronics of the PCB.
// These set defaults for the calibration values.
#if SY7T_M11_1P2W_CT        // equation 0, 1p2w, Wh = VA * IA
#define PCB_I_MAXA     867  // 86.7A CT 2500 turns, 5.1 Ohm burden
#define PCB_I_MAXB (PCB_I_MAXA) // Neutral sensor is the same
#define PCB_I_MIN       20  // milliamps
#define PCB_I_LIMIT    975  // I_MAXA * .95, 0.1A

#elif SY7T_M11_1P2W_SHUNT_CT // equation 0, 1p2w, Wh = VA * IA
#define PCB_I_MAXA     884   // 88.4 250uA, 8x gain
#define PCB_I_MAXB     867   // 86.7A CT 2500 turns, 5.1 Ohm burden
#define PCB_I_MIN      160   // milliamps
#define PCB_I_LIMIT    875   // 0.1A

#elif SY7T_M11_1P3W_CT      // Equation 1, 1p3w, Wh = 0.5*VA*(IA - IB)
#define PCB_I_MAXA     867  // 86.7A CT 2500 turns, 5.1 Ohm burden
#define PCB_I_MAXB (I_MAXA) // Neutral sensor is the same
#define PCB_I_MIN       20  // milliamps
#define PCB_I_LIMIT    975  // I_MAXA * .95, 0.1A

#else 
#error No symbol selects the sensors. Define it in the build.
#endif

// Voltage sensors depend on the electronics of the PCB.
#define PCB_V_MAX     48470 // 4847.0 V 
#define PCB_V_MIN     1000  // 100V
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
#define PCB_BUTTON_PUSHED false         // low = pushed
#define PCB_BUTTON_EW EW_PB             // Enable wake bit mask.
#define PCB_BUTTON_WS WS_PB             // Wake reason bit mask.

// Scroll button's locations
//#define PCB_SCROLL_SEG  3       // Port's bit number for scroll button
//#define PCB_SCROLL_PUSHED false         // low = pushed

// Case tamper-detect's locations. (sw3, low=off)
#define PCB_CASE_TAMPER_SEG  24         // Port's LCD-segment number
#define PCB_CASE_TAMPER_INT PCB_CASE_TAMPER_SEG // External interrupt number.
#define PCB_CASE_TAMPER 1               // high = tamper
#define PCB_CASE_TAMPER_EW EW_WK1       // Enable wake bit mask.
#define PCB_CASE_TAMPER_WF EW_WK1       // Wake reason bit mask.

// Tamper 2, usually for open terminal block. (sw4, low=off)
#define PCB_TERM_TAMPER_SEG  25         // Port's LCD-segment number
#define PCB_TERM_TAMPER_INT PCB_TERM_TAMPER_SEG // External interrupt number.
#define PCB_TERM_TAMPER 1               // high = tamper
#define PCB_TERM_TAMPER_EW EW_WK0       // Enable wake bit mask.
#define PCB_TERM_TAMPER_WF EW_WK0       // Wake reason bit mask.

/***************************************************************************/
// I2C interface, i2c.c, i2c.h
// I2C uses two wired-or lines: SCL, the clock, and SDA, the data.
// An external resistor pulls these up when the pin is high-impedance.
// To drive a pin, 1 = unpowered = high, 2 = low = powered low.
// So, the pin's state is changed by the direction flip flop.
// So, the output flip-flop for the pin is set to zero, 
// to drive the pin low when drive is enabled.
// Since the software only drives pins low, contention is not possible.
#define PCB_SDA_SEG i2c_sda_seg      // Use the SoC's segment.
#define PCB_SCL_SEG i2c_scl_seg      // Use the SoC's segment.

#endif  // MAPEB_H

/***************************************************************************
 * Copyright (C) 2019 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

