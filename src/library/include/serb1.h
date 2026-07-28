/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Sends and receives binary on UART1.
 *
 * The driver sends data from an array, and receives it into an array.
 * It is full-duplex.
 * It does not use XON/XOFF or other flow control.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 * $Id: serb1.h 16212 2017-12-08 00:31:28Z ptanwar $
 ****************************************************************************/
#ifndef SERB1_H
#define SERB1_H 1


// Library version
extern const char fw_ver_serb1[]; 


// These are public to enable monitoring.
extern volatile uint8_t *tx1_ptr;  // The next byte to transmit.
extern volatile uint8_t *rx1_ptr;  // The next byte-address of input data.
extern volatile int tx1_cnt;       // Count of untransmitted bytes (0 = done)
extern volatile int rx1_cnt;       // Count of received bytes.


// Fill an input buffer with received binary data.
// Parameters are a Pointer and length of the buffer to receive data.
void ser1_rx (uint8_t *buf_ptr, int len);

// Send an output buffer of binary data.
// Parameters are a Pointer and length of the buffer with data to send.
void ser1_tx (uint8_t *buf_ptr, int len);

// Initializes the serial module's variables and hardware.
// Takes a BAUD rate. Transmit and receive are 8N1, a hardware limitation.
// 7E1 and 7O1 can be done by replacing the MSB of the byte with parity.
void serb1_init (int baud);

#endif
/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

