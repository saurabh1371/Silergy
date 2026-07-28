/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * This implements a looped delay. The delay unit is one cycle of the crystal.  
 * It adapts to clock rate changes.
 *
 * $Id: delay.h 17454 2018-04-25 22:14:22Z rvandewa $
 **************************************************************************/

// delay loop API.
//
#ifndef DELAY_H
#define DELAY_H 1

// Firmware Device Driver version
extern const char fw_ver_delay[]; 

/* Loop delay implementation */
void delay (int32_t cclk);

// 60 seconds is ~1e9 cycles 
#define DELAY_CONSTANT 16995 // Assumes maximum speed.
#define DELAY_MS(__y__) ((__y__ * DELAY_CONSTANT) / 10)
#define DELAY_US(__y__) (((__y__ * DELAY_CONSTANT) / 10000))


#endif // delay
/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

