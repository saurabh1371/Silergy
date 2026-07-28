/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Interrupt Handler for GPIO ports using AHB 
 *
 * Copyright 2017, Silergy Corp.   All rights reserved.
 * $Id: external_int_ahb.c 24555 2020-04-23 21:41:40Z ptanwar $  
 ****************************************************************************/
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Booleans.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "external_int_ahb.h"

/*** Version ****/
const char fw_ver_external_int_ahb[] =
"external_int_ahb"    /* Device Driver name */
" v"      /* ver string */
"1.01"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

// External interrupt call-back.
// At start, the callback array is filled with zeros, i.e. NULL.
extern void HardFault_Handler(void); // delcared the in startup file
static void (*ext_int_fn[PORT_INT_CNT])(void); 

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
void external_int_ahb_init(int idx, bool edge, bool polarity, void (*fn)(void))
{
	PORT0_Type * AHB_GPIO;
	uint32_t mask;
	int parse_dio = 0;

    if(idx >= PORT_INT_CNT)
        return;
		
	// Initialize all uninitliatized callback to HardFault_Handler
	while(parse_dio < PORT_INT_CNT)
	{
		if(NULL == ext_int_fn[parse_dio])
			ext_int_fn[parse_dio] = HardFault_Handler;
		parse_dio++;
	}		
	
	ext_int_fn[idx] = fn; // register address of callback function
	
	if(idx<16)
	{
		AHB_GPIO =  PORT0;//  Ptr to GPIO port 0 (DIO 0 to 15)
	}
	else if (idx < 31)
	{
		AHB_GPIO = PORT1; // Ptr to GPIO port 1 (DIO 16 to 31)
		idx = idx - 16; // DIO16 is at idx 0...DIO31 is at idx 15
	}
	else
		return; // GPIO beyond 31 are not accessible thru AHB, use APB.
	
	mask = 1 << idx;
	
	// Writing '0' to GPIOx doesn't make any change
	// We can write 1 to a bit position, other bits will be untouched 
	AHB_GPIO->OUTENCLR = mask; // Make DIO_idx as i/p

	AHB_GPIO->INTSTATUS = mask; // Clear spurious interrupt flags
	AHB_GPIO->INTENSET = mask; // Set DIO_idx as interrupt
	
	if(true == edge)
		AHB_GPIO->INTTYPESET = mask; // Set interrupt as edge triggered
	else
		AHB_GPIO->INTTYPECLR = mask; // Set interrupt as level triggered
	
	if(true == polarity) 
		AHB_GPIO->INTPOLSET  = mask; // Polarity : rising edge / HIGH level
	else
		AHB_GPIO->INTPOLCLR  = mask; // Polarity : falling edge / LOW level

	if(idx<16)
		NVIC_EnableIRQ(PORT0_IRQn);
	else 
		NVIC_EnableIRQ(PORT1_IRQn);
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
void external_int_ahb_deinit(int idx)
{
	PORT0_Type * AHB_GPIO;
	uint32_t mask;
	
	ext_int_fn[idx] = HardFault_Handler; // Unregister the callback
	
	if(idx<16)
		AHB_GPIO = PORT0; // Ptr to GPIO port 0 (DIO 0 to 15)
	else if (idx < 31)
	{
		AHB_GPIO = PORT1; // Ptr to GPIO port 1 (DIO 16 to 31)
		idx = idx - 16; // DIO16 is at idx 0...DIO31 is at idx 15
	}
	else
		return; // GPIO beyond 31 are not accessible thru AHB, use APB.	
	
	mask = 1 << idx;
	
	AHB_GPIO->INTSTATUS = mask; // Clear interrupt flag
	AHB_GPIO->INTENCLR = mask; // Disable DIO_idx interrupt
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

    while(PORT0->INTSTATUS && idx++ < 16 )
    {
        if( PORT0->INTSTATUS & (1<<idx) )
        {
            PORT0->INTSTATUS = (1<<idx);					
            if(NULL != ext_int_fn[idx])
                (*ext_int_fn[idx])();
        }
    }
}

/****************************************************************************
* Description:
*     	Interrupt handler for PORT1 - DIOs 16..31
* Input:
*				None.
* Output:
*				None.
* Return Status:
*       None.
*****************************************************************************/
void PORT1_Handler(void)
{
    int idx = 0;

    while(PORT1->INTSTATUS && idx++ < 16 )
    {
        if( PORT1->INTSTATUS & (1<<idx) )
        {
            PORT1->INTSTATUS = (1<<idx);

            // PORT1 idx is 0..16 so add 16 to get absolute idx of call back 
            idx += 16;
            if(NULL != ext_int_fn[idx]) 
                (*ext_int_fn[idx])();

        }
    }
}

/***************************************************************************
 * Copyright (C) 2020 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
 

