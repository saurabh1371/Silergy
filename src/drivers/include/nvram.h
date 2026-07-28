/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION:POWER METER - Nonvolatile RAM (NVRAM) driver Code.
 * This code provide utility routines to access NVRAM.
 *
 * This code has protection from electromagnetic interference, which
 * is often able to corrupt the PC.
 * nvram-enable() should be called from a different execution path
 * than nvram-write(), so that no single corruption of the PC can 
 * cause a spurious write.
 * 
 * $Id: nvram.h 17420 2018-04-21 00:40:44Z rvandewa $
 ***************************************************************************/
#ifndef NVRAM_H
#define NVRAM_H 1

extern const char fw_ver_nvram[];

// Enable and disable writes to NVRAM.
void nvram_enable(void);
void nvram_disable(void);
#define nvram_init() nvram_disable()

// Read NVRAM.  Copies from the NVRAM address on the right
// to the address and length on the left.
// The addresses are all byte addresses, and the len
// is a count of bytes.
// The src is an integer byte index in NVRAM, not a pointer.
void nvram_read(uint8_t *dest, int len, int src);

// Write NVRAM.  Copies from the address and length on the right
// to the NVRAM address on the left.
// The addresses are all byte addresses, and the len
// is a count of bytes.
// The dest. is an integer byte index in NVRAM, not a pointer.
// nvram_enable() must be called earlier, in a different execution path.
// nvram_disable() should be called after all writes are done.
void nvram_write(int dest, uint8_t *src, int len);


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif // undefined NVRAM_H
