/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Timer0 header.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: timer0.h 15728 2017-10-30 23:36:53Z rvandewa $
 ****************************************************************************/

#ifndef TIMER0_H     
#define TIMER0_H

extern const char fw_ver_timer0[];

// Starts auto-reload timer and registers a event handler 
void timer0_start(uint32_t time_msec, void(*func)(void));

// Stops auto-reload timer
void timer0_stop(void);

#endif //TIMER0_H

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
