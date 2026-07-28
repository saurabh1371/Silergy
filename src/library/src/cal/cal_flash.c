/*****************************************************************************
 * DESCRIPTION: Meter run logic, the main loop.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2012...2013, Silergy Corp. All rights reserved.
 *
 * $Id: cal_flash.c 21397 2019-06-15 02:52:26Z rvandewa $
 ****************************************************************************/
#include <string.h>
#include <string.h>     // C library
#include "options.h"    // Define the IC, CE code and other options.
#include "crc.h"        // Checksum/CRC of data structures.
#include "afe.h"        // To read the AFE.
#include "flash.h"      // Flash writes and erases.
#include "hash_str.h"	// Makes a number from a string.
#include "info_block.h" // Info block has factory calibration data.
#include "version.h"    // Software version strings.
#include "ce.h"         // CE access routines.
#include "cal.h"        // Calibration


/*** Version ****/
const char fw_ver_cal[] =
"cal_flash" /* Library file name */
" v"     /* ver string */
"1.01"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


// Default calibration values.
#define CAL_DEFAULT 1
static const Cal_t cal_default =
{
    { // AFE DSP initialization, 0x30.. read from CE  // 0x22 34
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0
    }, // AFE values
    
    // Symbols beignning with PCB_* are from the PCB map.h file.
    PCB_I_MIN,          // )0, Starting current, 1/2000 of maximum, 0 = disable
    0,                  // )1, Configuration bits. (do nothing)
    PCB_V_MIN,          // )2, Min. permitted voltage (usually 80V)
    PCB_I_MAXA,         // )3, maximum current.
    PCB_V_MAX,          // )4, maximum voltage.
    PCB_WH_WHPP,        // )5, Wh of Wh per pulse, usually 1
    PCB_P_WHPP,         // )6, Pulses of Wh per pulse, usually 1
    PCB_I_LIMIT,        // )7, max. permitted current, LSB = 0.1A
    PCB_V_LIMIT,        // )8, Max permitted voltage, LSB = 0.1V
    PCB_I_MAXB,         // )9, Imax neutral.
    2,                  // )A, acc. intervals of autocalibration measurement.
    PCB_V_NOM,          // )B, 0.1 Volts rms of autocalibration.
    50,                 // )C, 0.1 Amp rms of autocalibration.
    45,                 // )D, default autocalibration voltage angle, degrees
    3,                  // )E, lcd_idx, Selects default LCD display. (Wh)
    0L,                 // )F, lcd_bit, Each bit enables a timed LCD display item.
    0x53593700L,        // )10, 3-character manufacturer ID string. (SY7)
    100000L,            // )11, Meter ID number, (Serial #100,000)
    -480,               // )12, GMT Offset for Los Angeles in signed minutes. PST = GMT-8hrs
    // Nominal RTC xtal: T0=25C, +3.00ppm drift, alpha=0.035ppm/c^2
    0x00000A2D,         // )13, Register image of TCAB (RTC adjustment)
    0xff49000D,         // )14, Register image of TCCD (RTC adjustment),
    PCB_PULSE_IDX,      // )1a, pulse src 0x84,WSUM_X for 3-phase, 0x85 for 1p
    2,                  // )1b, Minutes of the demand interval.
    0,                  // )15, preamp (1 = on); takes default from CE init.
    PCB_V_NOM,          // )1e, nominal voltage (from cexxx.h)
    0,                  // )1d, WSUM_PER_CNT
    {0, 0, 0},          // ppmc
    {0, 0, 0},          // ppmc2
    is_default,         // )16, calibration source
    0,                  // )17, Count of calibrations, >255=255
    0,                  // )18, hash of version string. (uninitialized)
    {0, 0}
};
#ifndef CAL_DEFAULT
#error No default values for meter!
#endif


/***************************************************************************
* Description:
*         Install a default calibration.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
***************************************************************************/
void cal_reset(void)
{
    uint8_t cal_cnt_tmp;

    // Save the calibration count.
    cal_cnt_tmp = global.cal.cal_cnt;

    // Reset the MPU's calibration to default.
    global.cal = cal_default;

    // Set the CE data from the CE's flash.
    // Calculate CE defaults (WRATE, THR, etc.)
    // Read it back into the array in cal_data.
    afe_get_cal_default(&global.cal);
    // Single-phase meters can record and replay the preamp settings.
    #if EQUATION==EQUATION0
    // This line assumes that the AFE is always initialized before the
    // calibration is gotten.
    global.cal.pre_en = ADC->VREF_CNTL_b.pre_en;
    #endif

    // Restore the calibration count.
    global.cal.cal_cnt = (uint8_t)cal_cnt_tmp;

    // Get the RTC's calibration to save it.
    rtc_default_trim();  // Default trim and RTCCAL value.

    // Make the checksum invalid.
    global.cal.data_ok_cal[0] = 0xff;

    // make the hash invalid.
    global.cal.ver_hash = (unsigned char)-1;
}



/***************************************************************************
* Description:
*         Get the configuration data from eeprom.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         uint8_t:  0 for success
*                   1 for failure
****************************************************************************/
int cal_get(void)
{
    int cal_bad = true;
    int my_ver_hash;

    // Unique value for this version is made from the date and time.
    my_ver_hash = hash_code_str (software_version);

    // Are the RAM copy's software version and checksums OK?
    if ( (global.cal.ver_hash == my_ver_hash)
         && (crc16 ((uint8_t *) &global.cal, sizeof (Cal_t), false))
       )
    {   // RAM copy is OK.
        cal_bad = false;
    }
    else
    {
        // Get the configuration from flash.
        memcpy((uint8_t *)&global.cal, (const void *)FLASH_CAL_DATA, sizeof (Cal_t));

        // Are the EEPROM copy's software version and checksums OK?
        if ( (global.cal.ver_hash == my_ver_hash)
           && (crc16 ((uint8_t *) &global.cal, sizeof (Cal_t), false))
           )
        {
            cal_bad = false;
        }
    }

    if(cal_bad) // Cal bad?
    {
        // Yes, so get the default data.
        global.cal.cal_cnt = 0;
        cal_reset ();
    }

    /* move the best calibration into the CE */
    ce_put_cal(
        &global.cal.afe_cal_a[0], 
        sizeof(global.cal.afe_cal_a)/4);
    // Single-phase meters can record and replay the preamp settings.
    #if EQUATION==EQUATION0
	ADC->VREF_CNTL_b.pre_en = global.cal.pre_en;
    #endif

    /* Restore the RTC's calibration */
    rtc_trim_write (global.cal.tcab, global.cal.tccd); // Write to Trim Registers.

    /* If the calibration is no good, record it. */
    log_state ( EV_CAL_BAD , cal_bad );
    return cal_bad;
}


/***************************************************************************
* Description:
*         Put the configuration data into flash.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         0 : Success , Non zero =  Failure
***************************************************************************/
int cal_put (void)
{
    int error;

    // save the CE calibration.
    ce_get_cal(
        &global.cal.afe_cal_a[0], 
        sizeof(global.cal.afe_cal_a)/4);
    // Single-phase meters can record and replay the preamp settings.
    #if EQUATION==EQUATION0
	global.cal.pre_en = ADC->VREF_CNTL_b.pre_en;
    #endif

    // Read the RTC temp. Calibration and compensation  
    // registers (TCAB, TCCD, RTCCAL) to Calibration data.
    rtc_trim_read (&global.cal.tcab, &global.cal.tccd);

    // Mark the software version.
    // Unique value for this version is made from the date and time.
    global.cal.ver_hash = hash_code_str (software_version);

    // Mark that the data will be from the EEPROM.
    global.cal.cal_src = is_read;

    // Update the calibration count.
    if(global.cal.cal_cnt < 255)
        ++global.cal.cal_cnt;

    // Update the CRC. 
    crc16 ((uint8_t *) &global.cal, sizeof (Cal_t), true);

    // save the data.
    // Erase the flash page.
    error = flash_page_erase(FLASH_CAL_DATA);
    // Now copy data to flash.
    error |= flash_write(
        FLASH_CAL_DATA, 
        (uint32_t *)&global.cal, 
        (3 + sizeof (Cal_t)));

    // Damage the CRC, forcing next restart to read from EEPROM.
    global.cal.cal_src = is_user;

    return (int)(error);
}


/***************************************************************************
 * Copyright (C) 2011..2019 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

