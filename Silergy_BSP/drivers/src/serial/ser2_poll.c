/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 *
 * DESCRIPTION: Low Level Serial Routines - using polling
 *
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 *
 * $Id: ser2_poll.c 24489 2020-04-20 14:35:55Z dhb $  
 *----------------------------------------------------------------------------*/
#include <stdint.h>     /* Standard integer definitions. */
#include <stdbool.h>    /* Booleans for C99 */
#include "SY7_device.h" /* System definitions, device, pins, etc. */
#include "wd.h"         /* Watchdog feed routine. */
#include "ser2_poll.h"  /* Serial function declarations         */

const char fw_ver_ser2_poll[] =
"ser2 poll"    /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

/****************************************************************************
* Description:
*    Initialize UART pins, Baudrate.
* Parameters:
*   Input:
*         uint32_t baud: desired baud rate
*   Output:
*         None.
* Return Status:
*        None.
****************************************************************************/
void ser2_poll_init(uint32_t baud)
{
    // program the baud rate divider register as i.e. 10,000,000/9600 = 1041
    UART2->BAUD_b.baud_div = SystemCoreClock/baud;

	LCD->DIO_b[tx2_seg].alt = 1; // TX2
	LCD->DIO_b[rx2_seg].alt = 1; // RX2
	
    UART2->STATE = 0x0C;        // Clear buffer overrun states
    UART2->INT = 0x0F;          // Clear all interrupt flags

    // TX, RX enable 
    UART2->CNTL_b.rx_en = 1; 
    UART2->CNTL_b.tx_en = 1;
    // TX, RX interrupt disable
    UART2->CNTL_b.rx_ie = 0;
    UART2->CNTL_b.tx_ie = 0;
    // TX, RX overrun interrupt disable
    UART2->CNTL_b.rxov_ie = 0;
    UART2->CNTL_b.txov_ie = 0;
}

/****************************************************************************
* Description:
*    Write a character to Serial Port - blocking call (for printf - retarget.c)
* Parameters:
*   Input:
*         int c: character to be transmitted
*   Output:
*         None.
* Return Status:
*        None.
****************************************************************************/
int ser2_poll_tx_ch(int c)
{
    /* Wait if Transmit Holding register is full */
    while (UART2->STATE_b.txbf)
    {
        wd_reset();
    }
    /* write to transmit holding register		 */
    UART2->DATA = c;
    return (c);
}


/****************************************************************************
* Description:
*    Read character from Serial Port   (blocking read)  (for retarget.c)
* Parameters:
*   Input:
*         None.
*   Output:
*         Character received at the SER1 data buffer.
* Return Status:
*        None.
****************************************************************************/
int ser2_poll_rx_ch (void) 
{
    /* Wait if Receive Holding register is empty */
    while (!UART2->STATE_b.rxbf)
    {
        wd_reset();
    }
    return (UART2->DATA);
}

/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
