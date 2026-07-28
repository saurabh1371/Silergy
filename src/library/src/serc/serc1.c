/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Sends and receives on UART1.
 *
 * The driver has a special problem for transmit: It has to be able to send
 * very large strings of ASCII help text.  A conventional circular buffer
 * would need to be several thousand bytes. So, it queues zero-terminated
 * string pointers in a circular buffer of strings.
 * The receive queue is a conventional small circular queue of characters.
 * The receive queue is drained as the main loop edits the command line.
 * Both queues are indexed, and wrap the indices around by masking with an
 * "and" operation.
 * Both queues have a count of entries, and use this for flow control.
 *
 * Copyright 2013, Silergy Corp. All rights reserved.
 * $Id: serc1.c 24036 2020-03-25 23:01:04Z rvandewa $
 ****************************************************************************/
#include <ctype.h>        // get iscntrl() 
#include "options.h"      // Specific device HW register definitions.
#include "cli.h"          // CLI driver definitions.
#include "irq.h"          // Interrupt management.
#include "ser_common.h"   // Common serial definitions.
#include "ser1.h"         // Serial function prototypes 
#include "stm.h"          // software timer
#include "lcd.h"          // LCD-GPIO map/unmap

/*** Version ****/
const char fw_ver_serc1[] =
"serc1"  /* Library file name */
" v"     /* ver string */
"1.01"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;


// Note: Symbols SERIAL_BAUD, B_SIZE and Q_SIZE are in cli.h

#define XON         0x11                // DC1
#define XOFF        0x13                // DC3

#undef ser_init
#define ser_init(__b__, __rx__, __ro__, __tx__, __to__) \
       ser1_init(__b__, __rx__, __ro__, __tx__, __to__)
#define ser_tx(__x__) ser1_tx_ch(__x__)
#define ser_cli_buf ser1_cli_buf
#define ser_tx_busy ser1_tx_busy
#undef ser_put_str
#define ser_put_str(__s__) put_str(__s__)

// xon timer- send XON repeatedly
static int ser_xon_timer;
// line buffer
char ser1_cli_buf [L_SIZE+1];            // CLI buffer for input and output

// queues
// Input RX circular buffer of characters.
// This buffer will be mostly copied to CLI for processing
static char rx_buf[I_SIZE];
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

// state flags
// Other side can send.
static volatile bool xon_sent;
// This side can send.
static volatile bool xon_recvd;
// Transmitting flag; port in use
       volatile int ser1_tx_busy;
// Need XOFF sent on next tx interrupt.
static volatile bool send_xoff_flag;
// Need XON sent on next tx interrupt.
static volatile bool send_xon_flag;

// Static function declaration
// Fetch a char from teh right kind(s) of memory, the right way.
static char get_any_char(const char * tx_char);

// Put a string.
static int put_str(const char *str);

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
    int get_tx_char;

    ser_tx_busy = true;         // Sending starts.
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
            ser_tx_busy = false;    // xmit stopped.
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
*       Sends XOFF
* Input:
* Parameters:
*       None.
* Output:
*       XOFF goes to the output.
* Return Status:
*****************************************************************************/
static void send_xoff (void)
{
    send_xoff_flag = true;     // Yes, Send XOFF ASAP
    if (! ser_tx_busy )        // Not transmitting?
    {
        send_xoff_flag = false;
        ser_tx_busy = true;    // busy for now, at least
        ser_tx(XOFF);          // Send XOFF now.
        xon_sent = false;      // Receive disabled
    }
} // send xoff


/****************************************************************************
* Description:
*       Sends XON
* Input:
* Parameters:
*       None.
* Output:
*       XON goes to the output.
* Return Status:
*****************************************************************************/
static void send_xon (void) 
{
    send_xon_flag = true;
    if (! ser_tx_busy )
    {
        send_xon_flag = false;
        ser_tx_busy = true;
        ser_tx(XON);
        xon_sent = true;
    }
} // send xon

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
        if (! ser_tx_busy )             // If not already transmitting?
        {
            send_one();                 // Send first char. (enables tx int.)
        }

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
*       Get a CLI input buffer
*       This function will return NULL if the line is not ready
*       for processing (no CR key received yet) or is being 
*       used as an output buffer.
* Input:
*       It checks the serial input queue of chars, the circular buffer.
* Parameters:
*       None.
* Output:
* Return Status:
*       Returns NULL if no data, or a zero-terminated string.
*       buf_ptr always points to a buffer.
*       len_ptr is set to the usable length, or zero
*****************************************************************************/
#if SER_ECHO
static const char backspace[] = "\b \b";
static const char crlf[] = "\r\n";
#endif
static char *get_str (char **buf_ptr, int *len_ptr)    
{
    static uint8_t cli_buf_idx = 0;
    bool line_done = false;
    #if SER_ECHO
    int idx;
    #endif
    int my_extract_idx = (int) rx_buf_get_idx;
    int my_rx_cnt;
    IRQ_DEFINES; // record whether interrupts are enabled.

    *buf_ptr = ser_cli_buf;

    // System is busy transmitting. This avoids using the CLI buffer 
    // when it's being used as output. No string point check in the 
    // queue is necessary
    if ( ser_tx_busy ) 
    {
        *len_ptr = 0;
        return NULL;
    }

    // No data received?
    my_rx_cnt = (int)rx_cnt;

    if (0 == my_rx_cnt)             // When empty
    {
        IRQ_DISABLE();              // Disable interrupts.
        if(!xon_sent || 0 == ser_xon_timer)
        {
            send_xon();
            IRQ_ENABLE();           // Restore the interrupt state.
            if(stm_start (&ser_xon_timer , STM_SEC(3) )) /* Restart the timer. */
                log_software();
            *len_ptr = 0;
            return NULL;
        }
        else
        {
            IRQ_ENABLE();           // Restore the interrupt state.
        }
        *len_ptr = L_SIZE;
        return NULL;
    }

    #if (SER_ECHO)
    // Remember the original index for echoing at the end   
    idx = cli_buf_idx;
    #endif

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
            case 0x18:                      // CAN, Ctrl-X (Start autoreport)
            case 0x19:                      // EM, Ctrl-Y (End autoreport)
                ser_cli_buf [cli_buf_idx++] = rx_buf[my_extract_idx];
                ser_cli_buf [cli_buf_idx] = '\0';
                line_done = true;          
                break;

            case '\r':                      // Return character
                ser_cli_buf [cli_buf_idx] = '\0';
                line_done = true;          
                break;

            case '\b':                      // Back-space character
                if (cli_buf_idx > 0)        // Only backspace when buffer
                {                           // is not empty
                    cli_buf_idx--;
                    #if (SER_ECHO)
                    ser_put_str (backspace);
                    #endif
                }
                break;

            case ',':                       // Repeat previous line
                if (cli_buf_idx == 0)       // Comma command? or just text?
                {
                    line_done = true;
                    break;
                }

                //lint -fallthrough  Fall through here is intentional.
            default:
                if (!iscntrl(rx_buf[my_extract_idx]))
                {
                    ser_cli_buf [cli_buf_idx++] 
                        = rx_buf[my_extract_idx];
                }
                break;
        }
        // Proceed to the next char
        my_extract_idx++;
        IRQ_DISABLE();          // Disable interrupts.
        my_rx_cnt = --rx_cnt;   // Adjust queue count shared with isr.
        IRQ_ENABLE();           // Restore the interrupt state.

        if (cli_buf_idx >= L_SIZE)
            line_done = true;
    }
    // put back the extraction index.
    rx_buf_get_idx = (uint8_t)(my_extract_idx & (I_SIZE - 1));

    #if (SER_ECHO)
    // Echo characters, if there's any input
    if ((cli_buf_idx > 0) && (idx < cli_buf_idx))
    {
        ser_cli_buf [cli_buf_idx] = '\0';
        ser_put_str ((char *)& ser_cli_buf [idx]);
    }
    #endif


    // Push result to CLI for process when command is ready
    if (line_done)
    {
        #if (SER_ECHO)
        ser_put_str (crlf);          
        #endif

        cli_buf_idx = 0;   // Reset the CLI buffer  
        *len_ptr = L_SIZE;
        return ( (char *)ser_cli_buf );
    }

    *len_ptr = 0;
    return (NULL);
}

#if 0
/****************************************************************************
* Description:
*       Get a character from a CLI input buffer
*       This function will return -1 if there is no data.
*       or is being used as an output buffer.
* Input:
*       It checks the serial input queue of chars, the circular buffer.
* Parameters:
*       None.
* Output:
* Return Status:
*       Returns -1 if no data, or a character.
*****************************************************************************/
static int get_char (void)    
{
    int ch = -1;    // EOF
    uint8_t my_rx_cnt;
    IRQ_DEFINES;    // record interrupt state.

    // No data received?
    my_rx_cnt = rx_cnt;

    // System is busy transmitting. This avoids using the CLI buffer 
    // when it's being used as output. No string point check in the 
    // queue is necessary.
    if ( ser_tx_busy ) 
        return ch;

    if (0 == my_rx_cnt)             // When empty
    {
        if(!xon_sent && (B_MARGIN > my_rx_cnt))
        {
            IRQ_DISABLE();          // Disable interrupts.
            send_xon();
            IRQ_ENABLE();           // Restore the interrupt state.
        }
        return ch;
    }

    // Copy each character from the RX buffer to the CLI buffer
    // and process if necessary.
    // Copy until no more character in the RX buffer or CLI buffer not full
    // When CLI is full, it is automatically pushed into the CLI for 
    // processing without waiting a CR
    if (0 != my_rx_cnt)
    {
        rx_buf_get_idx &= (B_SIZE - 1); // Wrap buffer index
        ch = rx_buf[rx_buf_get_idx];
        // Proceed to the next char
        rx_buf_get_idx++;
        // Adjust circular buffer index
        rx_buf_get_idx &= (B_SIZE - 1);

        IRQ_DISABLE();          // Disable interrupts.
        my_rx_cnt = --rx_cnt;   // Adjust queue count shared with isr.
        IRQ_ENABLE();           // Restore the interrupt state.
    }

    return ch;
}
#endif

#if 0
/****************************************************************************
* Description:
*       Check if data is available.
*       This function will return the number of bytes available.
* Parameters:
*       None.
* Output:
* Return Status:
*       Returns count of bytes to get, or zero.
*****************************************************************************/
static int get_cnt (void)    
{
    int my_rx_cnt = rx_cnt;
    IRQ_DEFINES;                    // record interrupt state.

    if (0 == my_rx_cnt)             // When empty
    {
        // If no XON was sent, and the receive buffer can take more.
        if(!xon_sent && (B_MARGIN > my_rx_cnt))
        {
            IRQ_DISABLE();          // Disable interrupts.
            send_xon();             // Send an XON.
            IRQ_ENABLE();           // Restore the interrupt state.
        }
    }
    return my_rx_cnt;
}
#endif


/****************************************************************************
* Description:
*       Serial interrupt, fill in str buffer, edit line, xmit output queue.
* Input:
*       The queue of output strings, set up by the CLI, and put_str()
* Parameters:
*       None.
* Output:
*       Sends strings to the serial output register.
* Return Status:
*       None.
*****************************************************************************/
static void rx_fn (uint8_t chr)
{
    // XON and XOFF are removed from incoming data
    if (XON == chr)             // XON received, transmit ok
    {
        xon_recvd = true;
        if (! ser_tx_busy )     // If not already transmitting...
        {
            send_one();         // Send first char and enable tx interrupt.
        }
    }
    else if (XOFF == chr)       // XOFF received, transmit not allowed
    {
        xon_recvd = false; 
    }
    else
    {
        // Save character to the queue.
        rx_buf_put_idx &= (I_SIZE - 1);
        rx_buf[rx_buf_put_idx++] = (char)chr; 
        ++rx_cnt;

        // wake up meter from power-save mode.
    }

    if (xon_sent)   // XON sent? i.e. receiving is permitted?
    {
        if ( (I_SIZE - I_MARGIN) < rx_cnt ) // out of space?
        {
            send_xoff();
        }
    }
} // rx_fn()


/****************************************************************************
* Description:
*       Serial interrupt, xmit output queue.
* Input:
*       The queue of output strings, set up by the CLI, and put_str()
* Parameters:
*       None.
* Output:
*       Sends strings to the serial output register.
* Return Status:
*       None.
*****************************************************************************/
static void tx_fn (void)
{
    // XOFF and XON are higher priority than other data
    if (send_xoff_flag)         // XOFF needed?
    {
        send_xoff_flag = false;
        // ser_tx_busy is already true
        ser_tx(XOFF);           // Send XOFF.
        xon_sent = false;       // Receive disabled
    }
    else if (send_xon_flag)     // XON Needed?
    {
        send_xon_flag = false;
        // ser_tx_busy is already true
        ser_tx(XON);            // Send XON.
        xon_sent = true;        // Receive enabled
    }
    else if (xon_recvd)         // Send next char?
    {
        send_one();
    }
    else    // nothing to send
    {
        ser_tx_busy = false;    // done for now, at least
    }
} // tx_fn()


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

    ser_cli_buf [0] = '\0';         // Empty CLI buffer
    ser_cli_buf [L_SIZE] = '\0';    // Zero-terminate CLI buffer at the end
    rx_buf_get_idx = 0;             // Reset RX buffer starting index
    rx_buf_put_idx = 0;             // Reset RX buffer ending index
    rx_cnt = 0;                     // No characters received
    xon_recvd = true;               // Able to send at start.
    ser_tx_busy = false;            // Not transmitting at start.
    tx_q_get_idx = 0;               // Reset queue starting index
    tx_q_put_idx = 0;               // Reset queue ending index
    tx_str = NULL;                  // Make the string pointer empty.
    send_xoff_flag = false;         // No XOFF needed

    lcd_map(rx1_seg,0);             // Unmap LCD segment for RX
    lcd_map(tx1_seg,0);             // Unmap LCD segment for TX

    // Initialize register-level serial driver.
    ser_init(SERIAL_BAUD, rx_fn, NULL, tx_fn, NULL);

    send_xon();                     // XON needed once at start
    if(stm_start (&ser_xon_timer , STM_SEC(3) )) /* Restart the timer. */
        log_software();

    IRQ_ENABLE();                   // Restore the interrupt state.
}


/****************************************************************************
* Description:
*       Fn pointer for structure to get polymorphic interface to cli driver.
=*****************************************************************************/
const cli_driver_t serc1_cli_driver = {
    init, get_str, put_str, &ser_tx_busy
};


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

