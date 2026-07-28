/****************************************************************************
* Copyright (C) 2018 Silergy Corp. All rights Reserved. 
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to 
* any party or parties not specified in the license agreement and/or 
* nondisclosure agreement is expressly prohibited. 
*
* Description: I2C bit-banging software.
* API to control an I2C bus.
*
* $Id: i2c_gpio.c 23204 2020-02-14 21:27:56Z rvandewa $
*****************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>    // Booleans.
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "irq.h"        // Nesting interrupt enable.disable.
#include "gpio.h"	// GPIO function declarations 
#include "i2c_master.h"


/***************************************************************************/
// I2C interface, i2c.c, i2c.h
// I2C uses two wired-or lines: SCL, the clock, and SDA, the data.
// An external resistor pulls these up when the pin is high-impedance.
// To drive a pin, 1/high = unpowered, 0/low = powered.
// So, the pin's state is changed by the direction flip flop.
// So, the output flip-flop for the pin is set to zero, 
// to drive the pin low when drive is enabled.
// Since the software only drives pins low, contention is not possible.

/*** Version ****/
const char fw_ver_i2c_master[] =
"i2c_gpio"    /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

// Change this stuff for new hardware. See the map file in options.h
#ifndef PCB_SDA_SEG
#define PCB_SDA_SEG     i2c_sda_seg
#endif

// Change this stuff for new hardware. See the map file in options.h
#ifndef PCB_SCL_SEG
#define PCB_SCL_SEG     i2c_scl_seg
#endif

// I2C interface, i2c_dio.c, i2c.h
// I2C uses two wired-or lines: SCL, the clock, and SDA, the data.
// An external resistor pulls these up when the pin is high-impedance.
// To drive a pin, 1/high = unpowered, 0/low = powered.
// So, the pin's state is changed by the direction flip flop in PD.
// So, the output flip-flop for the pin (in PO) is set to zero, 
// to drive the pin low when drive is enabled.
// Since the software only drives pins low, contention is not possible.


// Change this stuff for new hardware. See the map file in options.h
// SCL is attached according to mapeb.h
#define SCL_POWERED   (LCD->DIO[PCB_SCL_SEG] = 2)
#define SCL_UNPOWERED (LCD->DIO[PCB_SCL_SEG] = 0) 
#define SCL_ONE SCL_UNPOWERED   // The external resistor pulls it high.
#define SCL_ZERO SCL_POWERED    // The value in PO pulls it low.

// SDA is attached according to mapeb.h
#define SDA_POWERED   (LCD->DIO[PCB_SDA_SEG] = 2)
#define SDA_UNPOWERED (LCD->DIO[PCB_SDA_SEG] = 0) 
#define SDA_ONE SDA_UNPOWERED   // The external resistor pulls it high.
#define SDA_ZERO SDA_POWERED    // The value in PO pulls it low.
#define SDA_READ (LCD->DIO_b[PCB_SDA_SEG].data)




/*****************************************************************************
* Description:  delay_i2c loop
* The delay_i2c is unused and untested in this part
* Parameters:
*   Input:
*   Output: 
* Return Status: 
*         None
*****************************************************************************/
//#undef DELAY_I2C
#define DELAY_I2C 10
#ifdef DELAY_I2C
static void delay_i2c(void)
{
   uint_fast8_t cnt;
   for (cnt = DELAY_I2C; cnt > 0; --cnt)
   {
   }
}
#endif

/*****************************************************************************/
// start of portable code

// The start condition has the data change while the clock is high,
// but ends with the clock low so the clock can clock bits.
#ifdef DELAY_I2C
#define START_CONDITION \
    delay_i2c(); \
    SDA_ONE; \
    delay_i2c(); \
    SCL_ONE; \
    delay_i2c(); \
    SDA_ZERO; \
    delay_i2c(); \
    SCL_ZERO
#else
#define START_CONDITION \
    SDA_ONE; \
    SCL_ONE; \
    SDA_ZERO; \
    SCL_ZERO
#endif

// the stop condition makes the data go high when the clock is high
// It has to end with both lines high.
#ifdef DELAY_I2C
#define STOP_CONDITION \
    delay_i2c(); \
    SDA_ZERO; \
    delay_i2c(); \
    SCL_ONE; \
    delay_i2c(); \
    SDA_ONE
#else
#define STOP_CONDITION \
    SDA_ZERO; \
    SCL_ONE; \
    SDA_ONE
#endif

// Put a bit out to the I2C bus.
#ifdef DELAY_I2C
#define PUT_BIT(_b_,_bit_) \
    if (0 == (_b_ & _bit_)) \
    { \
        SDA_ZERO; \
    } \
    else \
    { \
        SDA_ONE; \
    } \
    delay_i2c(); \
    SCL_ONE; \
    delay_i2c(); \
    SCL_ZERO
#else
#define PUT_BIT(_b_,_bit_) \
    if (0 == (_b_ & _bit_)) \
    { \
        SDA_ZERO; \
    } \
    else \
    { \
        SDA_ONE; \
    } \
    SCL_ONE; \
    SCL_ZERO
#endif

// Put the last bit of a byte out to an I2C bus.
// This is special because it sets-up the bus to
// receive an ACK bit from the slave.
// An ACK bit of 1 (undriven bus) is an error; 
// 0 (driven) is ok.
#ifdef DELAY_I2C
#define PUT_LAST_BIT(_b_,_bit_) \
    if (0 == (_b_ & _bit_)) \
    { \
        SDA_ZERO; \
    } \
    else \
    { \
        SDA_ONE; \
    } \
    delay_i2c(); \
    SCL_ONE; \
    delay_i2c(); \
    SCL_ZERO; \
    SDA_UNPOWERED
#else
#define PUT_LAST_BIT(_b_,_bit_) \
    if (0 == (_b_ & _bit_)) \
    { \
        SDA_ZERO; \
    } \
    else \
    { \
        SDA_ONE; \
    } \
    SCL_ONE; \
    SCL_ZERO; \
    SDA_UNPOWERED
#endif

// This reads a bit from the I2C bus.
#ifdef DELAY_I2C
#define GET_BIT(_b_,_bit_) \
    delay_i2c(); \
    SCL_ONE; \
    if (SDA_READ) \
    { \
        _b_ |= _bit_; \
    } \
    delay_i2c(); \
    SCL_ZERO
#else
#define GET_BIT(_b_,_bit_) \
    SCL_ONE; \
    if (SDA_READ) \
    { \
        _b_ |= _bit_; \
    } \
    SCL_ZERO
#endif

// This reads 8 bits from the I2C bus.
#define GET_BYTE(_b_) \
    GET_BIT(_b_,128); \
    GET_BIT(_b_,64); \
    GET_BIT(_b_,32); \
    GET_BIT(_b_,16); \
    GET_BIT(_b_,8); \
    GET_BIT(_b_,4); \
    GET_BIT(_b_,2); \
    GET_BIT(_b_,1)


/*****************************************************************************
* Description:  
*        Initialize I2C master interface.
* Parameters:
*   Input:
*         None. 
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
void i2c_master_init(void)
{
    // Disable the I2C hardware
    SSI->EEX = 0x00; 	// disable i2c master
    SSI->INT = 0; 		// disable interrupt

    SDA_UNPOWERED;
    SCL_UNPOWERED;

    i2c_master_start();
}

/*****************************************************************************
* Description:  
*        De-initialize I2C master interface, saves power, maybe.
* Parameters:
*   Input:
*         None.
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
void i2c_master_deinit(void)
{
    // Perform a stop if not already stopped.
    i2c_master_stop();
}

/*****************************************************************************
* Description:  
*        	Sends start command to the I2C slave.
* Parameters:
*   Input:
*					None.
*   Output: 
*         None.
* 	Return Status: 
*         None
*****************************************************************************/
int i2c_master_start(void)
{
    IRQ_DEFINES;

    IRQ_DISABLE();

    // send start command
    START_CONDITION;

    IRQ_ENABLE();

    return 0;
}

/*****************************************************************************
* Description:  
*        	Sends stop command to the I2C slave.
* Parameters:
*   Input:
*					None.
*   Output: 
*         None.
* 	Return Status: 
*         None
*****************************************************************************/
int i2c_master_stop(void)
{
    IRQ_DEFINES;

    IRQ_DISABLE();

    // Send stop command
    STOP_CONDITION;

    IRQ_ENABLE();

    return 0;
}

/*****************************************************************************
* Description:  
*        Sends command (or data) to the slave address on the I2C bus.
* Parameters:
*   Input:
*         tx_byte: Data (address, or command) to be sent 
*   Output: 
*         None.
* 	Return Status: 
*         None
*****************************************************************************/
int i2c_master_tx_byte(uint8_t tx_byte)
{
    int b, ack;
    IRQ_DEFINES;
	
    ack = 0;

    b = tx_byte;
    SDA_POWERED;  // power SDA only for output
    PUT_BIT(b,128);
    PUT_BIT(b,64);
    PUT_BIT(b,32);
    PUT_BIT(b,16);
    PUT_BIT(b,8);
    PUT_BIT(b,4);
    PUT_BIT(b,2);
    IRQ_DISABLE();
    PUT_LAST_BIT(b,1);  // SCL=0, SDA off on same clock edge.
    IRQ_ENABLE();
    GET_BIT(ack, 1); // get the ack bit

    return ack; // Nonzero is error.
}

/*****************************************************************************
* Description:  
*        Receive a byte from the slave.
* Parameters:
*   Input:
*      ack: If 'true' then : receive a byte from EEPROM and send ACK
*           If 'false' then : receive a byte from EEPROM and send NACK
*   Output: 
*      None.
* 	Return Status: 
*      The byte received from the slave.
*****************************************************************************/
uint8_t i2c_master_rx_byte(bool ack)
{
    int b;
    IRQ_DEFINES;
	
    SDA_UNPOWERED;  // insurance: can't write and read at the same time!
    b = 0;       // get_byte() ors bits in, so start with 0
    GET_BYTE(b); // get a byte from the bus, send ACK
    if(ack) {
        IRQ_DISABLE();
        PUT_LAST_BIT(0,0); // Send ack for bytes except last.
        IRQ_ENABLE();
    }

    return b; // no errors, so never returns negative.
}

/*****************************************************************************
* Description:  
*        Wait in while loop until I2C bus is busy.
*        With software-clocking, it's never busy.
* Parameters:
*   Input:
*         None. 
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
int i2c_busy_wait(void)
{
    return 0;
}

/*****************************************************************************
* Description:  
*        Wait in while loop until ACK is received from EEPROM slave
*        With software-clocking, it's received already.
 Parameters:
*   Input:
*         None. 
*   Output: 
*         None.
* Return Status: 
*         None
*****************************************************************************/
int i2c_ack_wait(void)
{
    return 0;
}

/***************************************************************************
 * Copyright (C) 2018 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
