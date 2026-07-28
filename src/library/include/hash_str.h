/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * DESCRIPTION: This code makes a byte signature from a string.
 * It is used to detect a change of firmware version.
 *
 * $Id: hash_str.h 16026 2017-11-23 02:49:23Z rvandewa $
 **************************************************************************/

// Firmware Device Driver version
extern const char fw_ver_hash_str[]; 

// Generate a value from a string.
uint8_t hash_code_str (const char *a);

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

