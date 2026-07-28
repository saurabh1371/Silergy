/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 *
 * DESCRIPTION: Low Level Serial Routines
 *
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 *
 * $Id: ser0.c 24489 2020-04-20 14:35:55Z dhb $
 *----------------------------------------------------------------------------*/
#include <stdlib.h>     // Null needed.
#include <stdbool.h>
#include <stdint.h>     // Standard integer definitions.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "ser_common.h"
#include "ser0.h"       /* Serial function declarations */
#include "bitdef.h"
#include "Defines.h"
const char fw_ver_ser0[] =
"ser0"    /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

// SER0 Event Handlers
// Function pointer for Rx Callback
static rx_fnptr_t rx_cb;
// Function pointer for Rx Overrun Callback
static rx_ov_fnptr_t rx_ov_cb;
// Function pointer for Tx Callback
static tx_fnptr_t tx_cb;
// Function pointer for Tx Overrun Callback
static tx_ov_fnptr_t tx_ov_cb;

//For ASDAQ calibration protocol implementation

/****************************************************************************
* Description:
*    Initialize UART pins, Baudrate and registers a event handler function.
*    Event handler function is called when SER0 interrupt is raised.
* Parameters:
*   Input:
*         uint32_t baud: desired baud rate
*         rx_cb_f        : Pointer to the call-back function - Rx char
*         rx_ov_cb_f     : Pointer to the call-back function - Rx overrun
*         tx_cb_f        : Pointer to the call-back function - Tx done
*         tx_ov_cb_f     : Pointer to the call-back function - Tx overrun
*   Output:
*         None.
* Return Status:
*        None.
****************************************************************************/
void ser0_init (uint32_t baud, rx_fnptr_t rx_cb_f, rx_ov_fnptr_t rx_ov_cb_f,
                               tx_fnptr_t tx_cb_f, tx_ov_fnptr_t tx_ov_cb_f)
{
    UART0->CNTL = 0;        // Stop the device.

    // program the baud rate divider register as i.e. 10,000,000/9600 = 1041
    UART0->BAUD_b.baud_div = SystemCoreClock/baud;

    LCD->DIO_b[ rx0_seg ].alt = 1; // Set RX pin to alternate
    LCD->DIO_b[ tx0_seg ].alt = 1; // Set TX pin to alternate

    rx_cb = rx_cb_f; // callback when a new char is received
    rx_ov_cb = rx_ov_cb_f; // callback when rx overrun error occurs
    tx_cb = tx_cb_f; // callback when a character is sent
    tx_ov_cb = tx_ov_cb_f; // callback when tx overrun error occurs

    UART0->STATE = 0x0C; // Clear buffer overrun states
    UART0->INT = 0x0F;  // Clear all interrupt flags

    // TX, RX enable
    UART0->CNTL_b.rx_en = 1;
    UART0->CNTL_b.tx_en = 1;
    // TX, RX interrupt enable
    UART0->CNTL_b.rx_ie = 1;
    UART0->CNTL_b.tx_ie = 1;
    // TX, RX overrun interrupt enable
    UART0->CNTL_b.rxov_ie = 1;
    UART0->CNTL_b.txov_ie = 1;

    // Enable NVIC for RX0 and TX0
    NVIC_EnableIRQ(UARTTX0_IRQn);
    NVIC_EnableIRQ(UARTRX0_IRQn);
    // Enable NVIC for UART0 overflow (TX_OV and RX_OV share interrupt vector)
    NVIC_EnableIRQ(UARTOVF0_IRQn);

    NVIC_SetPriority( UARTTX0_IRQn, HIGH_PRIORITY );
    NVIC_SetPriority( UARTRX0_IRQn, HIGH_PRIORITY );
    NVIC_SetPriority( UARTOVF0_IRQn, HIGH_PRIORITY );

    return;
}

/****************************************************************************
* Description:
*    Deinitialize UART.
*    Pins are not affected, because they may be LCD.
* Parameters:
*   Input:
*   Output:
* Return Status:
*        None.
****************************************************************************/
void ser0_deinit (void)
{
    UART0->CNTL = 0;        // Stop the device.
    UART0->STATE = 0x0C;    // Clear buffer overrun states
    UART0->INT = 0x0F;      // Clear all interrupt flags
}

/****************************************************************************
* Description:
*    Write a character to Serial Port - non-blocking call
* Parameters:
*   Input:
*         int c: character to be transmitted
*   Output:
*         None.
* Return Status:
*        None.
****************************************************************************/
int ser0_tx_ch(int c)
{
    UART0->DATA = c;    /* write to transmit holding register   */
    return c;
}


/*****************************************************************************
* Description:
*         SER0 Tx interrupt handler. Clears Tx interrupt bit and
*         calls user-defined event handler function.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
******************************************************************************/
void UARTTX0_Handler (void)
{
    if (UART0->INT_b.tx_i)     // Tx interrupt
    {
        UART0->INT = TX_INT_ACK; // Clear Tx interrupt flag

        if(NULL != tx_cb) // If call back is registered
            tx_cb(); // Notify Tx is free by calling event handler function
    }
}


/*****************************************************************************
* Description:
*         SER0 Rx interrupt handler. Clears Rx interrupt bit,
*         calls user-defined event handler function, passes character received.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
******************************************************************************/
void UARTRX0_Handler (void)
{
    uint8_t c;
	
    if (UART0->INT_b.rx_i)         // Rx interrupt
    {
        c = UART0->DATA;
        UART0->INT = RX_INT_ACK; // Clear Rx interrupt flag

        if(NULL != rx_cb) // If call back is registered
        {
            rx_cb(c); // Pass character received to event handler function
        }
    }
}


/*****************************************************************************
* Description:
*         SER0 overrun interrupt handler. Clears overrun interrupt bit,
*         calls user-defined event handler function.
*         TX_OV and RX_OV share the same interrupt handler.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
******************************************************************************/
void UARTOVF0_Handler (void)
{
    if (UART0->INT_b.rxov_i) // RX Overrun occurred
    {
        UART0->INT = RX_OV_INT_ACK; // Clear RX_OV interrupt flag

        if(NULL != rx_ov_cb) // If call back is registered
            rx_ov_cb(); // Notify error by calling event handler function
    }

    if (UART0->INT_b.txov_i) // TX Overrun occurred
    {
        UART0->INT = TX_OV_INT_ACK; // Clear TX_OV interrupt flag

        if(NULL != tx_ov_cb) // If call back is registered
            tx_ov_cb(); // Notify error by calling event handler function
    }
}


/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
