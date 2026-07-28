/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: GPIO function declarations
 * $Id: gpio.h 17102 2018-03-26 18:32:26Z rvandewa $  
 ***************************************************************************/

#ifndef GPIO_H     
#define GPIO_H

extern const char fw_ver_gpio[];

// Returns nonzero if a GPIO is already an LCD pin.
extern int gpio_is_lcd(uint32_t idx);
// Initializes GPIOs on AHB bus
extern void gpio_init(void);
// Sets a GPIO direction as input 
extern void gpio_dir_in(uint32_t idx);
// Sets a GPIO direction as output 
extern void gpio_dir_out(uint32_t idx);
// Set a GPIO state
extern void gpio_set_state(uint32_t idx, int gpio_state);
// Read state of a GPIO
extern int gpio_get_state(uint32_t idx);
// Sets LCD segment to the GPIO use.  
extern void gpio_alt_off(uint32_t idx);
// Sets LCD segment to the alternate use.  
extern void gpio_alt_on(uint32_t idx);

#endif //GPIO_H


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
