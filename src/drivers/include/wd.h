/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Watchdog timer 
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: wd.h 15428 2017-09-28 22:05:38Z rvandewa $
 ****************************************************************************/
#ifndef WD_H
#define WD_H 1

// Firmware Device Driver version
extern const char fw_ver_wd[]; 

// Clear hardware's watchdog timer.
void wd_reset(void);

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
 
#endif /* wd.h */
