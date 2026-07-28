/*----------------------------------------------------------------------------
 * Purpose: Driver for GPIO ports on AHB 
 * $Id: gpio_m18_ahb.c 24540 2020-04-23 18:20:44Z ptanwar $ 
 *----------------------------------------------------------------------------
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *----------------------------------------------------------------------------*/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "gpio_ahb.h"

/*** Version ****/
const char fw_ver_gpio_m18_ahb[] =
"gpio_m18_ahb"    /* Device Driver name */
" v"      /* ver string */
"1.01"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;


/****************************************************************************
* Description:
*       Sets a GPIO direction as input 
* Note:
*   Not all DIOs are present in all of the ICs, refer HRM for availability
*   DIOs are not available in 68 pin part: DIO0,DIO1,DIO6,DIO7,DIO21, 
*   DIO22,DIO26,DIO36,DIO38,DIO47-DIO57,DIO63-DIO69.
* Input:
* Parameters:
*       port is the index of the GPIO port.
*       dio_in is the GPIO number to be made input.
* Output:
* Return Status:
*       None.
*****************************************************************************/
void gpio_dir_in(uint32_t port, uint32_t dio_in)
{
	switch(port)
	{
		case 0: // DIO0..DIO15
			PORT0->OUTENCLR = (1UL<<dio_in);      // port pin direction input
		break;
		case 1: // DIO16..DIO31
			dio_in-= 16; // bit 0..15
			PORT1->OUTENCLR = (1UL<<dio_in);      // port pin direction input
		break;
		case 2: // DIO31..DIO47
			dio_in-= 32; // bit 0..15
			PORT2->OUTENCLR = (1UL<<dio_in);      // port pin direction input
		break;
		case 3: // DIO48..DIO63
			dio_in-= 48; // bit 0..15
			PORT3->OUTENCLR = (1UL<<dio_in);      // port pin direction input
		break;		
	}
}

/****************************************************************************
* Description:
*       Sets a GPIO direction as output 
* Note:
*   Not all DIOs are present in all of the ICs, refer HRM for availability
*   DIOs are not available in 68 pin part: DIO0,DIO1,DIO6,DIO7,DIO21, 
*   DIO22,DIO26,DIO36,DIO38,DIO47-DIO57,DIO63-DIO69.
* Input:
* Parameters:
*       port is the index of the GPIO port.
*       dio_out is the GPIO number to be made output.
* Output:
* Return Status:
*       None.
*****************************************************************************/
void gpio_dir_out(uint32_t port, uint32_t dio_out)
{
	switch(port)
	{
		case 0: // DIO0..DIO15
			PORT0->OUTENSET = (1UL<<dio_out);      // port pin direction output
		break;
		case 1: // DIO16..DIO31
			dio_out-= 16; // bit 0..15
			PORT1->OUTENSET = (1UL<<dio_out);      // port pin direction output
		break;
		case 2: // DIO31..DIO47
			dio_out-= 32; // bit 0..15
			PORT2->OUTENSET = (1UL<<dio_out);      // port pin direction output
		break;
		case 3: // DIO48..DIO63
			dio_out-= 48; // bit 0..15
			PORT3->OUTENSET = (1UL<<dio_out);      // port pin direction output
		break;		
	}
}

/****************************************************************************
* Description:
*       Set a GPIO state by setting or clearing corresponding bit in GPIOx.
* Note:
*   Not all DIOs are present in all of the ICs, refer HRM for availability
*   DIOs are not available in 68 pin part: DIO0,DIO1,DIO6,DIO7,DIO21, 
*   DIO22,DIO26,DIO36,DIO38,DIO47-DIO57,DIO63-DIO69.
* Input:
* Parameters:
*       port is the index of the GPIO port.
*       dio_num is the GPIO number to be made output.
*       state is the output value (high=1 and Low=0 ) for the GPIO pin.
* Output:
* Return Status:
*       None.
*****************************************************************************/
void gpio_set_state(uint32_t port, uint32_t dio_num, int gpio_state)
{
	switch(port)
	{
		case 0: // DIO0..DIO15
			if(1 == gpio_state)  
				PORT0->DATAOUT |= (1UL<<dio_num);      // Drive output high
			else
				PORT0->DATAOUT &= ~(1UL<<dio_num);     // Drive output low
		break;
		case 1: // DIO16..DIO31
			dio_num -= 16; // Data output value [15:0]
			if(1 == gpio_state)  
				PORT1->DATAOUT |= (1UL<<dio_num);      // Drive output high
			else
				PORT1->DATAOUT &= ~(1UL<<dio_num);     // Drive output low
		break;
		case 2: // DIO32..DIO47
			dio_num -= 32; // Data output value [15:0] 
			if(1 == gpio_state)  
				PORT2->DATAOUT |= (1UL<<dio_num);      // Drive output high
			else
				PORT2->DATAOUT &= ~(1UL<<dio_num);     // Drive output low
		break;
		case 3: // DIO48..DIO63
			dio_num -= 48; // Data output value [15:0] 
			if(1 == gpio_state)  
				PORT3->DATAOUT |= (1UL<<dio_num);      // Drive output high
			else
				PORT3->DATAOUT &= ~(1UL<<dio_num);     // Drive output low
		break;			
	}
}

/****************************************************************************
* Description:
*       Read state of a GPIO by reading corresponding bit in GPIOx.
* Note:
*   Not all DIOs are present in all of the ICs, refer HRM for availability
*   DIOs are not available in 68 pin part: DIO0,DIO1,DIO6,DIO7,DIO21, 
*   DIO22,DIO26,DIO36,DIO38,DIO47-DIO57,DIO63-DIO69.
* Input:
* Parameters:
*       port is the index of the GPIO port.
*       dio_num is the GPIO number to be read.
* Output:
* Return Status:
*       state is GPIO logic state (true/false).
*****************************************************************************/
int gpio_get_state(uint32_t port, uint32_t dio_num)
{
  int gpio_state;
  uint32_t pi_copy;
	
	switch(port)
	{
		case 0: // DIO0..DIO15
			pi_copy = PORT0->DATA;
		break;
		case 1: // DIO16..DIO31
			dio_num -= 16; 
			pi_copy = PORT1->DATA;
		break;		
		case 2: // DIO32..DIO47
			dio_num -= 32;
			pi_copy = PORT2->DATA;
		break;		 
		case 3: // DIO48..DIO63
			dio_num -= 48; 
			pi_copy = PORT3->DATA;
		break;				
	}
  gpio_state = (pi_copy>>dio_num) & 1;
  return gpio_state;
}

/***************************************************************************   
 * Description:  
 *  Sets all LCD segment to the alternate use.  This causes the IC to manage
 *  it as a native GPIO pin, with the highest-priority use taking control
 * Note:
 *   Not all DIOs are present in all of the ICs, refer HRM for availability
 *   DIOs are not available in 68 pin part: DIO0,DIO1,DIO6,DIO7,DIO21, 
 *   DIO22,DIO26,DIO36,DIO38,DIO47-DIO57,DIO63-DIO69.
 * Parameters:
 *   Input:
 *         None.
 *   Output: 
 *         None.
 * Return Status: 
 *         None
 ***************************************************************************/
void gpio_init(void)
{
	// GPIOx->ALTFUNCCLR: Each of these bits controls the function of the corresponding 
	// LCD/DIO pin. These must be cleared or set somewhere. In a full meter, they
	// are usually set in the lcd logic. In other code, they are usually cleared
	// by SystemInit

	// If a pin is not in use by the LCD module, set it to alternate.
	// intialize GPIOs by clearing alternate functions
	PORT0->ALTFUNCCLR = 0xFFFF;  // DIO0..DIO15
	PORT1->ALTFUNCCLR = 0xFFFF;  // DIO16..DIO31
	PORT2->ALTFUNCCLR = 0xFFFF;  // DIO32..DIO47
	PORT3->ALTFUNCCLR = 0xFFFF;  // DIO48..DIO63
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 

