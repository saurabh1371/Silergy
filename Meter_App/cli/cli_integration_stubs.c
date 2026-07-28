/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: POWER METER SoC - Command Line Interface Routines.
 * There are two stub sets; cli_stub.c replaces the CLI with a non-functioning
 * set of code that takes minimal time and space.
 *
 * These stubs are used to integrate code into a new cli.  When the cli
 * is first brought up on a new IC, this is filled with stubs.  As the
 * features are added, the stubs are removed.
 *
 * Copyright 2013, Silergy Corp.   All rights reserved.
 *
 * $Id: cli_integration_stubs.c 25648 2020-06-10 11:00:29Z phuddar $
 ****************************************************************************/
#include "options.h"
#include "cli.h"
#include "cli_buf.h"    // To use the CLI's I/O routines and buffering.

/*** Public stubs declared within this module ***/
// Replace these with real code as the integration progresses.
void stub (const char *str) { cli_put_str (str); cli_put_crlf(); }
void cmd_serial (void) { stub ("cmd_serial"); }

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

