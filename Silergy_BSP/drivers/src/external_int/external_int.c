/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Interrupt Handler for GPIO ports using APB 
 *
 * Copyright 2017, Silergy Corp.   All rights reserved.
 * $Id: external_int.c 24555 2020-04-23 21:41:40Z ptanwar $  
 ****************************************************************************/
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Needed by rtc.h
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "external_int.h"

/*** Version ****/
const char fw_ver_external_int[] =
"external_int"    /* Device Driver name */
" v"      /* ver string */
"1.02"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

// External interrupt call-back.
// At start, the callback array is filled with zeros, i.e. NULL.
extern void HardFault_Handler(void); // delcared the in startup file
static void (*ext_int_fn[LCD_PIN_CNT])(void); 

/****************************************************************************
* Description:
*       Sets a GPIO as an external interrupt via APB 
* Input:
*				idx: DIO number
*				edge: Edge or level triggered?
*				polarity: Rising (high) or falling (low)
*				fn: function pointer to the call back 
* Output:
*				None.
* Return Status:
*       None.
*****************************************************************************/
void external_int_init(int idx, bool edge, bool polarity, void (*fn)(void))
{
	int parse_dio = 0;

	if (idx >= LCD_PIN_CNT)
			return;
	
	// Initialize all uninitliatized callback to HardFault_Handler
	while(parse_dio < LCD_PIN_CNT)
	{
		if(NULL == ext_int_fn[parse_dio])
			ext_int_fn[parse_dio] = HardFault_Handler;
		parse_dio++;
	}
	
	ext_int_fn[idx] = fn; // register address of callback function
	
	LCD->DIO_b[idx].dir = 0; // Make DIO_idx as i/p

	LCD->DIO_b[idx].flag = 0; // Clear spurious interrupt flags
	LCD->DIO_b[idx].mask = 1; // Set DIO_idx as interrupt
	
	if(true == edge)
		LCD->DIO_b[idx].alt = 0; // Set interrupt as edge triggered
	else
		LCD->DIO_b[idx].alt = 1; // Set interrupt as level triggered
	
	if(true == polarity) 
		LCD->DIO_b[idx].pol = 1; // Polarity : rising edge / HIGH level
	else
		LCD->DIO_b[idx].pol = 0; // Polarity : falling edge / LOW level

	// Enable IRQ for the port
	if(idx<16)
		NVIC_EnableIRQ(PORT0_IRQn);
	else if(idx < 31)
		NVIC_EnableIRQ(PORT1_IRQn);
	else 
		NVIC_EnableIRQ(EXT_IRQn);
}

/****************************************************************************
* Description:
*     	Disable external interrupt for a given dio
* Input:
*				idx: DIO number
* Output:
*				None.
* Return Status:
*       None.
*****************************************************************************/
void external_int_deinit(int idx)
{
	LCD->DIO_b[idx].flag = 0; // Clear  interrupt flags
	LCD->DIO_b[idx].mask = 0; // Disable DIO_idx as interrupt
	
	ext_int_fn[idx] = HardFault_Handler; // Unregister the callback
}


/****************************************************************************
* Description:
*     	Interrupt handler for PORT0 - DIOs 0..15
* Input:
*				None.
* Output:
*				None.
* Return Status:
*       None.
*****************************************************************************/
void PORT0_Handler(void)
{
	int idx = 0;
	while(idx++ < 16)
	{
		if(LCD->DIO_b[idx].flag) // Interrupt is raised
		{
			LCD->DIO_b[idx].flag = 1; // Clear the interrupt flag			
			if(HardFault_Handler != ext_int_fn[idx])
				(*ext_int_fn[idx])(); // Call the callback
		}
	}
}

/****************************************************************************
* Description:
*     	Interrupt handler for PORT0 - DIOs 16..31
* Input:
*				None.
* Output:
*				None.
* Return Status:
*       None.
*****************************************************************************/
void PORT1_Handler(void)
{
	int idx = 16;
	while(idx++ < 32)
	{
		if(LCD->DIO_b[idx].flag) // Interrupt is raised
		{
			LCD->DIO_b[idx].flag = 1; // Clear the interrupt flag			
			if(HardFault_Handler != ext_int_fn[idx])
				(*ext_int_fn[idx])(); // Call the callback

		}
	}
}

/****************************************************************************
* Description:
*     	Interrupt handler for DIOs past the last port.
* Input:
*				None.
* Output:
*				None.
* Return Status:
*       None.
*****************************************************************************/
void EXT_Handler(void)
{
	int idx = PORT_INT_CNT;
	while(idx++ < LCD_PIN_CNT)
	{
		if(LCD->DIO_b[idx].flag) // Interrupt is raised
		{
			LCD->DIO_b[idx].flag = 1; // Clear the interrupt flag
			if(HardFault_Handler != ext_int_fn[idx])
				(*ext_int_fn[idx])(); // Call the callback
		}
	}	
}

/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 

