/***************************************************************************
 * This code and information is provided "as is" without warranty of any   *
 * kind, either expressed or implied, including but not limited to the     *
 * implied warranties of merchantability and/or fitness for a particular   *
 * purpose.                                                                *
 *
 * DESCRIPTION: Main routine. 
 *
 * Copyright 2013, Silergy Corp.   All rights reserved.
 * AUTHOR: RGV
 * $Id: main.h 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/

#ifndef MAIN_H
#define MAIN_H 1

/*  Initialize the system */
void main_init(void);

// Reads the push button. Returns true if it is pressed.
int pb_read(void);

// Records a case-open tamper.
void main_record_tamper(void);

extern void reset_wd(void); // Reset of the watchdog is defined in main.c

// software watchdog bits to detect needed events in main.c
#define CE_WD   1

#endif // MAIN_H


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
 
