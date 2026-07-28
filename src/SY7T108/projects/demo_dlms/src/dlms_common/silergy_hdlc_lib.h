/*******************************************************************************
 * Copyright (C) 2013..2020 Silergy Corp, All rights Reserved.
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
 * $Id: silergy_hdlc_lib.h 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/

/********************************************************************************
*
*     Description: 
*        Filename: dlmsif.c
*
*          Author: PBH
*
*        Compiler: uVision ARMCC.exe
*
*         Version: uVision 5.27.1
*
*         Created: 				
*							
*   Modifications: 
*         
*         
*******************************************************************************/
/******************************************************************************
*         DO NOT MODIFY
*******************************************************************************/

/** \file silergy_hdlc_public.h
 *  \brief Silergy DLMS HDLC interface details
 *
 *
 */
#ifndef __SILERGY_HDLC_PUBLIC_H__
#define __SILERGY_HDLC_PUBLIC_H__

#include <stdint.h>
//#include "hdlc_config.h"

#define MSEC10_COUNT    (1<<0)
#define SEC1_COUNT      (1<<1)

#define HDLC_ERROR_NOERROR				0
#define ERROR_HDLC_NDM					-35

/** \struct HDLC_STRUCT
 *	@brief
 *
 */
typedef struct HDLC_STRUCT{
	uint8_t baud_rate;	///<Baud rate
	uint8_t window_size_tx;		///< maximum number of frames the station can transmit before it recvs an ACK. supported 1
	uint8_t window_size_rx;		///< maximum number of frames the station can recv before sending an ACK. supported 1
	uint16_t max_info_txlen;	///< maximum transmit length of info block
	uint16_t max_info_rxlen;	///< maximum recv length of info block
	uint16_t inter_octet_to;	///< inter octet timeout in 'milliseconds'
	uint16_t inactivity_to;		///< inactivity timeout in 'milliseconds'
	uint16_t dev_addr;			///< physical address
        uint32_t SystemCoreClock;        // system core clock

}hdlc_struct_t;

extern hdlc_struct_t hdlc_param;

/** \fn int8_t hdlc_init(hdlc_struct_t* p_hdlc);
 *  \brief Initialise HDLC
 *
 *  @param[in] p_hdlc		HDLC parameters
 *
 *	@return HDLC_ERROR_NOERROR for success else error
 */
int8_t hdlc_init(hdlc_struct_t* p_hdlc,void(*func)(uint8_t));
/** \fn int8_t hdlc_open(uint8_t* handle, uint16_t logical_addr);
 *  \brief Set logical address for HDLC
 *
 *  @param[in] handle			HDLC handle
 *  @param[in] logical_addr		logical address. Set it to 0x0001.
 *
 *	@return HDLC_ERROR_NOERROR for success else error
 */
int8_t hdlc_open(uint8_t* handle, uint16_t logical_addr);
/** \fn int8_t hdlc_transmit(uint8_t handle, uint8_t segment, uint8_t *info_block, uint16_t* length);
 *  \brief Send information block
 *
 *  @param[in] handle				HDLC handle
 *  @param[in] segment				Reserved. Do not use
 *  @param[in,out] info_block		Information block. . User to allocate memory for buffer
 *  @param[in,out] length			length of information block
 *
 *	@return HDLC_ERROR_NOERROR for success else error
 */
int8_t hdlc_transmit(uint8_t handle, uint8_t segment, uint8_t *info_block, uint16_t* length);
/** \fn int8_t hdlc_receive(uint8_t handle, uint8_t* segment, uint8_t *info_block, uint16_t* length);
 *  \brief Get information block
 *
 *  @param[in] handle				HDLC handle
 *  @param[in] segment				Reserved.  Do not use
 *  @param[in,out] info_block		Information block. User to allocate memory for buffer
 *  @param[in,out] length			length of information block
 *
 *	@return HDLC_ERROR_NOERROR for success, ERROR_HDLC_NDM for HDLC in disconnect mode
 */
int8_t hdlc_receive(uint8_t handle, uint8_t *segment, uint8_t *info_block, uint16_t *length);

/** \fn uint8_t hdlc_getaddress(uint8_t handle, uint8_t* client,uint16_t* logical);
 *  \brief Get client and server addresses from HDLC.
 *
 *  @param[in] handle			HDLC handle
 *  @param[out] client			client address
 *  @param[out] logical			server logical address
 *
 *	@return 0 for success non-zero for error
 */
int8_t hdlc_getaddress(uint8_t handle, uint8_t* client,uint16_t* logical);

/** \fn void hdlc_interframe_timeout()
 *  \brief process HDLC interframe timeout actions\n
 *         Called to reset HDLC status after interframe time
 *
 *	@return None
 */
void hdlc_interframe_timeout(void);
/** \fn void hdlc_inactivity_timeout()
 *  \brief process HDLC inactivity timeout actions\n
 *         Called to reset HDLC status after inactivity time
 *
 *	@return None
 */

void hdlc_inactivity_timeout(void);
/** \fn void hdlc_serial0_isr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial0_isr(void);
/** \fn void hdlc_serial1_isr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial1_isr(void);
/** \fn void hdlc_serial2_isr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial2_isr(void);
/** \fn void hdlc_serial3_isr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial3_isr(void);
/** \fn void hdlc_tmr_reset(void)
 *  \brief resets timer3 counters
 *
 *	@return None
 */
void hdlc_tmr_reset(uint8_t);

int hdlc_setparam(hdlc_struct_t* p_hdlc);

/** \fn void hdlc_serial0_txisr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial0_txisr(void);

/** \fn void hdlc_serial0_rxisr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial0_rxisr(uint8_t c);

/** \fn void hdlc_serial0_txisr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial1_txisr(void);

/** \fn void hdlc_serial1_rxisr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial1_rxisr(uint8_t c);

/** \fn void hdlc_serial1_txisr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial2_txisr(void);

/** \fn void hdlc_serial2_rxisr(void)
 *  \brief Call from user serial ISR routine
 *
 *	@return None
 */
void hdlc_serial2_rxisr(uint8_t c);
#endif // SILERGY_HDLC_PUBLIC_H__

