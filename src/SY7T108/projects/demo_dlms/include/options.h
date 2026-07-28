/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Option.h -Firmware and HW Board specific options
 *
 * $Id: options.h 25648 2020-06-10 11:00:29Z phuddar $
 **************************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H 1

// Standard Library 
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bitdef.h"
#include "SY7_device.h"  // I/O addresses and symbols, from /drivers/include
#include "ce.h"          // Define the CE's structure.

// Device name.
#define _SoC_device_ "\r\nSY7T_M11"

// User Include Files

// _32K and SY7T_M11_1P2W_CT are defined in the build file to 
// select these options.
#if _32K       
#define _software_version_ "v3.0.2 CT 1P2W "    
#define EQUATION EQUATION0          // i.e. Wh = VA * IA, IB is neutral
#define PHASE_CNT 1                 // Single phase use afe_0.c or afe_1.c
#include "cem11a02f.h"              // LSBs for CE    
#include "mapeb11_1.h"              // pin descriptions for CT eval PCB

// SY7T_M11_1P2W_CT is defined in the build file to select these options.
#elif SY7T_M11_1P2W_CT              // 1P2W CT build
#define _software_version_ "v3.0.2 CT 1P2W "    
#define EQUATION EQUATION0          // i.e. Wh = VA * IA, IB is neutral
#define PHASE_CNT 1                 // Single phase use afe_0.c or afe_1.c
#include "cem11a02f.h"              // LSBs for CE    
#include "mapeb11_1.h"              // pin descriptions for CT eval PCB

// SY7T_M11_1P2W_SHUNT is defined in the build file to select these options.
#elif SY7T108_1P2W_SHUNT_CT        // 1P2W SHUNT-CT build
#define _software_version_ "v3.0.2 SHUNT-CT 1P2W "    
#define EQUATION EQUATION0          // i.e. Wh = VA * IA, IB is neutral
#define PHASE_CNT 1                 // Single phase use afe_0.c or afe_1.c
#include "cem11a02f.h"              // LSBs for CE    
//#include "mapeb11_1.h"              // pin descriptions for CT eval PCB
#include "mapdb_am11_0_1.h"              


// SY7T_M11_1P3W_CT is defined in the build file to select these options.
#elif SY7T_M11_1P3W_CT              // 1P3W CT build
#define _software_version_ "v3.0.2 CT 1P3W "
#define EQUATION EQUATION1          // i.e. Wh = 0.5*VA*(IA - IB)
#define PHASE_CNT 1                 // Single phase 1P3W use afe_1_phase.c
#include "ce211a02c.h"              // LSBs for CE    
#include "mapeb11_1.h"              // pin descriptions for CT eval PCB

// SY7T_M11_1P2W_RMT is defined in the build file to select these options.
#elif SY7T_M11_1P2W_SHUNT_RMT       // 1P2W Remote (SY7M017) build
#define _software_version_ "v3.0.2 RMT 1P2W "
#define EQUATION EQUATION0          // i.e. Wh = 0.5*VA*(IA - IB)
#define PHASE_CNT 1                 // Single phase 1P3W use afe_1_phase.c
#include "cem11a03.h"               // LSBs for CE    
#include "mapeb_am11_1.h"           // pin descriptions for RMT eval PCB
#else
#error No Build option set 
#endif 

#include "system_SY7T_M11.h"    // Initialization and clock speed symbols
#include "mmath.h"      // Meter math: lmin, lmax, labs, lroundf, add(), etc.
#include "crc.h"        // used by error's data structures.
#include "log.h"        // Logging interface
#include "rtc.h"        // real time clock interface
#include "ce.h"         // CE RAM and variable mapping 
#include "cal.h"        // calibration API and data structures
#include "reg.h"        // billing register API and data structures
#include "meter.h"      // Meter API and data structures.
#include "autocal.h"    // Autocalibration
#include "main.h"       // Start up and miscellaneous functions.
// Define global data structures, using the module structures.
#include "version.h"    // Separate version, options should not change often.
#include "globals.h"    // Global structure definition and data.

// Global default CE in main.c
extern const ce_t *ce_ptr;

// Symbols beginning PCB_* are from the PCB's map.h file.
#define MAX_DEMAND ((float)PHASE_CNT)*(((float)PCB_V_MAX*(float)PCB_I_MAXA)/100.0)

#define MAX_OPERATING_SECS 360000000L // turn over hour counters at 100,000 hrs

// Begin options for command line interpreter. Not needed in every meter.
#if _32K
#define HELP 0                  // 32K version has no room for help.
#elif __ARMCC_VERSION           // Keil C has room for the help.
#define HELP 1                 // 0=remove all online help text to save space
#elif __GNUC__                  // GNU C has less room
#define HELP 0                  // 0=remove all online help text to save space
#else
#define HELP 0
#warning Unknown compiler type.
#endif
// End the options for the command line interpreter.

#define AUTOCAL 1               // 1 = Meter autocalibrates.

// Optional EEPROM addresses for billing registers, calibration and log data.
#define EEPROM_REGISTERS    0x0000UL    // Address of preserved energy data.
#define EEPROM_CAL_DATA     0x0600UL    // Address of calibration data.

// Flash page size.
#define FPAGE_SIZE 512
// Optional flash addresses for billing registers, calibration and error data.
#define FLASH_CAL_DATA (FLASH_SIZE-FPAGE_SIZE)  // Start of cal data
// Number of pages for billing data
#define FLASH_BILLING_PAGE_CNT 3
// Start of billing data
#define FLASH_BILLING_DATA \
    (FLASH_CAL_DATA-(FLASH_BILLING_PAGE_CNT*FPAGE_SIZE))


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif /* OPTIONS_H */
