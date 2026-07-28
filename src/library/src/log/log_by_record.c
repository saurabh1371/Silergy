/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Description: This manages three different logs, and coordinates them.
 * 1. A simple log that can work before the nonvolatile log is initialized: 
 * This bit-based log of event numbers records errors during start-up.
 * 2. A bit-based nonvolatile log that can record the current state and 
 * occurrence of events.
 * 3. A record-based log that can record other data for a limited number
 * of the highest-numbered (worst) events.
 * The log records are bigger than the bits, so there are less of them.
 * Only the most important (highest numbered) events go in the records.
 * The log API can be called at any time, including from interrupts, and
 * before the log's data structure is up and running.
 * At log initialization, any event from initialization goes in the standard log.
 * The standard log is bit-based. It has a "state" and "sticky-bit" word.
 * Both bit arrays are nonvolatile, stored in the billing register data.
 * There are also log records that record the most recent time and a count.
 * If the number of records is zero (0 >= LOG_REC_CNT), it just uses the 
 * bit logs.
 * The CLI has output routines for the log, using serial I/O.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * $Id: log_by_record.c 18741 2018-09-11 22:11:32Z rvandewa $
 ***************************************************************************/
#include "options.h"
#include "irq.h"
#include "log.h"


/*** Version ****/
const char fw_ver_log[] =
"log_by_record" /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


/*** External functions referenced by this module ***/
// Refer to include files.

/*** External variables referenced by this module ***/

/*** Public functions referenced by this module ***/

/*** Public variables referenced by this module ***/
uint32_t log_on, log_off; // Zero-initialized by C start-up.
uint8_t log_valid; // 0 = log is not running

/*** Private functions referenced by this module ***/

/*** Private variables referenced by this module ***/
#define LOG_VALID 0xA5


/***************************************************************************   
* Description:  
*   Software error handling function
* Parameters:
*   Input:
*         None.
*   Output: 
*         It logs a software error.
* Return Status: 
*         None
***************************************************************************/
void log_software(void)
{
  log_event ( EV_SOFTWARE ); 
} // void EV_software(void)


/***************************************************************************   
* Description:  
*   Log an event.
* Parameters:
*   Input:
*         ev is an event number from log.h
*   Output: 
*         It logs that an event occured.
* Return Status: 
*         None
***************************************************************************/
void log_event(Ev_t ev)
{
  log_state ( ev, true ); 
  log_state ( ev, false);
} // void EV_software(void)


/***************************************************************************   
* Description:  
*        Log a new state.
* Parameters:
*   Input:
*         ev is an event number from log.h
*         on tells the current state of that condition or event.
*   Output: 
*         It logs the event.
* Return Status: 
*         None
***************************************************************************/
void log_state (Ev_t ev, bool on)
{
    int i;
    #if 0 < LOG_REC_CNT
    int was_off = false;
    Ev_t ev_low;
    Log_rec_t *rec_ptr_low;
    #endif
    IRQ_DEFINES;

    // Use the simple log system before the log array is in from NVM.
    IRQ_DISABLE();
    if (LOG_VALID != log_valid) // log is not up?
    {
        uint32_t mask = 1UL << (int)ev;
        if(on) {
            log_on |= mask;
        } else {
            log_off |= mask;
        }
        IRQ_ENABLE();
        return;

    } else {   // The global log is valid.
        i = 1UL << (int)ev;
        if(on)
        {   // Add the event to the bit arrays.
            #if 0 < LOG_REC_CNT
            if(0 == (global.reg.log.bits & i))
                was_off = true;
            #endif
            global.reg.log.bits |= i;
            global.reg.log.bits_sticky |= i;
        } else { // Remove the event from the state bits.
            global.reg.log.bits &= ~i;
        }

        #if 0 < LOG_REC_CNT // If there are log records.
        // Prepare to search for a record to update.
        ev_low = EV_MAX;
        rec_ptr_low = NULL;

        // Search the array.
        for(i = 0; i < LOG_REC_CNT; ++i)
        {
            Log_rec_t *rec_ptr = &global.reg.log.rec_ary[i];

            IRQ_ENABLE(); // Briefly enable interrupts.
            IRQ_DISABLE();

            // Is that event type already in the record array?
            if(ev == (Ev_t)rec_ptr->ev)
            {
                rec_ptr_low = rec_ptr;
                ev_low = ev;
                break;
            } else { // find the smallest-numbered event record.
                if(ev_low > (Ev_t)rec_ptr->ev) // lower-priority event?
                {
                    ev_low = (Ev_t)rec_ptr->ev;
                    rec_ptr_low = rec_ptr;
                }
            }
        }

        if(on && ev_low <= ev)  // Is there room for the new event?
        {   // Yes!
            if(ev == ev_low && was_off)    // Already in the array?
            {   // Yes, so update the record.
                if(255 > rec_ptr_low->cnt) // Count the event.
                    rec_ptr_low->cnt = 1 + rec_ptr_low->cnt;
            } else if(was_off) { // Replace the lowest-priority event record.
                rec_ptr_low->ev = (uint8_t)ev; // The event
                rec_ptr_low->cnt = 1; // The count.
            }
            // Record the time.
            i = global.reg.tm.tm_year;
            if(!global.reg.tm.tm_isvalid) // the time is invalid?
                i |= 0x80; // Flag that the time is bad.
            rec_ptr_low->year = i;
            rec_ptr_low->mon    = global.reg.tm.tm_mon;
            rec_ptr_low->mday   = global.reg.tm.tm_mday;
            rec_ptr_low->hour   = global.reg.tm.tm_hour;
            rec_ptr_low->min    = global.reg.tm.tm_min;
            IRQ_ENABLE();
            return;
        }
        #endif // if there are log records.
    }
    // If it reaches here, it did not record the event.
    IRQ_ENABLE();
}


/***************************************************************************   
* Description:  
*   Clears the log of all errors.
*
* Parameters:
*   Input:
*         None.
*   Output: 
*         The error log is cleared.
* Return Status: 
*         None
***************************************************************************/
void log_clear (void)
{
    int i = 0;
    IRQ_DEFINES;

    IRQ_DISABLE();   // Disable interrupts.

    log_on = 0;
    log_off = 0;

    if (LOG_VALID != log_valid) {
        IRQ_ENABLE();
        return;
    }

    global.reg.log.bits = i;
    global.reg.log.bits_sticky = i;

    #if 0 < LOG_REC_CNT
    for(i = 0; i < LOG_REC_CNT; ++i)
    {
        global.reg.log.rec_ary[i].ev = (uint8_t)EV_NONE;
    }
    #endif 

    IRQ_ENABLE();
}


/***************************************************************************   
* Description:  
*   Clears the log of one error.
*
* Parameters:
*   Input:
*         error number
*   Output: 
*         None, except that the error log is cleared of one error type.
* Return Status: 
*         None
***************************************************************************/
void log_clear_one (Ev_t ev)
{
    int i;
    IRQ_DEFINES;

    IRQ_DISABLE();

    log_on &= ~(1UL << (int)ev);

    if (LOG_VALID != log_valid) {
        IRQ_ENABLE();
        return;
    }

    i = ~(1UL << (int)ev);
    global.reg.log.bits &= i;
    global.reg.log.bits_sticky &= i;

    #if 0 < LOG_REC_CNT
    for(i = 0; i < LOG_REC_CNT; ++i)
    {
        Log_rec_t *rec_ptr = &global.reg.log.rec_ary[i];
        if(ev == (Ev_t)rec_ptr->ev)
            rec_ptr->ev = (uint8_t)EV_NONE;
    }
    #endif

    IRQ_ENABLE();
}


/***************************************************************************   
* Description:  
*   Reads whether a condition is currently recorded as true.
*
* Parameters:
*   Input:
*         error number
*   Output: 
* Return Status: 
*         true (nonzero) if the condition is recorded as currently on.
***************************************************************************/
bool log_active (Ev_t ev)
{
    int tmp;
    IRQ_DEFINES;

    if (LOG_VALID != log_valid) {
        if(log_on & (1UL << (int)ev))
            return true;

        return false;
    }

    IRQ_DISABLE();

    tmp = global.reg.log.bits;

    IRQ_ENABLE();

    if(0 != (tmp & (1UL << (int)ev)))
        return true;

    return false;
}


/***************************************************************************   
* Description:  
*   Reads whether a condition was ever recorded as true.
*
* Parameters:
*   Input:
*         error number
*   Output: 
* Return Status: 
*         true (nonzero) if the condition is recorded as currently on.
***************************************************************************/
bool log_historic (Ev_t ev)
{
    int tmp;
    IRQ_DEFINES;

    if (LOG_VALID != log_valid)
        return log_active(ev);

    IRQ_DISABLE();

    tmp = global.reg.log.bits_sticky;

    IRQ_ENABLE();

    if(0 != (tmp & (1UL << (int)ev)))
        return true;

    return false;
}


#if 0 < LOG_REC_CNT
/***************************************************************************   
* Description:  
*   Clears the log of one error.
*
* Parameters:
*   Input:
*         error number
*   Output: 
*         None, except that the error log is cleared of one error type.
* Return Status: 
*         None
***************************************************************************/
Log_rec_t *log_get_rec (Ev_t ev)
{
    int i;
    IRQ_DEFINES;

    if (LOG_VALID != log_valid)
        return NULL;

    IRQ_DISABLE();

    for(i = 0; i < LOG_REC_CNT; ++i)
    {
        Log_rec_t *rec_ptr = &global.reg.log.rec_ary[i];
        if(ev == (Ev_t)rec_ptr->ev)
        {
            IRQ_ENABLE();
 
            return rec_ptr;
        }
    }

    IRQ_ENABLE();

    return NULL;
}
#endif


/***************************************************************************   
* Description:  
*   Initialize the logging system.  It clears transient, nonvolatile items,
*   then it logs the highest priority event, condition or error 
*   that occurred before the error system was started.
* Parameters:
*   Input:
*         None.
*   Output: 
*         None.
* Return Status: 
*         None
***************************************************************************/
void log_init (void)
{
    int i;

    log_valid = LOG_VALID;

    // Erase nonvolatile events. (make room for new events.)
    for(i = (int)EV_MIN; i < (int)EV_NONVOLATILE; ++i)
    {
        log_clear_one((Ev_t)i);
    }

    // Log anything discovered before this startup.
    for(i = (int)EV_MIN; i <= EV_MAX; ++i)
    {
        int mask = 1UL << i;
        if(log_on & mask)
            log_state((Ev_t)i, true);
        if(log_off & mask)
            log_state((Ev_t)i, false);
    }

    log_on = 0;
    log_off = 0;

    // If the nonvolatile log data is bad, tell someone.
    if(log_active(EV_REG_BAD)) // The log is in the billing-register data.
    {
        log_event(EV_LOG_BAD);
    }
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/

