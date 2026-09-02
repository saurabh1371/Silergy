/*******************************************************************************
 * dlms_stack.h
 * Global definitions for the DLMS/COSEM Protocol Stack
 ******************************************************************************/
#ifndef DLMS_STACK_H
#define DLMS_STACK_H

#include "dlms_config.h"
#include "dlms_hal.h"

/* MAC Layer (Implemented in dlms_hdlc.c) */
extern unsigned char dlms_rx_process_ready;
extern unsigned char dlms_frame_active; /* Set when a DLMS frame (0x7E...0x7E) is in progress */

/* Set on DLMS_HDLC_EVENT_VALID_FRAME, cleared on DLMS_HDLC_EVENT_DISCONNECT
 * (see DLMS_HAL_OnFrameActivity() in dlms_hdlc.c). Stays 1 for the whole
 * span of an active DLMS session/download - meant for the meter's display
 * code to show a "dload" indicator instead of its normal scroll. */
extern volatile unsigned char Dlms_Comm_Active;

/* --- EXTERN DECLARATIONS FOR DYNAMIC BLOCK SIZING --- */
extern unsigned int dlms_neg_max_info_tx;
extern unsigned int dlms_neg_max_info_rx;

void DLMS_HDLC_ProcessRxByte(unsigned char rx_data);
void DLMS_HDLC_ProcessFrame(void);
void DLMS_HDLC_Tick(void); /* call once/sec (or your chosen period) from the meter's system timer */
void HDLC_Send_I_Frame(unsigned char client, unsigned char server, unsigned char *payload, unsigned int len);

/* Application Layer (Implemented in dlms_cosem.c) */
extern unsigned char dlms_apdu_buf[DLMS_MAX_FRAME_SIZE];
extern unsigned char dlms_get_client_sap; /* client SAP of the GET currently being served */

extern unsigned char tx_is_dynamic;
extern unsigned char tx_invoke_id;
extern unsigned char tx_req_client;
extern unsigned char tx_req_server;
extern unsigned long int tx_blk_num;

/* IS 15959 event-type lookup, shared between the Class 7 event log builder
 * (meter_dlms_profiles.c) and the Instantaneous Profile's event summary
 * (dlms_cosem.c) */
extern const unsigned char dlms_to_internal[6][3];

/* Class 7 (Profile Generic) block generators - implemented in
 * meter_dlms_profiles.c since they know this meter's own EEPROM/history
 * layout. dlms_cosem.c's ProcessGetRequest sets the relevant position
 * counters then calls the matching Generate_*_Block() to kick off (or
 * continue, from ProcessGetNextBlock) a dynamic block transfer. */
extern unsigned char tx_billing_curr_row, tx_billing_total_rows;
extern unsigned int tx_block_curr_row, tx_block_total_rows;
extern unsigned int tx_daily_curr_row, tx_daily_total_rows;
extern unsigned int tx_event_curr_row, tx_event_total_rows;
extern unsigned char tx_event_type;
void DLMS_Generate_Billing_Block(void);
void DLMS_Generate_Event_Block(void);
void DLMS_Generate_BlockLoad_Block(void);
void DLMS_Generate_Daily_Block(void);

/* Class 7 GET dispatch - implemented in meter_dlms_profiles.c (OBIS routing
 * is meter data-model specific), calling back into
 * DLMS_Build_Class7_Metadata() (below, generic, stays in dlms_cosem.c) for
 * the shared attr 3/4/5/6/7/8 metadata encoding every profile needs. */
typedef enum
{
    DLMS_CLASS7_RESULT_REJECT,      /* not found / unhandled attr - caller should reject */
    DLMS_CLASS7_RESULT_SEND,        /* *apdu_len_ptr is ready - caller sends it normally */
    DLMS_CLASS7_RESULT_ALREADY_SENT /* a block transfer already sent its own response */
} DLMS_Class7_Result_t;
DLMS_Class7_Result_t DLMS_Meter_ProcessClass7Get(unsigned char client, unsigned char server, unsigned char invoke, unsigned char *obis, unsigned char attr, unsigned int *apdu_len_ptr);
unsigned char DLMS_Build_Class7_Metadata(unsigned int *apdu_len_ptr, unsigned char attr, unsigned long capture_period, unsigned char num_elements, const unsigned char *capture_objs, unsigned int capture_objs_size, unsigned long entries_in_use);

void DLMS_COSEM_ProcessAARQ(unsigned char client, unsigned char server, unsigned char *apdu, unsigned int apdu_len);
void DLMS_COSEM_ProcessGetRequest(unsigned char client, unsigned char server, unsigned char invoke, unsigned int class_id, unsigned char *obis, unsigned char attr);
void DLMS_COSEM_ProcessGetNextBlock(unsigned char client, unsigned char server, unsigned char invoke, unsigned long ack_block_num);

void DLMS_COSEM_ProcessSetRequest(unsigned char client, unsigned char server, unsigned char invoke, unsigned int class_id, unsigned char *obis, unsigned char attr, unsigned char *data);
void DLMS_COSEM_ProcessActionRequest(unsigned char client, unsigned char server, unsigned char invoke, unsigned int class_id, unsigned char *obis, unsigned char method_id);
void DLMS_HDLC_ResetSession(void); /* clears MAC/session state - call before a fresh comm window */
#endif /* DLMS_STACK_H */
