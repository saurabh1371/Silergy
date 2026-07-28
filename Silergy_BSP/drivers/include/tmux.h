/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: TMMUX functions and enum declarations
 * $Id: tmux.h 15907 2017-11-15 01:22:26Z rvandewa $  
 ***************************************************************************/

#ifndef TMUX_H     
#define TMUX_H

extern const char fw_ver_tmux[];

// List of signals available for TMUX0 and TMUX1 pin are in 
// SY7_device.h for each particular SoC.

// Configure TMUX 0 for test signals
extern void tmux0_config(e_tmux0_signal signal);

// Configure TMUX 1 for test signals
extern void tmux1_config(e_tmux1_signal signal);

#endif //TMUX_H


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
