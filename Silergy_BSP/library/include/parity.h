/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Sends and receives binary on UART0.
 *
 * The driver sends data from an array, and receives it into an array.
 * It is full-duplex.
 * It does not use XON/XOFF or other flow control.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 * $Id: parity.h 16212 2017-12-08 00:31:28Z ptanwar $
 ****************************************************************************/
#ifndef PARITY_H
#define PARITY_H 1


// Library version
extern const char fw_ver_parity[]; 


// Returns 1 if even parity.
int parity_even (uint8_t b);

// Returns 1 if odd parity.
int parity_odd (uint8_t b);

#endif
/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

