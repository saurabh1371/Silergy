/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Dual Timer header.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: dtimer.h 17148 2018-03-29 15:41:30Z ptanwar $
 ****************************************************************************/
#ifndef DTIMER_H
#define DTIMER_H

extern const char fw_ver_dtimer0[];
extern const char fw_ver_dtimer1[];

typedef enum {FREERUN, PERIODIC, ONESHOT} tmode;   

// Event handlers for Dual timer 0 and 1.
extern void (*pDtimerEventHandler0)(void);
extern void (*pDtimerEventHandler1)(void);

// Dualtimer0 start function.
void dtimer0_start(uint32_t ticks, tmode tm, void(*func)(void));

// Dualtimer0 stop function.
void dtimer0_stop(void); 

// Dualtimer1 start function.
void dtimer1_start(uint32_t ticks, tmode tm, void(*func)(void));

// Dualtimer1 stop function.
void dtimer1_stop(void); 

// Delay clock
void delay_clocks(uint32_t msec); 
#endif // DTIMER_H

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
