/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Serial function declarations
 * $Id: gpio_ahb.h 16212 2017-12-08 00:31:28Z ptanwar $  
 ***************************************************************************/

#ifndef GPIO_H     
#define GPIO_H

extern const char fw_ver_gpio_ahb[];

// Initializes GPIOs on AHB bus
extern void gpio_init(void);
// Sets a GPIO direction as input 
extern void gpio_dir_in(uint32_t port, uint32_t dio_in);
// Sets a GPIO direction as output 
extern void gpio_dir_out(uint32_t port, uint32_t dio_out);
// Set a GPIO state
extern void gpio_set_state(uint32_t port, uint32_t dio_num, int gpio_state);
// Read state of a GPIO
extern int gpio_get_state(uint32_t port, uint32_t dio_num);

#endif //GPIO_H


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
