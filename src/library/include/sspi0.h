/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Sends and receives on UART0.
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
 * $Id: sspi0.h 16026 2017-11-23 02:49:23Z rvandewa $
 ****************************************************************************/
#ifndef SSPI0_H
#define SSPI0_H


// Library version
extern const char fw_ver_sspi0[]; 


// Range 0x20000400 to 0x200004FC is accessible in SPI_Safe mode also 
#define SPIS_MBOX_ADDR 0x20000400 
#define SPIS_MBOX_TX_ADDR 0x20000480
#define SPIS_MBOX_STATUS_ADDR = (SPIS_MBOX_ADDR + 0xFC)

// This structure has function pointers to implement the driver.
extern const cli_driver_t sspi0_cli_driver;

#endif //SSPI0_H
/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

