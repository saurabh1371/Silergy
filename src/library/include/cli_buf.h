/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2005-2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Provides character-based I/O for numbers and strings.
 *
 * $Id: cli_buf.h 16212 2017-12-08 00:31:28Z ptanwar $
 ***************************************************************************/
#ifndef SER_H
#define SER_H 1

// Firmware Device Driver version
extern const char fw_ver_cli[]; 

// The init and deinit are handled by the cli. This is a stateless
// I/O library, not a driver.

// Attach the I/O routines to the port and its buffer.
// idx is the interface's index.
// cli_driver is the lower-lever driver's description structure.
// line is a zero-terminated line of text, the input to the CLI.
// If the line isn't done, this is supposed to be a NULL.
// buf is the buffer to use for output. The CLI is strictly half-duplex,
// so the buffer can be the same space that contained the line.
// len is the length of the buffer in bytes. If zero, the buffer can't
// be used for output, because it's in use.
void cli_attach(int idx, const cli_driver_t *cli_driver, 
        char* line, char *buf, int len);

// Detach the I/O routines from the port and its buffer.
// This is supposed to make the IO routines fail harmlessly. It's safety code.
void cli_detach(void);

// Put a zero-terminated string out the current I/O device.
// This is the only output to the lower-level driver.
void cli_put_str(const char *str);

// Recycle the output buffer.
void cli_buf_new(void);

/* Put a character out to the current port's buffer. */
void cli_put_chr (char c);
void cli_put_crlf (void);       /* Put <CR><LF> to the UART. */
//uint8_t cli_tx_busy(void);
void cli_flush (void);          /* Force buffered data into the I/O system. */
void cli_put_end_of_line(void); /* Waits till line is sent. */

/* Get a character from the line passed in ser-attach(). */
int cli_get_chr (void);         /* Get next character from CLI buffer. */
void cli_unget_chr (void);      /* Safely go back by one character. */
int cli_get_upper (void); /* Get next character. Force alpha to uppercase. */

/* Get next decimal (or hex) digit from CLI buffer. Ignore non numeric chars. */
int cli_get_digit (void);       /* Get next hexadecimal digit. */

int32_t cli_get_32 (void);      /* Get a 32 bit number in either base. */
int16_t cli_get_16 (void);      /* get a 16 bit number in either base. */
int8_t cli_get_8 (void);        /* Get an 8 bit number in either base. */
int32_t cli_get_32b10 (uint8_t c);      /* get a 32 bit number in base 10. */
int16_t cli_get_16b10 (void);   /* get a 32 bit number in base 10. */
int8_t cli_get_8b10 (void);     /* Get an 8 bit number in base 10. */
uint32_t cli_get_32b16 (void);  /* get a 32 bit number in base 16. */
uint8_t cli_get_8b16 (void);    /* Get an 8 bit number in base 16. */

void cli_put_digit (int c); /* Put a digit. */
void cli_put_32 (int32_t n, int8_t size, uint8_t base); /* put n, size digits, in base */
void cli_put_32b10 (int32_t n); /* Put a 32 bit number in base 10. */
void cli_put_16b10 (int16_t n); /* Put a 16 bit number in base 10. */
void cli_put_8b10 (int8_t n);   /* Put an 8 bit number in base 10. */
void cli_put_32b16 (uint32_t i);/* Put a 32-bit number in base 16. */
void cli_put_16b16 (uint16_t w);/* Put a 16-bit number in base 16. */
void cli_put_8b16 (uint8_t c);  /* Put an 8 bit number in base 16. */

/* Put a single-precision float in base 10 with p fraction digits. */
void cli_put_fb10 (float f, int8_t p);

/* Put the time on the currently attached serial port.  
 * tm_ptr is the time. */
void cli_put_tm (struct tm *tm_ptr);

//extern bool cli_timeout;        /* 1 = leave I/O wait loops. */
//extern uint8_t cli_port_idx;    // current port in use by serial system.


/***************************************************************************
 * Copyright (C) 2005-2013 Silergy Corp.
 * All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif
