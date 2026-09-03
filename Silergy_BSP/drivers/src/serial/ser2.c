/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 *
 * DESCRIPTION: Low Level Serial Routines
 *
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 *
 * $Id: ser2.c 24489 2020-04-20 14:35:55Z dhb $
 *----------------------------------------------------------------------------*/
#include <stdlib.h> // Null needed.
#include <stdint.h> // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "ser_common.h"
#include "ser2.h" /* Serial function declarations */
#include "bitdef.h"
#include "Prototypes.h"
#include "dlms_stack.h"
#include "wd.h"

const char fw_ver_ser2[] =
    "ser2" /* Device Driver name */
    " v"   /* ver string */
    "1.00" /* Major_ver.Minor_ver */
    "\r\n" /* CR + LF */
    ;

// SER2 Event Handlers
// Function pointer for Rx Callback
static rx_fnptr_t rx_cb;
// Function pointer for Rx Overrun Callback
static rx_ov_fnptr_t rx_ov_cb;
// Function pointer for Tx Callback
static tx_fnptr_t tx_cb;
// Function pointer for Tx Overrun Callback
static tx_ov_fnptr_t tx_ov_cb;

// For ASDAQ calibration protocol implementation
extern unsigned char recv_buf[25], recv_ctr, recv_ovf;
extern unsigned char enable_recv_buf;

volatile bool uart2_tx_ready = true;

/*
extern unsigned char transmit_complete;
extern unsigned char comm_delay_ctr;
extern unsigned long int comm_checksum;
extern unsigned char checksum, checksum_calc;

extern unsigned long int meter_no;
extern unsigned char pass_step,pass_ctr;
extern unsigned int vrms_reg3, vrms_reg3_actual, irms_reg3, irms1_reg3, irms2_reg3, kw_reg3, kva_reg3, pf_reg3, temp_reg3, freq_reg3;
extern unsigned char channel;
extern unsigned int volt_fact, ampp_fact,ampn_fact, pf_fact, freq_fact, kw_fact,n_cfdenn_val,n_cfdenp_val,m_cfden_val,iagain_val, ibgain_val;
extern int ph_val, iaph_val, ibph_val;
*/
// extern unsigned long real_time, real_date;//,prev_real_date;
// extern unsigned long int scratch, scratch1;

/****************************************************************************
 * Description:
 *    Initialize UART pins, Baudrate and registers a event handler function.
 *    Event handler function is called when SER2 interrupt is raised.
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
void ser2_init(uint32_t baud, rx_fnptr_t rx_cb_f, rx_ov_fnptr_t rx_ov_cb_f,
               tx_fnptr_t tx_cb_f, tx_ov_fnptr_t tx_ov_cb_f)
{
    UART2->CNTL = 0; // Stop the device.

    // program the baud rate divider register as i.e. 10,000,000/9600 = 1041
    UART2->BAUD_b.baud_div = SystemCoreClock / baud;

    LCD->DIO_b[rx2_seg].alt = 1; // Set RX pin to alternate
    LCD->DIO_b[tx2_seg].alt = 1; // Set TX pin to alternate

    rx_cb = rx_cb_f;       // callback when a new char is received
    rx_ov_cb = rx_ov_cb_f; // callback when rx overrun error occurs
    tx_cb = tx_cb_f;       // callback when a character is sent
    tx_ov_cb = tx_ov_cb_f; // callback when tx overrun error occurs

    UART2->STATE = 0x0C; // Clear buffer overrun states
    UART2->INT = 0x0F;   // Clear all interrupt flags

    // TX, RX enable
    UART2->CNTL_b.rx_en = 1;
    UART2->CNTL_b.tx_en = 1;
    // TX, RX interrupt enable
    UART2->CNTL_b.rx_ie = 1;
    UART2->CNTL_b.tx_ie = 1;
    // TX, RX overrun interrupt enable
    UART2->CNTL_b.rxov_ie = 1;
    UART2->CNTL_b.txov_ie = 1;

    // Enable NVIC for RX2 and TX2
    NVIC_EnableIRQ(UARTTX2_IRQn);
    NVIC_EnableIRQ(UARTRX2_IRQn);
    // Enable NVIC for UART2 overflow (TX_OV and RX_OV share interrupt vector)
    NVIC_EnableIRQ(UARTOVF2_IRQn);

    NVIC_SetPriority(UARTTX2_IRQn, HIGH_PRIORITY);
    NVIC_SetPriority(UARTRX2_IRQn, HIGH_PRIORITY);
    NVIC_SetPriority(UARTOVF2_IRQn, HIGH_PRIORITY);

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
void ser2_deinit(void)
{
    UART2->CNTL = 0;     // Stop the device.
    UART2->STATE = 0x0C; // Clear buffer overrun states
    UART2->INT = 0x0F;   // Clear all interrupt flags
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
int ser2_tx_ch(int c)
{
    // Wait here until the previous byte has physically left the register
    while (!uart2_tx_ready)
    {
        wd_reset(); // Keep the watchdog fed during long DLMS frame transmissions
    }

    uart2_tx_ready = false; // Lock the flag
    UART2->DATA = c;        // Send the new byte

    return c;
}

/*****************************************************************************
 * Description:
 *         SER2 Tx interrupt handler. Clears Tx interrupt bit and
 *         calls user-defined event handler function.
 * Parameters:
 *   Input:
 *         None.
 *   Output:
 *         None.
 * Return Status:
 *         None.
 ******************************************************************************/
void UARTTX2_Handler(void)
{
    if (UART2->INT_b.tx_i) // Tx interrupt
    {
        UART2->INT = TX_INT_ACK; // Clear Tx interrupt flag

        uart2_tx_ready = true; // Flag that hardware is free for the next byte

        if (NULL != tx_cb)
            tx_cb();
    }
}

/*****************************************************************************
 * Description:
 *         SER2 Rx interrupt handler. Clears Rx interrupt bit,
 *         calls user-defined event handler function, passes character received.
 * Parameters:
 *   Input:
 *         None.
 *   Output:
 *         None.
 * Return Status:
 *         None.
 ******************************************************************************/
void UARTRX2_Handler(void)
{
    uint8_t c;
    uint8_t recv_byte;

    if (UART2->INT_b.rx_i) // Rx interrupt
    {
        recv_byte = UART2->DATA;
        c = recv_byte;           // Prevent double-reading the hardware FIFO
        UART2->INT = RX_INT_ACK; // Clear Rx interrupt flag

        // --- FEED THE DLMS STACK ---
        DLMS_HDLC_ProcessRxByte(recv_byte);

        // Keep the legacy ASDAQ logic intact just in case
        if ((enable_recv_buf == 1) && (recv_ctr < 25))
        {
            recv_buf[recv_ctr] = recv_byte;
            recv_ctr++;
        }

        if (NULL != rx_cb) // If call back is registered
        {
            rx_cb(c); // Pass character received to event handler function
        }

        Communication_Enable_Counter = 0;
    }
}

/*****************************************************************************
 * Description:
 *         SER2 overrun interrupt handler. Clears overrun interrupt bit,
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
void UARTOVF2_Handler(void)
{
    if (UART2->INT_b.rxov_i) // RX Overrun occurred
    {
        UART2->INT = RX_OV_INT_ACK; // Clear RX_OV interrupt flag

        if (NULL != rx_ov_cb) // If call back is registered
            rx_ov_cb();       // Notify error by calling event handler function
    }

    if (UART2->INT_b.txov_i) // TX Overrun occurred
    {
        UART2->INT = TX_OV_INT_ACK; // Clear TX_OV interrupt flag

        if (NULL != tx_ov_cb) // If call back is registered
            tx_ov_cb();       // Notify error by calling event handler function
    }
}

/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
