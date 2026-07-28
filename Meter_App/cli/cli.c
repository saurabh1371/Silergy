/***************************************************************************
 * DESCRIPTION: SDK POWER METER - Command Line Interface Routines.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * $Id: cli.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/

#include <string.h>     // C strings.
#include <ctype.h>      // C data types.
#include "options.h"    // System options for compiler, hardware, etc.
#include "afe.h"        // for get_cal_default()
#include "autocal.h"    // To autocalibrate.
#include "bat.h"        // To measure the batteries.
#define CLI_PRIVATE_CODE 1
// cli_driver_t is needed by cli_buf.h, sercn.h
#include "cli.h"        // Command Line Interface (check function prototypes)
#undef CLI_PRIVATE_CODE


#include "cli_buf.h"    // To use the CLI's I/O routines and buffering.
#include "crc.h"        // To calculate CRCs.
#include "delay.h"      // looping delay
#include "dst.h"        // Daylight savings time calculation
#if CLI_ENABLE
#ifndef _32K
#include "eeprom.h"     // To use the EEPROM.
#endif
#include "external_int.h" // External interrupts.
#include "flash.h"      // To write to flash.
#include "gpio.h"       // To set-up pins for alternative uses.
#if HELP
#include "help.h"       // On-line help.
#endif
#include "info_block.h" // Access Flash Info Block
#include "irq.h"        // Interrupt management
#include "lcd.h"        // To write to LCD.
#include "lcd_strings.h" // LCD strings, used for many command lines.
#include "log.h"        // To record errors.
#include "meter.h"      // To run the meter.
#include "nvram.h"      // Access nonvolatile RAM.
#ifndef _32K
#include "pwm0.h"       // to use PWM0.
#include "pwm1.h"       // to use PWM1.
#endif
#include "pwrmode.h"    // To control the power modes.
#ifdef REMOTES
#include "rmt.h"        // remote driver, for SY7M017/AM97
#endif
#include "serc0.h"      // Serial I/O driver for UART 0
#include "serc1.h"      // Serial I/O driver for UART 1
#include "serc2.h"      // Serial I/O driver for UART 2
#include "sspi0.h"      // Serial I/O driver for Slave SPI
#include "stm.h"        // Software timers.
#include "tamper.h"     // tamper detect to rescane when events cleared.
#include "time.h"       // Time and calendar.
#include "tmux.h"       // Access to test-multiplexer driver.
#include "wd.h"         // Watchdog driver.
#include "cli_display.h" // Command Line display
#ifndef _32K
#include "cli_autoreport.h" // Command Line autoreporting
#endif


/*** Public variables declared within this module ***/
extern volatile bool wfi_enable;
extern const char demo_version[];
// Result code constants. (NOTE: these are indices, not result codes).
typedef enum {
	OK_ID = 0,        // Normal prompt. There's no error from last command.
	ERROR_ID,         // CLI has an error.
	NO_HELP_AVAIL_ID, // CLI has no on-line help.
	OK_NO_METER_ID,   // Prompt shows meter is off, battery is not on.
	OK_BROWNOUT_ID,   // Prompt shows battery mode.
	OK_VLCT_ID        // Prompt shows low power metering mode.
} cli_result_t;
volatile cli_result_t cli_result;

/*** Private functions declared within this module ***/

// Call CLI after serX_line_in() returns true, 
// and after the IO's port is set with set_port().
// The input line's buffer is reused to hold the output line.
// cli_idx is the I/O device's index.
static void cli (int cli_idx); // Command Line Interpreter.
static void cli_cal_put(void); // saves calibration with messages.
static uint8_t cli_get_cmd_chr (void);
       // Also used for 'V' command.
static void cli_put_help (const char * const str[]);
       // Looks up result code, primes pump for result codes.
static void cli_put_result (void); 

static void cmd_batmode (void); // Control battery modes.
static void cmd_ce_data_access (void); // Access CE data.
static void cmd_ce (void); // Command the CE.
static void cmd_dio (void); // Read and write DIOs.
#ifdef EEPROM_H
static void cmd_eeprom (void); // Command the EEPROM.
#endif
static void cmd_err_record (void); // Command the error recording system.
static void cmd_flash (void); // Command the flash reading and writing.
#if HELP
static void cmd_H (char c);             // '?'  Usage help
#endif
static void cmd_meter (void); // Command the metering logic.
static void cmd_mpu_data_access (void); // Access MPU data.
#ifdef PWM0_H
static void cmd_pwm (void); // Command the PWM.
#endif
static void cmd_ram (void);        // get and put 8-bit values in RAM
// Remotes Commands; Remotes defined in board support.h files, from options.h
#if REMOTES
static void cmd_rmt (void);
#endif 
static void cmd_rtc (void); // Command the RTC.
static void cmd_sfr (void);        // get and put 32-bit values in SFR
static void cmd_trim(void);

static void set_result (cli_result_t r);

      // Stubs
      void stub (const char *str);

/*** Private variables declared within this module ***/
#define BUFFER_SIZE       48
union buf
{
  uint8_t  buf8  [BUFFER_SIZE];
  uint32_t buf32 [BUFFER_SIZE/sizeof(uint32_t)];
} data_buf;

// Software timer for "CS" command that simulates CE data.
// cs_time is the simulated accumulation interval. If nonzero, the timer is on.
// cs_timer is the software timer. If zero, the simulated data is "ready."
int cs_time, cs_timer;

// vcode text are the CLI's status messages.
static const char  v1code_0[] = ">" ;
#if HELP
static const char  v1code_1[]
    = "\tINVALID COMMAND.  TYPE \'??\' FOR HELP\r\n>";
#else
static const char  v1code_1[] = "\tINVALID COMMAND.\r\n>";
#endif
static const char  v1code_2[] = "\tNO HELP AVAILABLE.\r\n>";
static const char  v1code_3[] = "NM>";
static const char  v1code_4[] = "B>";
static const char  v1code_5[] = "VLCT>";

static const char * const v1_result[] = {
    v1code_0, 
    v1code_1, 
    v1code_2,
    v1code_3,
    v1code_4,
    v1code_5
};

// Give the CLI polymorphic access to serial ASCII devices.
const cli_driver_t *cli_driver_ary[] = 
{
#if PCB_SER_0
    &serc0_cli_driver, // CLI driver for uart 0
#endif
#if PCB_SER_1
    &serc1_cli_driver, // CLI driver for uart 1
#endif
#if PCB_SER_2
    &serc2_cli_driver, // CLI driver for uart 2, or IRDA
#endif
#if PCB_SSPI_0
    &sspi0_cli_driver, // CLI driver for slave SPI 0
#endif
    NULL
};

/***************************************************************************
* Description:
*   Initializes the CLI (command line interface), and associated serial
*   drivers.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
void cli_init(void)
{
    const cli_driver_t **cli_driver_ptr_ptr = &cli_driver_ary[0];

    // For all the CLI drivers.
    while(NULL != *cli_driver_ptr_ptr)
    {
        (*((*cli_driver_ptr_ptr)->init))();   // Initialize them.

        ++cli_driver_ptr_ptr;           // Next cli driver.
    }
}


#if REMOTES
/***************************************************************************   
* Description:  
*   Displays Device and Version info for all the remote ICs.
* Parameters:
*   Input:
*         none 
*   Output: 
*         Version text string on the UART 
* Return Status: 
*         none
***************************************************************************/
#if EQUATION==EQUATION0 // 2w1p
#define FIRST_REMOTE 1  // Remote is on VA and IA.
#define LAST_REMOTE 1
#elif EQUATION==EQUATION1 || EQUATION==EQUATION2 // 3w1p, 4w3p
#define FIRST_REMOTE 1
#define LAST_REMOTE 2
#else // EQUATION5, 4w3p
#define FIRST_REMOTE 1
#define LAST_REMOTE 3
#endif
static void rmt_put_id (void)
{
	int rmt_idx;
    if (afe_status(AFE_RUNNING)) // If the CE is enabled.
    {
        // Put out the remote information.
        cli_put_str("\r\nRemotes: ");

        for (rmt_idx = FIRST_REMOTE; rmt_idx <= LAST_REMOTE; ++rmt_idx)
        {
            int version;

            // Put out the part number
            cli_put_str(rmt_get_part_no(rmt_idx));

            // Put out the part's version letter and number.
            version = rmt_get_version(rmt_idx);
            cli_put_chr(' ');
            cli_put_chr('A' + (((version >> 4) & 0x0F) - 1));
            cli_put_chr('0' + (version & 0x0F));

            cli_put_chr(' ');
            cli_put_chr(' ');
        }
    } // if CE enabled
}
#endif // REMOTES


/***************************************************************************
* Description:
*       Sends all the version data
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
// Read self-id data from the debug ROM in the CPU system area.
// The ROM starts at 0xf0000f00; The encoding is described in Silergy's bugzilla 2505.
// ARM's company ID is a JEP106 ID number and a count of continuation codes.
#define JEPCC (*(unsigned int *)0xf0000fd0) // Count of JEP106 continuation codes
#define JEPID_3_0 (((*(unsigned int *)0xf0000fe4) >> 4) & 0x0f)
#define JEPID_6_4 ((*(unsigned int *)0xf0000fe8) & 0x7)
#define JEPID ((JEPID_6_4 << 4) | JEPID_3_0) // JEP106 company ID
// The ARM part number field is a 16-bit number, defined by the designer.
// Silergy uses a 12-bit part number, 4 bits of major revision and 4 of minor.
#define PARTNO_7_0 (0xff & *(unsigned int *)0xf0000fe0) // Silergy part no.
#define PARTNO_11_8 (0x0f & *(unsigned int *)0xf0000fe4)
#define PARTNO ((PARTNO_11_8 << 8) | PARTNO_7_0)
// Silergy part revision. major & minor
#define MAJOR ((*(unsigned int *)0xf0000fe8) >> 4)
#define MINOR ((*(unsigned int *)0xf0000fec) >> 4)

static void put_version (void)
{
    uint32_t jepcc = JEPCC;
    uint32_t jepid = JEPID;
    uint32_t partno = PARTNO;
    uint32_t major = MAJOR;
    uint32_t minor = MINOR;

    cli_put_crlf ();
    /* the strings are in cli/version.c */
    cli_put_str (copyrightA);
    cli_put_chr(0x40 | SV->VERSION_b.major);
    cli_put_8b10(SV->VERSION_b.minor);
    cli_put_chr(' ');
    cli_put_str(afe_version());
    cli_put_chr(' ');
    /* this string is in cli/version.h */
    cli_put_str (software_version);
    cli_put_str("part:0x");
    cli_put_16b16((uint16_t)partno);
    cli_put_str(" ver:");
    cli_put_32b10(major);
    cli_put_chr('.');
    cli_put_32b10(minor);
    cli_put_str(" jep106 id:0x");
    cli_put_8b16(jepid);
    cli_put_str(" ccs:");
    cli_put_8b10(jepcc);
    cli_put_str (copyrightB);
#if REMOTES
    rmt_put_id();
#endif
    cli_put_crlf ();
}

/***************************************************************************
* Description:
*   Puts out the software version.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
void cli_put_version(void)
{
    const cli_driver_t **cli_driver_ptr_ptr = &cli_driver_ary[0];
    int driver_idx = 0;

    // For all the CLI drivers.
    while(NULL != *cli_driver_ptr_ptr)
    {
        char *line = NULL;
        char *buf; // Points to an output buffer area for the driver.
        int len = 0; // Has the available length of the buffer for this driver.

        while(0 == len) // wait till a buffer is free.
            line = (*(*cli_driver_ptr_ptr)->get_str)(&buf, &len);   // get buffer

        cli_attach(driver_idx, (*cli_driver_ptr_ptr), line, buf, len);
        put_version ();                         // put the version info
        cli_detach ();              // Detach the CLI I/O from this port.

        ++cli_driver_ptr_ptr;           // Next driver.
        ++driver_idx;
    }
}


/***************************************************************************
* Description:
*   Runs the command line interface.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         true = typing was received.
****************************************************************************/
bool cli_run(void)
{
    const cli_driver_t **cli_driver_ptr_ptr = &cli_driver_ary[0];
    int driver_idx = 0;
    bool typing = false;

    // For all the CLI drivers.
    while(NULL != *cli_driver_ptr_ptr)
    {
        char *line; // Points to an input (rx) buffer area for the driver.
        char *buf; // Points to an output (tx) buffer area for the driver.
        int len; // Has the available length of the buffer for this driver.

        line = (*((*cli_driver_ptr_ptr)->get_str))(&buf, &len);
        if(NULL != line) // Command string?
        {
            // Yes, process the command string
            // Attach the CLI I/O to this port.
            cli_attach(driver_idx, (*cli_driver_ptr_ptr), line, buf, len);
            cli (driver_idx);           // Interpret the line.
            cli_detach ();              // Detach the CLI I/O from this port.
            typing = true;              // Typing detected.
        }
        else if (0 != len) // output is possible.
        {
            // Send any pending data for this output.
            // If necessary, the CLI can use the driver index to
            // keep track in a status array.

            #ifdef CLI_AUTOREPORT_H
            if(driver_idx == ar_cli_idx)
            {
                cli_attach(driver_idx, (*cli_driver_ptr_ptr), line, buf, len);
                // Do any needed autoreporting.
                cli_ar_run(driver_idx);
                cli_detach ();              // Detach the CLI I/O from this port.
            }
            #endif
        }

        ++cli_driver_ptr_ptr;           // Next driver.
        ++driver_idx;
    }

    // Miscellaneous CLI tasks for main loop.

    // Run the simulated analog front end for command CS.
    if(0 != cs_time && 0 == cs_timer) // If simulated CE interval is done.
    {
        afe_data_ready(); // Report that data is ready.

        if(stm_start(&cs_timer, cs_time)) // Restart the timer.
            log_software();
    }

    return typing;
}


/***************************************************************************
* Description:
*        Reports an error if an unexpected character is printable.
*        This should be placed in the default case of all the character
*        decode cases.
* Parameters:
*   Input:
*        next character in a string.
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cli_error(char d)
{
    // If undecoded, nothing, or a visible, typed character,
    // it's a failure.
    if (isgraph(d))
    {
        set_result(OK_ID);
    }
}
static void cli_none_error(char d)
{
    // If undecoded, nothing, or a visible, typed character,
    // it's a failure.
    if (0 == d || isgraph(d))
    {
        set_result(OK_ID);
    }
}

/***************************************************************************
* Description:
*        Interprets commands in a string.
*        The input is directly from a serial driver's string.
*        The string is a command line, with the backspaces, etc. done.
*        The output is to a serial driver, putting strings into
*        a queue of strings.
*        Unchanging strings are queued, and the data goes directly from flash
*        to the I/O register.
*        Dynamic strings, like numbers, are placed in the same data array
*        that held the command line.
*        An I/O library helps this to happen.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
//static const char invoke_boot_fail[] =
//  "Invoke Bootloader failed, cannot write checkcode.\r\n";
static const char boot_not_supported[] =
  "Not supported, requires Bootloader build.\r\n";

void cli (int cli_idx)                      // Command Line Interpreter.
{
    int  d;
    set_result(OK_ID);              // Set the return message.

    flash_enable();

    d = cli_get_cmd_chr();          // Get a character, forced to upper case
    switch ( d )
    {
        #ifdef CLI_AUTOREPORT_H
        case 0x18:                 // Ctrl-X
            cli_ar_enable(cli_idx);
            break;

        case 0x19:                 // Ctrl-Y
            cli_ar_disable(cli_idx);
            break;
        #endif

        case ']':                  // ']' CE Data Access.
            cmd_ce_data_access ();
            break;

        case ')':                  // ')' MPU Data Access.
            cmd_mpu_data_access ();
            break;

        case 'B':
            cmd_batmode ();         // 'B' Battery modes.
            break;

        case 'C':
            cmd_ce ();             // 'C' (C)ompute Engine controls.
            break;

        case 'D':
            cmd_dio ();            // 'D' Dio read and write.
            break;

        case 'E':
            d = cli_get_cmd_chr ();
            switch( d )
            {
                #ifdef EEPROM_H
                case 'E':
                    cmd_eeprom ();    // (EE)prom controls.
                    break;
                #endif

                case 'R':
                    cmd_err_record ();    // error recording tests
                    break;

                default:
                    // If undecoded, but it's a visible, typed character,
                    // it's a failure.
                    cli_none_error(d);
                    break;
            }
            break;


        case 'F':
            cmd_flash ();     // 'F' (F)lash controls.
            break;

        case 'I':
            d = cli_get_cmd_chr ();
            switch( d )
            {

                #ifdef BOOT_BUILD  // If Bootloader build
                case 'B':              // (I)nvoke (B)ootLoader
                    val = 0x875A875A;  // Checkcode value
                    // 0x3FDF8 - Checkcode mem location 
                    if(flash_write32(0x3FDF8,(uint32_t*)&val, 1,FLASH_WR_ERS_PASSCODE))
                    {
                      cli_put_str(invoke_boot_fail); 
                    }
                    else
                    {
                      //jump to vector 0 to enter bootloader mode
                      SYSCN |= SYS_RESET;
                      while(true)
                      {
                      }
                    }
                    break;
                #else // #ifdef BOOT_BUILD             
                case 'B':
                    cli_put_str(boot_not_supported); // Bootloader  not supported
                    break;
                #endif // #ifdef BOOT_BUILD
                default:
                    put_version (); // Information Command
                    break;
            }
            break;

        case 'M':                  // 'M' (M)eter controls.
            cmd_meter ();
            break;

        case 'P':
            switch ( cli_get_cmd_chr () )
            {
                #ifdef PWM0_H
                case 'W':
                    cmd_pwm ();     // PWM controls.
                    break;
                #endif

                default:
                    cli_unget_chr();
                    // Basic pulse selection.
                    global.cal.pulse_src = (uint16_t)cli_get_32b16();
                    ce_data.ceconfig |= EXT_PULSE;
                    break;
            }
            break;

        case 'R':
            switch ( cli_get_cmd_chr () )
            {
                case 'T':
                    cmd_rtc ();     // 'RT' (R)eal (T)ime Clock controls.
                    break;

                case 'A':           // RAM access
                    cmd_ram ();
                    break;

                case 'M':
                    cmd_sfr();      // SFRs access
                    break;

                default:
                    cli_none_error(d);
                    break;
            }
            break;

        case 'T':
            cmd_trim ();
            break;

        case 'V':
            switch ( cli_get_32b10('+') )
            {
                default:
                case 0:             // SoC Common driver, 'V' with no number.
                    // Logic falls through here.
                case 1:             // SoC Common driver
                    cli_put_help (fw_ver_common);
                    break;

                case 2:             // SoC Specific driver
                    cli_put_help (fw_ver_specific);
                    break;

                case 3:             // SDK Library
                    cli_put_help (fw_ver_lib);
                    break;
            }
            delay(DELAY_MS(100)); // Let the driver put out the long data.
            break;

        case 'W':
            d = cli_get_cmd_chr ();
            if('0' == d)
            {
                // main loop does not stop CPU clock; Uses more power.
                wfi_enable = false;
                cli_put_str("No ");
            } else {
                // main loop stops CPU clock; Uses less power.
                wfi_enable = true;
            }
            cli_put_str("WFI in main loop. (changes power used)\r\n");
            break;

        #if REMOTES
        case 'X':
            cmd_rmt ();
            break;
        #else
        case 'X':
            cli_put_str("60 Second loop delay:\r\n");
            delay(DELAY_MS(60000));
            break;
        #endif

        case 'Z':
            {
                char d = cli_get_cmd_chr ();
                reg_crc();   // Make the data valid.
                nvram_write(PCB_NVRAM_ADR_NVM, (uint8_t*)&nvm, sizeof(Nvm_t));
                meter_save_data(); // Save data.
                switch (d)
                {
                    case 0: // no character: nvic reset.
                        // falls through
                    case '0': // zero: nvic reset.
                        NVIC_SystemReset();
				        break;

                    case '1': // 0: hardware reset.
                        SYS->MOD_CNTL_b.reset = 1;
                        while(true)
                        {
                            wd_reset();
                        }

                    case '2': // Watchdog reset.
                        IRQ_DISABLE();		// disable interrupts
                        while(true);        // watchdog reset

                    case '3': // Soft reset.
                        {
                        void (*fn_ptr)(void);
                        fn_ptr = (void (*)(void))*(int*)4;
                        (*fn_ptr)();
                        }
                        break;

                    case '4': // Hard fault.
                        // Write to an error address in the APB test slave.
                        *((uint32_t *)0x4000B0FC) = 0;
                        break;

                    default:
                        cli_error(d);
                        break;
                }
            }
            break;

        case '?':
            #if HELP
            if ('\0' == (d = cli_get_cmd_chr ( )))
            {
                cli_put_help (Usage);
            }
            else
            {
                cmd_H (d);
            }
            #else
            set_result(NO_HELP_AVAIL_ID);
            #endif
            break;

        case '/': // Ignore a commented line
            break;

        default:
            cli_error(d);
            break;
    }

    flash_disable();

    cli_put_result ();

    // Done with one line.
} // end cli


/***************************************************************************
* Description:
*       Enables the test mux output(s).
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
int32_t lcdmap0_copy;
int32_t lcdmap1_copy;
uint8_t lcd_save_cnt; // Save by default.
bool tmux_en_flag; // Disable by default.
static void lcd_unmap(int pin)
{
    if(pin < 32)
        LCD->MAPL &= ~(1UL << pin);
    else
        LCD->MAPH &= ~(1UL << (pin - 32));
}
static void lcd_save(void)
{
    if(0 == lcd_save_cnt)
    {
        lcdmap0_copy = LCD->MAPH;
        lcdmap1_copy = LCD->MAPL;
    }
    ++lcd_save_cnt;
}
static void lcd_restore(void)
{
    if(0 != lcd_save_cnt)
    {
        --lcd_save_cnt;
        if( 0 == lcd_save_cnt)
        {
            LCD->MAPH = lcdmap0_copy;
            LCD->MAPL = lcdmap1_copy;
        }
    }
}
static void tmux_enable(void)
{
    // Enable TMUX only if not enabled
    if(!tmux_en_flag)
    {
        lcd_save(); // Save the old lcdmaps
        lcd_unmap(tmux0_seg);
        lcd_unmap(tmux1_seg);

        // The LCD code will not write to disabled segments
        // if TMUXSEL is nonzero.

        tmux_en_flag = 1;  //Enable flag
    }
}

/***************************************************************************
* Description:
*       Disables the test mux output(s).
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void tmux_disable(void)
{
    // Clear the alternate for the TMUX pins.
    tmux0_config( (e_tmux0_signal) 0 );
    tmux1_config( (e_tmux1_signal) 0 );

    lcd_restore(); // Restore the old lcdmaps

    tmux_en_flag = 0; // Disable flag
}

/***************************************************************************
* Description:
*       Find a command character, rejecting inappropriate chars.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static uint8_t cli_get_cmd_chr (void)
{
    int chr;

    while (isspace(chr = (int)cli_get_upper()))
        ;

    return (uint8_t)chr;
}

/***************************************************************************
* Description:
*       Set a result code.  (We thought this was going to be harder.)
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
void set_result (cli_result_t r)
{
    cli_result = r;
}

/***************************************************************************
* Description:
*       Looks up and displays the result code, primes pump for result codes.
*       This is the code that knows if a command succeeded or not.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cli_put_result (void)
{
    uint8_t c = cli_result;

    /* if display is not up-to-date */
    if((global.cal.lcd_idx != global.misc.lcd_idx_old)
			|| afe_status(AFE_RUNNING))
        cli_display();

    if (c == OK_ID && pwrmode_is_brownout())
    {
        c = OK_BROWNOUT_ID; // display B>
    }
    cli_put_str (v1_result[ c ]);
}

/***************************************************************************
* Description:
*       Puts out an array of help strings.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
void cli_put_help (const char * const str[])   
{
    uint8_t cnt=0;

    cli_put_crlf ();
    while( NULL != str[cnt]){
      cli_put_str(str[cnt++]);
    } // Push all the strings to Serial , till NULL string is reached.
    // Help consists of array of strings. Help string array is concluded by a NULL string.
}


/***************************************************************************
* Description:
*       Parses a help request.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
#if HELP
static void cmd_H (char c)               // '?' help comamnd
{
    switch (toupper ((int)c))
    {
       case ']':
          cli_put_help (DAHelp);        // Compute Engine Data Access.
       break;

       case ')':
          cli_put_help (MAHelp);        // MPU Data Access.
       break;

       case 'B':
          cli_put_help (BHelp);         // battery mode commands
       break;

       case 'C':
          switch (cli_get_cmd_chr () )
          {
             case 'L':
                  cli_put_help (CLHelp);  // calibration.
             break;

             case 'R':
                  cli_put_help (CRHelp);  // RTM.
             break;

             default:
                  cli_put_help (CHelp);   // Compute Engine.
             break;
          }
          break;

       case 'D':
          cli_put_help (DIOHelp);         // DIO commands
          break;

       case 'E':
          switch (cli_get_cmd_chr () )
          {
              default:

              case 'E':
                  cli_put_help (EEHelp);      // EEProm.
              break;

              case 'R':
                  cli_put_help (ERHelp);      // error recording.
              break;
          }
       break;

       case 'F':
          cli_put_help (FHelp);           // Flash.
       break;

       case 'I':                          // Information messages
          cli_put_help (IHelp);
       break;

       case 'M':
           cli_put_help (MHelp);          // Meter Display Total  Control.
       break;


       case 'P':
          switch ( cli_get_cmd_chr() )
          {
             case 'W':
                cli_put_help (PWMHelp);   // PWM Control.
             break;

             default:
                cli_put_help (PHelp);     // Pulse Control.
             break;
           }
       break;

       case 'R':
          switch ( cli_get_cmd_chr() )
          {
             case 'T':
                cli_put_help (RTHelp);    // Real Time Clock controls.
             break;

             case 'A':
                cli_put_help (RAHelp);    // RAM access.
             break;

             case 'M':
                cli_put_help (RMHelp);    // SFR access.
             break;

          }
       break;


       case 'T':
          cli_put_help (THelp);           // Trim controls.
       break;

       case 'V':
          cli_put_help (VHelp);           // Device driver Version .
       break;


       case 'W':                          // Reset by watchdog
          cli_put_help (WHelp);
       break;
       #if REMOTES
       case 'X':                          // External Remote help
          cli_put_help (RMTHelp);
       break;
       #endif 
       case 'Z':                          // Reset part
          cli_put_help (ZHelp);
       break;

       default:
          set_result(NO_HELP_AVAIL_ID);
          cli_put_help (Usage);
       break;
    }
} // end cmd_H
#endif

#ifdef EEPROM_H
/***************************************************************************
* Description:
*       Parses a command to operate the EEPROM.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static const char eep_erase[] 
    = "EEPROM complete erase takes up to a few minutes ....\r\n"; 
static const char could_not_read_eeprom_at_addr[] 
    = "\tCould not read EEPROM at addr:";
static const char read_from_eeprom_at_addr[] 
    = "\tRead from EEPROM at addr:";
static const char could_not_write_to_eeprom_at_addr[] 
    = "\tCould not write to EEPROM at addr:";
static const char wrote_to_eeprom_at_addr[] 
    = "\tWrote to EEPROM at addr:";
static const char erase_failed[] 
    = "\r\n\tErase failed";
static const char erase_done[] 
    = "\r\nErase done";
static const char but_could_not_read_it[] 
    = "\tbut could not read it.";
static void cmd_eeprom (void)
{
    static int16_t cnt = 0;
    uint8_t chr, idx, d;
    int32_t adr;
    int status;

    {
        d = cli_get_cmd_chr();
        switch ( d )
        {      
            case 'E':
                cli_put_str (eep_erase);
                cli_put_crlf();
                eeprom_init();
                status = eeprom_erase ();  // clear the EEPROM
                eeprom_deinit();
                if (status)
                    cli_put_str (erase_failed);
                else
                    cli_put_str (erase_done);
                cnt = 0;
                cli_put_crlf();
                break;


            // Read EEPROM
            case 'R':
                {
                    adr = cli_get_32b16 ();
                    cli_get_chr ();                 // Skip delimiter.
                    cnt = (int16_t)cli_get_8b10();
                    cnt = lmin((BUFFER_SIZE-1) , cnt );    // Safe length.
                    
                    if ( cli_result == OK_ID )
                    {
                        eeprom_init();
                        status = eeprom_read (adr,(uint8_t *)&data_buf, cnt);
                        eeprom_deinit();
                        if (status)
                        {
                            cli_put_str (
                                could_not_read_eeprom_at_addr);
                            cli_put_32b16 (adr);
                        }
                        else
                        {
                            cli_put_str (
                                read_from_eeprom_at_addr);
                            cli_put_32b16 (adr);

                            for (idx = 0; idx < cnt; ++idx)
                            {
                                if ((idx & 0x0f) == 0)
                                    cli_put_crlf ();

                                cli_put_8b16 (data_buf.buf8[idx]);
                                cli_put_chr (' ');
                            }
                        }
                    }
                    cnt = 0;
                }
                cli_put_crlf();
                break;

            case 'S':
                cnt = 0;
                chr = cli_get_chr ();
                while ('\0' != chr && BUFFER_SIZE >= cnt)
                {
                    data_buf.buf8[cnt++] = chr;
                    chr = cli_get_chr ();
                }
                break;


            // Write EEPROM
            case 'T':
                {
                    adr = cli_get_32b16 ();
                    if ( cli_result == OK_ID )
                    {
                        eeprom_init();
                        status = eeprom_write (adr, (uint8_t *)&data_buf, cnt);
                        eeprom_deinit();
                        if (status){
                            cli_put_str (
                                could_not_write_to_eeprom_at_addr);
                            cli_put_32b16 (adr);
                        }
                        else
                        {
                            cli_put_str (wrote_to_eeprom_at_addr);
                            cli_put_32b16 (adr);
                            eeprom_init();
                            status = eeprom_read (adr,(uint8_t *)&data_buf, cnt);
                            eeprom_deinit();

                            if (status) {
                                cli_put_str( but_could_not_read_it );
                            }
                            else 
                            {
                                for (idx = 0; idx < cnt; ++idx)
                                {
                                    if ((idx & 0x0f) == 0)
                                        cli_put_crlf ();

                                    cli_put_8b16 (data_buf.buf8[idx]);
                                    cli_put_chr (' ');
                                }
                            }
                        }
                    }
                }

                cli_put_crlf();
                break;

            case 'W':
                cnt = 0;
                do
                    data_buf.buf8[cnt++] = cli_get_8b16 ();
                while ('.' == cli_get_chr () && (cnt < BUFFER_SIZE));

                cli_unget_chr ();             // Unget character.
                break;

            default:
                cli_none_error(d);
                break;
        }
    }
}
#endif

/***************************************************************************
* Description:
*       Parses a command to operate the RTC.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static const char invalid_date[] =
    "Invalid date (RTU unlocks the RTC)\r\n";
static const char invalid_time[] =
    "Invalid time (RTU unlocks the RTC)\r\n";
static void cmd_rtc (void)                     // 'RT' (R)eal (T)ime Clock controls.
{
    uint8_t c;

    switch ( c = cli_get_cmd_chr() )
    {
        default:
            cli_none_error(c);
            break;

        case 'D':                     // RTD set Date.
            {
                int tmp;

                // First, get current year.
                tmp = cli_get_32b10 ('+');
                tmp %= 100; // force 0-99
                global.reg.tm.tm_year = tmp;
                cli_get_chr ();                 // Skip delimiter.
                global.reg.tm.tm_mon = -1 + cli_get_8b10 ();
                cli_get_chr ();                 // Skip delimiter.
                global.reg.tm.tm_mday = cli_get_8b10 ();
                cli_get_chr ();                 // Skip delimiter.
                global.reg.tm.tm_wday = cli_get_8b10 ();
                global.reg.tm.gmt_offset = global.cal.gmt_offset;
                global.reg.tm.tm_isdst = dst(&global.reg.tm);
                global.reg.tm.tz = dst_tz(&global.reg.tm);

                if ( time_valid (&global.reg.tm) )
                    time_write (&global.reg.tm);
                else {
                    cli_put_str(invalid_date);
                    global.reg.tm = nvm.reg.tm;
                }
            }
            break;

        case 'M':                     // Set alarm.
            {
                uint8_t hr, min;
                hr = cli_get_8b10 ();
                cli_get_chr ();                 // Skip delimiter.
                min = cli_get_8b10 ();
                rtc_alarm_write(hr,min,0);
            }
            break;

        case 'R':
            cli_put_tm (&global.reg.tm);
            break;

        case 'T':                     // RTT Set Time of day.
            {
                uint8_t d;

                d = cli_get_cmd_chr ();
                if(!isdigit(d))
                {
                    cli_none_error(d);  // detect nonnumeric hours
                } else {
                    cli_unget_chr();

                    global.reg.tm.tm_hour = cli_get_8b10 ();
                    cli_get_chr ();                 // Skip delimiter.
                    global.reg.tm.tm_min = cli_get_8b10 ();
                    cli_get_chr ();                 // Skip delimiter.
                    global.reg.tm.tm_sec = cli_get_8b10 ();
                    cli_get_chr ();                 // Skip delimiter.
                    global.reg.tm.tm_subsec = 0;    // Start of second.
                    global.reg.tm.gmt_offset = global.cal.gmt_offset;
                    global.reg.tm.tm_isdst = dst(&global.reg.tm);
                    global.reg.tm.tz = dst_tz(&global.reg.tm);

                    if ( time_valid (&global.reg.tm) )
                        time_write (&global.reg.tm);
                    else {
                        cli_put_str(invalid_time);
                        global.reg.tm = nvm.reg.tm;
                    }
                }
            }
            break;

        case 'U':      // RTU unlock (0) and lock (1) the clock.
            {
                int c = ( 0 == cli_get_8b10 () ) ? false : true ;
                global.reg.tm.tm_isvalid = c;
                nvm.reg.tm.tm_isvalid = c;
                log_state(EV_RTC_UNSET,!c);
            }
            break;

        case 'Z': /* RTZ display zero-meridian (GMT) time */
            {
                struct tm tm_tmp;
                gmtime(&tm_tmp, mktime(&global.reg.tm));
                tm_tmp.tm_isvalid = global.reg.tm.tm_isvalid;
                cli_put_tm (&tm_tmp);
            }
            break;
    }
}

#ifdef PWM0_H
/***************************************************************************
* Description:
*       Interprets commands for pulse width modulation.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
#if pwm1_seg==tx1_seg
int saved_uart_config;
#endif
static void cmd_pwm(void)
{
    uint32_t freq_hz;
    uint8_t duty_cycle;

    cli_get_chr ();                     // Skip delimiter.

    switch ( cli_get_cmd_chr () )
    {
        case '0':
            cli_get_chr ();                     // Skip delimiter.
            // Get ASCII long decimal for Frequency
            freq_hz = cli_get_32b10 ('+');
            cli_get_chr ();                     // Skip delimiter.
            duty_cycle = cli_get_8b10();        // Get Duty Cycle
            // duty_cycle: Ranges from 1 to 100 (for 1% to 100% duty Cycle)
            duty_cycle = lmin(100,duty_cycle);

            if(freq_hz) 
            {
                lcd_save();
                lcd_unmap(pwm0_seg);
                pwm0_start(freq_hz,duty_cycle); // Start PWM out
            } else {
                pwm0_stop();                    // Start PWM out
                lcd_restore();
            }
            break;

        case '1':
            cli_get_chr ();                     // Skip delimiter.
            // Get ASCII long decimal for Frequency
            freq_hz = cli_get_32b10 ('+');
            cli_get_chr ();                     // Skip delimiter.
            duty_cycle = cli_get_8b10();        // Get Duty Cycle
            // duty_cycle: Ranges from 1 to 100 (for 1% to 100% duty Cycle)
            duty_cycle = lmin(100,duty_cycle);

            if(freq_hz)
            {
                lcd_save();
                lcd_unmap(pwm1_seg);
                #if pwm1_seg==tx1_seg
                saved_uart_config = UART1->CNTL; // Shared pin on SY7T_M11
                UART1->CNTL = 0;                 // disable UART1
                #endif
                pwm1_start(freq_hz,duty_cycle); // Start PWM out
            } else {
                #if pwm1_seg==tx1_seg
                if(saved_uart_config)
                    UART1->CNTL = saved_uart_config;
                #endif
                pwm1_stop();                    // Stop PWM out
                lcd_restore();
            }
            break;
    }
} // void cmd_pwm(void)
#endif


/***************************************************************************
* Description:
*       Put out the current error state on the serial port.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static const char spacer[] = "---\r\n";
typedef struct {
	Ev_t ev; const char *txt_ptr;
} log_text_t;
	const char TXT_LINE_BAD[] = "Metered from neutral.\r\n";
    const char TXT_COM[] = "Communications occurred.\r\n";
    const char TXT_RTC_ALARM[] = "RTC alarm interrupt detected.\r\n";
    const char TXT_TEMP_ALARM[] = "Temperature range interrupt detected.\r\n";
    const char TXT_SOFTWARE[] = "Software event: log_software() called.\r\n";
    const char TXT_WATCHDOG[] = "Software Watchdog Reset.\r\n";
    const char TXT_OVERVOLTAGE[] = "Mains V too high: Arcing?\r\n";
    const char TXT_OVERCURRENT[] = "Mains I too high: Heating?\r\n";
    const char TXT_HARDFAULT[] = "Hard fault event.\r\n";
    const char TXT_HARDWARE[] = "Hardware event: log_event(EV_HARDWARE) called.\r\n";
    const char TXT_SPURIOUS[] = "Spurious interrupt detected.\r\n";
    const char TXT_RTC_CORRUPT[] = "RTC Corrupted. Set from old copy.\r\n";
    const char TXT_VLCT_BAD[] = "Vlct pin V too low.\r\n";
    const char TXT_CAL_BAD[] = "Calibration CRC bad. Using defaults.\r\n";
    const char TXT_BAT_BAD[] = "Bat V too low.\r\n";
    const char TXT_SAG[] = "Mains V too low.\r\n";
    const char TXT_RTC_UNSET[] = "Clock is unset.\r\n";
    const char TXT_DEBUGGER[] = "Insecure debugger or SSPI.\r\n";
    const char TXT_RTC_TAMPER[] = "RTC tamper. Set by more than 2 hours?\r\n";
    const char TXT_MAG_TAMPER[] = "Magnetic tamper.\r\n";
	const char TXT_I_NO_V[] = "Current without voltage.\r\n";
    const char TXT_TERM_TAMPER[] = "Wiring block tamper.\r\n";
    const char TXT_CASE_TAMPER[] = "Case tamper.\r\n";
    const char TXT_REG_BAD[] = "Billing data CRC bad. Cleared registers.\r\n";
    const char TXT_LOG_BAD[] = "Event log failed. Events lost?\r\n";
	const char TXT_NONE[] = "";

const log_text_t log_text_ary[] = {
	{EV_LINE_BAD, TXT_LINE_BAD},
    {EV_COM, TXT_COM},
    {EV_RTC_ALARM, TXT_RTC_ALARM},
    {EV_TEMP_ALARM, TXT_TEMP_ALARM},
    {EV_SOFTWARE, TXT_SOFTWARE},
    {EV_WATCHDOG, TXT_WATCHDOG},
    {EV_OVERVOLTAGE, TXT_OVERVOLTAGE},
    {EV_OVERCURRENT, TXT_OVERCURRENT},
    {EV_HARDFAULT, TXT_HARDFAULT},
    {EV_HARDWARE, TXT_HARDWARE},
    {EV_SPURIOUS, TXT_SPURIOUS},
    {EV_RTC_CORRUPT, TXT_RTC_CORRUPT},
    {EV_VLCT_BAD, TXT_VLCT_BAD},
    {EV_CAL_BAD, TXT_CAL_BAD},
    {EV_BAT_BAD, TXT_BAT_BAD},
    {EV_SAG, TXT_SAG},
    {EV_RTC_UNSET, TXT_RTC_UNSET},
    {EV_DEBUGGER, TXT_DEBUGGER},
    {EV_RTC_TAMPER, TXT_RTC_TAMPER},
    {EV_MAG_TAMPER, TXT_MAG_TAMPER},
	{EV_I_NO_V, TXT_I_NO_V},
    {EV_TERM_TAMPER, TXT_TERM_TAMPER},
    {EV_CASE_TAMPER, TXT_CASE_TAMPER},
    {EV_REG_BAD, TXT_REG_BAD},
    {EV_LOG_BAD, TXT_LOG_BAD},
	{EV_NONE, TXT_NONE}
};
static void cli_put_ev(Ev_t ev)
{
    const log_text_t *p = &log_text_ary[0];
    cli_put_32b10((int)ev);
    if(log_active(ev)) {
        cli_put_str(", in progress:");
    } else {
        cli_put_str(", historical:");
    }
    while (EV_NONE != p->ev) {
        if(ev == p->ev) {
            cli_put_str(p->txt_ptr);
            return;
        }
        ++p;
    }
    cli_put_str("No text.\r\n");
    log_software();
}
#if 0 < LOG_REC_CNT
static void cli_put_ev_log(Ev_t ev)
{
    Log_rec_t *p = log_get_rec(ev);
    if(NULL != p) {
        int tmp = (int)p->year;

        if(127 < tmp) {
            cli_put_str("\tClock was unset for the above error. ");
        } else {
            cli_put_str("\t");
            cli_put_8b10(p->hour);
            cli_put_str(":");
            cli_put_8b10(p->min);
            cli_put_str(" ");
            cli_put_32b10(RTC_ERA+tmp);
            cli_put_str("-");
            cli_put_8b10(p->mon+1);
            cli_put_str("-");
            cli_put_8b10(p->mday);
            cli_put_str(" ");
        }
        tmp = (int)(unsigned int)p->cnt;
        if(255 <= tmp)
            cli_put_str(" >255");
        else
            cli_put_32b10(tmp);
        cli_put_str(" times\r\n");
        delay(DELAY_MS(240));
        cli_buf_new();
    }
}
#endif
static void cli_put_error (void)
{
    Ev_t ev;

    // Print events in decreasing importance.
    cli_put_str(spacer);
    for (ev = EV_MAX; ev != EV_MIN; --ev)
    {
        wd_reset();
        if (log_historic(ev)) {
            cli_put_ev(ev); // basic bit-log.
            #if 0 < LOG_REC_CNT
            cli_put_ev_log(ev); // Log records.
            #endif
        }
    }
    cli_put_str(spacer);
} // static void cli_put_error (void)


/***************************************************************************
* Description:
*       Interprets commands for the error recording.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_err_record (void)
{
    uint8_t c;

    switch ( c = cli_get_cmd_chr() )
    {
        case 'C': // clear
            log_clear();

            // Retest for tamper.
            tamper_run();
            break;
        case 'D': // display
            cli_put_error ();
            break;
        case 'S': // set
            log_state((Ev_t)cli_get_8b10(),1);
            break;
        case 'U': // unset
            log_clear_one((Ev_t)cli_get_8b10());
            break;
        default:
            cli_none_error(c);
            break;
    }
} // void cmd_err_record (void)

/***************************************************************************
* Description:
*       Get and put flash info block word (Trim)
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_flash_info(int32_t m)
{
    int32_t c,d;

    // Get action for SFR.
    c = cli_get_cmd_chr ();

    switch (c)
    {
        case '?':
        case '$':
            do
            {
                d = info_block_read(m);

                if ('?' == c)
                    cli_put_32b10 (d);
                else
                    cli_put_32b16 (d);
                cli_put_chr(' ');

                ++m;
                c = cli_get_cmd_chr () ;
            } while (('?' == c) || ('$' == c));

            cli_put_crlf ();
            break;
				
				case '=':
						d = cli_get_32 ();		
						info_block_write(m,d);
						break;						
        default:
            cli_none_error(c);
            break;
    }
} // static void cmd_flash_info(int32_t m)


/***************************************************************************
* Description:
*   Get and put 32-bit values in SFR
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_sfr (void)
{
    int sfr_addr, c, d;

    sfr_addr = (cli_get_32b16 () | 0x40000000) & 0xFFFFFFFC;

    // Get action for SFR.
    c = cli_get_cmd_chr ();

    switch (c)
    {
        case '?':
        case '$':
            do
            {
                /* Wait till the shadow registers are free. */
                sys_shadow_busy_wait();

                /* Read RTC calibration values. */
                SYS->WAKE_SRC_b.rtc_rd = 1;

                d = *(volatile uint32_t *)sfr_addr;

                SYS->WAKE_SRC_b.rtc_rd = 0;

                /* Wait till the shadow registers are free. */
                sys_shadow_busy_wait();


                if ('?' == c)
                    cli_put_32b10 (d);
                else
                    cli_put_32b16 (d);
                cli_put_chr(' ');

                sfr_addr += 4;
                c = cli_get_cmd_chr () ;
            } while (('?' == c) || ('$' == c));

            cli_put_crlf ();
            break;

        case '=':
            do
            {
                d = cli_get_32 ();

                /* Wait till the shadow registers are free. */
                sys_shadow_busy_wait();

                // Freeze the shadow registers.
                SYS->WAKE_SRC = SHADOW_UNLOCK;

                *(volatile uint32_t*)sfr_addr = d;

                // Unfreeze the shadow registers.
                SYS->WAKE_SRC_b.rtc_wr = 0;

                /* Wait till the write is done. */
                sys_shadow_busy_wait();

                sfr_addr += 4;
                c = cli_get_cmd_chr ();
            } while('=' == c);
            break;
        default:
            cli_none_error(c);
            break;
    }
} // static void cmd_sfr (void)


/***************************************************************************
* Description:
*   Get and put 8-bit values in RAM.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_ram (void)
{
    int8_t c,d;
    uint8_t *addr;

    addr = (uint8_t *) cli_get_32b16 ();

    // Get action for SFR.
    c = cli_get_cmd_chr ();

    switch (c)
    {
        case '?':
        case '$':
            do
            {
                d = *addr++;      //de-reference RAM address pointer

                if ('?' == c)
                    cli_put_8b10 (d);
                else
                    cli_put_8b16 (d);
                cli_put_chr(' ');

                c = cli_get_cmd_chr () ;
            } while (('?' == c) || ('$' == c));

            cli_put_crlf ();
            break;

        case '=':
            do
            {
                d = cli_get_32 ();
                // Assign value to de-referenced RAM address pointer
                *addr++ = d;
                c = cli_get_cmd_chr ();
            } while('=' == c);
            break;
        default:
            cli_none_error(c);
            break;
    }
} // static void cmd_ram (void)

/***************************************************************************
* Description:
*   Get the meter command: m1.2; It selects the output value.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_meter(void)
{
    extern bool lcd_data;
    char d = cli_get_cmd_chr();
    if(0 == d)
    {
        cli_none_error(d);
    }
    else
    {
        cli_unget_chr();
        global.cal.lcd_idx = cli_get_16b10();          // Basic LCD selection.
        cli_get_chr();
        global.cal.lcd_idx += 100 * cli_get_16b10();   // Phase is coded * 100
        lcd_data = true;                    // Force a redisplay.
        if (0 != global.cal.lcd_bit)                   // Timed scrolling on?
            if(stm_start(&global.misc.lcd_timer, STM_SEC(60)))         // display remains stable for 5 minutes.
                log_software();
    }
} // void cmd_meter(void)


/***************************************************************************
* Description:
*   Read and write the CE's data area.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_ce_data_access (void)
{
    uint8_t c;
    uint8_t num_cnt;
    int32_t *x;
    int32_t n;

    c = cli_get_cmd_chr ();         // Check for possible FLASH update.
    if ('U' == c)
    {
        cli_cal_put();              // Saves calibration with messages
        return;
    }    

    // Put last character back into the character queue.
    cli_unget_chr();
    // Get data's two-byte address.
    x = (int32_t *) (CE_RAM + (cli_get_32 ()));
    c = cli_get_cmd_chr ();        // Get action for address.

    switch (c)
    {
        case '?':
        case '$':
            num_cnt = 0;
            do
            {
                //n = *x++;
                n = *(int32_t *)x;
                ++x;

                if ('?' == c)
                    cli_put_32b10 (n);
                else
                    cli_put_32b16 (n);

                cli_put_chr(' ');

                if (4 <= ++num_cnt) {
                    num_cnt = 0;
                    cli_put_end_of_line();
                }

                c = cli_get_cmd_chr ();
            } while (('?' == c) || ('$' == c));
            cli_put_crlf ();
            break;
        case '=':
            global.cal.cal_src = is_user;
            do
            {
                //*x = cli_get_32 ();
                *(int32_t *)x = cli_get_32();
                ++x;
                c = cli_get_cmd_chr ();
            } while ('=' == c);
            break;
        default:
            cli_none_error(c);
            break;
    }
} // void cmd_ce_data_access (void)


/***************************************************************************
* Description:
*   CLI table to locate parameters and their sizes.
****************************************************************************/
typedef struct Parm_s
{
    uint8_t *ptr;
    int cnt; // negative indicates a signed value
} Parm_t ;
#define parm_ptr_cnt 0x39
#define parm_idx_max (parm_ptr_cnt - 1)
// Find global RAM parameters in MPU for use by CLI
// negative lengths indicate signer integers
// io_merge adds the lengths to find a relative offset, so lengths
// of zero will fail silently in io_merge
static const Parm_t parm_ptr_ary[parm_ptr_cnt]=
{
    // 0x00: )0  Configurations.
    {(uint8_t *) &global.cal.i_min,       -(int)sizeof(global.cal.i_min)},// 0 Min permitted current
    {(uint8_t *) &global.cal.cfg,          sizeof(global.cal.cfg)},// 1 Configuration bits
    {(uint8_t *) &global.cal.v_min,       -(int)sizeof(global.cal.v_min)},// 2 Min permitted volts
    {(uint8_t *) &global.cal.i_max,       -(int)sizeof(global.cal.i_max)},// 3 ADC A peak, 0.1A
    {(uint8_t *) &global.cal.v_max,       -(int)sizeof(global.cal.v_max)},// 4 ADC V peak, 0.1V
    {(uint8_t *) &global.cal.wh_whpp,     -(int)sizeof(global.cal.wh_whpp)},// 5 Wh of Wh/pulse
    {(uint8_t *) &global.cal.p_whpp,      -(int)sizeof(global.cal.p_whpp)},// 6 pulse of Wh/pulse
    {(uint8_t *) &global.cal.i_limit,     -(int)sizeof(global.cal.i_limit)},// 7 Max A, 0.1A
    {(uint8_t *) &global.cal.v_limit,     -(int)sizeof(global.cal.v_limit)},// 8 Max V, 0.1V
    {(uint8_t *) &global.cal.i_max_neutral, sizeof(global.cal.i_max_neutral)}, // 9 ADC A peak, Neut.
    {(uint8_t *) &global.cal.s_cal,        sizeof(global.cal.s_cal)},// A S of autocal measurement
    {(uint8_t *) &global.cal.v_cal,       -(int)sizeof(global.cal.v_cal)},// B V of autocal signal
    {(uint8_t *) &global.cal.i_cal,       -(int)sizeof(global.cal.i_cal)},// C A of autocal signal
    {(uint8_t *) &global.cal.theta_cal,   -(int)sizeof(global.cal.theta_cal)}, // D -Phi of autocal signal
    {(uint8_t *) &global.cal.lcd_idx,     -(int)sizeof(global.cal.lcd_idx)},// E Default display index
    {(uint8_t *) &global.cal.lcd_bit,      sizeof(global.cal.lcd_bit)},// F Bitmask selects displays
    // 0x10, )10
    {(uint8_t *) &global.cal.mfr_id,       sizeof(global.cal.mfr_id)},// 10 3-letter Mfr ID
    {(uint8_t *) &global.cal.meter_id,     sizeof(global.cal.meter_id)},// 11 Meter no.
    {(uint8_t *) &global.cal.gmt_offset,  -(int)sizeof(global.cal.gmt_offset)},// 12 Min from GMT
    {(uint8_t *) &global.cal.tcab,        -(int)sizeof(global.cal.tcab)},// 13 RTC cal*dT^2
    {(uint8_t *) &global.cal.tccd,        -(int)sizeof(global.cal.tccd)},// 14 RTC cal*dT
    {(uint8_t *) &global.cal.pre_en,       sizeof(global.cal.pre_en)}, // 15 Preamp value
    {(uint8_t *) &global.cal.cal_src,      sizeof(global.cal.cal_src)},// 16 where cal was from
    {(uint8_t *) &global.cal.cal_cnt,      sizeof(global.cal.cal_cnt)},// 17 Number of saves
    {(uint8_t *) &global.cal.ver_hash,     sizeof(global.cal.ver_hash)},// 18 Firmware version
    {(uint8_t *) NULL,          0}, // 19 reserved for future use
    {(uint8_t *) &global.cal.pulse_src,    sizeof(global.cal.pulse_src)},// 1a pulse source
    {(uint8_t *) &global.cal.interval,     sizeof(global.cal.interval)},// 1b Min of demand interval
    {(uint8_t *) NULL,          0}, // 1c reserved for future use
    {(uint8_t *) &global.cal.wsum_per_cnt, -(int)sizeof(global.cal.wsum_per_cnt)},// 1d WSUM counts/LCD count
    {(uint8_t *) &global.cal.v_nom,        sizeof(global.cal.v_nom)}, // 1e Nominal voltage
    {(uint8_t *) &global.misc.acc_cnt,    -(int)sizeof(global.misc.acc_cnt)},// 1F Count acc. intervals
    // 20: )20 Register data.
    {(uint8_t *) &global.reg.log.bits_sticky, sizeof(global.reg.log.bits_sticky)},// 20 error bits
    {(uint8_t *) &global.reg.wh_cnt,      -(int)sizeof(int32_t)},// 21 Wh
    {(uint8_t *) &global.reg.wh_cnt_ex,   -(int)sizeof(int32_t)},// 22 Wh Exp.
    {(uint8_t *) &global.reg.varh_cnt,    -(int)sizeof(int32_t)},// 23 VARh
    {(uint8_t *) &global.reg.varh_cnt_ex, -(int)sizeof(int32_t)},// 24 VARh Cap
    {(uint8_t *) &global.reg.dmd_max,     -(int)sizeof(global.reg.dmd_max)},// 25 W Max (Max demand)
    {(uint8_t *) &global.reg.dmd_mon,      sizeof(global.reg.dmd_mon)}, // 26 reserved for future use
    {(uint8_t *) &global.reg.dmd_mday,     sizeof(global.reg.dmd_mday)}, // 27
    {(uint8_t *) &global.reg.dmd_hour,     sizeof(global.reg.dmd_hour)}, // 28
    {(uint8_t *) &global.reg.dmd_min,      sizeof(global.reg.dmd_min)}, // 29
    {(uint8_t *) &global.reg.tamper_sec,   sizeof(global.reg.tamper_sec)}, // 2A S of tamper
    {(uint8_t *) &global.reg.sag_sec,      sizeof(global.reg.sag_sec)}, // 2B S of sag
    {(uint8_t *) &global.reg.operating_sec, sizeof(global.reg.operating_sec)}, // 2C S of operation
    {(uint8_t *) &global.reg.reg_cnt,      sizeof(global.reg.reg_cnt)}, // 2D No. of reg. saves
    {(uint8_t *) &global.reg.ver_hash,     sizeof(global.reg.ver_hash)}, // 2E Firmware ver.
    {(uint8_t *) NULL,                     0}, // 2f reserved for future use
    // 30
    {(uint8_t *) &nvm.last_wake_src,       sizeof(nvm.last_wake_src)}, // 30 wake src at reset
    {(uint8_t *) &nvm.last_wake,           sizeof(nvm.last_wake)}, // 31 wake at reset
    {(uint8_t *) &global.misc.temp_c,     -(int)sizeof(global.misc.temp_c)}, // 32 Temperature

    // equation 0 has Vref compensation for SoC:0, phase A:1, neutral:2
    // equation 1 has Vref compensation for SoC:0, phase A:1, neutral:2
    // equation 2 has Vref compensation for SoC:0, phase A:1, phase B:2
    {(uint8_t *) &global.cal.ppmc[0], -(int)sizeof(global.cal.ppmc[0])}, // 33 Vref compensation, channel 0 is the SoC MPU.
    {(uint8_t *) &global.cal.ppmc[1], -(int)sizeof(global.cal.ppmc[0])}, // 34 Vref compensation, channel 1 is the remote for phase C.
    {(uint8_t *) &global.cal.ppmc[2], -(int)sizeof(global.cal.ppmc[0])}, // 35 Vref compensation, channel 2 is the remote for phase B.
    {(uint8_t *) &global.cal.ppmc2[0], -(int)sizeof(global.cal.ppmc2[0])}, // 36 Vref compensation, channel 0 is the SoC MPU.
    {(uint8_t *) &global.cal.ppmc2[1], -(int)sizeof(global.cal.ppmc2[0])}, // 37 Vref compensation, channel 1 is the remote for phase C.
    {(uint8_t *) &global.cal.ppmc2[2], -(int)sizeof(global.cal.ppmc2[0])}, // 38 Vref compensation, channel 2 is the remote for phase B.
    // 0x39 total count
};


/***************************************************************************
* Description:
*   Fetch a parameter
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static uint32_t fetch (uint16_t parm_idx)
{
    uint32_t v;

    if (parm_idx > parm_idx_max) // defaults to fetch a 32-bit number
    {
        return 0;
    }

    switch (parm_ptr_ary[ parm_idx ].cnt)
    {
    	case 0:
            v = 0;
            break;
    	default:
    	case 1:
    	    v = (uint32_t) (*((uint8_t *) parm_ptr_ary[ parm_idx ].ptr));
    	    break;

    	case 2:
    	    v = (uint32_t) (*((uint16_t *) parm_ptr_ary[ parm_idx ].ptr));
    	    break;

    	case 4:
    	case -4:
    	    v = *((uint32_t *) parm_ptr_ary[ parm_idx ].ptr);
    	    break;

    	case -1:
    	    v = (uint32_t) (*((uint8_t *) parm_ptr_ary[ parm_idx ].ptr));
            if (v & 0x80L) v |= 0xFFFFFF80L; // sign extend
    	    break;

    	case -2:
    	    v = (uint32_t) (*((uint16_t *) parm_ptr_ary[ parm_idx ].ptr));
            if (v & 0x8000L) v |= 0xFFFF8000L; // sign extend
    	    break;
    }

    return v;
} // fetch()

/***************************************************************************
* Description:
*   Store a parameter
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void store (uint16_t parm_idx, uint32_t v)
{
    if (parm_idx > parm_idx_max) // does nothing...
    {
        return;
    }

    switch (parm_ptr_ary[ parm_idx ].cnt)
    {
        case 0:
            break;
    	default:
    	case 1:
    	case -1:
    	    *((uint8_t *) parm_ptr_ary[ parm_idx ].ptr) = v;
    	    break;

    	case 2:
    	case -2:
    	    *((uint16_t *) parm_ptr_ary[ parm_idx ].ptr) = v;
    	    break;

    	case 4:
    	case -4:
    	    *((uint32_t *) parm_ptr_ary[ parm_idx ].ptr) = v;
    	    break;
    }
} // store()


/***************************************************************************
* Description:
*   Access MPU's variables.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_mpu_data_access (void)         // ')' MPU data access.
{
    uint8_t  c;
    uint8_t num_cnt;
    uint16_t x;

    x = cli_get_16 ();
    c = cli_get_cmd_chr ();             // Get action for address.

    switch (c)
    {
        case '?':
        case '$':
            num_cnt = 0;
            do
            {
                if ('?' == c)
                    cli_put_32b10 (fetch (x++));
                else
                    cli_put_32b16 (fetch (x++));
                cli_put_chr(' ');

                if (4 <= ++num_cnt) {
                    num_cnt = 0;
                    cli_put_end_of_line();
                }

                c = cli_get_cmd_chr () ;
            } while (('?' == c) || ('$' == c));

            cli_put_crlf ();
            break;

        case '=':
            global.cal.cal_src = is_user;
            do
            {
                store (x++, cli_get_32 ());
                c = cli_get_cmd_chr () ;
            } while ('=' == c);
            break;

        default:
            cli_none_error(c);
            break;
    }

    cli_unget_chr ();                      // Unget last character.
} // void cmd_mpu_data_access (void)


/***************************************************************************
* Description:
*   Command the RTM (real time monitor) interface.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void ce_cmd_rtm (void)
{
    uint8_t c;

    c = cli_get_cmd_chr ();

    switch (c)
    {
        case 'E': // Enable RTM
            c = cli_get_cmd_chr ();
            if ('0' == c) {
                CE->CNTL_b.rtm_en = 0;
                tmux_disable();
            } else if ('1' == c) {
                tmux_enable();
                tmux0_config( rtm_dat );       // RTM data
                tmux1_config( clk_rtm );       // RTM clock
                CE->CNTL_b.rtm_en = 1; // enable the RTM.
            } else {
                cli_none_error(c);
            }
            break;

        case 'S': // Set RTM registers
            // RTM registers
            CE->RTM0 = (0xFFF & cli_get_16 ()) << 2;
            cli_get_cmd_chr ();       // Skip delimiter.
            CE->RTM1 = (0xFFF & cli_get_16 ()) << 2;
            cli_get_cmd_chr ();       // Skip delimiter.
            CE->RTM2 = (0xFFF & cli_get_16 ()) << 2;
            cli_get_cmd_chr ();       // Skip delimiter.
            CE->RTM3 = (0xFFF & cli_get_16 ()) << 2;
            tmux_enable();
            tmux0_config( rtm_dat );       // RTM data
            tmux1_config( clk_rtm );       // RTM clock
            CE->CNTL_b.rtm_en = 1; // enable the RTM.
            break;

        default:
            cli_none_error(c);
            break;
    }
} // static void ce_cmd_rtm (void)


/***************************************************************************
* Description:
*   Control calibration
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static const char save_failed_turn_ce_off[] =
   "Save failed; turn CE off with 'CE0'.\r\n";
static const char restore_failed[] =  "Cal restore failed. perform CLS\r\n";
static const char restore_ok[] = "Cal restored.\r\n";
static const char default_ok[] = "Cal default ok.\r\n";
//static const char cal_save_ok[] ="Cal. save okay.\r\n";
static void cli_cal_put(void) // saves calibration, with messages.
{
    const char *msg = NULL;
    if (afe_status(AFE_RUNNING))
    {
        msg = save_failed_turn_ce_off;
    }
    else
    {
        if(cal_put ())
        {
            msg = save_failed_turn_ce_off;
        } 
        else if(cal_get())    // retrieve calibration coefficients.
        {
            // Saved failed; restored to default
            msg = restore_failed;
        }
    }
    if(NULL != msg)
    {
        cli_put_str (msg);
    }
}
static void ce_cmd_cal (void)
{
    uint8_t c;

    c = cli_get_cmd_chr ();

    switch (c)
    {
        #if AUTOCAL
        case 'B':
            cal_begin();        // start a calibration.
            break;
        #endif

        case 'C':               // Calculate after a change of i-max, etc.
            meter_stop();
						meter_save_data();
            afe_recalc(&global.cal);
            meter_start(ce_ptr);
            break;

        case 'S':
            cli_cal_put();      // Saves calibration with messages
            break;

        case 'R':
            // restore a calibration.
            // Make the CRC of the calibration in memory invalid.
            global.cal.cal_src = is_default;
            meter_stop();
						meter_save_data();
            if(cal_get())    // retrieve calibration coefficients.
                // Restore failed; restored to default
                cli_put_str (restore_failed);
            else
                cli_put_str (restore_ok);  // Restore Successful 

            meter_start(ce_ptr);
            break;
        case 'D':
            meter_stop();
						meter_save_data();
            cal_reset ();  // Use the defaults.  Resets AFE
            meter_start(ce_ptr);
            cli_put_str (default_ok); // restored to default
            break;
        default:
            cli_none_error(c);
            break;
    }
} // static void ce_cmd_cal (void)

/***************************************************************************
* Description:
*   Commands the CE (compute engine, the meter's DSP)
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_ce (void)
{
    uint8_t c;

    #if HELP
    bool help = false;
    #endif

    c = cli_get_cmd_chr ();

    switch (c)
    {
        #if HELP
        case '?':
            help = true;
            break;
        #endif

        case 'E':
            if (cli_get_8b16 ())
            {
                meter_start(ce_ptr);      // Enable AFE.
                global.misc.wd_want |= CE_WD;
            } else {
                meter_stop();	    // Disable AFE.
                global.misc.wd_want &= ~CE_WD;
            }

            // If simulating CE data, 
            if(cs_time)
            {   // Stop the timer simulating CE data.
                cs_time = 0;
                if(stm_start(&cs_timer, 0))
                    log_software();
            }
            break;

        case 'L':
            ce_cmd_cal ();
            break;

        case 'R':
        #if HELP
            if (help)
            {
                cli_put_help (CRHelp);
                help = false;
            }
            else
        #endif
                ce_cmd_rtm ();
            break;

        case 'S':
            // Stop CE, run simulated data using cal. signal description.
            afe_sim();

            // Set the simulated accumulation interval.
            cs_time = STM_MS((ce_ptr->nacc * 1000)/(int)ce_ptr->fs);
            if(stm_start(&cs_timer, cs_time))
                log_software();
            break;

        case 'T':
            c = cli_get_8b16 ();
            cli_get_cmd_chr (); // skip the delimiter
            switch (c)
            {
                // "T" the operator doesn't know what he's doing.
                case 0:
                default:
                    tmux0_config( (e_tmux0_signal) cli_get_16 () );
                    break;
                case 1:
                    tmux1_config( (e_tmux1_signal) cli_get_16 () );
                    break;
            }
            if (0 == (TMUX->TMUXSEL))
            {
                tmux_disable();
            } else {
                tmux_enable();
            }
            break;

        default:
            cli_none_error(c);
            break;
    }
} // void cmd_ce (void)

/***************************************************************************
* Description:
*   Trim Command -  Access flash Info block
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_trim (void)
{
    int32_t m;
    char d = cli_get_cmd_chr();

    switch (d)
    {
        case 'R':
				case 'W':					
            // Get Flash info address as TR1...15  or TW1...15
            m = cli_get_32b16 ();
            cmd_flash_info(m);          // Read/write flash info block command
            break;
				
        case 'F':
            // Fuse refresh control
            d = cli_get_cmd_chr ();
            cmd_fuse_refresh(d);          // Read/write flash info block command
            break;				

        default:
            cli_none_error(d);
            break;
    }
} // static void cmd_trim (void)

/* Cmd Flash message strings */
//static const char c_flash_str1[] = "Do not overwrite or erase code!\r\n";
static const char c_flash_str2[] = "Flash write failed!\r\n";
static const char c_flash_str3[] = "Wrote to Flash at addr 0x";
static const char c_flash_str4[] = "Flash page erase failed!\r\n";
static const char c_flash_str5[] = "Flash page erase okay at addr:";

/***************************************************************************
* Description:
*   Command the flash hardware.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_flash (void)
{                                               // FCn.n, FD, FR, FT..
    uint8_t c;
    int status;
    uint32_t flash_ptr;
    static uint16_t cnt,i;

    c = cli_get_cmd_chr ();

    flash_enable();
    switch ( c )
    {
        case 'T':
            {
                // Address.
                flash_ptr  = 0xFFFFFFFC & ((uint32_t) cli_get_32b16 ());
                // Length.
                cnt = lmin((BUFFER_SIZE/4),cnt);
                cli_put_crlf ();
                // BUG Need a test to avoid writing to code.
                status = flash_write(flash_ptr,(uint32_t*)&data_buf, cnt*4);
                if(status)
                {
                    cli_put_str(c_flash_str2); 
                }
                else
                {
                    cli_put_str(c_flash_str3);
                    cli_put_32b16 (flash_ptr);
                    cli_put_chr(':');
                    memcpy(&data_buf, (const void*)flash_ptr, cnt*4);
                    for(i=0;i<cnt;i++)
                    {
                        cli_put_32b16 ((uint32_t)data_buf.buf32[i]);
                        cli_put_chr(' ');
                    }
                    cli_put_crlf ();
                }
            }
            break;

        case 'R':
            cnt = 0;
            flash_ptr  = 0xFFFFFFFC & ((uint32_t) cli_get_32b16 ());
            cli_get_cmd_chr ();                              // Skip delimiter.
            cnt = cli_get_32b16 ();                          // Length.
            cnt = lmin(BUFFER_SIZE/4, cnt);                  // Safe length.
            cli_put_crlf ();
            memcpy(&data_buf, (const void*)flash_ptr, cnt*4);

            for(i=0;i<cnt;i++)
            {
                cli_put_32b16 ((uint32_t)data_buf.buf32[i]);
                cli_put_chr(' ');
            }
            cli_put_crlf ();
            break;

        case 'E':
            // Page size is 512 bytes, i.e. 0x200 bytes.
            flash_ptr  = 0xFFFFFE00 & ((uint32_t) cli_get_32b16 ());
            cli_put_crlf ();
            status = flash_page_erase(flash_ptr);
            if(status)
            {
                cli_put_str(c_flash_str4);
            }
            else
            {
                cli_put_str(c_flash_str5);
                cli_put_32b16 (flash_ptr);
                cli_put_crlf ();
                memcpy(&data_buf, (const char *)flash_ptr, 4);
                cli_put_32b16 ((uint32_t)data_buf.buf32[0]);
                cli_put_crlf ();
            }
            break;

        case 'W':
            cnt = 0;
            do
            {
                data_buf.buf32[cnt++] = cli_get_32();
            }
            // Max flash word write supported is 6.
            while ('.' == cli_get_cmd_chr() && (cnt < ( (BUFFER_SIZE/4) -1)));   

            cli_unget_chr();                   // Unget character.
            break;

        default:
            cli_none_error(c);
            break;
    }
    flash_disable();
} // void cmd_flash (void)


/***************************************************************************
* Description:
*   Detect if the current mode is right.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
//static const char not_in_this_mode[] =
//    "Not in this mode.\r\n";
//static bool it_has (int flag)
//{
//   if (flag) {
//        return true;
//    } else {
//        cli_put_str (not_in_this_mode);
//        return false;
//    }
//} // static bool it_has (int flag)

//static const char lcd_mode_str[] = "LCD Mode";
/***************************************************************************
* Description:
*   Commands the system to enter a battery mode.
* Parameters:
*   Input:
*         None
*   Output:
*         None
* Return Status:
*         None
****************************************************************************/
static void cmd_batmode (void)
{
    uint8_t c;
    uint16_t time_cnt;

    c = cli_get_cmd_chr ();

    switch (c)
    {
        case 'L':
            // Save volatile data, especially the log.
            reg_crc();   // Make the data valid.
            nvram_write(PCB_NVRAM_ADR_NVM, (uint8_t*)&nvm, sizeof(Nvm_t));
            meter_save_data();  // Save the meter data.

            lcd_clear();
            lcd_put_str(LINE1,lcd_str);
            // the UART causes spurious wakes on some PCBs.
            // So, it is not permitted to wake the part.
            #ifdef PCB_BUTTON_EW
            pwrmode_lcd (SYS->WAKE | tamper_wakes(PCB_BUTTON_EW));
            #else
            pwrmode_lcd (tamper_wakes(SYS->WAKE));
            #endif
            break;
        case 'R':
            pwrmode_set_wake (cli_get_32b16());
            break;
        case 'S':
            // Save volatile data, especially the log.
            reg_crc();   // Make the data valid.
            nvram_write(PCB_NVRAM_ADR_NVM, (uint8_t*)&nvm, sizeof(Nvm_t));
            meter_save_data();  // Save the meter data.

            // the UART causes spurious wakes on some PCBs.
            // batmode_lcd (EW_VSYS|PCB_BUTTON_EW|PCB_UART_RX_EW);
            lcd_deinit();
            #ifdef PCB_BUTTON_EW
            pwrmode_sleep (SYS->WAKE | tamper_wakes(PCB_BUTTON_EW));
            #else
            pwrmode_sleep (tamper_wakes(SYS->WAKE));
            #endif
            break;
        case 'T':
            { int tmp = false; // No error.
            #if PCB_HAS_VBAT // PCB_* symbols are from the PCB map file.
            tmp |= MIN_LOGIC_VOLTS > bat_volts();
            #endif
            #if PCB_HAS_VBAT_RTC
            tmp |= MIN_LOGIC_VOLTS > bat_volts_rtc();
            #endif
            log_state(EV_BAT_BAD, tmp);
            }
            break;
        case 'W':
            c = cli_get_cmd_chr ();

            time_cnt = cli_get_16 ();

            if (cli_result == OK_ID)
            {
                if ('S' == c)
                    pwrmode_wait ( PWRMODE_SEC(time_cnt) );
                else
                {
                    cli_result = OK_ID;
                }
            }
            break;
        default:
            cli_none_error(c);
            break;
    }
} // void cmd_batmode (void)


#if REMOTES
/***************************************************************************   
* Description:  
*   Communication commands for Remote IC. 
* Parameters:
*   Input:
*         None
*   Output: 
*         None
* Return Status: 
*         None
****************************************************************************/
/* Cmd rmt message strings */
static const char rmt_err[] = ",Remote error.\r\n";


/***************************************************************************   
* Description:  
*   Communication commands for Remote interface chip.
* Parameters:
*   Input:
*         None
*   Output: 
*         None
* Return Status: 
*         None
****************************************************************************/
static void cmd_rmt (void)
{
    uint32_t adr, cmd;
    uint16_t result;

    adr = cli_get_8b16 ();          // Get address.
    cli_get_chr ();                 // Skip delimiter.
    cmd = cli_get_8b16 ();          // Get field
    result = rmt_get_reg(adr, cmd); // Read remote
    cli_put_16b16(result);          // Print hex result.
    if (rmt_error())                // Get the errors
    {
        cli_put_str (rmt_err);
    }
    else
    {
        cli_put_crlf ();
    }
} // cmd rmt
#endif // remote


/***************************************************************************
* Description:
*   Read and write the DIO array.
* Parameters:
*   Input:
*         DIO index, then data or read commands.
*   Output:
*         Set the DIOs, or print their content.
* Return Status:
*         None
****************************************************************************/
static void cmd_dio (void)
{
    uint8_t c;
    uint8_t num_cnt;
    int32_t x;
    int32_t n;

    // Get DIO's number.
    x = cli_get_32b10 ('+');
    if(x < 0)
        return;
    c = cli_get_cmd_chr ();                 // Get action for address.

    switch (c)
    {
        case '?':
        case '$':
            num_cnt = 0;
            do {
                if(LAST_DIO >= x && 0 <= x) {

                    lcd_unmap(x);   // No longer an LCD pin. (stop LCD updates)

                    //n = *x++;
                    n = LCD->DIO[x];

                    if ('?' == c)
                        cli_put_8b10 (n);
                    else
                        cli_put_8b16 (n);

                    cli_put_chr(' ');

                    if (7 <= ++num_cnt) {
                        num_cnt = 0;
                        cli_put_end_of_line();
                    }
                }
                ++x;

                c = cli_get_cmd_chr ();
            } while (('?' == c) || ('$' == c));
            cli_put_crlf ();
            break;
        case '=':
            do
            {
                n = cli_get_32();
                if(LAST_DIO >= x && 0 <= x) {

                    lcd_unmap(x);   // No longer an LCD pin. (stop LCD updates)

                    //*x = cli_get_32 ();
                    LCD->DIO[x] = n;
                } 
                ++x;
                c = cli_get_cmd_chr ();
            } while ('=' == c);
            break;
        default:
            cli_none_error(c);
            break;
    }
} // void cmd_dio ()

#endif // #if CLI_ENABLE
/***************************************************************************
 * Copyright (C) 2005..2018 Silergy Corp. 
 * All Rights Reserved.
 * This program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/


