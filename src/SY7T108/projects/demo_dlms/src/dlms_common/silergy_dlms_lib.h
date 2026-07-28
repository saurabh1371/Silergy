/******************************************************************************* 
 * Copyright (C) 2020 Silergy Corp., All rights Reserved.
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
 * $Id: silergy_dlms_lib.h 25648 2020-06-10 11:00:29Z phuddar $
 ********************************************************************************/ 

/********************************************************************************
*
*     Description: 
*        Filename: silergy_dlms_lib.h
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
*         DO NOT MODIFY THIS FILE. 
*         See 'USER CONFIGURABLE VALUES' for user configurable parameters
*******************************************************************************/

#include <stdint.h>

/** \file silergy_dlms_lib.h
 *  \brief Silergy DLMS stack interface details
 *
 *  Details
 */

#ifndef __SILERGY_DLMS_LIB_H__
#define __SILERGY_DLMS_LIB_H__

#define SERVER_MAX_RECEIVE_PDU_SIZE 	128										///<Maximum Recieve PDU size
#define SERVER_MAX_TRANSMIT_PDU_SIZE	512										///<Maximum transmit PDU size
#define MINIMUM_DLMS_PROTOCOL_VERSION 	6										///<DLMS protocol version number
#define MINIMUM_CLIENT_MAX_PDU_SIZE 	12										///<Maximum client PDU size
#define DLMS_VERSION_NUMBER      	0x06									///<DLMS version number




/* HDLC setup defines */
#define DLMS_WINDOWS_TX_SIZE 		1											///<DLMS server maximum transmit window size
#define DLMS_WINDOWS_RX_SIZE 		1											///<DLMS server maximum recieve window size
#define DLMS_INFOFIELD_TX_SIZE		SERVER_MAX_TRANSMIT_PDU_SIZE				///<DLMS maximum tranmit PDU size in bytes
#define DLMS_INFOFIELD_RX_SIZE		SERVER_MAX_RECEIVE_PDU_SIZE					///<DLMS maximum recieve PDU size in bytes




#define MAX_SECRET_LLS_LEN	   8 												///<maximum secret size for HLS LLS layer	
#define MAX_SECRET_HLS_LEN	   16 												///<maximum secret size for HLS LLS layer



/***********************************
 * USER CONFIGURABLE VALUES
 ************************************/
#define DLMS_INTEROCTET_TIME		500 										///< inter_octet_timeout (in milliseconds)
#define DLMS_INACTIVITY_TIME		60											///<inactivity_timeout (in seconds)
#define DLMS_PHYDEV_ADDR		0x0100										///<dev_physical_addr


//extern char dlms_lls_password[];												///< password for LLS association
//extern char dlms_hls_password[];												///< password for HLS association

/** \fn int16_t dlms_server_init( void)
 *  \brief Initialize DLMS Server stack
 *
 *	@return DLMS_STATUS_SUCCESS
 */
int16_t dlms_server_init(void);

/** \fn void dlms_server_process_request( void)
 *  \brief Processes dlms request if there is any request pending
 *
 *	@return none
 */

void dlms_server_error(void);


void dlms_server_process_request(uint8_t handle,uint16_t frame_size,uint8_t *dlms_data_buffer);

/** \fn uint8_t dlms_obj_setval(uint8_t class,int16_t object_index,int8_t attribute_id,uint8_t choice,void *data_ptr,uint8_t size);
 *  \brief set the value for the object attribute
 *
 *  @param[in] class class identifier.
 *  @param[in] object_index 	object index in the list.
 *  @param[in] attribute_id		attribute index in the object.
 *  @param[in] choice			data type of value.
 *  @param[in,out] data_ptr		pointer to the value.
 *  @param[in] size				size of the value.
 *	@return 0 on success, else fail
 */
uint8_t dlms_obj_setval(uint8_t class,int16_t object_index,int8_t attribute_id,uint8_t choice,void *data_ptr,uint8_t size);

/** \fn void dlms_obj_getpg_value(uint16_t obj_id,uint8_t param,uint16_t cur_array,uint16_t cur_obj,void* data_ptr);
 *  \brief Get the value of attribute in profile generic buffer	object list
 *
 *  @param[in] obj_id 			Object index within the list
 *  @param[in] param 			0=num of entries/rows in the buffer,
 *  @param[in] cur_array		Current array/row number
 *  @param[in] cur_obj			Current column/object number
 *  @param[in,out] data_ptr		pointer to the value.
 *
 *	@return None
 */
void dlms_obj_getpg_value(uint16_t obj_id,uint8_t param,uint16_t cur_array,uint16_t cur_obj,void* data_ptr);

/** \fn void dlms_get_access_byrange(uint16_t id,uint32_t *cur_array,uint32_t *num_array,uint32_t from_time, uint32_t totime);
 *  \brief Get first and total number of rows for access range descriptor of profile generic buffer
 *
 *  @param[in] id	 			Object index within the list
 *  @param[in,out] cur_array 	pointer to the current row/array
 *  @param[in,out] num_array	pointer to the total number of array
 *  @param[in] from_time		unique value of date. See DateGen()
 *  @param[in] totime			unique value of date. See DateGen()
 *
 *	@return None
 */
void dlms_get_access_byrange(uint16_t id,uint32_t *cur_array,uint32_t *num_array,uint32_t from_time, uint32_t totime);

/** \fn uint8_t dlms_get_access_byentries(uint16_t object_index);
 *  \brief return success if profile generic object supports access selection by entry.
 *
 *  @param[in] object_index	 	Object index within the profile generic object list
 *
 *	@return o on success, else fail
 */
uint8_t dlms_get_access_byentries(uint16_t object_index);
/** \fn void dlms_obj_get_acval(uint8_t choice,uint8_t type,uint8_t id,void* data_ptr);
 *  \brief Get values of active calendar object
 *
 *  @param[in] choice	 	Choice.	\n
 *                          0 = active calendar name\n
 *                          1 = active season profile name\n
 *                          2 = active season start\n
 *                          3/4 = week profile name
 *                          7 = script start time
 *  @param[in] type     	type\n
 *							0 = active calendar\n
 *							1 = passive calendar
 *  @param[in] id 		Address offset
 *  @param[in,out] data_ptr	pointer to tvalue
 *
 *	@return None
 */
void dlms_obj_get_acval(uint8_t choice,uint8_t type,uint8_t id,uint8_t *length, void* data_ptr);
/** \fn void dlms_obj_get_acval(void);
 *  \brief set values of active calendar object
 *

 *	@return None
 */
void dlms_obj_set_acval(void);
/** \fn uint32_t DateGen(uint8_t yy, uint8_t mm, uint8_t dd);
 *  \brief calcuate unique value corresponding to date 
 *
 *  @param[in] yy	 	year (LSB only)
 *  @param[in] mm     	month
 *  @param[in] dd 		date
 *
 *	@return unsigned 4 byte value
 */

uint32_t DateGen(uint8_t yy, uint8_t mm, uint8_t dd);

void dlmsif_get_password(void);

/** \fn void dlmsif_set_lls_password
 *  \brief update LLS password in application
 *
 *  @param[in] password - password
 *  @param[in] len     - len
 *
 *	@return 0 for success, else 1
 */
uint8_t  dlmsif_set_lls_password(uint8_t *password, uint8_t len);

/** \fn void dlmsif_set_hls_password
 *  \brief update HLS password in application
 *
 *  @param[in] password - password
 *  @param[in] len     - len
 *
 *	@return 0 for success, else 1
 */
void  dlmsif_set_hls_password(uint8_t *password, uint8_t len);

uint8_t  dlms_server_set_lls_password(uint8_t *password, uint8_t len);

uint8_t  dlms_server_set_hls_password(uint8_t *password, uint8_t len);

uint8_t dlms_obj_getpg_billing_entries_inuse(uint16_t obj_id,void* data_ptr);

uint8_t dlms_obj_is_billing(uint16_t obj_id);

#endif //__SILERGY_DLMS_LIB_H__

