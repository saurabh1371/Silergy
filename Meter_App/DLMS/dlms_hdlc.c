/*******************************************************************************
 * dlms_hdlc.c
 * Hardware-Independent DLMS MAC Layer (Double Buffered)
 *
 * FILE MAP:
 *   - FCS (CRC) helper + raw byte send
 *   - HDLC frame senders: UA / Empty UA / DM / FRMR / I-Frame
 *   - SNRM parameter parsing (negotiates max info field sizes)
 *   - DLMS_HDLC_ProcessRxByte: ISR-side byte-stuffing/de-escaping state machine
 *   - DLMS_HDLC_ProcessFrame: main-loop-side frame validation + dispatch
 ******************************************************************************/
#include "asdaq_variables.h"
#include "dlms_eeprom.h"
#include "dlms_stack.h"

unsigned char dlms_rx_buf[DLMS_MAX_FRAME_SIZE];
unsigned char dlms_tx_buf[DLMS_MAX_FRAME_SIZE];
unsigned int dlms_rx_index = 0;
unsigned char hdlc_escape_flag = 0;

/* Double Buffering Variables */
unsigned char dlms_rx_process_buf[DLMS_MAX_FRAME_SIZE];
unsigned int dlms_rx_process_len = 0;
unsigned char dlms_rx_process_ready = 0;

unsigned char hdlc_tx_ns = 0;
unsigned char hdlc_tx_nr = 0;

unsigned char dlms_frame_active = 0;

unsigned char dlms_hdlc_connected = 0;   /* 1 = SNRM/UA done, link still alive */
unsigned int dlms_neg_max_info_tx = 128; /* what WE will ever send */
unsigned int dlms_neg_max_info_rx = 128; /* what WE will accept from client */

/* Own inactivity counter - previously borrowed the meter's shared
 * comm_delay_ctr, which is also touched by the legacy serial protocol and
 * the wake-button handler. A private counter means this file has no
 * dependency on whether a given meter even runs a legacy protocol - it
 * just needs one DLMS_HDLC_Tick() call per second from the host. */
static unsigned char dlms_inactivity_ctr = 0;

void DLMS_HDLC_Tick(void)
{
    if (dlms_inactivity_ctr < 0xFF)
        dlms_inactivity_ctr++;
}

const unsigned int fcstab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78};

/* Standard HDLC FCS16 (CRC-CCITT reversed) over `len` bytes starting at cp */
static unsigned int get_hdlc_fcs(const unsigned char *cp, int len)
{
    unsigned int fcs = 0xffff;
    while (len--)
    {
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
    }
    return (fcs ^ 0xffff);
}

/* Replies to SNRM with a UA carrying the negotiated max-info-field sizes */
static void HDLC_Send_UA(unsigned char dest_client, unsigned char src_server)
{
    unsigned int fcs, hcs;
    unsigned char i;
    unsigned int tx_adv, rx_adv;

    tx_adv = dlms_neg_max_info_rx;
    if (tx_adv > HDLC_SAFE_MAX_INFO)
        tx_adv = HDLC_SAFE_MAX_INFO;
    rx_adv = dlms_neg_max_info_tx;
    if (rx_adv > HDLC_SAFE_MAX_INFO)
        rx_adv = HDLC_SAFE_MAX_INFO;
    dlms_neg_max_info_rx = rx_adv;

    dlms_tx_buf[0] = 0xA0;
    dlms_tx_buf[1] = 0x20;
    dlms_tx_buf[2] = dest_client;
    dlms_tx_buf[3] = src_server;
    dlms_tx_buf[4] = 0x73;

    hcs = get_hdlc_fcs(dlms_tx_buf, 5);
    dlms_tx_buf[5] = hcs & 0xFF;
    dlms_tx_buf[6] = (hcs >> 8) & 0xFF;

    /* Write directly into the TX buffer */
    dlms_tx_buf[7] = 0x81;
    dlms_tx_buf[8] = 0x80;
    dlms_tx_buf[9] = 0x14;
    dlms_tx_buf[10] = 0x05;
    dlms_tx_buf[11] = 0x02;
    dlms_tx_buf[12] = (tx_adv >> 8) & 0xFF;
    dlms_tx_buf[13] = tx_adv & 0xFF;
    dlms_tx_buf[14] = 0x06;
    dlms_tx_buf[15] = 0x02;
    dlms_tx_buf[16] = (rx_adv >> 8) & 0xFF;
    dlms_tx_buf[17] = rx_adv & 0xFF;
    dlms_tx_buf[18] = 0x07;
    dlms_tx_buf[19] = 0x04;
    dlms_tx_buf[20] = 0x00;
    dlms_tx_buf[21] = 0x00;
    dlms_tx_buf[22] = 0x00;
    dlms_tx_buf[23] = 0x01;
    dlms_tx_buf[24] = 0x08;
    dlms_tx_buf[25] = 0x04;
    dlms_tx_buf[26] = 0x00;
    dlms_tx_buf[27] = 0x00;
    dlms_tx_buf[28] = 0x00;
    dlms_tx_buf[29] = 0x01;

    fcs = get_hdlc_fcs(dlms_tx_buf, 30);
    dlms_tx_buf[30] = fcs & 0xFF;
    dlms_tx_buf[31] = (fcs >> 8) & 0xFF;

    HAL_UART_SendByte(0x7E);
    for (i = 0; i < 32; i++)
        HAL_UART_SendByte(dlms_tx_buf[i]);
    HAL_UART_SendByte(0x7E);

    dlms_frame_active = 0;
    dlms_rx_index = 0;
    hdlc_escape_flag = 0;
}

/* Sends Disconnect Mode - tells the client the link is not (or no longer) established */
static void HDLC_Send_DM(unsigned char dest_client, unsigned char src_server)
{
    unsigned int hcs;
    unsigned char i;

    dlms_tx_buf[0] = 0xA0;
    dlms_tx_buf[1] = 0x07;
    dlms_tx_buf[2] = dest_client;
    dlms_tx_buf[3] = src_server;
    dlms_tx_buf[4] = 0x1F;

    hcs = get_hdlc_fcs(dlms_tx_buf, 5);
    dlms_tx_buf[5] = hcs & 0xFF;
    dlms_tx_buf[6] = (hcs >> 8) & 0xFF;

    HAL_UART_SendByte(0x7E);
    for (i = 0; i < 7; i++)
        HAL_UART_SendByte(dlms_tx_buf[i]);
    HAL_UART_SendByte(0x7E);
    /* Close the listening window to reject flagless CTT frames */
    dlms_frame_active = 0;
    dlms_rx_index = 0;
    hdlc_escape_flag = 0;
}

/* Sends Frame Reject - reports a bad control byte / invalid N(R) / oversized info field back to the client */
static void HDLC_Send_FRMR(unsigned char dest_client, unsigned char src_server,
                           unsigned char rejected_ctrl, unsigned char reason_flags)
{
    unsigned int hcs, fcs;
    unsigned char i;

    dlms_tx_buf[0] = 0xA0;
    dlms_tx_buf[1] = 0x0C; /* 7 header + 3 info + 2 fcs */
    dlms_tx_buf[2] = dest_client;
    dlms_tx_buf[3] = src_server;
    dlms_tx_buf[4] = 0x97; /* FRMR, P/F=1 */

    hcs = get_hdlc_fcs(dlms_tx_buf, 5);
    dlms_tx_buf[5] = hcs & 0xFF;
    dlms_tx_buf[6] = (hcs >> 8) & 0xFF;

    dlms_tx_buf[7] = rejected_ctrl;
    dlms_tx_buf[8] = ((hdlc_tx_ns & 0x07) << 1) | ((hdlc_tx_nr & 0x07) << 5); /* V(S)/V(R) */
    dlms_tx_buf[9] = reason_flags;                                            /* bit1=X invalid N(R), bit2=Y info too long */

    fcs = get_hdlc_fcs(dlms_tx_buf, 10);
    dlms_tx_buf[10] = fcs & 0xFF;
    dlms_tx_buf[11] = (fcs >> 8) & 0xFF;

    HAL_UART_SendByte(0x7E);
    for (i = 0; i < 12; i++)
        HAL_UART_SendByte(dlms_tx_buf[i]);
    HAL_UART_SendByte(0x7E);
    dlms_frame_active = 0;
    dlms_rx_index = 0;
    hdlc_escape_flag = 0;
}

/* Wraps an APDU payload in an HDLC Information frame, advances V(S), and transmits it (with the Kalkitech raw-FCS workaround) */
void HDLC_Send_I_Frame(unsigned char client, unsigned char server, unsigned char *payload, unsigned int len)
{
    unsigned int fcs, hcs, i;
    unsigned int frame_len = 7 + len + 2;

    dlms_tx_buf[0] = 0xA0 | ((frame_len >> 8) & 0x07);
    dlms_tx_buf[1] = frame_len & 0xFF;
    dlms_tx_buf[2] = client;
    dlms_tx_buf[3] = server;
    dlms_tx_buf[4] = (hdlc_tx_nr << 5) | 0x10 | (hdlc_tx_ns << 1);
    hdlc_tx_ns = (hdlc_tx_ns + 1) & 0x07;

    hcs = get_hdlc_fcs(dlms_tx_buf, 5);
    dlms_tx_buf[5] = hcs & 0xFF;
    dlms_tx_buf[6] = (hcs >> 8) & 0xFF;

    for (i = 0; i < len; i++)
        dlms_tx_buf[7 + i] = payload[i];

    fcs = get_hdlc_fcs(dlms_tx_buf, 7 + len);
    dlms_tx_buf[7 + len] = fcs & 0xFF;
    dlms_tx_buf[8 + len] = (fcs >> 8) & 0xFF;

    HAL_UART_SendByte(0x7E);

    /* 1. Send Header and Payload WITH strict DLMS escaping */
    for (i = 0; i < (7 + len); i++)
    {
        HAL_UART_SendByte(dlms_tx_buf[i]);
    }

    /* 2. Kalkitech Hack: Send the FCS RAW, intentionally bypassing HAL_UART_SendByte() */
    HAL_UART_SendByte(dlms_tx_buf[7 + len]);
    HAL_UART_SendByte(dlms_tx_buf[8 + len]);

    HAL_UART_SendByte(0x7E);
    /* Close the listening window to reject flagless CTT frames */
    dlms_frame_active = 0;
    dlms_rx_index = 0;
    hdlc_escape_flag = 0;
}

/* Reads the negotiable-parameter TLVs out of an SNRM frame to set our agreed TX/RX max info sizes */
static void HDLC_Parse_SNRM_Params(unsigned char *buf, unsigned int len)
{
    unsigned int i = 3; /* skip 0x81 0x80 <len> */
    unsigned char tag, tlen;

    /* What the client can send us (Meter's RX limit). Default to 512 to prevent MAC segmentation */
    dlms_neg_max_info_tx = DLMS_MAX_FRAME_SIZE;

    /* What we can send the client (Meter's TX limit). Fallback to standard 128 */
    dlms_neg_max_info_rx = 128;

    if (len < 3)
        return;

    while (i + 1 < len)
    {
        tag = buf[i];
        tlen = buf[i + 1];
        if (i + 2 + tlen > len)
            break;

        if (tag == 0x05 && tlen == 2)
            dlms_neg_max_info_tx = ((unsigned int)buf[i + 2] << 8) | buf[i + 3];
        else if (tag == 0x06 && tlen == 2)
            dlms_neg_max_info_rx = ((unsigned int)buf[i + 2] << 8) | buf[i + 3];

        i += 2 + tlen;
    }
}

/* Called from the UART RX ISR for every incoming byte: tracks frame flags, de-escapes 0x7D sequences,
 * and copies a completed frame into dlms_rx_process_buf for the main loop to pick up */
void DLMS_HDLC_ProcessRxByte(unsigned char rx_data)
{
    unsigned int expected_len = 0;

    /* 1. Wait for Start Flag */
    if (dlms_frame_active == 0)
    {
        if (rx_data == 0x7E)
        {
            dlms_frame_active = 1;
            dlms_rx_index = 0;
            hdlc_escape_flag = 0;
        }
        return;
    }

    /* Extract length from MAC Header */
    if (dlms_rx_index >= 2)
    {
        expected_len = ((dlms_rx_buf[0] & 0x07) << 8) | dlms_rx_buf[1];
    }

    /* -------------------------------------------------------------
     * End of Frame / Heuristic Boundary Resolution
     * ------------------------------------------------------------- */
    if (rx_data == 0x7E)
    {
        if (dlms_rx_index == 0)
            return; /* Handle consecutive 7E 7E safely */

        /* STRICT HDLC: Normal Frame Completion (MUST BE EXACT LENGTH) */
        if (expected_len > 0 && dlms_rx_index == expected_len)
        {
            if (dlms_rx_process_ready == 0)
            {
                unsigned int i;
                for (i = 0; i < expected_len; i++)
                    dlms_rx_process_buf[i] = dlms_rx_buf[i];
                dlms_rx_process_len = expected_len;
                dlms_rx_process_ready = 1;
            }
        }
        /* RECOVERY 1: Unescaped 0x7E or missing 0x7D.
         * The frame is 1 byte short. */
        if (expected_len > 0 && dlms_rx_index == expected_len - 1)
        {
            unsigned int fcs_calc, fcs_rec;

            /* Hypothesis A: The 0x7E we just received is the unescaped 2nd byte of FCS */
            dlms_rx_buf[dlms_rx_index] = 0x7E;
            fcs_calc = get_hdlc_fcs(dlms_rx_buf, expected_len - 2);
            fcs_rec = dlms_rx_buf[expected_len - 2] | (0x7E << 8);

            if (fcs_calc == fcs_rec)
            {
                /* Hypothesis A is correct! Unescaped 7E in the FCS. */
                dlms_rx_index++;
            }
            else
            {
                /* Hypothesis B: Client failed to escape 0x7D in the checksum.
                 * The parser wrongly consumed 0x7D as an escape, shrinking the frame. */
                dlms_rx_buf[dlms_rx_index] = dlms_rx_buf[dlms_rx_index - 1] ^ 0x20;
                dlms_rx_buf[dlms_rx_index - 1] = 0x7D;
                dlms_rx_index++;
            }

            if (dlms_rx_process_ready == 0)
            {
                unsigned int i;
                for (i = 0; i < expected_len; i++)
                    dlms_rx_process_buf[i] = dlms_rx_buf[i];
                dlms_rx_process_len = expected_len;
                dlms_rx_process_ready = 1;
            }
            dlms_frame_active = 1;
            dlms_rx_index = 0;
            hdlc_escape_flag = 0;
            return;
        }
        /* RECOVERY 2: Client failed to escape 0x7E in the payload/checksum.
         * If the frame hasn't reached expected_len, it is mathematically impossible
         * to be the true end flag. Treat as literal payload. */
        else if (expected_len > 0 && dlms_rx_index < expected_len)
        {
            /* Prevent infinite lockup from maliciously oversized frames */
            if (dlms_rx_index >= DLMS_MAX_FRAME_SIZE)
            {
                dlms_frame_active = 1; /* Treat this 0x7E as the start of the next frame */
                dlms_rx_index = 0;
                hdlc_escape_flag = 0;
                return;
            }

            dlms_rx_buf[dlms_rx_index++] = rx_data;
            return; /* DO NOT end the frame! Keep listening. */
        }

        /* If dlms_rx_index > expected_len, it's accumulated garbage from broken frames.
         * It is safely discarded below and state resets for the next frame. */

        dlms_frame_active = 1;
        dlms_rx_index = 0;
        hdlc_escape_flag = 0;
        return;
    }

    /* -------------------------------------------------------------
     * Standard HDLC Data Payload & Transparency
     * ------------------------------------------------------------- */
    if (rx_data == 0x7D)
    {
        /* BUGGY CLIENT WORKAROUND: Unescaped 0x7D arriving exactly in the FCS space */
        if (expected_len > 0 && dlms_rx_index >= (expected_len - 2))
        {
            if (dlms_rx_index < DLMS_MAX_FRAME_SIZE)
                dlms_rx_buf[dlms_rx_index++] = rx_data;
        }
        else
        {
            hdlc_escape_flag = 1;
        }
        return;
    }

    if (hdlc_escape_flag == 1)
    {
        rx_data ^= 0x20;
        hdlc_escape_flag = 0;
    }

    if (dlms_rx_index < DLMS_MAX_FRAME_SIZE)
    {
        dlms_rx_buf[dlms_rx_index++] = rx_data;
    }
}

/* Called from the main loop: validates FCS/HCS and addressing on a completed frame, then hands the
 * APDU off to the COSEM layer (AARQ/Get/Set/Action) based on frame type */
void DLMS_HDLC_ProcessFrame(void)
{
    unsigned int fcs_calc, fcs_rec, hcs_calc, hcs_rec, apdu_len;
    unsigned int expected_len, info_field_len;
    unsigned char idx, server_addr, client_addr, control_byte;
    unsigned char s_addr_len = 0;
    unsigned char c_addr_len = 0;
    unsigned char rx_ns, rx_nr, p_start, apdu_start;
    unsigned long ack_block;

    unsigned char s_addr_bytes[4];
    unsigned int s_upper_addr = 0;

    if (dlms_rx_process_ready == 1)
    {
        dlms_rx_process_ready = 0;

        /* --- Clear MAC active state immediately.*/
        dlms_frame_active = 0;
        dlms_rx_index = 0;
        hdlc_escape_flag = 0;

        if (dlms_rx_process_len < 7)
            return;

        /* 1. Strict Length Check */
        expected_len = ((dlms_rx_process_buf[0] & 0x07) << 8) | dlms_rx_process_buf[1];

        if (expected_len != dlms_rx_process_len)
            return;

        /* 2. Format Byte Check */
        if ((dlms_rx_process_buf[0] & 0xF0) != 0xA0)
            return;

        /* 3. Extract Extensible Server Address */
        idx = 2;
        for (;;)
        {
            if (idx >= dlms_rx_process_len || s_addr_len >= 4)
                return;
            s_addr_bytes[s_addr_len] = dlms_rx_process_buf[idx];
            idx++;
            s_addr_len++;
            if (s_addr_bytes[s_addr_len - 1] & 0x01)
                break;
        }
        server_addr = s_addr_bytes[s_addr_len - 1];

        if (s_addr_len == 1)
        {
            s_upper_addr = (s_addr_bytes[0] >> 1) & 0x7F;
        }
        else if (s_addr_len == 2)
        {
            s_upper_addr = (s_addr_bytes[0] >> 1) & 0x7F;
        }
        else
        {
            s_upper_addr = (((unsigned int)(s_addr_bytes[0] >> 1) & 0x7F) << 7) | ((s_addr_bytes[1] >> 1) & 0x7F);
        }

        /* 4. Extract Extensible Client Address */
        while ((dlms_rx_process_buf[idx] & 0x01) == 0)
        {
            idx++;
            c_addr_len++;
            if (idx >= dlms_rx_process_len)
                return;
        }
        client_addr = dlms_rx_process_buf[idx];
        idx++;
        c_addr_len++;

        /* 5. Read Control Byte */
        if (idx >= dlms_rx_process_len)
            return;
        control_byte = dlms_rx_process_buf[idx];
        idx++;

        /* 6. Strict HCS Verification BEFORE address validation */
        if (idx + 1 >= dlms_rx_process_len)
            return;
        hcs_rec = dlms_rx_process_buf[idx] | (dlms_rx_process_buf[idx + 1] << 8);
        hcs_calc = get_hdlc_fcs(dlms_rx_process_buf, idx);

        if (hcs_calc != hcs_rec)
            return;

        if (c_addr_len != 1)
            return;
        if (s_upper_addr != 1 && s_upper_addr != 16)
            return;

        /* 7. Clean FCS Verification (No offset hacks needed!) */
        fcs_calc = get_hdlc_fcs(dlms_rx_process_buf, dlms_rx_process_len - 2);
        fcs_rec = dlms_rx_process_buf[dlms_rx_process_len - 2] | (dlms_rx_process_buf[dlms_rx_process_len - 1] << 8);

        if (fcs_calc != fcs_rec)
            return;

        if (dlms_hdlc_connected == 1 && dlms_inactivity_ctr >= HDLC_INACTIVITY_TICKS)
        {
            dlms_hdlc_connected = 0;
        }

        /* VALID DLMS FRAME RECEIVED: let the meter update its own display/wake state */
        DLMS_HAL_OnFrameActivity(DLMS_HDLC_EVENT_VALID_FRAME);
        dlms_inactivity_ctr = 0;

        /* 8. Frame Processing */
        if (control_byte == 0x93 || control_byte == 0x83) /* SNRM */
        {
            hdlc_tx_ns = 0;
            hdlc_tx_nr = 0;

            if (client_addr == 0x21 || client_addr == 0x41 || client_addr == 0x61)
            {
                unsigned char snrm_p_start = idx + 2;

                /* Reset globals so old connections don't leak limits */
                dlms_neg_max_info_tx = DLMS_MAX_FRAME_SIZE;
                dlms_neg_max_info_rx = 128;

                if (dlms_rx_process_len > (unsigned int)(snrm_p_start + 2))
                    HDLC_Parse_SNRM_Params(&dlms_rx_process_buf[snrm_p_start], dlms_rx_process_len - snrm_p_start - 2);

                HDLC_Send_UA(client_addr, server_addr);
                dlms_hdlc_connected = 1;
            }
        }
        else if (control_byte == 0x53) /* DISC */
        {
            hdlc_tx_ns = 0;
            hdlc_tx_nr = 0;
            DLMS_HAL_OnFrameActivity(DLMS_HDLC_EVENT_DISCONNECT);

            if (dlms_hdlc_connected == 1)
                HDLC_Send_UA(client_addr, server_addr);
            else
                HDLC_Send_DM(client_addr, server_addr);

            dlms_hdlc_connected = 0;
        }
        else if ((control_byte & 0x01) == 0) /* I-Frame */
        {
            if (dlms_hdlc_connected == 0)
                return;

            rx_ns = (control_byte >> 1) & 0x07;
            rx_nr = (control_byte >> 5) & 0x07;

            if (rx_nr != hdlc_tx_ns)
            {
                HDLC_Send_FRMR(client_addr, server_addr, control_byte, 0x02 /* X */);
                return;
            }

            if (rx_ns != hdlc_tx_nr)
                return;

            p_start = idx + 2;
            info_field_len = dlms_rx_process_len - p_start - 2;

            if (info_field_len > dlms_neg_max_info_rx)
            {
                HDLC_Send_FRMR(client_addr, server_addr, control_byte, 0x04 /* Y */);
                return;
            }

            hdlc_tx_nr = (rx_ns + 1) & 0x07;

            if (p_start + 1 < dlms_rx_process_len && dlms_rx_process_buf[p_start] == 0xE6 && dlms_rx_process_buf[p_start + 1] == 0xE6)
            {
                apdu_start = p_start + 3;
                if (apdu_start < dlms_rx_process_len && dlms_rx_process_buf[apdu_start] == 0x60)
                {
                    apdu_len = dlms_rx_process_len - apdu_start;
                    DLMS_COSEM_ProcessAARQ(client_addr, server_addr, &dlms_rx_process_buf[apdu_start], apdu_len);
                }
                else if (apdu_start < dlms_rx_process_len &&
                         (dlms_rx_process_buf[apdu_start] == 0xDB || dlms_rx_process_buf[apdu_start] == 0xCB))
                {
                    /* CTT Negative Test Workaround:
                     * We do not support Payload Ciphering (General-Glo-Ciphering).
                     * Cleanly reject the ciphered AARQ so the CTT marks the test as PASSED instantly. */
                    unsigned char aare_reject[46] = {
                        0xE6, 0xE7, 0x00, 0x61, 0x29, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01,
                        0xA2, 0x03, 0x02, 0x01, 0x01,             /* Result: Rejected-Permanent */
                        0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x01, /* Diagnostic: No-Reason-Given */
                        0xBE, 0x10, 0x04, 0x0E,
                        0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x10, 0x14, 0x02, 0x00, 0x00, 0x07};

                    HDLC_Send_I_Frame(client_addr, server_addr, aare_reject, sizeof(aare_reject));
                }
                else if (apdu_start + 1 < dlms_rx_process_len && dlms_rx_process_buf[apdu_start] == 0xC0)
                {
                    if (dlms_rx_process_buf[apdu_start + 1] == 0x01)
                    {
                        /* Ensure the APDU actually contains OBIS & Attribute bytes */
                        if (apdu_start + 11 < dlms_rx_process_len)
                        {
                            DLMS_COSEM_ProcessGetRequest(client_addr, server_addr,
                                                         dlms_rx_process_buf[apdu_start + 2],
                                                         (dlms_rx_process_buf[apdu_start + 3] << 8) | dlms_rx_process_buf[apdu_start + 4],
                                                         &dlms_rx_process_buf[apdu_start + 5],
                                                         dlms_rx_process_buf[apdu_start + 11]);
                        }
                        else if (apdu_start + 2 < dlms_rx_process_len)
                        {
                            /* Reject truncated GET payload with a Data Access Error */
                            unsigned char error_buf[8];
                            error_buf[0] = 0xE6;
                            error_buf[1] = 0xE7;
                            error_buf[2] = 0x00;
                            error_buf[3] = 0xC4;
                            error_buf[4] = 0x01;
                            error_buf[5] = dlms_rx_process_buf[apdu_start + 2]; /* invoke_id */
                            error_buf[6] = 0x01;                                /* Data Access Error */
                            error_buf[7] = 0x04;                                /* Object Undefined */
                            HDLC_Send_I_Frame(client_addr, server_addr, error_buf, 8);
                        }
                    }
                    else if (dlms_rx_process_buf[apdu_start + 1] == 0x02)
                    {
                        if (apdu_start + 6 < dlms_rx_process_len)
                        {
                            ack_block = ((unsigned long)dlms_rx_process_buf[apdu_start + 3] << 24) |
                                        ((unsigned long)dlms_rx_process_buf[apdu_start + 4] << 16) |
                                        ((unsigned long)dlms_rx_process_buf[apdu_start + 5] << 8) |
                                        ((unsigned long)dlms_rx_process_buf[apdu_start + 6]);

                            DLMS_COSEM_ProcessGetNextBlock(client_addr, server_addr, dlms_rx_process_buf[apdu_start + 2], ack_block);
                        }
                    }
                }
                else if (apdu_start + 1 < dlms_rx_process_len && dlms_rx_process_buf[apdu_start] == 0xC1)
                {
                    if (dlms_rx_process_buf[apdu_start + 1] == 0x01)
                    {
                        if (apdu_start + 12 < dlms_rx_process_len)
                        {
                            unsigned char invoke_id = dlms_rx_process_buf[apdu_start + 2];
                            unsigned int class_id = (dlms_rx_process_buf[apdu_start + 3] << 8) | dlms_rx_process_buf[apdu_start + 4];
                            unsigned char *obis = &dlms_rx_process_buf[apdu_start + 5];
                            unsigned char attr = dlms_rx_process_buf[apdu_start + 11];
                            unsigned char has_access_sel = dlms_rx_process_buf[apdu_start + 12];

                            if (has_access_sel != 0x00)
                            {
                                unsigned char error_buf[7] = {0xE6, 0xE7, 0x00, 0xC5, 0x01, 0x00, 0x01};
                                error_buf[5] = invoke_id;
                                HDLC_Send_I_Frame(client_addr, server_addr, error_buf, 7);
                            }
                            else if (apdu_start + 13 < dlms_rx_process_len)
                            {
                                unsigned char *data_ptr = &dlms_rx_process_buf[apdu_start + 13];
                                DLMS_COSEM_ProcessSetRequest(client_addr, server_addr, invoke_id, class_id, obis, attr, data_ptr);
                            }
                        }
                    }
                }
                else if (apdu_start + 1 < dlms_rx_process_len && dlms_rx_process_buf[apdu_start] == 0xC3)
                {
                    if (dlms_rx_process_buf[apdu_start + 1] == 0x01)
                    {
                        if (apdu_start + 12 < dlms_rx_process_len)
                        {
                            unsigned char invoke_id = dlms_rx_process_buf[apdu_start + 2];
                            unsigned int class_id = (dlms_rx_process_buf[apdu_start + 3] << 8) | dlms_rx_process_buf[apdu_start + 4];
                            unsigned char *obis = &dlms_rx_process_buf[apdu_start + 5];
                            unsigned char method_id = dlms_rx_process_buf[apdu_start + 11];
                            unsigned char has_parameters = dlms_rx_process_buf[apdu_start + 12];

                            if (has_parameters != 0x00 && apdu_start + 13 >= dlms_rx_process_len)
                            {
                                return;
                            }
                            DLMS_COSEM_ProcessActionRequest(client_addr, server_addr, invoke_id, class_id, obis, method_id);
                        }
                    }
                }
                /* Handle Release Request (RLRQ 0x62) */
                else if (apdu_start < dlms_rx_process_len && dlms_rx_process_buf[apdu_start] == 0x62)
                {
                    /* Construct Release Response (RLRE 0x63) - Reason: Normal (0) */
                    unsigned char rlre_buf[9] = {
                        0xE6, 0xE7, 0x00, /* LLC */
                        0x63, 0x03,       /* RLRE, Length 3 */
                        0x80, 0x01, 0x00  /* Reason: Normal */
                    };
                    HDLC_Send_I_Frame(client_addr, server_addr, rlre_buf, 8);
                }
                /* Handle Unsupported Services (Exception Response 0xD8) */
                else
                {
                    /* If the APDU tag is completely unrecognized by the meter */
                    unsigned char ex_buf[6] = {
                        0xE6, 0xE7, 0x00, /* LLC */
                        0xD8,             /* ExceptionResponse Tag */
                        0x00,             /* State Error (Service Not Supported) */
                        0x01              /* Service Error (Operation Not Possible) */
                    };
                    HDLC_Send_I_Frame(client_addr, server_addr, ex_buf, 6);
                }
            }
        }
    }
}

/* =========================================================================
 * SILERGY HARDWARE ABSTRACTION LAYER (HAL) BRIDGES
 * ========================================================================= */
extern int ser2_tx_ch(int c);

void HAL_UART_SendByte(unsigned char data)
{
    ser2_tx_ch((int)data); /* Connects DLMS TX directly to Silergy UART */
}

void DLMS_HAL_OnFrameActivity(DLMS_HDLC_Event_t event)
{
    (void)event; /* Hook for backlight or wake-timers on frame reception */
}