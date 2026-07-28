/***************************************************************************
 * DESCRIPTION: SoC POWER METER - Command Line Interface Routines.
 * This autoreports meter data on the CLI I/O devices.
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: cli_autoreport.h 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
#ifndef CLI_AUTOREPORT_H
#define CLI_AUTOREPORT_H 1

// The CLI's index of the I/O device used by autoreport.
extern int ar_cli_idx;

// Enable autoreporting on the indexed CLI device.  
// Parameter is the CLI index of the CLI I/O device to get the data. 
void cli_ar_enable(int cli_idx);

// Disable autoreporting for the indexed CLI I/O.
void cli_ar_disable(int cli_idx);

// Run autoreporting on the indexed CLI device.  
// Parameter is the CLI index of the CLI I/O device to get the data. 
void cli_ar_run(int cli_idx);

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. 
 * All Rights Reserved.
 * This program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp. 
 ***************************************************************************/
#endif

