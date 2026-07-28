/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Maps pins in the demo PCB for the SoC. 
 * This is a blue PCB.
 * It has an LCD with one line of digits.
 * Next to the Silergy Logo it says: DB_AM11_0-1.PcbDoc
 * It's second generation, following: EB_AM11_0-0.PcbDoc
 *
 * Copyright 2020, Silergy Corp.   All rights reserved.
 *
 * $Id: mapeb11_1.h 25014 2020-05-07 23:06:44Z rvandewa $
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
#if ASDAQ
#define PCB_24C256 1 // Atmel AT24LC256
#define PCB_EEPROM_I2C_ADDR 0xA0 // A2=0, A1=1, A0=0; (address in low nybble)
#define I2C_EE_RD_ADDR 0xA1
#define I2C_EE_WR_ADDR 0xA0

#define PCB_BUTTON_PUSHED true         // high = pushed
#endif
//#define PCB_24C512 1 // Atmel AT24LC512
#define PCB_24C1024 1 // Atmel AT24LC1024

// EEPROM Address mapping for Billing register, 
// Calibration and Error Data.
#define PCB_EEPROM_REGISTERS    0x0000UL    // Address of preserved energy data.
#define PCB_EEPROM_CAL_DATA     0x0600UL    // Address of calibration data.
#define PCB_EEPROM_ERR_DATA     0x0A00UL    // Address of error data in EEPROM.

// 2 Wire I2C EEPROM Read and Write adddress for 24LC256-E I2C EEPROM
//#define I2C_EE_RD_ADDR 0xA5
//#define I2C_EE_WR_ADDR 0xA4
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


// Define the commons.
#define COM(x) (1 << (8 + x))
enum COMMONS { COM0 = COM(0), COM1 = COM(1), COM2 = COM(2), 
               COM3 = COM(3), COM4 = COM(4), COM5 = COM(5) };

//#include "LCD_BQ14053.h" // LCD Panel BQ14053 is used on this PCB.

// Set-up values for LCD (derived by running the full LCD init code once.)
//#define LCD_MODE_DEF 0xE2000068
//#define LCD_MAPL_DEF 0xF8E79FFF
//#define LCD_MAPH_DEF 0x000000F7

// Used in meter\mmath.c; These depend on the number of digits
// of the LCD, which depends on the LCD, which is on the PCB.
#define PCB_WHPDISPMAX  1000000L    // Turn over display at this number.
#define PCB_WHPMAX      1000000000L // Turn over a pulse counter at this number.
#define PCB_WH_PLCDCNT  1.0         // 1.0 Wh per LCD count.
#define PCB_PULSE_IDX   0           // Use billable Wh


/***************************************************************************/
// I2C interface, i2c.c, i2c.h
// I2C uses two wired-or lines: SCL, the clock, and SDA, the data.
// An external resistor pulls these up when the pin is high-impedance.
// To drive a pin, 1 = unpowered = high, 2 = low = powered low.
// So, the pin's state is changed by the direction flip flop.
// So, the output flip-flop for the pin is set to zero, 
// to drive the pin low when drive is enabled.
// Since the software only drives pins low, contention is not possible.
// Note that the hardware master SPI and hardware I2C are actually the 
// same hardware, with two different drivers!
#define PCB_SDA_SEG i2c_sda_seg      // Use the SoC's segment.
#define PCB_SCL_SEG i2c_scl_seg      // Use the SoC's segment.

/***************************************************************************/
// Slave SPI interface, sspi.c, sspi.h
// The hardware slave SPI sometimes shares pins with other I/O. 
// So, if an SSPI dongle is attached, disable the use of the I/O.
// The test for this software is that start code pulls up the SSPI clock. 
// If it is sensed to be high, then the pin is attached to a high-impedance
// item such as the LCD. If low, then it is attached to an SPI dongle.
#define PCB_SSPI_DISABLED (LCD->DIO_b[sspi_sclk_seg].data)
//#define PCB_SSPI_DISABLED (0) // If no conflict with other I/O use this.

/***************************************************************************/
// SPI interface, spi.c, spi.h
// The hardware master SPI uses any DIOs as the chip selects.
// The following DIOs are the default chip selects for the hardware 
// master SPIs on this PCB. Other DIOs can be assigned. If an SPI bus is 
// multiplexed, more DIOs will be needed.
// Note that the master SPI and I2C are actually the same hardware,
// with two different drivers!
#define spi_cs_seg 30


#endif  // MAPEB_H

/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

