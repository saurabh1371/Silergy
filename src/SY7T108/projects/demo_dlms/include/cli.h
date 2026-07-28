/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2005-2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: POWER METER - Command Line Interface Routines.
 *
 * $Id: cli.h 25648 2020-06-10 11:00:29Z phuddar $
 ***************************************************************************/
#ifndef CLI_H
#define CLI_H 1

#define CLI_ENABLE    0
#if CLI_ENABLE
#define CLI_STUB      0
#else
#define CLI_STUB      1
#endif // #if CLI_ENABLE
/***********************************/
// Serial PORT Settings
/***********************************/
#ifndef SERIAL_BAUD
#define SERIAL_BAUD 9600 // 9600 bps 
#endif
#define SER_ECHO 1
#define XON_TIMEOUT 5
/***********************************/

// Define buffer and queue sizes for command line interfaces.
#define Q_SIZE      32                  // Circular pointer queue size
#define I_SIZE      32                  // Input buffer size
#define I_MARGIN    (I_SIZE/4)          // Margin for xon/xoff 
#define L_SIZE      80                  // Line buffer size

typedef struct { // The function pointers of a CLI driver.

    // Get string from the input, and also the available output buffer.
    void (*init)(void);

    // Get string from the input, and also the available output buffer.
    char *(*get_str)(char **buf, int *buf_len);

    // Puts a string to the output device. Up to 32 strings can be queued.
    int (*put_str)(const char *str);

    // Read whether the output is done. Zero = done.
    volatile int *tx_done_ptr;

} cli_driver_t;

extern const cli_driver_t *cli_driver_ary[];

extern const cli_driver_t *cli_driver;

/* initialize the cli */
void cli_init(void);

/* put out the version, if needed */
void cli_put_version(void);

/* run the CLI, and return true if any typing occurred. */
bool cli_run(void);

/* read and write MPU variables */
int cli_fetch(int adr);
void cli_store(int adr, int val);


/***************************************************************************
 * Copyright (C) 2005..2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif

