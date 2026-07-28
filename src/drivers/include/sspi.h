/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: Slave SPI header.
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: sspi.h 16212 2017-12-08 00:31:28Z ptanwar $
 ****************************************************************************/
#ifndef SSPI_H
#define SSPI_H

// Version string.
extern const char fw_ver_sspi[];

typedef enum {PASSTHRU, SAFEMODE} smode;   

// Initialize SPI master interface
void sspi_init(smode sm, void(*func)(void));

// De-Initialize SPI master interface
void sspi_deinit(void);
	
#endif // SSPI_H

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
