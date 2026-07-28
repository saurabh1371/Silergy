/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Provides character-based I/O for numbers and strings
 * 
 * Design: This is a buffering system for cooked I/O, i.e. a command line
 * interface.  It can use a half duplex line buffer for I/O on each line. 
 * It uses less code and less RAM than printf.
 * This I/O can also be used with any ASCII I/O device, not just serial ports.
 *
 * How it works:
 * The "get line" function is called repeatedly.  It performs line editing.
 * When a line is complete, it calls "cli_attach()" , to 
 * attach the command line interface to the line buffer.
 * The command line interface very quickly processes the input line, then
 * places numeric output as strings in the line buffer. 
 * Constant strings take no space in the output buffer,
 * because they are simply queued for output in the driver.
 * The "cli_put_str()" operation is called to queue strings for output.
 * This includes the strings representing numbers, which are in the command line
 * buffer.
 * The "cli_detach()" function is called after the CLI has buffered the data.  
 * The driver's interrupts then send all the queued strings.
 * After the driver's interrupt sends all the output, the driver enables
 * input again, and the process repeats.
 *
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * $Id: cli_buf.c 19937 2019-01-08 23:20:51Z rvandewa $
 ****************************************************************************/
#include <ctype.h>
#include <string.h>
#include "options.h"    // System definitions.
#include "cli.h"        // Define cli_driver_t, the I/O driver.
#include "cli_buf_str.h"    // Strings used in this module.
#include "cli_buf.h"        // Test function definitions.


/*** Version ****/
const char fw_ver_cli[] =
"cli_buf"/* Library file name */
" v"     /* ver string */
"1.00"   /* Major_ver.Minor_ver */
"\r\n"   /* CR + LF */
;



/*** Public variables used within this module ***/

#define CH_CNT_CMD_SAVABLE (L_SIZE - 10)  /* Characters saved in line buffer. */

const cli_driver_t *cli_driver_ptr; // Which I/O the data is for.
int cli_io_idx = -1;                // Which I/O device is attached.

/*** Private functions declared within this module ***/
void put_str(const char *str);


/*** Private variables used within this module ***/
static char * ch_ptr_in;          // after attaching, where to get data
static char * ch_ptr_in_prev;     // the former place to get data (for unget)

static char * ch_ptr_out;         // Where to put the next character.
static char * ch_ptr_out_str;     // The start of the most recent string.
static char * ch_ptr_out_base;    // The first possible place to put data.
static char * ch_ptr_out_max;     // The last possible place to put data.
static char * ch_ptr_out_num;     // The start of the most recent number.


/****************************************************************************
* Description:
*       Put a string out, located in flash, to the serial driver.
*       Always called from the main loop.
* Input:
* Parameters:
*       str is a pointer to a zero-terminated C string in flash. Note that
*       since the driver sends strings, data in flash takes no space in
*       a RAM serial buffer, and can be as long as needed.
* Output:
*       The string pointer is moved into a ciricular queue of strings
*       that the serial interrupt of the current UART puts out.
* Return Status:
*       None
*****************************************************************************/
void put_str(const char *str) 
{
    if(NULL == cli_driver_ptr)
        return;
    (*(cli_driver_ptr->put_str))(str);
}


/****************************************************************************
* Description:
*       Is the serial driver's transmit busy?
* Input:
*       It uses the serial port index.
* Parameters:
*       None.
* Output:
* Return Status:
*       Returns true it the transmitter is busy.
*****************************************************************************/
int cli_tx_busy(void)
{
    int buf_len;
    char *buf;

    if(NULL == cli_driver_ptr)
        return true;

    (*(cli_driver_ptr->get_str))(&buf, &buf_len);
    return (0 == buf_len) ? true : false;
}


/****************************************************************************
* Description:
*       Put out any buffered data.
* Input:
*       The serial line buffer, and associated global variables,
*       including especially the port index, which tells which uart to start.
* Parameters:
*       None.
* Output:
*       Puts a string into the current serial port's queue of strings.
* Return Status:
*       None.
*****************************************************************************/
void cli_flush (void)
{
    // If there's output data after the previous output position. 
    if (ch_ptr_out > ch_ptr_out_str)
    {
        // Terminate the current output string.
        cli_put_chr('\0');
        // Send the current string to the current output device.
        put_str(ch_ptr_out_str);
        // Start the next output string.
        ch_ptr_out_str = ch_ptr_out;
    }
}


/****************************************************************************
* Description:
*       Attach the I/O routines to the input and output buffer of a
*       particular UART, so that the command line interface can
*       parse the command... and then put I/O into the same line buffer.
* Input:
* Parameters:
*       port_idx is the index of the serial port.
*       buf points to the serial port's line buffer (this is the routine
*           in which a line buffer is passed to a serial driver.)
*       len is the length of the buffer in characters.
* Output:
*       The CLI's serial I/O can reach a line buffer.
*       The serial driver starts to use the line buffer.
* Return Status:
*       None.
*****************************************************************************/
void cli_attach(int idx, const cli_driver_t *cli_driver, 
        char* line, char *buf, int len)
{
    int cmd_len;

    cli_io_idx = idx;               // Remember the I/O index.
    cli_driver_ptr = cli_driver;    // and the current I/O device(s).

    ch_ptr_in = line;               // Set next char to read
    ch_ptr_in_prev = line;          // and previous char (for ungetc)

    if (NULL == line)               // if no data in, then
        cmd_len = 0;                // the input buffer length is zero.
    else
        cmd_len = 1 + strlen(line); // Find the command's length.

    if(cmd_len < CH_CNT_CMD_SAVABLE)        // If the command is short.
    {
        // Save the short command for reuse by ','.
        buf += cmd_len;
        len -= cmd_len;
    }
    ch_ptr_out_max = buf + len;
    // If there is room for the output buffer.
    if(0 < len)
        *ch_ptr_out_max = '\0';     // Terminate output. (safety code)

    ch_ptr_out_base = buf;          // Find the start of the output.
    ch_ptr_out = ch_ptr_out_base;   // Start the output.
    ch_ptr_out_str = ch_ptr_out;    // Start the most recent string.
}


/****************************************************************************
* Description:
*       Recycle the line buffer.
* Input:
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
void cli_buf_new(void)
{
    ch_ptr_out = ch_ptr_out_base;   // Start the output.
    ch_ptr_out_str = ch_ptr_out;    // Start the most recent string.
}


/****************************************************************************
* Description:
*       Detach the CLI I/O routines from any input or output buffer.
*       In this mode, the CLI IO routines fail harmlessly.
* Input:
* Parameters:
*       None.
* Output:
*       The CLI I/O globals are set to refuse buffered data.
* Return Status:
*       None.
*****************************************************************************/
void cli_detach(void)
{
    cli_flush();            // put out any buffered data.

    cli_io_idx = -1;        // no current I/O device 
    cli_driver_ptr = NULL;

    ch_ptr_in = NULL;       // No input data
    ch_ptr_in_prev = NULL;

    ch_ptr_out_base = NULL; // No output buffer.
    ch_ptr_out_max = NULL;
    ch_ptr_out = NULL;
    ch_ptr_out_str = NULL;
    ch_ptr_out_num = NULL;
}


/****************************************************************************
* Description:
*       Put a string out, located in RAM, to the current port.
* Input:
* Parameters:
*       str is a zero-terminated string in RAM.
* Output:
*       The string is placed in the current uart's queue of strings to send.
* Return Status:
*       None.
*****************************************************************************/
void cli_put_str(const char *str)
{
    cli_flush();    // Put out any buffered data.
    put_str(str);   // Put out the string.
}


/****************************************************************************
* Description:
*       This routine waits for data to be sent,
*       enabling an infinite amount of serial output.
*       However, it starves the rest of the meter for CPU time.
* Input:
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
void cli_put_end_of_line(void)
{
    if(NULL == cli_driver_ptr)
        return;

    cli_flush();    // Put out any buffered data.

    // Wait for the I/O to be done.
    while (0 != *(cli_driver_ptr->tx_done_ptr))
    {
        // Ideally, one should run the rest of the meter here.
    }

    cli_buf_new();
}


/****************************************************************************
* Description:
*       Send <CR><LF> to UART.
* Input:
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
void cli_put_crlf (void)
{
    cli_put_str("\r\n");
}

/****************************************************************************
* Description:
*       Put a character out (into the line buffer).
*       This is the basic output routine called by all the others.
* Input:
* Parameters:
*       c is a character to put out to the string, in the line buffer, 
*           whcih string is eventually sent to the current UART
* Output:
*       c is, eventually put out, as part of a string in the line buffer.
* Return Status:
*****************************************************************************/
void cli_put_chr (char c)
{
    // This safety code is to prevent memory corruption if EMI upsets
    // the PC. Most of the time, there is no I/O and NULL == ch ptr out.
    if (NULL == ch_ptr_out // If the I/O is detached, don't send.
        // If sending beyond the end.
        || ch_ptr_out >= ch_ptr_out_max
        // If the pointers are backwards.
        || ch_ptr_out < ch_ptr_out_base)
    {
        log_software();       // Buffer full!  Tell somebody!
        return;
    }
    *ch_ptr_out++ = c; // Put the char in the buffer.
}


/****************************************************************************
* Description:
*       Get a char from the line buffer.
* Input:
*       The line buffer, filled with data by the half-duplex serial
*       driver.  The global pointers are attached when it finds data
*       in the line buffer in cli_get_line().
* Parameters:
*       None.
* Output:
* Return Status:
*       returns the next char of input.
*****************************************************************************/
int cli_get_chr (void)    // Get next character from CLI buffer.
{
    int ch;

    ch_ptr_in_prev = ch_ptr_in; // Provide a safe way to go backward.
    if (NULL == ch_ptr_in)      // out of data? Or, no device attached?
        return '\0';            // End of a string.
    else
    {
        ch = (int)(uint8_t)*ch_ptr_in++;   // Get the next char.
        // Stay inside the data.
        if ('\0' == ch)
        {
            --ch_ptr_in;
        }
        return ch;
    }
}

/****************************************************************************
* Description:
*       Un-get a character.  This lets the parsing restart, for example
*       if a numeric conversion fails.
* Input:
*       It uses an internal global pointer to step backwards.
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
void cli_unget_chr (void)    // Safely go back by one character.
{
    ch_ptr_in = ch_ptr_in_prev;
}

/****************************************************************************
* Description:
*       Get a char, and force it to be upper case.  This simplifies
*       parsing commands.
* Input:
* Parameters:
*       None.
* Output:
* Return Status:
*       returns the next char of input.
*****************************************************************************/
int cli_get_upper (void)
{
    return (toupper ( cli_get_chr () ) );
}

/****************************************************************************
* Description:
*       Get next decimal (or hex) digit from CLI buffer.
* Input:
* Parameters:
*       None.
* Output:
* Return Status:
*       returns the next char of input.
*****************************************************************************/
int cli_get_digit (void)
{
    int c;

    if (isxdigit (c = cli_get_upper ()))
    {
       c -= '0';                        // '0' mapped to  0;

       if (c >= 10)
       {
          c -= ('A' - '0') - 10;        // 'A' mapped to 10.
       }
    }
    else
       c = 0x7f;      

    return (c);
}

/****************************************************************************
* Description:
*       Get a 32-bit base 10 number from serial input.
* Input:
*       The buffer...
* Parameters:
*       c is the first char of the number.  A caller can force it to be
*           positive by always passing '+'.
* Output:
* Return Status:
*****************************************************************************/
int32_t cli_get_32b10 (uint8_t c)
{
    bool sign;
    int i;
    int n;

    // Maximum number of digits allowed for decimal input.
    i = 10;                             
    // Number to be returned.
    n = 0;

    sign = '-' == c;

    while (0 < i && (c = cli_get_digit ()) < 10)
    {
       // Convert ASCII decimal number to binary number.
       n = (n * 10) + c;
       i--;
    }

    // Unget last character, it wasn't a digit.
    if (0 != i) cli_unget_chr();

    if (sign)
       n = -n;

    return (n);
}

/****************************************************************************
* Description:
*       Get i digits of hexadecimal numerals and convert to an
*       unsigned binary number.
* Input:
*       Serial data.
* Parameters:
*       i is the number of digits to get.
* Output:
* Return Status:
*       An unsigned 32-bit number.
*****************************************************************************/
static uint32_t cli_get_b16 (uint8_t i)
{                             
    int c;
    unsigned int n;

    // Number to be returned.
    n = 0;
    while (i > 0 && (c = cli_get_digit ()) < 0x10)
    {
       // Convert ASCII hexadecimal number to binary number.
       n = (n << 4) + c;
       --i;
    }

    // if it didn't get all the desired digits
    // Unget last character, it wasn't a digit.
    if (i) cli_unget_chr();
    return (n);
}


/****************************************************************************
* Description:
*       Get 32 bit (8 digits) of hexadecimal numerals and convert to an
*       unsigned binary number.
* Input:
*       Serial data.
* Parameters:
*       None.
* Output:
* Return Status:
*       An unsigned 32-bit binary integer.
*****************************************************************************/
uint32_t cli_get_32b16 (void)
{
    return cli_get_b16(8);
}

// Convert ascii decimal (or hex) long to binary number.
/****************************************************************************
* Description:
*       Get 32 bit of hex or decimal numerals and convert to an
*       signed binary number.
* Input:
*       Serial data.
* Parameters:
*       None.
* Output:
* Return Status:
*       A signed 32-bit 2s complement integer.
*****************************************************************************/
int32_t cli_get_32 (void)
{
    uint8_t c;

    c = cli_get_chr ();

    if ('+' == c || '-' == c)
       return (cli_get_32b10 (c));
    else
    {
       cli_unget_chr();                         // Unget last character.
       return ((int32_t) cli_get_b16 (8));  // Default to hexadecimal input.
    }
}

/****************************************************************************
* Description:
*       get 16 bits of hex or decimal number.
* Input:
*       Serial data.
* Parameters:
*       None.
* Output:
* Return Status:
*       A signed 16-bit 2s complement integer.
*****************************************************************************/
int16_t cli_get_16 (void)
{
    return ((int16_t) cli_get_32 ());
}

/****************************************************************************
* Description:
*       get 8 bits of hex or decimal number.
* Input:
*       Serial data.
* Parameters:
*       None.
* Output:
* Return Status:
*       A signed 8-bit 2s complement integer.
*****************************************************************************/
int8_t cli_get_8 (void)
{
    return ((int8_t) cli_get_32 ());
}

/****************************************************************************
* Description:
*       get 16 bits of positive base 10 number.
* Input:
*       Serial data.
* Parameters:
*       None.
* Output:
* Return Status:
*       A signed 16-bit 2s complement integer.
*****************************************************************************/
int16_t cli_get_16b10 (void)
{
    return ((int16_t) cli_get_32b10 ('+'));
}

/****************************************************************************
* Description:
*       get 8 bits of positive base 10 number.
* Input:
*       Serial data.
* Parameters:
*       None.
* Output:
* Return Status:
*       A signed 8-bit 2s complement integer.
*****************************************************************************/
int8_t cli_get_8b10 (void)
{
    return ((int8_t) cli_get_32b10 ('+'));
}

/****************************************************************************
* Description:
*       get 8 bits of unsigned base 16 number.
* Input:
*       Serial data.
* Parameters:
*       None.
* Output:
* Return Status:
*       A 16-bit binary integer.
*****************************************************************************/
uint8_t cli_get_8b16 (void)    // Convert ascii hexdecimal byte to binary
{
    return ((uint8_t) cli_get_b16(2));
}

/****************************************************************************
* Description:
*       Converts an 8-bit number to a hex character.
* Input:
* Parameters:
*       c is an unsigned 4 bit number in an 8-bit byte.
* Output:
* Return Status:
*       a character 0-9, 'A'-'Z' (and beyond)
*****************************************************************************/
static int htoc (int c)
{
    if (c > 9)
       c += 'A' - 10;
    else
       c += '0';
    
    return (c);
}

/****************************************************************************
* Description:
*       Send single numeral to the serial port (i.e. into the line buffer,
*       which is eventually terminated as a string, and then the string
*       is send to the current UART's serial driver.)
* Input:
* Parameters:
*       An 8-bit number representing a single numeral.
* Output:
* Return Status:
*       None.
*****************************************************************************/
void cli_put_digit (int c)
{
    cli_put_chr (htoc (c));
}

/****************************************************************************
* Description:
*       Reverse a string. p1 < p2!  No effect when pointers are NULL.
* Input:
* Parameters:
*       p1 and p2 are pointers to the beginning and end of a string.
* Output:
*       a string that has been reversed in place.
* Return Status:
*       None.
*****************************************************************************/
static void strrev(char *p1, char *p2)
{
    char ch;

    // while the pointers have not met.
    while (p1 < p2)
    {
        // Interchange the beginning and end of a string.
        ch = *p2;
        // And also, step the pointers toward each other.
        *p2-- = *p1;
        *p1++ = ch;
    }
}

/****************************************************************************
* Description:
*   Reverse the digits of a number, and print it.  The number output
*   routines build the numbers from least significant to most significant.
*   That happens to be the opposite from the order that they are read.
* Input:
*   A reversed number string in the line buffer.
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
static void flush_num(void)
{
    if(NULL == ch_ptr_out)
			  return;
    // Reverse the numeric string. (No effect when pointers are NULLs).
    strrev(ch_ptr_out_num, (ch_ptr_out - 1));
}

/****************************************************************************
* Description:
*       Put a signed number out in any base.
* Input:
* Parameters:
*       n is the number to put out.
*       size is the minimum number of digits to use.  i.e. the number
*           of zero numerals to force.
*       base is the base, usually 10, but 2..16 would work OK.
* Output:
*       is a number string in the buffer without a zero-termination. So, it's
*           easy to add a space, comma, more numbers or some other decoration 
*           to the end. flush() terminates the string.
* Return Status:
*       None.
*****************************************************************************/
void cli_put_32 (int32_t n, int8_t size, uint8_t base)
{
    int minus,m;

    if (base > 16)
        base = 16;

    ch_ptr_out_num = ch_ptr_out;

    minus = (n < 0);
    if(minus)
        n = -n;

    // Compute digits in reverse order..LSD to MSD.
    for (; 0 < size || n != 0; --size)
    {
        m = n % base;
        cli_put_digit(m);
        n /= base;
    }

    if (minus)
    {
        cli_put_chr ('-');
    }

    *ch_ptr_out = '\0'; // Assure that the string ends.

    // Reverse digits.
    flush_num();
}

/****************************************************************************
* Description:
*       Put a hex number out.
* Input:
* Parameters:
*       n is the unsigned number to put out (even though it's signed.)
*       size is the minimum number of hex digits.  That is, the number
*           of zero numerals to force.
* Output:
*       is a number string in the buffer without a zero-termination. So, it's
*           easy to add a space, comma, more numbers or some other decoration 
*           to the end. flush() terminates the string.
* Return Status:
*       None.
*****************************************************************************/
static void cli_put_32_16 (int32_t n, int size)
{
    ch_ptr_out_num = ch_ptr_out;

    // Compute digits in reverse order..LSD to MSD.
    for (; 0 < size; --size)
    {
        cli_put_digit((uint8_t)(n & 0x0F));
        n >>= 4;
    }

    // Reverse digits, add space.
    flush_num();
}

/****************************************************************************
* Description:
*   Send a minimum-length 32-bit signed decimal
*   [-2,147,483,648...2,147,483,647] value to DTE.
* Input:
* Parameters:
*       n is a 32-bit 2's complement (signed) bianry integer.
* Output:
*       A number is the line buffer, but without a string termination.
* Return Status:
*       None.
*****************************************************************************/
void cli_put_32b10 (int32_t n)
{
    cli_put_32 (n, 1, 10);
}

/****************************************************************************
* Description:
*   Send a minimum-length 16-bit signed decimal [-32,768...32,767] value to DTE.
* Input:
* Parameters:
*       n is a 16-bit 2's complement (signed) bianry integer.
* Output:
*       A number is the line buffer, but without a string termination.
* Return Status:
*       None.
*****************************************************************************/
void cli_put_16b10 (int16_t n)                  // Send a [0, 65,536] value to DTE.
{
    cli_put_32 ((int32_t)n, 1, 10);
}

/****************************************************************************
* Description:
*   Send a minimum-length 8-bit signed decimal [-128...127] value to DTE.
* Input:
* Parameters:
*       n is an 8-bit 2's complement (signed) bianry integer.
* Output:
*       A number is the line buffer, but without a string termination.
* Return Status:
*       None.
*****************************************************************************/
void cli_put_8b10 (int8_t n)
{
    cli_put_32 ((int32_t)n, 1, 10);
}

/****************************************************************************
* Description:
*   Send a full-length 32-bit unsigned hexadecimal [00000000...
*   FFFFFFFF] value to DTE.
* Input:
* Parameters:
*       n is an 32-bit (unsigned) binary integer.
* Output:
*       A number is the line buffer, but without a string termination.
* Return Status:
*       None.
*****************************************************************************/
void cli_put_32b16 (uint32_t i)
{
    cli_put_32_16 ((int32_t)i, 8);
}

/****************************************************************************
* Description:
*   Send a full-length 16-bit unsigned hexadecimal [0000...FFFF] value to DTE.
* Input:
* Parameters:
*       n is an 16-bit (unsigned) binary integer.
* Output:
*       A number is the line buffer, but without a string termination.
* Return Status:
*       None.
*****************************************************************************/
void cli_put_16b16 (uint16_t w)
{
    cli_put_32_16 ((int32_t)w, 4);
}

/****************************************************************************
* Description:
*   Send a full-length 8-bit unsigned hexadecimal [00...FF] value to DTE.
* Input:
* Parameters:
*       n is an 8-bit (unsigned) binary integer.
* Output:
*       A number is the line buffer, but without a string termination.
* Return Status:
*       None.
*****************************************************************************/
void cli_put_8b16 (uint8_t c)
{
    cli_put_32_16 ((int32_t)c, 2);
}

/****************************************************************************
* Description:
*       Put a float out to DTE.
*       This routine intentionally does not produce scientific notation.
*       The idea is to produce a number that's easy to read.
* Input:
* Parameters:
*       f is a 32-bit single-precision floating-point number.
*       p is the number of fractional digits to display.
* Output:
*       A number is the line buffer, but without a string termination.
* Return Status:
*       None.
*****************************************************************************/
#define FLOAT_PRECISION 4   /* no real measurement needs more precision */
static const float f_d_ary[FLOAT_PRECISION+1] 
    = {1.0, 10.0, 100.0, 1000.0, 10000.0};
static const int32_t l_d_ary[FLOAT_PRECISION+1]
    = {1, 10, 100, 1000, 10000};
void cli_put_fb10 (float f, int8_t p)
{
    long l,l1;
    int8_t q;
    float f1;

    if(0.0 > f)
    {
        cli_put_chr('-');
        f = -f;
    }
    /* There's intentionally no scientific notation;
     * The display logic is supposed to produce real units. */
    q = p;
    if (q > FLOAT_PRECISION) {
        q = FLOAT_PRECISION;
    }
    if (q < 0) {
        q = 0;
    }
    l1 = l_d_ary[q];
    f1 = f_d_ary[q];
    l = lroundf(f * f1);
    cli_put_32 (l / l1, 1, 10); /* signed, non-fractional part */
    if (0 <= p) {   /* if p < 0, put no decimal point */
        cli_put_chr('.');
        if (0 < p) {    /* p > 0, put fraction digits */
            cli_put_32 (l % l1, q, 10); /* unsigned, fractional part */
        }
    }
}


/****************************************************************************
* Description:
*       Put the time on the currently attached serial port.  
* Input:
* Parameters:
*       tm_ptr is the time.
* Output:
*       A number is the line buffer, but without a string termination.
* Return Status:
*       None.
*****************************************************************************/
void cli_put_tm (struct tm *tm_ptr)
{
    int tmp;
    const char *ptr;
    cli_put_32(tm_ptr->tm_hour,2,10);   // Put out the hour.
    cli_put_str(colon);
    cli_put_32(tm_ptr->tm_min,2,10);    // Put out the minute.
    cli_put_str(colon);
    cli_put_32(tm_ptr->tm_sec,2,10);    // Put out the second.
    if(NULL != tm_ptr->tz)              // If there is a time zone string.
        if(isalpha(*tm_ptr->tz))
        {
            tmp = strlen(tm_ptr->tz);
            if(0 < tmp && tmp < 10)
                cli_put_str(tm_ptr->tz);    // Put out the time zone.
        }
    cli_put_chr (' ');
    ptr = week_day_str_ary[ tm_ptr->tm_wday ];  // Put the day of week.
    cli_put_str (ptr);
    ptr = month_str_ary[ tm_ptr->tm_mon ];  // Put the month of the year.
    cli_put_str (ptr);
    cli_put_32((int32_t)tm_ptr->tm_mday,2,10);  // Put the day of month.
    cli_put_str (comma_space);
    cli_put_32((int32_t)(RTC_ERA + tm_ptr->tm_year),4,10);  // Put the year.
    if (tm_ptr->tm_isvalid) {               // Show if the time is valid.
        cli_put_str(clock_is_set);
    } else {
        cli_put_str(clock_is_not_set);
    }
    cli_put_16b10(1 + tm_ptr->tm_yday);         // Show the day of year.
    cli_put_str(of366);
    cli_put_crlf ();
}

/***************************************************************************
 * Copyright (C) 2005..17 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

