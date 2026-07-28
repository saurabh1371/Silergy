/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Common declarations for serial 
 * $Id: ser_common.h 24470 2020-04-20 12:11:56Z dhb $  
 ***************************************************************************/

#ifndef SER_COMMON_H     
#define SER_COMMON_H

#include <stdint.h>

#define ERR_SER_OK 0
#define ERR_TX_OV  BIT0
#define ERR_RX_OV  BIT1

#define TX_INT_ACK 		BIT0
#define RX_INT_ACK 		BIT1
#define TX_OV_INT_ACK BIT2
#define RX_OV_INT_ACK	BIT3

// Function pointer for Rx Callback 
typedef void (*rx_fnptr_t)(uint8_t rx_char);
// Function pointer for Rx Overrun Callback 
typedef void (*rx_ov_fnptr_t)(void);
// Function pointer for Tx Callback 
typedef void (*tx_fnptr_t)(void);
// Function pointer for Tx Overrun Callback 
typedef void (*tx_ov_fnptr_t)(void);

#endif //SER_COMMON_H


/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
