/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Driver for GPIO ports on APB 
 *
 * Copyright 2017, Silergy Corp.   All rights reserved.
 * $Id: gpio.c 22277 2019-11-13 20:05:32Z rvandewa $ 
 ****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "gpio.h"

/*** Version ****/
const char fw_ver_gpio[] =
"gpio"    /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;


/****************************************************************************
* Description:
*       Tests if a pin is an LCD pin.
*       In LCD->MAPH and LCD->MAPL, if a bit is 1, the corresponding pin
*       is an LCD pin,
*       LCD->MAPH and LCD->MAPL must be cleared or set somewhere. 
*       In a full meter, they are usually set in the lcd logic. 
*       In other code, they are set very early by SystemInit()
*       And, this logic can be called very early in the set-up.
*       After a pin is initialized to be an LCD pin, other writing
*       to it will cause the LCD to be defective.
*       So, this module does not write to LCD pins.
* Input:
* Parameters:
*       idx is the GPIO number to be made input.
* Output:
* Return Status:
*       None.
*****************************************************************************/
int gpio_is_lcd(uint32_t idx)
{
    uint32_t mask = 0;

    // If a pin is not in use by the LCD module, set it to GPIO.
    // intialize GPIOs by clearing LCD_MAP alternate functions
    if(idx < 32) // DIO (SEG) 0..31
    {
        mask = 1 << idx;
        mask &= LCD->MAPL; // Is it an LCD pin?
    }
    else // DIO (SEG) 32..39
    {
        idx %= 32;
        mask = 1 << idx;
        mask &= LCD->MAPH; // Is it an LCD pin?
    }
    // if the LCD map bit was set, mask is not zero, so
    // the pin is an LCD pin, so do not redefine it.
    return mask;
}

/****************************************************************************
* Description:
*       Sets a GPIO direction as input 
* Input:
* Parameters:
*       idx is the GPIO number to be made input.
* Output:
* Return Status:
*       None.
*****************************************************************************/
void gpio_dir_in(uint32_t idx)
{
    if(gpio_is_lcd(idx))
        return; // do not write to its register.

    LCD->DIO_b[idx].dir = 0; // set GPIO_idx as input
}

/****************************************************************************
* Description:
*       Sets a GPIO direction as output 
* Input:
* Parameters:
*       idx is the GPIO number to be made output.
* Output:
* Return Status:
*       None.
*****************************************************************************/
void gpio_dir_out(uint32_t idx)
{
    if(gpio_is_lcd(idx))
        return; // do not write to its register.

	LCD->DIO_b[idx].dir = 1; // set GPIO_idx as output
}

/****************************************************************************
* Description:
*       Set a GPIO state by setting or clearing corresponding bit in GPIOx.
* Input:
* Parameters:
*       idx is the GPIO number to be made output.
*       state is the output value (high=1 and Low=0 ) for the GPIO pin.
* Output:
* Return Status:
*       None.
*****************************************************************************/
void gpio_set_state(uint32_t idx, int gpio_state)
{
    if(gpio_is_lcd(idx))
        return; // do not write to its register.

	LCD->DIO_b[idx].data = gpio_state; // set GPIO_x value
}

/****************************************************************************
* Description:
*       Read state of a GPIO by reading corresponding bit in GPIOx.
* Input:
* Parameters:
*       idx is the GPIO number to be read.
* Output:
* Return Status:
*       state is GPIO logic state (true/false).
*****************************************************************************/
int gpio_get_state(uint32_t idx)
{
    if(gpio_is_lcd(idx))
        return 0; // do not write to its register.

  return LCD->DIO_b[idx].data;
}

/***************************************************************************   
 * Description:  
 *  Sets LCD segment to the GPIO use.  
 * Parameters:
 *   Input:
 *         idx is the GPIO number to be set as GPIO
 *   Output: 
 *         None.
 * Return Status: 
 *         None
 ***************************************************************************/
void gpio_alt_off(uint32_t idx)
{
    if(gpio_is_lcd(idx))
        return; // do not write to its register.

	LCD->DIO_b[idx].alt = 0; // Not alternate, it is GPIO
}

/***************************************************************************   
 * Description:  
 *  Sets LCD segment to the alternate use.  
 * Parameters:
 *   Input:
 *         idx is the GPIO number to be set as alternate
 *   Output: 
 *         None.
 * Return Status: 
 *         None
 ***************************************************************************/
void gpio_alt_on(uint32_t idx)
{
    if(gpio_is_lcd(idx))
        return; // do not write to its register.

	LCD->DIO_b[idx].alt = 1; // Alternate functionality
}

/***************************************************************************   
 * Description:  
 *  Sets all LCD segment to the GPIO use. 
 *   Input:
 *         None.
 *   Output: 
 *         None.
 * Return Status: 
 *         None
 ***************************************************************************/
void gpio_init(void)
{
    char idx = 0;
    // LCD->MAP: Each of these bits controls the function of the 
    // corresponding LCD/DIO pin. These must be cleared or set somewhere. 
    // In a full meter, they are usually set in the lcd logic. 
    // In other code, they are usually cleared by SystemInit

    // If a pin is not in use by the LCD module, set it to GPIO input.
    for(idx = 0; idx<LCD_PIN_CNT; ++idx)
    {
        gpio_dir_in(idx);
        gpio_alt_off(idx);
    }
}


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 

