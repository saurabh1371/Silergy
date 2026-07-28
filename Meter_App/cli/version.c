///***************************************************************************
// * This code and information is provided "as is" without warranty of any
// * kind, either expressed or implied, including but not limited to the
// * implied warranties of merchantability and/or fitness for a particular
// * purpose.
// *
// * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
// *
// * FILE: version.c
// *
// * DESCRIPTION: Meter software version number file.
// * What it means: <num1>.<num2>.<num3>, e.g. 8.9a
// * <num1> changes for total redesigns.
// * <num2> changes for each formal release, new feature or a
// * specification change.
// * <num3> if present, distinguishes minor versions with fixed defects.
// *
// * $Id: version.c 25648 2020-06-10 11:00:29Z phuddar $
// ***************************************************************************/

//#include "options.h"

//// Common Driver Version info
//#include "bat.h"       // Battery driver
//#include "ce.h"        // CE driver
//#include "delay.h"     // Delay driver
//#include "dtimer.h"    // Dual timer driver
//#include "eeprom.h"    // EEPROM driver
//#include "external_int.h" // External Interrupt driver
//#include "flash.h"     // FLash driver
//#include "gpio.h"      // GPIO driver
//#include "i2c_master.h" // I2C driver
//#include "info_block.h" // Trim access
//#include "lcd.h"       // LCD driver
//#include "nvram.h"     // NVRAM driver
//#include "opt.h"       // Optical serial driver
//#include "pwm0.h"      // PWM0 driver
//#include "pwm1.h"      // PWM1 driver
//#include "pwrmode.h"   // Power mode driver
//#include "rtc.h"       // RTC driver
//#include "ser_common.h" // Definitions shared by ser0..2.c
//#include "ser0.h"      // Serial 0 driver
//#include "ser1.h"      // Serial 1 driver
//#include "ser2.h"      // Serial 2 driver
////#include "spi.h"       // SPI master driver (not currently used)
//#include "sspi.h"      // SPI slave driver
//#include "tmp.h"       // Temperature driver
//#include "timer0.h"    // Timer0 Auto-reload driver
//#include "timer1.h"    // Timer1 Auto-reload driver
//#include "tmux.h"      // Test mux driver
//#include "wd.h"        // Watchdog driver

//// SoC specific Driver Version info
//#include "autocal.h"   // autocal

//// SDK Library
//#include "afe.h"
//#include "cal.h"
//#include "cli.h"        // Needed by cli_buf.h, sercn.h
//#include "cli_buf.h"
//#include "crc.h"
//#include "dst.h"
//#include "hash_str.h"
//#include "log.h"
//#include "meter.h"
//#include "mmath.h"
//#include "reg.h"
//#include "serc0.h"
//#include "serc1.h"
//#include "serc2.h"
//#include "sspi0.h"
//#include "stm.h"
//#include "tamper.h"
//#include "time.h"

//const char sdk_library_str[] = {"\r\nSDK Library code:\r\n"};
//const char cmn_drivers_str[] = {"\r\nCommon Drivers:\r\n"};
//const char specific_drivers_str[] = {"SY7T_M11 Drivers:\r\n"};
//const char ver_help_str[] = {"Try v1, v2, v3\r\n"};
//static const char crlf_str[] = {"\r\n"};
//const char * const fw_ver_common[] =
//{
////    // Common Driver Version info
////    cmn_drivers_str,
////    fw_ver_bat,         // bat.h
////    fw_ver_ce,          // ce.h
////    fw_ver_delay,       // delay.h
////    fw_ver_dtimer0,     // dtimer.h
////    fw_ver_dtimer1,     // dtimer.h
////    fw_ver_eeprom,      // eeprom.h
////    fw_ver_external_int,// ext.h
////    fw_ver_flash,       // flash.h
////    fw_ver_gpio,        // gpio.h
////    fw_ver_i2c_master,  // i2c_master.h
////    fw_ver_info_block,  // info_block.h
////    fw_ver_lcd,         // lcd.h
////    fw_ver_nvram,       // nvram.h
////    fw_ver_opt,         // opt.h
////    fw_ver_pwm0,        // pwm0.h
////    fw_ver_pwm1,        // pwm1.h
////    fw_ver_pwrmode,     // pwrmode.h
////    fw_ver_rtc,         // rtc.h
////    fw_ver_ser0,        // ser0.h
////    fw_ver_ser1,        // ser1.h
////    fw_ver_ser2,        // ser2.h
////    // fw_ver_spi,         // spi.h (master SPI) (not currently used)
////    fw_ver_sspi,        // sspi.h (slave SPI)
////    fw_ver_tmp,         // tmp.h
////    fw_ver_timer0,      // timer0.h
////    fw_ver_timer1,      // timer1.h
////    fw_ver_tmux,        // tmux.h
////    fw_ver_wd,          // wd.h
////    ver_help_str,
////    crlf_str, NULL
//};

//const char * const fw_ver_specific[] =
//{
////    // SoC specific Driver Version info
////    specific_drivers_str,
////    fw_ver_autocal,
////    fw_ver_ce_code,
////    ver_help_str,
////    crlf_str, NULL
//};

//const char * const fw_ver_lib[] =
//{
////    // Library versions
////    sdk_library_str,
////    fw_ver_afe,         // afe.h
////    fw_ver_cal,         // cal.h
////    fw_ver_cli,         // cli_buf.h
////    fw_ver_crc,         // crc.h
////    fw_ver_dst,         // dst.h
////    fw_ver_hash_str,    // hash_str.h
////    fw_ver_log,         // log.h
////    fw_ver_meter,       // meter.h
////    fw_ver_mmath,       // mmath.h
////    fw_ver_reg,         // reg.h
////    fw_ver_serc0,       // serc0.h
////    //fw_ver_serc1,       // serc1.h
////    //fw_ver_serc2,       // serc2.h
////    fw_ver_sspi0,       // sspi0.h
////    fw_ver_stm,         // stm.h
////    fw_ver_tamper,      // tamper.h
////    fw_ver_time,        // time.h
////    ver_help_str,
////    crlf_str, NULL
//};


//// Firmware version and copyright text.
//const char copyrightA[] = _SoC_device_; // defined in options.h
//const char copyrightB[] = " (c) Silergy Corp. ";
//const char software_version[] =
//_software_version_ // defined in options.h
//" "
//__TIME__
//" "
//__DATE__
//"\r\n"
//;

///***************************************************************************
// * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
// * this program is fully protected by the United States copyright
// * laws and is the property of Silergy Corp.
// ***************************************************************************/

