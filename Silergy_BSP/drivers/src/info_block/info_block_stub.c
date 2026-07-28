/***************************************************************************
 * DESCRIPTION: POWER METER - Access Flash InfoBlock (Trim).
 *
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * $Id: info_block_stub.c 17489 2018-04-27 19:40:09Z rvandewa $ 
 ***************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "info_block.h"

// Firmware Device Driver version
const char fw_ver_info_block[] =
"info_block_stub" /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

/* Read from Flash Info Block */
int info_block_read(int index)
{ 
	switch(index)
	{
		case CHIPID_INDEX: return 0xDEADC0DE;
		default: return 0;
	}
}

/* Refresh trim ISR */
void refresh_trim_isr(void) { }

/***************************************************************************
 * 2005 August 17; First Version. 
 * Copyright (C) 2005 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

