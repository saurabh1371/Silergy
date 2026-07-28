/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Mailbox management for Slave SPI
 *
 *
 * Copyright 2013, Silergy Corp. All rights reserved.
 * $Id: sspi0.c 17915 2018-06-02 06:22:17Z rvandewa $
 ****************************************************************************/
#include <ctype.h>        // get iscntrl() 
#include "options.h"      // Specific device HW register definitions.
#include "cli.h"          // CLI driver definitions.
#include "irq.h"          // Interrupt management.
#include "sspi.h"         // Slave SPI low level driver
#include "sspi0.h"        // Slave SPI CLI driver declaration
#include "stm.h"          // software timer


/*** Version ****/
const char fw_ver_sspi0[] =
"sspi0"   /* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


// Note: Symbols SERIAL_BAUD, I_SIZE, L_SIZE and Q_SIZE are in cli.h

char sspi0_cli_buf [L_SIZE+1];          // CLI line buffer for input and output

#define ser_tx(__x__) sspi0_tx(__x__)

// queues
// Input RX circular buffer of characters.
// This buffer will be mostly copied to CLI for processing
#if __ARMCC_VERSION
static char rx_buf[I_SIZE] __attribute__((at(SPIS_MBOX_ADDR)));   
char tx_buf[I_SIZE] __attribute__((at(SPIS_MBOX_TX_ADDR)));
#else
// Portable, but not visible in symbolic debugger.
#define rx_buf ((char *)SPIS_MBOX_ADDR)
#define tx_buf ((char *)SPIS_MBOX_TX_ADDR)
#endif

volatile int sspi_tx_idx = 0; // Index to add char in SSPI Tx mailbox

// RX buffer index to remove from buffer
static volatile uint8_t rx_buf_get_idx;
// RX buffer index to insert to buffer
static volatile uint8_t rx_buf_put_idx;
// count of characters in rx buf
static volatile uint8_t rx_cnt;
                                                
// Queue index for tx interrupt to get strings
static volatile uint8_t tx_q_get_idx;
// Queue index for put-str() to put strings
static volatile uint8_t tx_q_put_idx;
// Char pointer to currently transmitting tx string.
static const char *tx_str = NULL;
// Circular queue of output strings
static const char * tx_q[Q_SIZE];  


// Static function declaration
// Fetch a char from teh right kind(s) of memory, the right way.
static char get_any_char(const char * tx_char);

// Put a string.
static int put_str(const char *str);

// Put a char
static int sspi0_tx(const char c);

/****************************************************************************
* Description:
*       Transmit next character from a queue of zero-terminated strings.
* Input:
*       The queue of strings.
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
static void send_one (void) 
{
    char get_tx_char;

    get_tx_char = get_any_char(tx_str);

    // More string to send?
    if ( '\0' != get_tx_char )
    {
        ser_tx(get_tx_char);    // Yes, so send the character.
        ++tx_str;               // Increment to next character
        return;
    }
    else
    {
        // No. The string is done. Try to get another.
        // More strings in the queue? (does the string have content?)
        tx_q_get_idx &= (Q_SIZE - 1);  // wrap the index
        tx_str = tx_q[tx_q_get_idx]; // get next string
        if ( NULL != tx_str ) 
        {
            // Yes. String has content.  Send it.
            // Send the first char of the next string.
            ser_tx(get_any_char(tx_str++));
            // delete the string from the queue of strings.
            tx_q[tx_q_get_idx++] = NULL;
            return;
        }
        else
        {
            // No strings in the queue, so stop transmitting
            tx_str = NULL;          // make the string pointer empty.
            return;
        }
    }
} // send one


/****************************************************************************
* Description:
*       Get a character from either flash or RAM
* Input:
* Parameters:
*       tx_char is a character address with a bit set for flash, 
*           or cleared for RAM.
* Output:
* Return Status:
*       a character
*****************************************************************************/
static char get_any_char(const char * tx_char)
{
    if(NULL == tx_char)
        return '\0';
    return (char)*tx_char;
} // get any char


/****************************************************************************
* Description:
*       Sends a zero-terminated string located in Data RAM.
* Input:
* Parameters:
*       str is a zero-terminated string in RAM.
* Output:
*       The string is placed in the output queue, and set to DTE.
* Return Status:
*       0 - no Error.
*       1  - Error, Queue is full.
*****************************************************************************/
static int put_str (const char *str) 
{
    int err=0;                          // No error
    IRQ_DEFINES;                        // record interrupts state.

    IRQ_DISABLE();                      // Disable interrupts.

    tx_q_put_idx &= (Q_SIZE - 1);       // Wrap queue of strings.
    if (NULL == tx_q[tx_q_put_idx])     // Q entry free?
    {   // Q Entry empty.  Insert string. 
        tx_q[tx_q_put_idx++] = str;     // Insert string in queue
        send_one();                     // Send first char. (enables tx int.)
        IRQ_ENABLE();                   // Restore the interrupt state.
    }
    else
    {   // Q full
        IRQ_ENABLE();                   // Restore the interrupt state.
        err=1;                          // Q full. Report it
    }

    return err;
}

/****************************************************************************
* Description:
*       Store a charcter in the Slave SPI Tx mailbox.
* Input:
* Parameters:
*       c is a character
* Output:
*       The string is placed in the Slave SPI mail box.
* Return Status:
*       0 - no Error.
*       1  - Error, Queue is full.
*****************************************************************************/
static int sspi0_tx (const char c) 
{
    int err=0;                          // No error
    tx_buf[sspi_tx_idx++] = c;
    return err;
}


/****************************************************************************
* Description:
*       Get a CLI input buffer
*       This function will return NULL if the line is not ready
*       for processing (no CR key received yet) or is being 
*       used as an output buffer.
* Input:
*       It checks the Slave SPI input queue of chars, the circular buffer.
* Parameters:
*       None.
* Output:
* Return Status:
*       Returns NULL if no data, or a zero-terminated string.
*       buf_ptr always points to a buffer.
*       len_ptr is set to the usable length, or zero
*****************************************************************************/
static char *get_str (char **buf_ptr, int *len_ptr)    
{
    static uint8_t cli_buf_idx = 0;
    bool line_done = false;
    int my_extract_idx = (int) rx_buf_get_idx;
    int my_rx_cnt;

    *buf_ptr = tx_buf; // tx buffer

    // No data received?
    my_rx_cnt = (int)rx_cnt;

    if (0 == my_rx_cnt)             // When empty
    {
        *len_ptr = L_SIZE;
        return NULL;
    }

    // Copy each character from the RX buffer to the CLI buffer
    // and process if necessary.
    // Copy until no more characters in the RX buffer or CLI buffer not full
    // When CLI is full, it is automatically pushed into the CLI for 
    // processing without waiting for a CR
    while ((0 != my_rx_cnt) && (!line_done))
    {
        // Wrap buffer index
        my_extract_idx &= (I_SIZE - 1);
        switch(rx_buf[my_extract_idx])
        {
            case '\r':                      // Return character
                sspi0_cli_buf [cli_buf_idx] = '\0';
                line_done = true;          
                break;

            case '\b':                      // Back-space character
                if (cli_buf_idx > 0)        // Only backspace when buffer
                {                           // is not empty
                    cli_buf_idx--;
                }
                break;

            case ',':                       // Repeat previous line
                if (cli_buf_idx == 0)       // Comma commnd? or just text?
                {
                    line_done = true;
                    break;
                }
                //lint -fallthrough  Fall through here is intentional.
            default:
                if (!iscntrl(rx_buf[my_extract_idx]))
                {
                    sspi0_cli_buf [cli_buf_idx++] 
                        = rx_buf[my_extract_idx];
                }
                break;
        }
        // Proceed to the next char
        my_extract_idx++;

        if (cli_buf_idx <= L_SIZE)
            line_done = true;
    }
    // put back the extraction index.
    rx_buf_get_idx = (uint8_t)(my_extract_idx & (I_SIZE - 1));

    // Push result to CLI for process when command is ready
    if (line_done)
    {
        cli_buf_idx = 0;   // Reset the CLI buffer  
        *len_ptr = L_SIZE;
        return ( (char *)sspi0_cli_buf );
    }

    *len_ptr = 0;
    return (NULL);
}

/****************************************************************************
* Description:
*       Slave SPI interrupt, fill in str buffer, edit line, xmit output queue.
* Input:
*       The queue of output strings, set up by the CLI, and put_str()
* Parameters:
*       None.
* Output:
*       Sends strings to the Slave SPI output register.
* Return Status:
*       None.
*****************************************************************************/
static void rx_fn (void)
{
    rx_buf_put_idx  = 0;
    sspi_tx_idx = 0;
    ++rx_cnt;
} // rx_fn()

/****************************************************************************
* Description:
*       Initializes Slave SPI module's variables and hardware.
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
static void init (void)
{
    int i;
    IRQ_DEFINES;                    // record whether interrupts are enabled.

    IRQ_DISABLE();                  // Disable interrupts.

    // Set all output strings in queue to the null string
    for (i = 0; i < (Q_SIZE); i++) 
    {
        tx_q[i] = NULL;
    }

    sspi0_cli_buf [0] = '\0';       // Empty CLI buffer
    sspi0_cli_buf [L_SIZE] = '\0';  // Zero-terminate CLI line buffer at the end
    rx_buf_get_idx = 0;             // Reset RX buffer starting index
    rx_buf_put_idx = 0;             // Reset RX buffer ending index
    rx_cnt = 0;                     // No characters received
    tx_q_get_idx = 0;               // Reset queue starting index
    tx_q_put_idx = 0;               // Reset queue ending index
    tx_str = NULL;                  // Make the string pointer empty.
    sspi_tx_idx = 0;                // SSPI Tx mailbox index

    // Initialize register-level Slave SPI driver.
    sspi_init(SAFEMODE, rx_fn);

    IRQ_ENABLE();                   // Restore the interrupt state.
}


/****************************************************************************
* Description:
*       Fn pointer for structure to get polymorphic interface to cli driver.
=*****************************************************************************/
const cli_driver_t sspi0_cli_driver = {
    init, get_str, put_str, &sspi_tx_idx
};


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

