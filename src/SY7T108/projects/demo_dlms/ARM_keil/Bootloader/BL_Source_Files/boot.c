/*----------------------------------------------------------------------------
 * Purpose: boot loader code 
 * Note(s): This code loads an Intel Hex file into flash on an SY7T meter IC,
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * $Id: boot.c 25396 2020-05-19 21:09:02Z ptanwar $
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "SY7T_M11.h"   /* Common hardware. */
#include "system_SY7T_M11.h"
#include "boot.h"
#include "flash.h"  // For IC flash size and page size

/*** Private variables declared within this module ***/
/* System Clock Frequency (Core Clock)*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;

// UART buffer management
#define BUFF_SIZE   8      // 32-bit word buffer.
static uint32_t data_buff[BUFF_SIZE];
static uint8_t *data_ptr;

// Hex record management
static uint8_t record_len, record_type, record_checksum;
static uint8_16_32_t record_addr;
static uint8_t rec_len_cpy;
// State of hex record parser.
bstate_t boot_state; // 
union {
    uint8_t byte[4]; /* bytes */
    uint32_t word;   /* 32-bit words */
} flash_address; 

// Flash management
static int flash_passcode;
// To keep a copy of flash controller (FLASH->FLASH_CN)
static uint32_t flash_cn; 
const int retry_cnt = 1; // flash write retry count

// App. start address, accessed in the startup assembly file also
const uint32_t app_start_addr = APP_START; 


/*** Private functions declared within this module ***/
// UART, buffer management
static uint8_t get_hex (void);
static uint8_t get_digit (void);
static int boot_loop(void);
static void boot_system_init(void);
static void put_str(const char *str);
//static char flush_ser(void);
// Flash management
static void flash_erase_app(void);					
static void wait_flash_busy(void);
// HW reset
static void hwreset_device(void);

/****************************************************************************
* Description:
*     Bootloader main routine.
*     Manages the system state, 
*     makes basic decision when to run the application code. 
*     On 'FE' command erase app. and call routine to to upgrade 
* Input:
*       none
* Parameters:
* Output: Upgrades the application firmware in flash memory.
* Return Status:
*       None.
*****************************************************************************/
void boot(void)
{
	volatile uint8_t ch;
	uint32_t * check_code = (uint32_t*)(CHKCODE_ADDR);
  uint32_t * app_init_sp = (uint32_t*)(APP_START);
	const char send_BOOT[] = "\r\nBOOT";
	const char send_FLASH_ERASE[] = "\r\nFLASH ERASE DONE!";
	const char send_ack[] = "\r\nCOMPLETED";
	
	// If flash is not blank (FFs) at APP_START,
	// And, if check_code is matching, it means we need to run App.
	if( FLASH_BLANK != (*app_init_sp) &&
		  CHECKCODE == (*check_code))
	{
		return; // Condition met to start the App, return to start the app
	}
  
  boot_system_init();     // initialize the hardware.
  ser_poll_init(9600);    // initialize UART.
	
	LCD->DIO_b[30].alt = 0; //  Optical Port Power Supply Pin
	LCD->DIO_b[30].dir = 1;//
	LCD->DIO_b[30].data = 0;
	
//	LCD->DIO_b[11].alt = 0; //  Pulses Supply Power Supply Pin
//	LCD->DIO_b[11].dir = 1; // Pulses Supply
//	LCD->DIO_b[11].data = 1;
//	
//	LCD->DIO_b[42].alt = 0;
//	LCD->DIO_b[42].dir = 1;//Set WPULSE as output
	
	
  // Refresh WATCHDOG timer.
  RESET_WD(); 
 
	put_str(send_BOOT);

 
  
	// 'FE' commnad  and [Y]es is given so erase the App (flash pages)
	flash_erase_app(); 
	
	put_str(send_FLASH_ERASE);
	
 // Initial State of the Boot loop, look for Colon, start of Intel hex record.
 // Watchdog is refreshed in serial receive polling function till first colon 
 // character is received.  
  boot_state = FIRST_REC_COLON;
  
  // Initial absolute address is zero. When an extended linear address record is read,
  // the extended linear address stored in the data field is saved and 
  // is applied to subsequent records read from the Intel HEX file.
  record_addr.l = 0x00;  	
	
	flash_passcode = FLASH_UNLOCK; // flash_enable
  while(boot_loop());     // state machine for Hex record parsing.
	flash_passcode = 0;     // flash_disable
  
	put_str(send_ack);
	/* Reset, so app has normal hardware state. */
  hwreset_device(); // watchdog reset, jump to reset vector '0'
  
} 

/****************************************************************************
* Description:
*       This function transitions to different states in order to parse
*       Intel hex record and after receiving the complete valid record 
*       it writes the record data on to the flash memory.
* Input:
*       Intel hex record received over the UART
* Parameters:
* Output:
*       Performs intel hex parsing and flash write operations 
* Return Status:
*       Returns false (0) when EOF record is received. This exit the 
*       Bootloader mode.
*       Return true (1) - code execution remains in bootloader mode.
*****************************************************************************/
char led_flash_var;
static int boot_loop(void)
{
   int done = true; 
   uint8_t *char_ptr;

   switch(boot_state)
   {

// Record COLON (marker): 1 character; a colon (ASCII 0x3A) to identify the line as an Intel HEX file
// WatchDog is refreshed in serial receive polling loop, till the first COLON is received from the host.
   case FIRST_REC_COLON:
         if(':' == ser_poll_rx_ch())
         {   
           data_ptr = (uint8_t*)&data_buff[0];   // ready for new data record, start of 32-bit data buffer assigned 
                                                 // to a byte pointer.
           boot_state = REC_LEN;                 // Colon received, start of Intel Hex Record.
         } 
   break;
				 
// Record COLON (marker): 1 character; a colon (ASCII 0x3A) to identify the line as an Intel HEX file
   case REC_COLON:
         if(':' == ser_poll_rx_ch_no_wd())
         {   
           data_ptr = (uint8_t*)&data_buff[0];   // ready for new data record, start of 32-bit data buffer assigned
                                                 // to a byte pointer.
           boot_state = REC_LEN;                 // Colon received, start of Intel Hex Record.
         } 
   break;

// Record Length: 2-digit hex number; This field contains the number of data bytes in the data field. 
   case REC_LEN:
         record_len = get_hex();
         record_checksum = record_len;   // First character for checksum calculation
         rec_len_cpy = record_len;       // Copy of record len 
         boot_state = REC_ADDR;
   break;

// Record Address: 4-digit hex number; This field contains the address where the data should be loaded into the code memory. 
   case REC_ADDR:
         record_addr.c[LO_HI] = get_hex();
         record_checksum += record_addr.c[LO_HI];   // Compute checksum
         record_addr.c[LO_LO] = get_hex();
         record_checksum += record_addr.c[LO_LO];   // Compute checksum
         boot_state = REC_TYPE;
   break;

// Record Type:  2-digit hex number; This field indicates the type of record.
   case REC_TYPE:
        record_type =  get_hex();
        record_checksum += record_type;   // Compute checksum
        boot_state = REC_DATA;            // Get record data
   break;

// Record Data: 2x digit hex array for a sequence of x bytes of the data.  
   case REC_DATA:
       while(record_len)  // If record length is zero, like for EOF Record type, go directly to the next state (checksum).
       {
           *data_ptr = get_hex();
           record_checksum += *data_ptr;  // Compute checksum
           data_ptr++;
           record_len--;
       } 
       boot_state = REC_CHKSUM;
   break;

// Record Checksum: 2-digit hex number, 2's complement of the sum of the values of all fields except fields Colon and checksum. 
   case REC_CHKSUM:
        record_checksum += get_hex();      // Add Checksum, if zero checksum correct
        if(record_checksum)                // non-zero, Checksum incorrect
        {
          boot_state = REC_COLON;
          ser_poll_tx_ch('F');             // (F)ail , checksum incorrect
        }
        else  // Record checksum okay
        {
          if (record_type == DATA_REC)   // Data record
          {
              boot_state = FLASH_RECORD; // Flash the data 
          }
          
          else if(record_type == EOF_REC) // End of file record, All data records have been received, 
                                          // now exit boot-loader.
          {
              //ser_poll_tx_ch('.');     // '.', record okay 
              done = false;            // Exit boot while loop.
              boot_state = REC_COLON;  
          } 
          
          else  // For Extended record types -  ESA_REC , SSA_REC , ELA_REC, SLA_REC 
          {
              char_ptr = (uint8_t*)data_buff;
              record_addr.c[HI_HI] =  *char_ptr++;
              record_addr.c[HI_LO] =  *char_ptr++;
              //ser_poll_tx_ch('.');                    // '.', record okay 
              boot_state = REC_COLON;  
          } 
        }
   break;

// Write data record in the flash memory
   case FLASH_RECORD:
        
        if(record_addr.l >= app_start_addr)  // Check valid Address range, Boot-loader doesn't erase/write to itself.
        { 
          // 32-bit word or its multiple in Intel Hex record. 
          // Hex file post-processing is required to align record data length to be multiple of 4. 
          flash_write(record_addr.l,(uint32_t *)&data_buff[0],rec_len_cpy);
          if(0 == memcmp((uint32_t *)record_addr.l,(uint32_t *)&data_buff[0],rec_len_cpy))
            ser_poll_tx_ch('P');               // (P)ass, record write success 
          else
            ser_poll_tx_ch('F');               // (F)ail, record write failed 
        }
        else
        {
            ser_poll_tx_ch('X'); // Bootloader    
        }
				
					//*********************************************    

						
//        led_flash_var++;
//						
//				if(led_flash_var%5==0)
//				{
//					LCD->DIO_b[42].data = 1;
//				}
//					
//			if(led_flash_var%10==0)
//			{
//					
//				LCD->DIO_b[42].data = 0; 
//				led_flash_var=0;
//			}
  //*********************************************    

        boot_state = REC_COLON;               // Look for next record.
   break;

  } 
  
  // refresh WATCHDOG timer.
  RESET_WD(); 
 
  return done;
}

/****************************************************************************
* Description:
*     Initialze the system clock
* Input:
*       none
* Parameters:
* Output: Adjusts SystemCoreClock
* Return Status:
*       None.
*****************************************************************************/
static void boot_system_init(void)
{
  if(true == SYS->STAT_b.v3a_nok)
  { // IC power supply is not ok
      while(1)
			{
				SYS->MOD_CNTL = 3; // go to sleep
			}
  }
  
  // Wait for the clock's PLL to stabilize.
  // This is absolutely required before the write to the flash controller,
  // below.
  while(0 == SYS->STAT_b.pll_ok)
    ;

  FLASH->CNTL = 0;                    // Clear any pass code.
    
//	SYS->MPU_CLK_b.mpu_spd = 0; // Update mpu_spd
//  ADC->CNTL_b.adc_vls  = 0; // Update adc_vls
//	SystemCoreClock = PLL_CLOCK/2;
	
	//	SYS->MPU_CLK_b.mpu_spd = 1; // Update mpu_spd
//  ADC->CNTL_b.adc_vls  = 0; // Update adc_vls
//	SystemCoreClock = PLL_CLOCK/4;
	
		SYS->MPU_CLK_b.mpu_spd = 2; // Update mpu_spd
  ADC->CNTL_b.adc_vls  = 0; // Update adc_vls
	SystemCoreClock = PLL_CLOCK/8;
		//Dividing Factor
		//mpu_spd=0---10.223616MHz
		//mpu_spd=1---5.11808MHz
		//mpu_spd=2---2.555904MHzS ystemCoreClock,8
		//mpu_spd=3---851.968kHz SystemCoreClock,24
		//mpu_spd=4---425.984kHz
}

/****************************************************************************
* Description:
*    Initialize UART pins, Baudrate.
* Parameters:
*   Input:
*         uint32_t baud: desired baud rate
*   Output:
*         None.
* Return Status:
*        None.
****************************************************************************/
void ser_poll_init(uint32_t baud)
{
    UART->CNTL = 0;        // Stop the device.
    // program the baud rate divider register as i.e. 10,000,000/9600 = 1041
    UART->BAUD_b.baud_div = SystemCoreClock/baud;

	  LCD->DIO_b[tx_seg].alt = 1; // TX2
	  LCD->DIO_b[rx_seg].alt = 1; // RX2
	
    UART->STATE = 0x0C;        // Clear buffer overrun states
    UART->INT = 0x0F;          // Clear all interrupt flags

    // TX, RX enable 
    UART->CNTL_b.rx_en = 1; 
    UART->CNTL_b.tx_en = 1;
    // TX, RX interrupt disable
    UART->CNTL_b.rx_ie = 0;
    UART->CNTL_b.tx_ie = 0;
    // TX, RX overrun interrupt disable
    UART->CNTL_b.rxov_ie = 0;
    UART->CNTL_b.txov_ie = 0;
}

/****************************************************************************
* Description:
*    Write a character to Serial Port - blocking call (for printf - retarget.c)
* Parameters:
*   Input:
*         int c: character to be transmitted
*   Output:
*         None.
* Return Status:
*        None.
****************************************************************************/
int ser_poll_tx_ch(int c)
{
    /* Wait if Transmit Holding register is full */
    while (UART->STATE_b.txbf)
    {
        RESET_WD();  // Set the watchdog clear bit
    }
    /* write to transmit holding register		 */
    UART->DATA = c;
    return (c);
}

/****************************************************************************
* Description:
*       Send a char string to serial.
* Input:
*       str: pointer to output stirng
*       sz: size of the output string in bytes
* Parameters:
* Output: Output string is sent to serial out
* Return Status:
*       None.
*****************************************************************************/
static void put_str(const char *str)
{
    while(0 != *str)
        ser_poll_tx_ch(*str++);
}

/****************************************************************************
* Description:
*    Read character from Serial Port   (blocking read)  
* Parameters:
*   Input:
*         None.
*   Output:
*         Character received at the SER1 data buffer.
* Return Status:
*        None.
****************************************************************************/
int ser_poll_rx_ch (void) 
{
    /* Wait if Receive Holding register is empty */
    while (!UART->STATE_b.rxbf)
    {
       RESET_WD();  // Set the watchdog clear bit
			
			if (SYS->STAT_b.v3a_nok)//If Power Supply is not OK
			SYS->MOD_CNTL = 1;//Sleep
    }
    return (UART->DATA);
}

/****************************************************************************
* Description:
*    Read string from Serial Port   (blocking read)  
* Parameters:
*   Input:
*         None.
*   Output:
*         Character received at the SER data buffer.
* Return Status:
*        Number of characters received.
****************************************************************************/
int ser_poll_rx_str (char * str) 
{
  int index = 0;
  char ch = 0xff;
  // Wait for carriage return
  while('\r' != ch) 
  {
    ch = str[index] = ser_poll_rx_ch();
    ser_poll_tx_ch(ch); // echo
    if('\b' == str[index]) // Backspace
    {
      if(index)
        index--; // Move index back on backspace
    }
    else
      index++; // Get ready for next character
   }
  return index; // Number of characters are read
}

/****************************************************************************
* Description:
*    Read character from Serial Port but don't wait endllesly
* Parameters:
*   Input:
*         None.
*   Output:
*         Character received at the SER1 data buffer.
* Return Status:
*        None.
****************************************************************************/
int ser_poll_rx_ch_no_wd (void) 
{
    /* Wait if Receive Holding register is empty */
    while (!UART->STATE_b.rxbf)
    {
       // Let watchdog reset the part if stuck here
    }
    return (UART->DATA);
}

/****************************************************************************
* Description:
*       Get next hexadecimal digit.
* Input:
*       character from UART.
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
static uint8_t get_digit (void)                     
{
    uint8_t chr;
    
    chr = ser_poll_rx_ch_no_wd();         // Get character from UART.
    chr -= '0';                      // '0' mapped to  0.
    if (chr >= 10)                       
    {
        chr &= ~TO_UPPERCASE;        // Covert lower case alphabets to Upper; 
                                     // 'a' 0x61  to 'A' 0x41 and likewise for others.
        chr -= 'A' - '0' - 10;       // 0xA (hex) is converted to 10 (decimal)
    }
    return (chr);
}

/****************************************************************************
* Description:
*       Converts two ASCII hex digits to a character.
* Input:
*       UART receive characters
* Parameters:
* Output: a binary format character. 
* Return Status:
*       None.
*****************************************************************************/
static uint8_t get_hex (void)
{
       return (uint8_t)((get_digit() << 4) | get_digit());
}


/****************************************************************************
* Description:
*       Flush out serial rx (HW) buffer 
* Input:
* Parameters:
* Output:
* Return Status:
*       None.
*****************************************************************************/
//static char flush_ser(void)
//{
//	char ch;
//	// Flush the rx buffer
//	if (UART->STATE_b.rxbf)
//	{
//		ch = UART->DATA;
//	}
//	return ch;
//}

/*****************************************************************************
* Description:
*  Erase all the flash pages of the application space. 
*  Boot loader spade is intactr
* Parameters:
*   Input:
*   Output:
*         Erase flash array of entire application space.
* Return:
*         0 - OK,  1 - Failed
*****************************************************************************/
static void flash_erase_app(void)
{
    unsigned int adr;

    for(adr = APP_START; adr < FLASH_SIZE; adr += FLASH_PAGE_SZ)
    {
        FLASH->PROT = 0x100; // Enable flash page erase

        // Specify page address
        FLASH->ADDR  = FLASH_ADDR_LONG_WORD(adr);

        // Issue page erase command
        FLASH->CNTL = (FLASH_UNLOCK | FLASH_PAGE_ERASE);

        // Wait for erasure to complete
				while( FLASH_BUSY_BAR != (FLASH->CNTL & FLASH_BUSY_BAR))
				{
					RESET_WD();
				} // loop here till flash controller is busy
    }
    /* Clear Flash control register */
    FLASH->ADDR  = ADD_OUTOFBOUND;
    FLASH->CNTL = 0;
    FLASH->PROT = 0;
}


/*****************************************************************************
* Description:
*  Program Flash Memory for given buffer and address
* Parameters:
*   Input:
*         adr:  Start Address
*         buf:  Write buffer (pattern)
*          sz:  Size of write buffer
*   Output:
*         Write data buf to flash (appliction space)
* Return:
*         0 - OK,  1 - Failed
*****************************************************************************/
int flash_write(uint32_t adr, uint32_t *buf, int sz)
{
    int retries = retry_cnt;
    uint32_t *data = (uint32_t *)adr;

    //adr must be in the 4-byte boundaries, e.g. 0, 4, 8 etc.
    if ((adr % WORD_SZ) != 0 || adr < APP_START)
        return 1;
    if (sz >= WORD_SZ)
    {    //Must be writing in long Word
        // write in 32-bit units while we can
        flash_cn  = FLASH->CNTL;
        flash_cn &= ~FLASH_WR;
        flash_cn |= (flash_passcode | FLASH_WR);

        FLASH->PROT = 0x100; // Enable flash write

        while (sz >= WORD_SZ)
        {
            while(*data != *buf && retries-- > 0)
            {
                FLASH->ADDR  = FLASH_ADDR_LONG_WORD(adr);
                FLASH->DATA = *buf;
                flash_cn |= (flash_passcode | FLASH_WR);
                FLASH->CNTL = flash_cn;

                wait_flash_busy();// loop here till flash controller is busy
            }

            /* Wait until flash operation is complete */

            buf++;
            data++;
            adr += WORD_SZ;
            sz  -= WORD_SZ;
            retries = retry_cnt;
        }

        flash_cn  = FLASH->CNTL;
        flash_cn &= ~(flash_passcode | FLASH_WR); // clear 'FLO', disable flash op.
        FLASH->CNTL = flash_cn;

        FLASH->PROT = 0; // Prevent flash write
    }
    
    return (0); // Finished without Errors
} // flash-write()



/*****************************************************************************
* Description:
*     Wait for flash operation to complete.
* Parameters:
* Input:
* Output:
* Return:
*****************************************************************************/
static void wait_flash_busy(void)
{
    while( FLASH_BUSY_BAR != (FLASH->CNTL & FLASH_BUSY_BAR))
    {
    } // loop here till flash controller is busy
} // wait-flash-busy()

/*****************************************************************************
* Description:
*     HW reset the part
* Parameters:
* Input:
* Output:
* Return:
*****************************************************************************/
static void hwreset_device(void)
{
	SYS->MOD_CNTL_b.reset = 1;
  __disable_irq(); // fallback for SYSTEM reset
  while(1);     
}
/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

/* end of file */



