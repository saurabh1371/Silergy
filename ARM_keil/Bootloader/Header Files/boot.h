/****************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 *
 * DESCRIPTION: Boot-loader utility routines. 
 *
 *
 * $Id: boot.h 25396 2020-05-19 21:09:02Z ptanwar $
 *
 ****************************************************************************/
 
// Which UARTx to be used? 
#ifdef SER0
#define UART UART0
#define tx_seg tx0_seg
#define rx_seg rx0_seg
#endif
#ifdef SER1
#define UART UART1
#define tx_seg tx1_seg
#define rx_seg rx1_seg
#endif
#ifdef SER2
#define UART UART2
#define tx_seg tx2_seg
#define rx_seg rx2_seg
#endif
#ifdef IRDA
#define UART UART2
#define tx_seg tx2_seg
#define rx_seg rx2_seg
#endif

// Initializes UART pins, Baudrate
extern void ser_poll_init(uint32_t baud);
// Write a character to Serial Port (blocking call)
extern int ser_poll_tx_ch(int c);
// Read character from Serial Port   (blocking read) 
extern int ser_poll_rx_ch (void);
// Read character from Serial Port   (blocking read) 
extern int ser_poll_rx_str (char *str);
// Read character from Serial Port but don't wait endlessly
extern int ser_poll_rx_ch_no_wd(void);

#define APP_START (0x800UL) // Where the App begin in flash?
extern const uint32_t app_start_addr; // Accessed in startup assembly file also
#define RAM_START 0x20000000 // Physical address of beginig on RAM in this IC
#define CHECKCODE 0x875A875A // Checkcode when bootloader is commanded
#define CHKCODE_ADDR (FLASH_SIZE - FLASH_PAGE_SZ -4) 

/* Watchdog reset */
#define RESET_WD() TEMP->CNTL_b.wd_clr = 1

// Definitions for Intel HEX Records.
#define DATA_REC    0x00                // Data record.
#define EOF_REC     0x01                // End-of-file record.
#define ESA_REC     0x02                // Extended-Segment-Address record.
#define SSA_REC     0x03                // Start-Segment-Address record.
#define ELA_REC     0x04                // Extended-Linear-Address record (HEX-386).
#define SLA_REC     0x05                // Start-Linear-Address record (HEX-386).

// state of hex record parser.
typedef enum  { FIRST_REC_COLON,REC_COLON, REC_LEN, REC_ADDR, REC_TYPE, REC_DATA, 
                REC_CHKSUM, FLASH_RECORD } bstate_t;

// Typedef to store flash address.
typedef union Uint8_16_32_t \
   { uint8_t c[4]; uint16_t i[2]; uint32_t l; } uint8_16_32_t;
	 
// Array index used to access Uint8_16_32_t elements. 
#define HI 1
#define LO 0
#define HI_HI 3
#define HI_LO 2
#define LO_HI 1
#define LO_LO 0	 	 
         
// Definitions for various ASCII control characters
#define TO_UPPERCASE   0x20        // Converts Lowercase alphabet to uppercase.
#define LF             0x0A        // LINE FEED
#define CR             0x0D        // CARRIAGE RETURN
#define SPACE          0x20        // SPACE CHARACTER
#define XON            0x11        // Value of XON  
	 

/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

