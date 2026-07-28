/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Slave SPI driver.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: sspi.c 24633 2020-04-27 19:04:25Z ptanwar $
 ****************************************************************************/
#include <stdlib.h>     // define NULL
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "gpio.h"		// GPIO function declarations 
#include "sspi.h"

/*** Version ****/
const char fw_ver_sspi[] =
"sspi"    /* Device Driver name */
" v"      /* ver string */
"1.00"    /* Major_ver.Minor_ver */
"\r\n"    /* CR + LF */
;

// Static declaration
// SPI Slave Interrupt Callback handler
static void (*pSSPICallBack)(void);  // Set to NULL when cleared to zero.  


/****************************************************************************
* Description:
*     Initialize SPI Slave (Passthrough Mode)
* Parameters:
*   Input:
*         smode md: SSPI Modes - PASSTHRU, SAFEMODE
*         void(*func)(void) : Pointer to the call-back function.
*   Output:
*         None.
* Return Status:
*        None.
****************************************************************************/
void sspi_init(smode sm, void(*func)(void))	
{
    sspi_deinit();

    // Sets SSPI Mode
    switch (sm)
    {
        case PASSTHRU:
            SSPI->CNTL_b.sspi_safe = 0; 
            break;

        case SAFEMODE:
            SSPI->CNTL_b.sspi_safe = 1; 
            break;
    }		 

    gpio_alt_on(sspi_csb_seg);      // alt=spi_cs
    gpio_alt_on(sspi_sclk_seg);     // alt=spi_scl 
    gpio_alt_on(sspi_cdin_seg);     // alt=spi_din
    gpio_alt_on(sspi_cdout_seg);    // alt=spi_dout

    if (NULL != func)
    {
        // Assign function pointer to event handler.
        pSSPICallBack = func;
    }	

    SSPI->CNTL_b.sspi_i = 1;        // Clear SPI Slave Interrupt 
    SSPI->CNTL_b.sspi_ie = 1;       // Enable SPI Slave Interrupt 
    SSPI->CNTL_b.sspi_en = 1;       // Enable SPI Slave
}


/*****************************************************************************
* Description:
*    de-initialize SPI Slave
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
*****************************************************************************/
void sspi_deinit(void)
{
    SSPI->CNTL = 0; // Disable SSPI, disable Interrupt.
    SSPI->CNTL_b.sspi_safe = 1; // Safe mode
}


/*****************************************************************************
* Description:
*         If SPI cmd is not 0x80 or 0x40, an SSPI interrupt is generated. 
*
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None.
******************************************************************************/
void SSPI_Handler(void)
{
    if(SSPI->CNTL_b.sspi_i) // Interrupt occurred?
    {
        SSPI->CNTL_b.sspi_i = 1; // Clear SSPI Interrupt.

        // Call Interupt Call-Back, if any
        if (NULL != pSSPICallBack)
            (*pSSPICallBack)();
    }	 
}

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
