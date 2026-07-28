/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2008 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Soft Timer code.
 * Many systems need more timers.  So, this code makes slow software timers.
 * To use this, make a 32-bit variable.  Pass its address to stm_start().
 * The timer will count down the timer. Then, it forgets the timer variable.
 * Forgetting is good, because if the timer system is corrupted, it will heal.
 * The advantage of this design is that it leaves more hardware timers free.
 *
 * HISTORY: $Id: stmrtc.c 17640 2018-05-14 17:46:51Z rvandewa $
 ***************************************************************************/
#include "options.h"    // System definitions, device, pins, etc.
#include "irq.h"        // Interrupt management.
#include "stm.h"        // Check defines.


/*** Version ****/
const char fw_ver_stm[] =
"stmrtc" /* Device Driver name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;

/*** Private variables declared within this module ***/
// Safety code, to discover and expire corrupted timer entries.
static int junk_cnt;
// array of pointers to tick counts.
volatile int *cnt_ptr_ary[ STM_CNT ];
static int old_tick_cnt;            // global counting up tick count
static uint8_t stm_cnt;             // count of active software timers.

/****************************************************************************
* Description:
*       Deallocate a timer. This is a private routine.
*       Timers are deallocated automatically, so if RAM is corrupted, 
*       the timer system will heal as it is used.
* Input:
*       Needs the array of pointers to counters, cnt_ptr_ary[], above.
* Parameters:
*       idx is the index of a timer in the timer array.
* Output:
*       The timer is deallocated from the timer array.
* Return Status:
*       None.
*****************************************************************************/
static void deallocate (int idx)
{
    IRQ_DEFINES;                    // record interrupt state.

    IRQ_DISABLE();                  // Disable interrupts.

    if (idx >= (int8_t)stm_cnt)     // already deallocated?
    {
        IRQ_ENABLE();               // Restore the interrupt state.
        return;
    }

    if (stm_cnt > 0) // Any timers?
    {
        --stm_cnt;   // one less timer now.
        // Move the last timer into this timer's place.
        cnt_ptr_ary[idx] = cnt_ptr_ary[stm_cnt];
        // Safety code so unused pointers are harmless.
        cnt_ptr_ary[stm_cnt] = &junk_cnt;
    }
    IRQ_ENABLE();                   // Restore the interrupt state.
}


/****************************************************************************
* Description:
*       Start a software timer.
*       Every time that this happens, the timer system reruns, bringing
*       all the timing up to date.
* Input:
* Parameters:
*       cnt_ptr points to a 32-bit variable used as the timer's counter.
*       time is the time of the timer in SUBSEC units (use the macros
*           in stm.h to get real units like seconds and milliseconds)
* Output:
*       When a timer expires, it becomes zero.
* Return Status:
*       false/0 = no error.
*****************************************************************************/
int stm_start (volatile int *cnt_ptr, int time)
{
    int idx;
    IRQ_DEFINES;                        // record interrupt state.

    // Eliminate jitter from different starting times.
    stm_run();

    // Is the timer already running?
    if (*cnt_ptr)
    {
        IRQ_DISABLE();                  // Disable interrupts.

        // Find the timer.  Usually, it will be the last timer,
        // and there are not many, so searching backwards is not as 
        // slow as it looks. It also only searches timers that are
        // running.
        for (idx = stm_cnt - 1; 0 <= idx; --idx)
        {
            if ( cnt_ptr == cnt_ptr_ary[idx] )
            {
                *cnt_ptr = time;        // The timer actually gets its value.
                if (0 == time)
                {
                    // Since the loop counts down, the deallocated
                    // timer is overwritten with the last timer, 
                    // which is already processed.
                    deallocate(idx);    // deallocate it if it's clear.
                }
                IRQ_ENABLE();           // Restore the interrupt state.
                return false;           // No error
            }
            IRQ_ENABLE();               // Restore the interrupt state.
            // Permit interrupts here.
            IRQ_DISABLE();              // Disable interrupts.
        }
    }
    // Timer not running.  Yet. Because it's not got a table entry.

    // Is it needed? Or is this a repeated attempt to stop the timer?
    if (0 == time)
    {
        *cnt_ptr = 0;                   // force it to expire.
        IRQ_ENABLE();                   // Restore the interrupt state.
        return false;                   // No error
    }

    // Need a new timer, so allocate it.
    // Note, still in critical region.  Interrupts disabled.
    idx = stm_cnt;
    if (STM_CNT <= idx)     // idx too big?
    {
        #ifdef LOG_H        // If the log is present
        log_software();     // Log a software failure (too few timer entries)
        #endif
        stm_cnt = STM_CNT;  // Make it the largest possible.
        *cnt_ptr = 0;       // force it to expire.
        IRQ_ENABLE();       // Restore the interrupt state.
        return true;        // Error.
    }

    // Finally, actually start the timer.
    cnt_ptr_ary[idx] = cnt_ptr;
    ++stm_cnt;
    *cnt_ptr = time;        // The timer actually gets its value.

    IRQ_ENABLE();           // Restore the interrupt state.
    return false;           // No error
}


/****************************************************************************
* Description:
*   Run the timer system- this should be called from a main loop.
*   It must run at least once in each half second.
* Input:
*       It runs when there are unexpired timers in the array.
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
void stm_run (void)
{
    int rtcsub;

    // RTCSUB is accurate in all clock speeds and battery modes.
    // Any timers?  Has the time changed? (usually not)
    rtcsub = (int)(unsigned int)RTC->SUB_b.rtc_sub;
    if (old_tick_cnt != rtcsub)
    {
        // Any timers exist?
        if (stm_cnt > 0)
        {
            int32_t tick_cnt;
            int idx;
            IRQ_DEFINES;               // record interrupt state.

            // Unsigned masked subtract fixes wrap-around
            // of the timer using modular arithmetic.
            // E.g. 0xff & (0x02 - 0xFE) = 4 
            tick_cnt = 0xff & (rtcsub - old_tick_cnt); 

            // Safety code.
            if (STM_CNT < stm_cnt)     // idx too big?
            {
                stm_cnt = STM_CNT;     // fix it.
            }
            junk_cnt = 0;              // expire any timers using junk_cnt.

            // Count-down the timers.
            for (idx = stm_cnt - 1; 0 <= idx; --idx)
            {
                int32_t cnt;
                volatile int *ptr;

                // Counters and booleans can be set in interrupts.
                // The timers can be allocated in interrupts.
                IRQ_DISABLE();          // Disable interrupts.

                ptr = cnt_ptr_ary[idx];

                cnt = *ptr;

                if (tick_cnt < cnt) // Timer expired?
                {
                    // count timer down.
                    cnt = cnt - tick_cnt;
                    *ptr = cnt;
                } else {
                    // The count expired, so stop the timer.
                    *ptr = cnt = 0;
                }   

                if (0 == cnt)   // Did the timer expire?
                {
                    // Deallocate an expired timer.
                    // Since the loop counts down, the deallocated
                    // timer is overwritten with the last timer, 
                    // which is already processed.
                    deallocate(idx);
                }

                IRQ_ENABLE(); // Restore the interrupt state.
            }   // end for all timers.
        } // end if there are timers.

        old_tick_cnt = rtcsub;
    }   // end if time changed.
}


/****************************************************************************
* Description:
*   initialize the software timers
* Input:
* Parameters:
* Output:
*       Sets the count of poionters to zero, deallocating any timers.
* Return Status:
*       None.
*****************************************************************************/
void stm_init(void)
{
    int idx;

    junk_cnt = 0;   // Expire any counters pointing here.

    // Safety code so pointers are harmless.
    stm_cnt = STM_CNT;  // stop any allocation.
    for (idx = STM_CNT-1; idx >= 0; --idx)
    {
        cnt_ptr_ary[idx] = &junk_cnt;
    }

    // The timer clock starts at this instant.
    old_tick_cnt = (int)(unsigned int)RTC->SUB_b.rtc_sub;

    stm_cnt = 0;        // Permit allocation.
}


#ifdef STM_WAIT
/****************************************************************************
* Description:
*       Wait for cnt SUBSECs to expire.  While waiting,
*       the background loop is not running!
* Input:
* Parameters:
*       cnt is the count of SUBSECs this waits.
* Output:
* Return Status:
*       None.
*****************************************************************************/
void stm_wait (int cnt)
{
    int old_tck_cnt, tck_cnt, rtcsub;

    old_tck_cnt = (int)(unsigned int)RTC->SUB_b.rtc_sub;

    // RTCSUB is accurate in all clock speeds and battery modes.
    while (0 != cnt)
    {
        // Has the time changed? (usually not)
        rtcsub = (int)(unsigned int)RTC->SUB_b.rtc_sub;
        if (old_tck_cnt != rtcsub)
        {
            // Unsigned masked subtract fixes wrap-around
            // of the timer using modular arithmetic.
            // E.g. 0xff & (0x02 - 0xFE) = 4 
            tck_cnt = 0xff & (rtcsub - old_tck_cnt); 

            if (tck_cnt < cnt) // Timer expired?
            {
                // count timer down.
                cnt = cnt - (uint16_t)tck_cnt;
            } else {
                // The count expired, so stop the timer.
                cnt = 0;
            }   

            old_tck_cnt = rtcsub;
        }
    }
}
#endif

/***************************************************************************
 * Copyright (C) 2008 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

