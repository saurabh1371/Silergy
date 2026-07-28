/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 *
 * DESCRIPTION:POWER METER - Nonvolatile RAM (NVRAM) driver Code.
 * This code provides utility routines to access NVRAM.
 *
 * This code protects against corruption from EMI.
 * nvram-enable() should be called before nram-write(), but on a separate
 * execution path.
 * nvram-disable() should be called after the writes are done.
 *
 * The NVRAM is aligned on 32-bit words.  Byte-alignment is more
 * convenient, so the code simulates a byte-aligned RAM.
 * The code is somewhat slower, but reduces programming errors
 * from misalignment.
 * 
 * $Id: nvram.c 17447 2018-04-25 20:28:26Z ptanwar $
 ***************************************************************************/
#include <stdint.h>     // Standard integer definitions.
#include <stdbool.h>
#include "SY7_device.h" // System definitions, device, pins, etc.
#include "nvram.h"

const char fw_ver_nvram[] =
"nvram"         /* Device Driver name */
" v"      		/* ver string */
"1.00"          /* Major_ver.Minor_ver */
"\r\n"          /* CR + LF */
;

volatile uint32_t *nvram_addr_ptr;   // Address of the NVRAM address register.
volatile uint32_t *nvram_data_ptr;   // Address of the NVRAM data register.
uint32_t nvram_sink;                 // Place to absorb spurious writes.


/*****************************************************************************
* Description:
*       Enable writes to NVRAM.
*       Permit the write to access the NVRAM registers.
*
*       This code disables protection from electromagnetic interference, which
*       is often able to corrupt the PC.
*       nvram-enable() should be called from a different execution path
*       than nvram-write(), so that no single corruption of the PC can 
*       cause a spurious write.
* Parameters:
* Input:
* Output:
* Return Status:
*       none
*****************************************************************************/
void nvram_enable(void)
{
    nvram_addr_ptr = &NVRAM->ADDR;    // Set the address of NVRAM_ADDR
    nvram_data_ptr = &NVRAM->DATA;    // Set the address of NVRAM_DATA
}


/*****************************************************************************
* Description:
*       Disable writes to NVRAM. This is reentrant.
*       The idea is that the write will not access the NVRAM registers.
* Parameters:
* Input:
* Output:
* Return Status:
*       none
*****************************************************************************/
void nvram_disable(void)
{
    nvram_data_ptr = &nvram_sink;    // Clear the address of the NVRAM_DATA
    nvram_addr_ptr = &nvram_sink;    // Clear the address of the NVRAM_ADDR

    // Let accidental writes to NVRAM go to the last, hopefully unused cell.
    NVRAM->ADDR = NVRAM_LAST_CELL;    // Protect NVRAM from EMI.
}


/*****************************************************************************
* Description:
*       Read NVRAM. Copies from the address on the right
*       to the address and length on the left.
* Parameters:
* Input:
*       src index of the first byte to read from the NVRAM.
* Output:
*       *dest Address of the first byte read from the NVRAM.
*       len the number of bytes to read.
* Return Status:
*       none
*****************************************************************************/
void nvram_read(uint8_t *dest, int len, int src)
{
    uint32_t a, i, v;
    union {                 // Union to access the bytes in a word.
        uint32_t l;
        struct {
            uint32_t b0:8;
            uint32_t b1:8;
            uint32_t b2:8;
            uint32_t b3:8;
        } b;
    } u;

    for (; len >= 0; --len) // For all the bytes.
    {
        a = src/4;          // Find the source byte's address in NVRAM.
        i = src%4;
        src++;              // Step to the next source byte.
        NVRAM->ADDR = a;    // Set the word address of NVRAM.
        u.l = NVRAM->DATA;  // Read the word.
        switch(i)           // Decode the byte address in the word.
        {
            case 0: v = u.b.b0; break;  // Get the indexed byte.
            case 1: v = u.b.b1; break;
            case 2: v = u.b.b2; break;
            case 3: v = u.b.b3; break;
            default: v = 0; break; // makes data-flow analyzers happy.
        }
        *dest++ = (uint8_t)v;   // Write the byte to regular RAM.
    }

    // Let accidental writes to NVRAM go to the last, hopefully unused cell.
    NVRAM->ADDR = NVRAM_LAST_CELL;    // Protect NVRAM from EMI.
} // nvram_read()


/*****************************************************************************
* Description:
*       Write NVRAM. Copies from the address and length on the right
*       to the NVRAM byte index on the left.
*
*       This code has protection from electromagnetic interference, which
*       is often able to corrupt the PC.
*       nvram-enable() should be called from a different execution path
*       than nvram-write(), so that no single corruption of the PC can 
*       cause a spurious write.
*       nvram_disable() should be called after all writes are done.
* Parameters:
* Input:
*       *src Address of the first byte to write to the NVRAM.
*       len the number of bytes to write.
* Output:
*       dest index of the first byte to write in the NVRAM.
* Return Status:
*       none
*****************************************************************************/
void nvram_write(int dest, uint8_t *src, int len)
{
    uint32_t a, i, v;
    union {                 // Union to access the bytes in a word.
        uint32_t l;
        struct {
            uint32_t b0:8;
            uint32_t b1:8;
            uint32_t b2:8;
            uint32_t b3:8;
        } b;
    } u;

    // If not enabled, or wrong length, exit.
    if(&NVRAM->ADDR != nvram_addr_ptr || 0 >= len || NVRAM_SIZE < len)
        return;

    for (; len >= 0; --len) // for all the bytes.
    {
        v = (uint32_t)*src++;   // get the next byte.
        a = dest/4;         // Find the byte address in the NVRAM.
        i = dest%4;

        // Exit if the write is wrapping-around.
        if(NVRAM_LAST_CELL < a)
        {
            NVRAM->ADDR = NVRAM_LAST_CELL;    // Protect NVRAM from EMI.
            return;
        }

        dest++;                 // Step to the next destination.
        *nvram_addr_ptr = a;   // Set the word address.
        u.l = *nvram_data_ptr; // Fetch the NVRAM data word.
        switch(i)               // Select the byte to change.
        {
            case 0: u.b.b0 = v; break;  // Write to the byte in the word.
            case 1: u.b.b1 = v; break;
            case 2: u.b.b2 = v; break;
            case 3: u.b.b3 = v; break;
        }
        *nvram_data_ptr = u.l;  // Write the changed word to NVRAM.
    }

    // Let accidental writes to NVRAM go to the last, hopefully unused cell.
    NVRAM->ADDR = NVRAM_LAST_CELL;    // Protect NVRAM from EMI.
} // nvram_write()


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * This program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

