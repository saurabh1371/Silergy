/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Serial function declarations
 * $Id: ser0.h 24469 2020-04-20 11:55:03Z dhb $  
 ***************************************************************************/

#ifndef SER0_H     
#define SER0_H 1

#include <stdint.h>
#include "ser_common.h"

extern const char fw_ver_ser0[];

// Initializes UART pins, Baud rate
extern void ser0_init(uint32_t baud, rx_fnptr_t rx_cb, rx_ov_fnptr_t rx_ov_cb, 
                                     tx_fnptr_t tx_cb, tx_ov_fnptr_t tx_ov_cb);
// De-initialize UART pins, uart.
extern void ser0_deinit(void);
// Write a character to Serial Port (non-blocking call)
extern int ser0_tx_ch(int c);

#endif //SER0_H


/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
