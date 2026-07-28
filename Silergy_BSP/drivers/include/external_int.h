/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: Serial function declarations
 * $Id: external_int.h 24517 2020-04-22 16:38:20Z rvandewa $  
 ***************************************************************************/

#ifndef EXT_INT_H     
#define EXT_INT_H

#define PORT_INT_CNT 32

extern const char fw_ver_external_int[];

// Configure a dio as an external interrupt
extern void external_int_init(int idx, bool edge, bool polarity, void (*fn)(void));
// Deinitialize a dio as an external interrupt
extern void external_int_deinit(int idx);

#endif //EXT_INT_H


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
