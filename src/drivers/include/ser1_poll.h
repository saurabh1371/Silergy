/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Serial function declarations
 * $Id: ser1_poll.h 15596 2017-10-16 20:51:48Z ptanwar $  
 ***************************************************************************/

#ifndef SER1_POLL_H     
#define SER1_POLL_H

// Initializes UART pins, Baudrate
extern void ser1_poll_init(uint32_t baud);

// Write a character to Serial Port (blocking call)
extern int ser1_poll_tx_ch(int c);
// Read character from Serial Port   (blocking read) 
extern int ser1_poll_rx_ch (void);

#endif //SER1_POLL_H


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
