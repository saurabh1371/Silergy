/******************************************************************************* 
 * Copyright (C) 2020 Silergy Corp, All rights Reserved.
 * 
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of Maxim Integrated 
 * Products, Inc. shall not be used except as stated in the Maxim Integrated 
 * Products, Inc. Branding Policy.** The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all 
 * ownership rights.
 *
  * $Id: dlms_utils.c 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/ 

/********************************************************************************
*
*     Description: 
*        Filename: appl_utils.c
*
*        Compiler: uVision ARMCC.exe
*
*         Version: uVision 5.27.1
*
*         Created: 				
*							
*   Modifications: 
*         
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "dlms_variables.h"
#include "dlms_utils.h"
#include "rtc.h"
#define LITTLE_ENDAIN       // MAXQ/ROWLEY stores values in little endian format



#if BIG_ENDIAN
/* copy data from Tamper[] to variable.
   from: offset in Tamper[]
   src : destination variable
   len : variable len in bytes
*/
void copy_from_buf(uint8_t from,void *src, uint8_t len)
{
	uint8_t *src_ptr;
	switch(len)
	{
	 	case 2:
 			*((uint16_t *)src) = (uint16_t)((Tamper[from] << 8) | Tamper[from+1]);
			break;
		case 4:
			*(uint32_t *)src = ((uint32_t)Tamper[from] << 24) | 
							((uint32_t)Tamper[from+1] << 16) |
							((uint32_t)(Tamper[from+2]) << 8) | 
							((uint32_t)Tamper[from+3]);
 			
			break;
		case 12:
			src_ptr = (uint8_t *)src;
			do
			{
				*src_ptr++ = Tamper[from++];
			}while(--len);

			break;

	}

}
/* copy data to Tamper[] from variable.
   from: offset in Tamper[]
   src : source variable
   len : variable len in bytes
*/
void copy_to_buf(uint8_t from,void *src,uint8_t len)
{

	uint8_t *src_ptr;
	switch(len)
	{
	 	case 2:	// uint16
			Tamper[from] = (*(uint16_t *)src) >> 8;
			Tamper[from+1] = (*(uint16_t *)src) & 0xFF;
			break;

		case 4:	// uint32
			Tamper[from] = (*(uint32_t *)src) >> 24;
			Tamper[from+1] = (*(uint32_t *)src) >> 16;
			Tamper[from+2] = (*(uint32_t *)src) >> 8;
			Tamper[from+3] = (*(uint32_t *)src) & 0xFF;
			break;
		case 12:
			src_ptr = (uint8_t *)src;
			do
			{
				Tamper[from++] = *src_ptr++;
			}while(--len);

			break;
	}		
}

#else       // !BIG_ENDIAN

/* copy data from Tamper[] to variable.
   from: offset in Tamper[]
   src : destination variable
   len : variable len in bytes
*/
/*
void copy_from_buf(uint8_t from,void *src, uint8_t len)
{
	uint8_t *src_ptr;
	switch(len)
	{
	 	case 2:
 			*((uint16_t *)src) = (uint16_t)((Tamper[from+1] << 8) | Tamper[from]);
			break;
		case 4:
			*(uint32_t *)src = ((uint32_t)Tamper[from+3] << 24) | 
							((uint32_t)Tamper[from+2] << 16) |
							((uint32_t)(Tamper[from+1]) << 8) | 
							((uint32_t)Tamper[from]);
 			
			break;
		case 12:
			src_ptr = (uint8_t *)src;
			do
			{
				*src_ptr++ = Tamper[from++];
			}while(--len);

			break;

	}
}
*/

/* copy data to Tamper[] from variable.
   from: offset in Tamper[]
   src : source variable
   len : variable len in bytes
*/
/*
void copy_to_buf(uint8_t from,void *src,uint8_t len)
{

	uint8_t *src_ptr;
	switch(len)
	{
	 	case 2:	// uint16
                        Tamper[from+1] = (*(uint16_t *)src) >> 8;
			Tamper[from] = (*(uint16_t *)src) & 0xFF;
			break;

		case 4:	// uint32
			Tamper[from+3] = (*(uint32_t *)src) >> 24;
			Tamper[from+2] = (*(uint32_t *)src) >> 16;
			Tamper[from+1] = (*(uint32_t *)src) >> 8;
			Tamper[from] = (*(uint32_t *)src) & 0xFF;
			break;
		case 12:
			src_ptr = (uint8_t *)src;
			do
			{
				Tamper[from++] = *src_ptr++;
			}while(--len);

			break;
	}		
}
*/

#endif //   #if BIG_ENDIAN

/******************************************************************************
*   Function Name : maximum
*     Description : Find a maximum of 3 numbers
*        Input(s) : none
*       Output(s) : none
*           Notes : 
*
******************************************************************************/
uint32_t maximum(uint32_t a, uint32_t b, uint32_t c)
{
	return MAX(a,MAX(b,c));
}
unsigned char SlotGen( uint8_t slotperday, unsigned char Minutes)
{
    unsigned char Slot;

    switch(slotperday)
	{

		case 48:						// 48 slots of 1/2 hour .
	        if ( Minutes < 30 )
	        {
	            Slot = 2 * HOUR;
	        }
	        else
	        {
	            Slot = 2 * HOUR + 1;
	        }
			break;

		default:						// 96 slots of 1/4 hour 
	        if ( Minutes < 15 )
	        {
	            Slot = 4 * HOUR;
	        }
	        else if((Minutes >= 15) && (Minutes < 30))
	        {
	            Slot = 4 * HOUR + 1;
            }
	        else if((Minutes >= 30) &&(Minutes < 45))
	        {
			    Slot = 4 * HOUR + 2;
			}
            else
	        {
	            Slot = 4 * HOUR + 3;
	        }
			break;
    }
    
    return Slot;
}
/** \fn uint32_t DateGen(uint8_t yy, uint8_t mm, uint8_t dd);
 *  \brief calcuate unique value corresponding to date 
 *
 *  @param[in] yy	 	year (LSB only)
 *  @param[in] mm     	month
 *  @param[in] dd 		date
 *
 *	@return None
 */
uint32_t DateGen(uint8_t yy, uint8_t mm, uint8_t dd)
{
    uint32_t DateNumber = 0;
    uint32_t  i;
    uint32_t  j;

    if ( (yy % 4) )
    {
        MonthArray[2] = 28;
    }
    else
    {
        MonthArray[2] = 29;
    }
    j = mm;
    for ( i = 0; i < j; i = i + 1 )
    {
        DateNumber += MonthArray[i];
    }
    DateNumber += dd;
    DateNumber += yy * 365;
    for ( i = 1; i <= yy; i = i + 4 )
    {
        DateNumber += 1;
    }
    
    return DateNumber;
}
