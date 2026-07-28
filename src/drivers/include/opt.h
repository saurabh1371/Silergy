/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Optical UART (IRDA and Optical PWM) header.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: opt.h 16212 2017-12-08 00:31:28Z ptanwar $
 ****************************************************************************/
#ifndef OPT_H
#define OPT_H

#define OPT_NARROW_PWIDTH 	(1.0/16.10) // Consumes less power, send small pulse
#define OPT_STANDARD_PWIDTH (3.0/16.10) // Standard pulse width for IrDA

// FW version 
extern const char fw_ver_opt[];

// Initialize Optical Receiver (IRDA).
int opt_rx_init (uint32_t baud); 


// Initialize Optical Transmitter (OPT PWM)
int opt_tx_init(int baud, float pwidth); 

// de-initialize Optical Rx(IRDA) and Tx().
int opt_deinit(void); 


#endif // OPT_H

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
