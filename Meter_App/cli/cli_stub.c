/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: POWER METER SoC - Command Line Interface Routines.
 * Replace the command line interface with code that does nothing.
 *
 * Copyright 2013, Silergy Corp.   All rights reserved.
 *
 * $Id: cli_stub.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/

#include "options.h"
#include "cli.h"
#if CLI_STUB
/*** Public stubs declared within this module ***/

/* initialize the cli */
void cli_init(void)
{ 
	UART0->CNTL = 0;
	UART1->CNTL = 0;
	UART2->CNTL = 0;
}

/* put out the version, if needed */
void cli_put_version(void) { }

/* run the CLI, and return true if any typing occurred. */
bool cli_run(void) { return false; }

#endif //#if CLI_STUB
/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

