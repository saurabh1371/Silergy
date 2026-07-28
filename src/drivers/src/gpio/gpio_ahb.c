/*----------------------------------------------------------------------------
 * Purpose: Driver for GPIO ports on AHB 
 * $Id: gpio_ahb.c 24540 2020-04-23 18:20:44Z ptanwar $ 
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
const char fw_ver_gpio_ahb[] =
"gpio_ahb"    /* Device Driver name */
" v"      /* ver string */
"1.01"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;


/****************************************************************************
* Description:
*       Sets a GPIO direction as input 
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
  if(0 == port) // Port 0  DIO0..DIO15
  {
    PORT0->OUTENCLR = (1UL<<dio_in);      // port pin direction input
  }
  else if(1 == port)// port 1 DIO16..DIO31
  {
		dio_in -= 16; // bit 0..15
    PORT1->OUTENCLR = (1UL<<dio_in);      // port pin direction input
  }

}

/****************************************************************************
* Description:
*       Sets a GPIO direction as output 
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
  if(0 == port) // port 0 DIO0..DIO15
  {
    PORT0->OUTENSET = (1UL<<dio_out);      // port pin direction output
  }
  else if (1 == port) // port 1 DIO32..DIO47
  {
		dio_out -= 16; // bit 0..15
    PORT1->OUTENSET = (1UL<<dio_out);      // port pin direction output
  }
}

/****************************************************************************
* Description:
*       Set a GPIO state by setting or clearing corresponding bit in GPIOx.
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
  if(0 == port) // Port 0 DIO0..DIO15
  {
    if(1 == gpio_state)  
      PORT0->DATAOUT |= (1UL<<dio_num);      // Drive output high
    else
      PORT0->DATAOUT &= ~(1UL<<dio_num);     // Drive output low
  }
  else if(1 == port)// port 1 DIO32..DIO47
  {
		dio_num -= 16; // bit 0..15
    if(1 == gpio_state)
      PORT1->DATAOUT |= (1UL<<dio_num);      // Drive output high
    else
      PORT1->DATAOUT &= ~(1UL<<dio_num);     // Drive output low
  }
}

/****************************************************************************
* Description:
*       Read state of a GPIO by reading corresponding bit in GPIOx.
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

  if(0 == port) // Port 0 DIO0..DIO15
    pi_copy = PORT0->DATA;
  else if(1 == port) // Port 1 DIO32..DIO47
	{
		dio_num -= 16; // bit 0..15
    pi_copy = PORT1->DATA;
	}

  gpio_state = (pi_copy>>dio_num) & 1;
  return gpio_state;
}

/***************************************************************************   
 * Description:  
 *  Sets all LCD segment to the alternate use.  This causes the IC to manage
 *  it as a native GPIO pin, with the highest-priority use taking control
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
	PORT1->ALTFUNCCLR = 0xFFFF;  // DIO32..DIO47
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 

