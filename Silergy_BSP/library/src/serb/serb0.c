/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Sends and receives on UART0.
 *
 * The driver sends data from an array, and receives it into an array.
 * It is full-duplex.
 * It does not use XON/XOFF or other flow control.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 * $Id: serb0.c 17706 2018-05-16 23:31:04Z rvandewa $
 ****************************************************************************/
#include "options.h"    // System definitions, device, pins, etc.
#include "irq.h"        // Interrupt management.
#include "ser_common.h" // Common serial definitions.
#include "ser0.h"       // Serial function prototypes

/*** Version ****/
const char fw_ver_serb0[] =
"serb0"   /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


// Portability defines, used so the device drivers can have similar code.
#undef ser_init
#define ser_init(__b__, __rx__, __ro__, __tx__, __to__) \
       ser0_init(__b__, __rx__, __ro__, __tx__, __to__)
#define ser_tx(__x__) ser0_tx_ch(__x__)
#define tx_cnt tx0_cnt
#define rx_cnt rx0_cnt
#define tx_ptr tx0_ptr
#define rx_ptr rx0_ptr

volatile uint8_t *tx0_ptr;      // These are public to enable monitoring.
volatile uint8_t *rx0_ptr;
volatile int tx0_cnt;
volatile int rx0_cnt;


/****************************************************************************
* Description:
*       Serial interrupt, fill in buffer.
* Input:
*       The receive buffer.
* Parameters:
*       None.
* Output:
* Return Status:
*       None.
*****************************************************************************/
static void rx_fn (uint8_t chr)
{
    if(0 < rx_cnt)          // If there is more space in the buffer.
    {
        *rx_ptr++ = chr;    // Put the byte in the buffer.
        --rx_cnt;
    }
} // rx-fn()


/****************************************************************************
* Description:
*       Serial interrupt, xmit output.
* Input:
*       The transmit buffer
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
static void tx_fn (void)
{
    if (0 < tx_cnt)                 // If there is more data to transmit.
    {
        ser_tx(*tx_ptr++);          // Send the next byte.
        --tx_cnt;                   // Count the byte sent.
    }
} // tx-fn()


/****************************************************************************
* Description:
*       Fill an input buffer with received binary data.
* Input:
*
* Parameters:
*       Pointer and length of the buffer to receive data.
* Output:
* Return Status:
*       None
*****************************************************************************/
void ser0_rx (uint8_t *buf_ptr, int len)
{
    rx_ptr = buf_ptr;
    rx_cnt = len;       // Enable receiving.
} // ser-rx()


/****************************************************************************
* Description:
*       Send an output buffer of binary data.
* Input:
* Parameters:
*       Pointer and length of the buffer to receive data.
* Output:
* Return Status:
*       None
*****************************************************************************/
void ser0_tx (uint8_t *buf_ptr, int len)
{
    tx_ptr = buf_ptr;
    tx_cnt = len;
    tx_fn();            // Start the transmission.
} // ser-tx()


/****************************************************************************
* Description:
*       Initializes serial module's variables and hardware.
* Input:
* Parameters:
*       None.
* Output:
*       Empty input and output queues.
*       The receiver is ready.
*       XON is sent.
*       The transmitter is otherwise idle.
* Return Status:
*       None.
*****************************************************************************/
void serb0_init (int baud)
{
    IRQ_DEFINES;  // record whether interrupts are enabled.

    IRQ_DISABLE();// Disable interrupts.

    tx_cnt = 0;   // Prevent receive or transmit.
    rx_cnt = 0;

    // Initialize register-level serial driver.
    ser_init(baud, rx_fn, NULL, tx_fn, NULL);

    IRQ_ENABLE();         // Restore the interrupt state.
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

