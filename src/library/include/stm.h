/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2005 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: POWER METER - Soft Timer Definitions.
 * Many systems need more timers.  So, this code makes slow software timers.
 * To use this, define a uint16_t variable.  Pass the addresses to stm_start().
 * The timer code will remember the variable, and make it count down.
 *
 * HISTORY: $Id: stm.h 16397 2018-01-09 23:55:14Z rvandewa $
 ***************************************************************************/
#ifndef STM_H
#define STM_H 1

// Library version.
extern const char fw_ver_stm[];

// Many timers can be active at the same time.
#define STM_CNT 8                   // count of software timer structures.

// To change a timer's time, just write to the variable.

// Units, converts milliseconds, or seconds to the RTC subsecond units
#define STM_MS(_v_) (((_v_ * 64) + 124)/250)    
#define STM_SEC(_v_) (_v_ * 256)

// Initialize the software timer system.
// Must be called before any other call to stm.
void stm_init(void);

// Start a timer. Returns nonzero if it fails.
int stm_start (volatile int *cnt_ptr, int tck_cnt);
                                      
// Call in main loop to make timers run.
void stm_run(void);

//#define STM_WAIT
#ifdef STM_WAIT
// Wait, a delay loop.  When waiting, the main loop is not running!
void stm_wait (uint32_t cnt);
#endif
 
/***************************************************************************
 * Copyright (C) 2008 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // ifdef STM_H
