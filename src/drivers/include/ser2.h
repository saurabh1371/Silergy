/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Serial function declarations
 * $Id: ser2.h 17103 2018-03-26 18:33:08Z rvandewa $  
 ***************************************************************************/

#ifndef SER2_H     
#define SER2_H 1

extern const char fw_ver_ser2[];

// Initializes UART pins, Baudrate
extern void ser2_init(uint32_t baud, rx_fnptr_t rx_cb, rx_ov_fnptr_t rx_ov_cb, 
                                     tx_fnptr_t tx_cb, tx_ov_fnptr_t tx_ov_cb);
// De-initialize UART pins, uart.
extern void ser2_deinit(void);
// Write a character to Serial Port (non-blocking call)
extern int ser2_tx_ch(int c);

#endif //SER2_H


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
