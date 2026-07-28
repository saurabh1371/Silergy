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
 * $Id: dlms_utils.h 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/

/********************************************************************************
*
*     Description:
*        Filename: appl_utils.h
*
*        Compiler: uVision ARMCC.exe
*
*        Version: uVision 5.27.1
*
*         Created:
*
*   Modifications:
*
*******************************************************************************/
#ifndef __DLMS_UTILS_H__
#define __DLMS_UTILS_H__

#include <stdint.h>

#define MAX(_v0_,_v1_) ((_v0_ > _v1_) ? _v0_ : _v1_)
#define min(_v0_,_v1_) ((_v0_ < _v1_) ? _v0_ : _v1_)

/* copy data from Tamper[] to variable.
   from: offset in Tamper[]
   src : destination variable
   len : variable len in bytes
*/
void copy_from_buf(uint8_t from,void *src, uint8_t len);
/* copy data to Tamper[] from variable.
   from: offset in Tamper[]
   src : source variable
   len : variable len in bytes
*/
void copy_to_buf(uint8_t from,void *src,uint8_t len);

uint32_t maximum(uint32_t a, uint32_t b, uint32_t c);

unsigned char SlotGen( uint8_t LS_SlotsPerDay,unsigned char Minutes);

uint32_t DateGen(uint8_t yy, uint8_t mm, uint8_t dd);

#endif //#ifndef __DLMS_UTILS_H__
