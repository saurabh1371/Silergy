/*******************************************************************************
 * meter_dlms_profiles.c
 * Meter-specific profile-generic (Class 7) block generators: billing,
 * event log, block-load, and daily-survey. These know this meter's own
 * EEPROM memory map and history layout, so - unlike the pure protocol
 * dispatch in dlms_cosem.c - they don't move to a different controller
 * unchanged; they're the part you port deliberately.
 *
 * Each DLMS_Generate_*_Block() is a self-contained, no-argument entry
 * point: dlms_cosem.c's ProcessGetRequest (to kick a transfer off) and
 * ProcessGetNextBlock (to continue one already in progress) both just
 * call the relevant one directly and return - same as before this file
 * existed, nothing about that call pattern changes.
 ******************************************************************************/
#include "asdaq_variables.h"
#include "dlms_eeprom.h"
#include "dlms_stack.h"
#include "dlms_api.h"

// --- SILERGY-TO-RENESAS TRANSLATION BRIDGE ---
#define vrms_reg3 inst_voltage
#define irms1_reg3 inst_phase_current
#define irms2_reg3 inst_neutral_current
#define freq_reg3 inst_freq
#define pf_reg3 inst_pf
#define tot_kw_reg3 inst_kw
#define kva_reg3 inst_kva
#define ontime reset_on_time
// #define tamper_cnt all_tamper_cnt

// Memory and Config mappings
// #define config_change_cnt scratch
#define config_event_pos scratch2
#define CONFIG_EVENT_SIZE 10
#define CONFIG_EVENT_LOC 0
#define DLMS_PFAIL_EVENT_SIZE 10
#define DLMS_PFAIL_EVENT_LOC 0
#define DLMS_PFAIL_EVENT_POS_LOC 0
#define tamper_pos event_pos
#define TAMPER_SIZE EVENT_SIZE
#define TAMPER_LOC VOLT_EVENT_LOC
#define INST_TAMPER_LOC VOLT_EVENT_LOC
#define HIST_BILL_TIME_LOC LAST_BILL_TIME_LOC

// TOD and Billing mappings
#define passive_num_of_zone num_of_zone
#define passive_t_zone t_zone
#define PASSIVE_NUM_ZONE_LOC NUM_ZONE_LOC
#define tod_activation_time activate_time
#define TOD_ACTIVATION_TIME_LOC ACTIVATE_TIME_LOC
#define spec_bill_active ep_clear_stat
#define spec_bill_hr single_billing_hr
#define spec_bill_mn single_billing_min
#define billing_hour billing_hr
#define billing_minute billing_min
#define spec_bill_yr single_billing_year
#define spec_bill_mo single_billing_month
#define spec_bill_dy single_billing_day
#define BILL_HOUR_LOC BILL_TIME_LOC
#define BILL_MINUTE_LOC (BILL_TIME_LOC + 1)
#define SPEC_BILL_LOC SCHEDULED_BILL_DAY_LOC
#define last_stored_tod_kwh_val last_hr_load_val[0]
#define last_stored_tod_kvah_val last_hr_load_val[1]

// Profile Generic mappings
#define LOAD_DATE_LOC DAILY_SURVEY_LOC
#define DAILY_SURVEY_KVAH_LOC DAILY_SURVEY_LOC
#define g_Class07_Blockload_EntriesInUse Load_Profile_Entries_In_Use
#define g_Class07_Blockload_MaxEntries Load_Profile_Entries

extern unsigned int dlms_pfail_event_pos;
extern void reset_ls_data(void);
void convt_utc(unsigned long tmp);
void set_rtc(int a, int b, int c, int d, int e, int f);
void store_log_dt(int a);
void log_config_change_event(int a);

#ifndef DLMS_STRUCTS_DEFINED
#define DLMS_STRUCTS_DEFINED
typedef struct
{
    struct
    {
        unsigned char year_high, year_low, month, day_of_month, hour, minute;
    } clock_value;
    unsigned int voltage_value, kWh_value, kVAh_value, current_value;
} class07_blockload_entry_t;
extern class07_blockload_entry_t g_Class07_BlockLoadBuffer;
#endif
// ---------------------------------------------

extern void get_lp_data(unsigned int index);

/* --- Per-profile block-transfer position state ---
 * Generic transfer bookkeeping (tx_is_dynamic, tx_blk_num, tx_invoke_id,
 * tx_req_client, tx_req_server) stays owned by dlms_cosem.c - see
 * dlms_stack.h - since it's shared with the Class 15 object-list transfer
 * too and isn't specific to any one profile. */
unsigned char tx_billing_curr_row = 0;
unsigned char tx_billing_total_rows = 0;
unsigned int tx_block_curr_row = 0;
unsigned int tx_block_total_rows = 0;
unsigned int tx_daily_curr_row = 0; /* Must be int to count up to 365 */
unsigned int tx_daily_total_rows = 0;
unsigned int tx_event_curr_row = 0;
unsigned int tx_event_total_rows = 0;
unsigned char tx_event_type = 0;

#define MAX_APDU_BLOCK_SIZE 480 /* Max bytes per block - mirrors dlms_cosem.c's copy */

void DLMS_Inject_EEPROM_Date(unsigned int *len_ptr, unsigned long check_val, unsigned long eeprom_date, unsigned int eeprom_time)
{
    if (check_val == 0 || eeprom_date == 0)
    {
        DLMS_Inject_Dummy_DateTime(len_ptr);
    }
    else
    {
        DLMS_Inject_DateTime(len_ptr,
                             2000 + (eeprom_date % 100),
                             (eeprom_date / 100) % 100,
                             (eeprom_date / 10000) % 100,
                             eeprom_time / 100,
                             eeprom_time % 100,
                             0);
    }
}

/* ==================================================================
 * BILLING PROFILE GENERATORS
 * ================================================================== */
/* Encodes one row of the billing (monthly) profile into the APDU buffer */
static void DLMS_Append_Billing_Row(unsigned int *len_ptr, int row)
{
    unsigned int apdu_len = *len_ptr;
    unsigned int loc_kwmd, loc_kvamd;
    unsigned long tmp_date, tmp_kwh, tmp_kvah, tmp_kwmd, tmp_kvamd;
    unsigned int tmp_pf, yr, tmp_md_tm, hist_idx;
    unsigned char mo, dy, hr, mn;
    int i;
    unsigned long tz_kwh;
    unsigned int hist_ontime;

    dlms_apdu_buf[apdu_len++] = 0x02; /* Structure */
    dlms_apdu_buf[apdu_len++] = 0x19; /* 25 Columns */

    if (row == 0)
    {
        /* Live / Current Month */
        DLMS_Inject_DateTime(&apdu_len, 2000 + d_yr, d_mnth, d_day, t_hr, t_min, t_sec);

        /* Inside DLMS_Append_Billing_Row (row == 0) */
        DLMS_Inject_Type05_Uint32(&apdu_len, load_val[0] * 10);

        for (i = 0; i < 8; i++)
        {
            tz_kwh = 0;
            if (i < TOD_SIZE)
            {
                /* 1. Read the saved baseline from EEPROM */
                tz_kwh = from_eeprom(TOD_KWMD_LOC + (mnth_pos * (TOD_SIZE * 9)) + (i * 9), 3);

                /* Only apply this to the currently active Time Zone! */
                if (i == t_prev_zone)
                {
                    if (load_val[0] > last_stored_tod_kwh_val)
                    {
                        tz_kwh = tz_kwh + (load_val[0] - last_stored_tod_kwh_val);
                    }
                }
            }
            DLMS_Inject_Type05_Uint32(&apdu_len, tz_kwh * 10);
        }

        /* Live Apparent Energy (kVAh) */
        DLMS_Inject_Type05_Uint32(&apdu_len, load_val[1] * 10);
        for (i = 0; i < 8; i++)
        {
            tz_kwh = 0;
            if (i < TOD_SIZE)
            {
                tz_kwh = from_eeprom(TOD_KVAMD_LOC + (mnth_pos * (TOD_SIZE * 9)) + (i * 9), 3);
                if (i == t_prev_zone)
                {
                    if (load_val[1] > last_stored_tod_kvah_val)
                        tz_kwh = tz_kwh + (load_val[1] - last_stored_tod_kvah_val);
                }
            }
            DLMS_Inject_Type05_Uint32(&apdu_len, tz_kwh * 10);
        }
        DLMS_Inject_Type05_Uint32(&apdu_len, kwmd_val);

        loc_kwmd = KWMD_LOC + (mnth_pos * 21);
        tmp_date = from_eeprom(loc_kwmd + 9, 3);
        tmp_md_tm = from_eeprom(loc_kwmd + 12, 2);

        DLMS_Inject_EEPROM_Date(&apdu_len, kwmd_val, tmp_date, tmp_md_tm);

        DLMS_Inject_Type05_Uint32(&apdu_len, kvamd_val);

        loc_kvamd = KVAMD_LOC + (mnth_pos * 10);
        tmp_date = from_eeprom(loc_kvamd + 5, 3);
        tmp_md_tm = from_eeprom(loc_kvamd + 8, 2);

        DLMS_Inject_EEPROM_Date(&apdu_len, kvamd_val, tmp_date, tmp_md_tm);

        /* Billing Power ON Duration (ontime * 6 converts it to minutes) */
        DLMS_Inject_Type05_Uint32(&apdu_len, ontime * 6);

        /* Average Power Factor */
        DLMS_Inject_Type12_Uint16(&apdu_len, avg_pf);
    }
    else
    {
        /* Historical Months */
        hist_idx = (mnth_pos + HISTORY_SIZE - row) % HISTORY_SIZE;
        loc_kwmd = KWMD_LOC + (hist_idx * 19);
        loc_kvamd = KVAMD_LOC + (hist_idx * 11);

        tmp_date = from_eeprom(loc_kwmd, 3);
        tmp_kwh = from_eeprom(loc_kwmd + 5, 4);
        tmp_kwmd = from_eeprom(loc_kwmd + 9, 2);
        tmp_pf = read_eeprom(loc_kwmd + 18);
        tmp_kvah = from_eeprom(loc_kvamd, 4);
        tmp_kvamd = from_eeprom(loc_kvamd + 4, 2);

        if (tmp_date == 0)
        {
            DLMS_Inject_Dummy_DateTime(&apdu_len);
        }
        else
        {
            yr = 2000 + (tmp_date % 100);
            mo = (tmp_date / 100) % 100;
            dy = (tmp_date / 10000) % 100;
            /* === READ EXACT HISTORICAL TIME === */
            hr = read_eeprom(HIST_BILL_TIME_LOC + (hist_idx * 2));
            mn = read_eeprom(HIST_BILL_TIME_LOC + (hist_idx * 2) + 1);
            if (hr > 23)
                hr = 0; /* Safety fallback for empty EEPROM */
            if (mn > 59)
                mn = 0;
            DLMS_Inject_DateTime(&apdu_len, yr, mo, dy, hr, mn, 0);
        }

        DLMS_Inject_Type05_Uint32(&apdu_len, tmp_kwh * 10);
        for (i = 0; i < 8; i++)
        {
            tz_kwh = 0;
            if (i < TOD_SIZE)
                tz_kwh = from_eeprom(TOD_KWMD_LOC + (hist_idx * (TOD_SIZE * 9)) + (i * 9), 3);
            DLMS_Inject_Type05_Uint32(&apdu_len, tz_kwh * 10);
        }

        /* Historical Apparent Energy (kVAh) */
        DLMS_Inject_Type05_Uint32(&apdu_len, tmp_kvah * 10);
        for (i = 0; i < 8; i++)
        {
            tz_kwh = 0;
            if (i < TOD_SIZE)
                tz_kwh = from_eeprom(TOD_KVAMD_LOC + (hist_idx * (TOD_SIZE * 9)) + (i * 9), 3);
            DLMS_Inject_Type05_Uint32(&apdu_len, tz_kwh * 10);
        }
        DLMS_Inject_Type05_Uint32(&apdu_len, tmp_kwmd);

        tmp_date = from_eeprom(loc_kwmd + 11, 3);
        tmp_md_tm = from_eeprom(loc_kwmd + 14, 2);

        DLMS_Inject_EEPROM_Date(&apdu_len, tmp_kwmd, tmp_date, tmp_md_tm);

        DLMS_Inject_Type05_Uint32(&apdu_len, tmp_kvamd);

        tmp_date = from_eeprom(loc_kvamd + 6, 3);
        tmp_md_tm = from_eeprom(loc_kvamd + 9, 2);

        DLMS_Inject_EEPROM_Date(&apdu_len, tmp_kvamd, tmp_date, tmp_md_tm);

        /* Billing Power ON Duration */
        hist_ontime = from_eeprom(loc_kwmd + 17, 2);
        DLMS_Inject_Type05_Uint32(&apdu_len, hist_ontime * 6);

        /* Average Power Factor */
        DLMS_Inject_Type12_Uint16(&apdu_len, tmp_pf);
    }
    *len_ptr = apdu_len;
}

/* Fills dlms_apdu_buf with one Get-Response block of billing rows and sends it, or arms tx_is_dynamic for the next GetNextBlock request */
void DLMS_Generate_Billing_Block(void)
{
    unsigned int apdu_len = 0;
    unsigned int payload_len;
    unsigned char rows_to_send = 0;
    unsigned char is_last_block = 0;
    unsigned int len_pos;
    unsigned int data_start;
    int r;

    dlms_apdu_buf[apdu_len++] = 0xE6;
    dlms_apdu_buf[apdu_len++] = 0xE7;
    dlms_apdu_buf[apdu_len++] = 0x00;

    dlms_apdu_buf[apdu_len++] = 0xC4;
    dlms_apdu_buf[apdu_len++] = 0x02; /* Get Response with DataBlock */
    dlms_apdu_buf[apdu_len++] = tx_invoke_id;

    /* DYNAMIC SIZING: ~137 bytes per row */
    {
        unsigned int max_dynamic = dlms_neg_max_info_rx - 20;
        unsigned int max_rows = max_dynamic / 137;

        if (max_rows > 2)
            max_rows = 2; /* RAM safety cap */
        if (max_rows == 0)
            max_rows = 1;

        if ((tx_billing_total_rows - tx_billing_curr_row) <= max_rows)
        {
            rows_to_send = tx_billing_total_rows - tx_billing_curr_row;
            is_last_block = 1;
        }
        else
        {
            rows_to_send = max_rows;
            is_last_block = 0;
        }
    }

    dlms_apdu_buf[apdu_len++] = is_last_block;

    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 24) & 0xFF;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 16) & 0xFF;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 8) & 0xFF;
    dlms_apdu_buf[apdu_len++] = tx_blk_num & 0xFF;

    dlms_apdu_buf[apdu_len++] = 0x00; /* Raw Data */

    len_pos = apdu_len;
    dlms_apdu_buf[apdu_len++] = 0x82; /* Expecting > 127 bytes */
    dlms_apdu_buf[apdu_len++] = 0x00; /* High Byte length placeholder */
    dlms_apdu_buf[apdu_len++] = 0x00; /* Low Byte length placeholder */
    data_start = apdu_len;

    if (tx_blk_num == 1)
    {
        dlms_apdu_buf[apdu_len++] = 0x01; /* Array */
        dlms_apdu_buf[apdu_len++] = tx_billing_total_rows;
    }

    /* Inject the dynamically generated rows */
    for (r = 0; r < rows_to_send; r++)
    {
        DLMS_Append_Billing_Row(&apdu_len, tx_billing_curr_row + r);
    }

    /* Back-fill the actual payload length */
    payload_len = apdu_len - data_start;
    dlms_apdu_buf[len_pos + 1] = (payload_len >> 8) & 0xFF;
    dlms_apdu_buf[len_pos + 2] = payload_len & 0xFF;

    tx_billing_curr_row += rows_to_send;
    tx_blk_num++;

    HDLC_Send_I_Frame(tx_req_client, tx_req_server, dlms_apdu_buf, apdu_len);
}

/* Maps internal tamper to the official IS15959 Event Code */
unsigned char get_is15959_event_code(unsigned char internal_type, unsigned char is_rest)
{
    if (internal_type == 0)
        return is_rest ? 202 : 201; /* Magnet (Others) */
    if (internal_type == 1)
        return is_rest ? 70 : 69; /* Earth (Current) */
    if (internal_type == 2)
        return is_rest ? 52 : 51; /* Reverse (Current) */
    if (internal_type == 3)
        return is_rest ? 208 : 207; /* N-Miss (Others) */
    if (internal_type == 4)
        return is_rest ? 204 : 203; /* N-Dist (Others) */
    if (internal_type == 5)
        return 251; /* Cover Open (Non-Rollover) - No restoration */
    return 0;
}

/* DLMS Profile ID -> internal tamper index mapping for event multiplexing */
/* Internal Tampers: 0=Magnet, 1=Earth, 2=Reverse, 3=N-Miss, 4=N-Dist */
const unsigned char dlms_to_internal[6][3] = {
    {0xFF, 0xFF, 0xFF}, /* 0: Voltage -> None implemented*/
    {1, 2, 0xFF},       /* 1: Current -> Earth (1), Reverse (2) */
    {0xFF, 0xFF, 0xFF}, /* 2: Power -> None */
    {0xFF, 0xFF, 0xFF}, /* 3: Transaction -> Handled separately (100) */
    {0, 3, 4},          /* 4: Others -> Magnet (0), N-Miss (3), N-Dist (4) */
    {0xFF, 0xFF, 0xFF}  /* 5: Non-Rollover -> Cover Open handled separately (251) */
};

/* ==================================================================
 * EVENT LOG GENERATORS
 * ================================================================== */
/* Encodes one row of the event log into the APDU buffer */
static void DLMS_Append_Event_Row(unsigned int *len_ptr, int row)
{
    unsigned int apdu_len = *len_ptr;
    unsigned int loc;
    unsigned int ev_code;
    unsigned long ev_vrms, ev_irms, ev_pf, ev_kwh;
    unsigned long tmp_date;
    unsigned int yr;
    unsigned char mo, dy, hr, mn;
    unsigned int hist_idx;
    unsigned char is_active, is_restoration;
    int eeprom_record_idx;

    unsigned int max_size;
    unsigned int curr_pos;
    unsigned int base_loc;
    unsigned int rec_size;
    unsigned char int_type = 0;
    int running_row;
    int rows_for_this_type;
    int j;
    int i;
    unsigned long tmp_time;
    unsigned int valid_count;

    /* 1. Handle Transaction AND Power Failures (2 Columns) */
    if (tx_event_type == 100 || tx_event_type == 200)
    {
        eeprom_record_idx = row;

        max_size = (tx_event_type == 100) ? CONFIG_EVENT_SIZE : DLMS_PFAIL_EVENT_SIZE;
        curr_pos = (tx_event_type == 100) ? config_event_pos : dlms_pfail_event_pos;
        base_loc = (tx_event_type == 100) ? CONFIG_EVENT_LOC : DLMS_PFAIL_EVENT_LOC;
        rec_size = (tx_event_type == 100) ? 26 : 6;

        if (curr_pos == 0)
            hist_idx = (max_size - 1) - eeprom_record_idx;
        else
        {
            if (eeprom_record_idx < curr_pos)
                hist_idx = curr_pos - 1 - eeprom_record_idx;
            else
                hist_idx = max_size - (eeprom_record_idx - curr_pos) - 1;
        }

        loc = base_loc + (hist_idx * rec_size);
        ev_code = (read_eeprom(loc) << 8) | read_eeprom(loc + 1);
        tmp_date = from_eeprom(loc + 2, 4);

        dlms_apdu_buf[apdu_len++] = 0x02; /* Structure */
        dlms_apdu_buf[apdu_len++] = 0x02; /* 2 Columns! */

        if (tmp_date == 0 || tmp_date == 0xFFFFFFFF)
        {
            DLMS_Inject_Dummy_DateTime(&apdu_len);
        }
        else
        {
            convt_utc(tmp_date);
            yr = 2000 + (scratch1 % 100);
            mo = (scratch1 / 100) % 100;
            dy = (scratch1 / 10000);
            hr = scratch / 100;
            mn = scratch % 100;
            DLMS_Inject_DateTime(&apdu_len, yr, mo, dy, hr, mn, 0);
        }

        DLMS_Inject_Type12_Uint16(&apdu_len, ev_code);
        *len_ptr = apdu_len;
        return;
    }
    /* 2. Handle Cover Open (Non-Rollover) Event */
    else if (tx_event_type == 251)
    {
        loc = CUOPEN_LOC;
        tmp_date = from_eeprom(loc + 1, 3);
        tmp_time = from_eeprom(loc + 4, 3);
        ev_code = 251;
        ev_vrms = 0;
        ev_irms = 0;
        ev_pf = 0;
        ev_kwh = 0;

        dlms_apdu_buf[apdu_len++] = 0x02; /* Structure */
        dlms_apdu_buf[apdu_len++] = 0x06; /* 6 Columns */

        if (tmp_date == 0 || tmp_date == 0xFFFFFF)
        {
            DLMS_Inject_Dummy_DateTime(&apdu_len);
        }
        else
        {
            yr = 2000 + (tmp_date % 100);
            mo = (tmp_date / 100) % 100;
            dy = (tmp_date / 10000) % 100;
            hr = tmp_time / 100;
            mn = tmp_time % 100;
            DLMS_Inject_DateTime(&apdu_len, yr, mo, dy, hr, mn, 0);
        }

        DLMS_Inject_Type12_Uint16(&apdu_len, ev_code);
        DLMS_Inject_Type05_Uint32(&apdu_len, ev_irms);
        DLMS_Inject_Type05_Uint32(&apdu_len, ev_vrms);
        DLMS_Inject_Type05_Uint32(&apdu_len, ev_pf);
        DLMS_Inject_Type05_Uint32(&apdu_len, ev_kwh * 10);
        *len_ptr = apdu_len;
        return;
    }
    /* 3. Handle Standard Tamper Events via Multiplexer Matrix */
    else
    {
        running_row = row;

        for (j = 0; j < 3; j++)
        {
            int_type = dlms_to_internal[tx_event_type][j];
            if (int_type == 0xFF)
                continue;

            valid_count = 0;
            for (i = 0; i < TAMPER_SIZE; i++)
            {
                loc = TAMPER_LOC + (int_type * 520) + (i * 26);
                if (read_eeprom(loc) != 0 && read_eeprom(loc) != 0xFF)
                    valid_count++;
            }
            is_active = (store_tamper_stat & (1 << int_type)) ? 1 : 0;
            rows_for_this_type = (valid_count * 2) + is_active;

            if (running_row < rows_for_this_type)
            {
                break;
            }
            running_row -= rows_for_this_type;
        }

        /* SAFETY FALLBACK*/
        if (int_type == 0xFF)
        {
            dlms_apdu_buf[apdu_len++] = 0x02; /* Structure */
            dlms_apdu_buf[apdu_len++] = 0x06; /* 6 Columns */
            DLMS_Inject_Dummy_DateTime(&apdu_len);
            DLMS_Inject_Type12_Uint16(&apdu_len, 0);
            DLMS_Inject_Type05_Uint32(&apdu_len, 0);
            DLMS_Inject_Type05_Uint32(&apdu_len, 0);
            DLMS_Inject_Type05_Uint32(&apdu_len, 0);
            DLMS_Inject_Type05_Uint32(&apdu_len, 0);
            *len_ptr = apdu_len;
            return;
        }

        if (is_active)
        {
            if (running_row == 0)
            {
                is_restoration = 0;
                eeprom_record_idx = -1;
            }
            else
            {
                is_restoration = ((running_row - 1) % 2 == 0) ? 1 : 0;
                eeprom_record_idx = (running_row - 1) / 2;
            }
        }
        else
        {
            is_restoration = (running_row % 2 == 0) ? 1 : 0;
            eeprom_record_idx = running_row / 2;
        }

        if (eeprom_record_idx == -1)
        {
            loc = INST_TAMPER_LOC + (int_type * 13);
            tmp_date = from_eeprom(loc + 1, 4);
            ev_vrms = from_eeprom(loc + 5, 2);
            ev_irms = from_eeprom(loc + 7, 2);
            ev_pf = read_eeprom(loc + 9);
            ev_kwh = from_eeprom(loc + 10, 3);
            ev_code = get_is15959_event_code(int_type, 0);
        }
        else
        {
            hist_idx = (tamper_pos[int_type] + TAMPER_SIZE - eeprom_record_idx) % TAMPER_SIZE;

            loc = TAMPER_LOC + (int_type * 520) + (hist_idx * 26);
            if (is_restoration)
            {
                tmp_date = from_eeprom(loc + 14, 4);
                ev_vrms = from_eeprom(loc + 18, 2);
                ev_irms = from_eeprom(loc + 20, 2);
                ev_pf = read_eeprom(loc + 22);
                ev_kwh = from_eeprom(loc + 23, 3);
                ev_code = get_is15959_event_code(int_type, 1);
            }
            else
            {
                tmp_date = from_eeprom(loc + 1, 4);
                ev_vrms = from_eeprom(loc + 5, 2);
                ev_irms = from_eeprom(loc + 7, 2);
                ev_pf = read_eeprom(loc + 9);
                ev_kwh = from_eeprom(loc + 10, 3);
                ev_code = get_is15959_event_code(int_type, 0);
            }
        }

        dlms_apdu_buf[apdu_len++] = 0x02; /* Structure */
        dlms_apdu_buf[apdu_len++] = 0x06; /* 6 Columns */

        if (tmp_date == 0 || tmp_date == 0xFFFFFFFF)
        {
            DLMS_Inject_Dummy_DateTime(&apdu_len);
        }
        else
        {
            convt_utc(tmp_date);
            yr = 2000 + (scratch1 % 100);
            mo = (scratch1 / 100) % 100;
            dy = (scratch1 / 10000);
            hr = scratch / 100;
            mn = scratch % 100;
            DLMS_Inject_DateTime(&apdu_len, yr, mo, dy, hr, mn, 0);
        }

        DLMS_Inject_Type12_Uint16(&apdu_len, ev_code);
        DLMS_Inject_Type05_Uint32(&apdu_len, ev_irms);
        DLMS_Inject_Type05_Uint32(&apdu_len, ev_vrms);
        DLMS_Inject_Type05_Uint32(&apdu_len, ev_pf);
        DLMS_Inject_Type05_Uint32(&apdu_len, ev_kwh * 10);

        *len_ptr = apdu_len;
    }
}

/* Fills dlms_apdu_buf with one Get-Response block of event-log rows and sends it, or arms tx_is_dynamic for the next GetNextBlock request */
void DLMS_Generate_Event_Block(void)
{
    unsigned int apdu_len = 0;
    unsigned int payload_len;
    unsigned int rows_to_send = 0;
    unsigned char is_last_block = 0;
    unsigned int len_pos, data_start;
    int r;

    dlms_apdu_buf[apdu_len++] = 0xE6;
    dlms_apdu_buf[apdu_len++] = 0xE7;
    dlms_apdu_buf[apdu_len++] = 0x00;

    dlms_apdu_buf[apdu_len++] = 0xC4;
    dlms_apdu_buf[apdu_len++] = 0x02; /* Get Response with DataBlock */
    dlms_apdu_buf[apdu_len++] = tx_invoke_id;

    /* DYNAMIC SIZING: 39 bytes per row */
    {
        unsigned int max_dynamic = dlms_neg_max_info_rx - 20;
        unsigned int max_rows = max_dynamic / 39;

        if (max_rows > 10)
            max_rows = 10; /* RAM safety cap */
        if (max_rows == 0)
            max_rows = 1;

        if ((tx_event_total_rows - tx_event_curr_row) <= max_rows)
        {
            rows_to_send = tx_event_total_rows - tx_event_curr_row;
            is_last_block = 1;
        }
        else
        {
            rows_to_send = max_rows;
            is_last_block = 0;
        }
    }

    dlms_apdu_buf[apdu_len++] = is_last_block;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 24) & 0xFF;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 16) & 0xFF;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 8) & 0xFF;
    dlms_apdu_buf[apdu_len++] = tx_blk_num & 0xFF;
    dlms_apdu_buf[apdu_len++] = 0x00; /* Raw Data */

    len_pos = apdu_len;
    dlms_apdu_buf[apdu_len++] = 0x82; /* Expecting > 127 bytes */
    dlms_apdu_buf[apdu_len++] = 0x00;
    dlms_apdu_buf[apdu_len++] = 0x00;
    data_start = apdu_len;

    if (tx_blk_num == 1)
    {
        dlms_apdu_buf[apdu_len++] = 0x01; /* Array */
        if (tx_event_total_rows > 127)
        {
            dlms_apdu_buf[apdu_len++] = 0x82;
            dlms_apdu_buf[apdu_len++] = (tx_event_total_rows >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = tx_event_total_rows & 0xFF;
        }
        else
        {
            dlms_apdu_buf[apdu_len++] = tx_event_total_rows & 0xFF;
        }
    }

    for (r = 0; r < rows_to_send; r++)
    {
        DLMS_Append_Event_Row(&apdu_len, tx_event_curr_row + r);
    }

    payload_len = apdu_len - data_start;
    dlms_apdu_buf[len_pos + 1] = (payload_len >> 8) & 0xFF;
    dlms_apdu_buf[len_pos + 2] = payload_len & 0xFF;

    tx_event_curr_row += rows_to_send;
    tx_blk_num++;

    HDLC_Send_I_Frame(tx_req_client, tx_req_server, dlms_apdu_buf, apdu_len);
}

/* ==================================================================
 * BLOCK LOAD SURVEY GENERATORS (1.0.99.1.0.255)
 * ================================================================== */
static void DLMS_Append_BlockLoad_Row(unsigned int *len_ptr, int row)
{
    unsigned int apdu_len = *len_ptr;
    unsigned int yr;

    dlms_apdu_buf[apdu_len++] = 0x02; /* Structure */
    dlms_apdu_buf[apdu_len++] = 0x05; /* 5 Columns */

    if (g_Class07_Blockload_EntriesInUse == 0)
    {
        DLMS_Inject_Dummy_DateTime(&apdu_len);
        DLMS_Inject_Type12_Uint16(&apdu_len, 0);
        DLMS_Inject_Type05_Uint32(&apdu_len, 0);
        DLMS_Inject_Type05_Uint32(&apdu_len, 0);
        DLMS_Inject_Type05_Uint32(&apdu_len, 0);
        *len_ptr = apdu_len;
        return;
    }

    /* 1. CALL YOUR REFERENCE FUNCTION TO LOAD THE BUFFER */
    get_lp_data((unsigned int)row);

    /* 2. INJECT THE CLOCK FROM STRUCT */
    if (g_Class07_BlockLoadBuffer.clock_value.year_high == 0xFF)
    {
        DLMS_Inject_Dummy_DateTime(&apdu_len);
    }
    else
    {
        yr = (g_Class07_BlockLoadBuffer.clock_value.year_high << 8) | g_Class07_BlockLoadBuffer.clock_value.year_low;
        DLMS_Inject_DateTime(&apdu_len, yr,
                             g_Class07_BlockLoadBuffer.clock_value.month,
                             g_Class07_BlockLoadBuffer.clock_value.day_of_month,
                             g_Class07_BlockLoadBuffer.clock_value.hour,
                             g_Class07_BlockLoadBuffer.clock_value.minute, 0);
    }

    /* 3. INJECT THE VALUES DIRECTLY FROM YOUR UNPACKED STRUCT */
    /* Vrms (Uint16) */
    DLMS_Inject_Type12_Uint16(&apdu_len, g_Class07_BlockLoadBuffer.voltage_value);
    /* Block Active Energy Wh (Uint32) */
    DLMS_Inject_Type05_Uint32(&apdu_len, (unsigned long)g_Class07_BlockLoadBuffer.kWh_value);
    /* Block Apparent Energy VAh (Uint32) */
    DLMS_Inject_Type05_Uint32(&apdu_len, (unsigned long)g_Class07_BlockLoadBuffer.kVAh_value);
    /* Current (Uint32) */
    DLMS_Inject_Type05_Uint32(&apdu_len, (unsigned long)g_Class07_BlockLoadBuffer.current_value);

    *len_ptr = apdu_len;
}

/* Fills dlms_apdu_buf with one Get-Response block of load-survey rows and sends it, or arms tx_is_dynamic for the next GetNextBlock request */
void DLMS_Generate_BlockLoad_Block(void)
{
    unsigned int apdu_len = 0;
    unsigned int payload_len;
    unsigned int rows_to_send = 0;
    unsigned char is_last_block = 0;
    unsigned int len_pos, data_start;
    int r;

    dlms_apdu_buf[apdu_len++] = 0xE6;
    dlms_apdu_buf[apdu_len++] = 0xE7;
    dlms_apdu_buf[apdu_len++] = 0x00;

    dlms_apdu_buf[apdu_len++] = 0xC4;
    dlms_apdu_buf[apdu_len++] = 0x02; /* Get Response with DataBlock */
    dlms_apdu_buf[apdu_len++] = tx_invoke_id;

    /* DYNAMIC SIZING: 34 bytes per row */
    {
        unsigned int max_dynamic = dlms_neg_max_info_rx - 20;
        unsigned int max_rows = max_dynamic / 34;

        if (max_rows > 10)
            max_rows = 10; /* RAM safety cap */
        if (max_rows == 0)
            max_rows = 1;

        if ((tx_block_total_rows - tx_block_curr_row) <= max_rows)
        {
            rows_to_send = tx_block_total_rows - tx_block_curr_row;
            is_last_block = 1;
        }
        else
        {
            rows_to_send = max_rows;
            is_last_block = 0;
        }

        /* CTT TOOL BUG WORKAROUND: Avoid 126-byte frame (0x7E MAC length)
         * 3 rows * 34 bytes = 102 bytes + 15 header = 117 APDU.
         * 7 + 117 + 2 = exactly 126 bytes. The CTT parser crashes on this. */
        if (tx_blk_num > 1 && rows_to_send == 3)
        {
            rows_to_send = 2; /* Shrink to 2 rows to avoid 0x7E flag */
            is_last_block = 0;
        }
    }

    dlms_apdu_buf[apdu_len++] = is_last_block;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 24) & 0xFF;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 16) & 0xFF;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 8) & 0xFF;
    dlms_apdu_buf[apdu_len++] = tx_blk_num & 0xFF;
    dlms_apdu_buf[apdu_len++] = 0x00; /* Raw Data */

    len_pos = apdu_len;
    dlms_apdu_buf[apdu_len++] = 0x82; /* Expecting > 127 bytes */
    dlms_apdu_buf[apdu_len++] = 0x00;
    dlms_apdu_buf[apdu_len++] = 0x00;
    data_start = apdu_len;

    if (tx_blk_num == 1)
    {
        dlms_apdu_buf[apdu_len++] = 0x01; /* Array */
        if (tx_block_total_rows > 127)
        {
            dlms_apdu_buf[apdu_len++] = 0x82;
            dlms_apdu_buf[apdu_len++] = (tx_block_total_rows >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = tx_block_total_rows & 0xFF;
        }
        else
        {
            dlms_apdu_buf[apdu_len++] = tx_block_total_rows & 0xFF;
        }
    }

    for (r = 0; r < rows_to_send; r++)
    {
        DLMS_Append_BlockLoad_Row(&apdu_len, tx_block_curr_row + r);
    }

    payload_len = apdu_len - data_start;
    dlms_apdu_buf[len_pos + 1] = (payload_len >> 8) & 0xFF;
    dlms_apdu_buf[len_pos + 2] = payload_len & 0xFF;

    tx_block_curr_row += rows_to_send;
    tx_blk_num++;

    HDLC_Send_I_Frame(tx_req_client, tx_req_server, dlms_apdu_buf, apdu_len);
}

/* ==================================================================
 * DAILY PROFILE GENERATORS
 * ================================================================== */
/* Encodes one row of the daily-billing profile into the APDU buffer */
static void DLMS_Append_Daily_Row(unsigned int *len_ptr, int row)
{
    unsigned int apdu_len = *len_ptr;
    unsigned int hist_idx;
    unsigned long tmp_date, tmp_kwh, tmp_kvah;
    unsigned int yr;
    unsigned char mo, dy;

    if (day_pos == 0)
    {
        hist_idx = 364 - row;
    }
    else
    {
        if (row < day_pos)
            hist_idx = day_pos - 1 - row;
        else
            hist_idx = 365 - (row - day_pos) - 1;
    }

    tmp_date = from_eeprom(LOAD_DATE_LOC + (hist_idx * 3), 3);
    tmp_kwh = from_eeprom(DAILY_SURVEY_LOC + (hist_idx * 3), 3);
    tmp_kvah = from_eeprom(DAILY_SURVEY_KVAH_LOC + (hist_idx * 3), 3);

    dlms_apdu_buf[apdu_len++] = 0x02; /* Structure */
    dlms_apdu_buf[apdu_len++] = 0x03; /* 3 Columns */

    /* 1. Clock */
    if (tmp_date == 0)
    {
        DLMS_Inject_Dummy_DateTime(&apdu_len);
    }
    else
    {
        yr = 2000 + (tmp_date % 100);
        mo = (tmp_date / 100) % 100;
        dy = (tmp_date / 10000) % 100;
        DLMS_Inject_DateTime(&apdu_len, yr, mo, dy, 0, 0, 0); /* Midnight */
    }

    /* 2 to 3. EEPROM Values */
    DLMS_Inject_Type05_Uint32(&apdu_len, tmp_kwh * 10); /* Act Energy */

    /* Inject the historical Apparent Energy instead of 0 */
    DLMS_Inject_Type05_Uint32(&apdu_len, tmp_kvah * 10); /* App Energy */

    *len_ptr = apdu_len;
}

/* Fills dlms_apdu_buf with one Get-Response block of daily-profile rows and sends it, or arms tx_is_dynamic for the next GetNextBlock request */
void DLMS_Generate_Daily_Block(void)
{
    unsigned int apdu_len = 0;
    unsigned int payload_len;
    unsigned int rows_to_send = 0;
    unsigned char is_last_block = 0;
    unsigned int len_pos, data_start;
    int r;

    dlms_apdu_buf[apdu_len++] = 0xE6;
    dlms_apdu_buf[apdu_len++] = 0xE7;
    dlms_apdu_buf[apdu_len++] = 0x00;

    dlms_apdu_buf[apdu_len++] = 0xC4;
    dlms_apdu_buf[apdu_len++] = 0x02; /* Get Response with DataBlock */
    dlms_apdu_buf[apdu_len++] = tx_invoke_id;

    /* DYNAMIC SIZING: 26 bytes per row */
    {
        unsigned int max_dynamic = dlms_neg_max_info_rx - 20;
        unsigned int max_rows = max_dynamic / 26;

        if (max_rows > 10)
            max_rows = 10; /* RAM safety cap */
        if (max_rows == 0)
            max_rows = 1;

        if ((tx_daily_total_rows - tx_daily_curr_row) <= max_rows)
        {
            rows_to_send = tx_daily_total_rows - tx_daily_curr_row;
            is_last_block = 1;
        }
        else
        {
            rows_to_send = max_rows;
            is_last_block = 0;
        }
    }

    dlms_apdu_buf[apdu_len++] = is_last_block;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 24) & 0xFF;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 16) & 0xFF;
    dlms_apdu_buf[apdu_len++] = (tx_blk_num >> 8) & 0xFF;
    dlms_apdu_buf[apdu_len++] = tx_blk_num & 0xFF;
    dlms_apdu_buf[apdu_len++] = 0x00; /* Raw Data */

    len_pos = apdu_len;
    dlms_apdu_buf[apdu_len++] = 0x82; /* Expecting > 127 bytes */
    dlms_apdu_buf[apdu_len++] = 0x00; /* High Byte placeholder */
    dlms_apdu_buf[apdu_len++] = 0x00; /* Low Byte placeholder */
    data_start = apdu_len;

    if (tx_blk_num == 1)
    {
        dlms_apdu_buf[apdu_len++] = 0x01; /* Array */
        if (tx_daily_total_rows > 127)
        {
            dlms_apdu_buf[apdu_len++] = 0x82;
            dlms_apdu_buf[apdu_len++] = (tx_daily_total_rows >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = tx_daily_total_rows & 0xFF;
        }
        else
        {
            dlms_apdu_buf[apdu_len++] = tx_daily_total_rows & 0xFF;
        }
    }

    for (r = 0; r < rows_to_send; r++)
    {
        DLMS_Append_Daily_Row(&apdu_len, tx_daily_curr_row + r);
    }

    payload_len = apdu_len - data_start;
    dlms_apdu_buf[len_pos + 1] = (payload_len >> 8) & 0xFF;
    dlms_apdu_buf[len_pos + 2] = payload_len & 0xFF;

    tx_daily_curr_row += rows_to_send;
    tx_blk_num++;

    HDLC_Send_I_Frame(tx_req_client, tx_req_server, dlms_apdu_buf, apdu_len);
}

/* =========================================================
 * CLASS 7 (PROFILE GENERIC) - OBIS DISPATCH
 * Capture-object metadata tables and the GET dispatcher that routes
 * by OBIS to the right profile, moved here from dlms_cosem.c since
 * they describe this meter's own object model.
 * ========================================================= */

/* --- Nameplate Profile Capture Objects (8 Columns) --- */
static const unsigned char nameplate_capture_objs[] = {
    /* 1. Clock (0.0.1.0.0.255, Class 8) */
    0x02, 0x04, 0x12, 0x00, 0x08, 0x09, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 2. Meter Serial Number (0.0.96.1.0.255, Class 1) */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x60, 0x01, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 3. Manufacturer Name (0.0.96.1.1.255, Class 1) */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x60, 0x01, 0x01, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 4. Firmware Version (1.0.0.2.0.255, Class 1) */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x01, 0x00, 0x00, 0x02, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 5. Meter Type (0.0.94.91.9.255, Class 1) */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x5E, 0x5B, 0x09, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 6. Meter Category (0.0.94.91.11.255, Class 1) */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x5E, 0x5B, 0x0B, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 7. Current Rating (0.0.94.91.12.255, Class 1) */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x5E, 0x5B, 0x0C, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 8. Year of Manufacture (0.0.96.1.4.255, Class 1) */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x60, 0x01, 0x04, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00};

/* --- Instantaneous Profile Capture Objects (18 Columns) --- */
static const unsigned char inst_capture_objs[] = {
    0x02, 0x04, 0x12, 0x00, 0x08, 0x09, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 1. Clock */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0C, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 2. Voltage */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0B, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 3. Current */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x5B, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 4. Neutral Current */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0D, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 5. Power Factor */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0E, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 6. Frequency */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 7. Apparent Power */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 8. Active Power */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 9. Active Energy */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 10. Apparent Energy */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x01, 0x06, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 11. Active MD */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x01, 0x06, 0x00, 0xFF, 0x0F, 0x05, 0x12, 0x00, 0x00, /* 12. Active MD Time */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x09, 0x06, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 13. Apparent MD */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x09, 0x06, 0x00, 0xFF, 0x0F, 0x05, 0x12, 0x00, 0x00, /* 14. App MD Time */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x00, 0x00, 0x5E, 0x5B, 0x0E, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 15. Tamper Status */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x5E, 0x5B, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 16. Meter Status */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 17. Bill Counter */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x60, 0x02, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00  /* 18. Power Fail Count */
};

/* --- Instantaneous Scalar Profile Capture Objects (12 Columns) --- */
static const unsigned char inst_scalar_capture_objs[] = {
    // 0x02, 0x04, 0x12, 0x00, 0x08, 0x09, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1. Clock */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0C, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 2. Voltage */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0B, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 3. Current */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x5B, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 4. Neutral Current */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0D, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 5. Power Factor */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0E, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 6. Frequency */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 7. Apparent Power */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 8. Active Power */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 9. Active Energy */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 10. Apparent Energy */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x01, 0x06, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 11. Act MD */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x09, 0x06, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 12. App MD */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x00, 0x00, 0x5E, 0x5B, 0x0E, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00};

/* --- Billing Profile Capture Objects (25 Columns) ---*/
static const unsigned char bill_capture_objs[] = {
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x00, 0x01, 0x02, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 1. Billing Timestamp (Class 1) */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 2. Act Energy Imp */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x01, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 3. Act Energy TZ1 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x02, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 4. Act Energy TZ2 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x03, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 5. Act Energy TZ3 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x04, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 6. Act Energy TZ4 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x05, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 7. Act Energy TZ5 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x06, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 8. Act Energy TZ6 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x07, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 9. Act Energy TZ7 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x08, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 10. Act Energy TZ8 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 11. App Energy Imp */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x01, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 12. App Energy TZ1 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x02, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 13. App Energy TZ2 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x03, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 14. App Energy TZ3 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x04, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 15. App Energy TZ4 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x05, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 16. App Energy TZ5 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x06, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 17. App Energy TZ6 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x07, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 18. App Energy TZ7 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x08, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 19. App Energy TZ8 */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x01, 0x06, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 20. Act MD */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x01, 0x06, 0x00, 0xFF, 0x0F, 0x05, 0x12, 0x00, 0x00, /* 21. Act MD Time */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x09, 0x06, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 22. App MD */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x09, 0x06, 0x00, 0xFF, 0x0F, 0x05, 0x12, 0x00, 0x00, /* 23. App MD Time */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x00, 0x00, 0x5E, 0x5B, 0x0D, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 24. Billing Power ON Duration */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0D, 0x00, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00  /* 25. Avg PF for Billing Period */
};

/* ================================================================= */
/* --- BILLING SCALAR ARRAYS (6 COLUMNS) --- */
static const unsigned char bill_scalar_capture_objs[] = {
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1.0.1.8.0.255 (Wh) */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1.0.9.8.0.255 (VAh) */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x01, 0x06, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1.0.1.6.0.255 (Act MD) */
    0x02, 0x04, 0x12, 0x00, 0x04, 0x09, 0x06, 0x01, 0x00, 0x09, 0x06, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1.0.9.6.0.255 (App MD) */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0D, 0x00, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1.0.13.0.0.255 (Avg PF) */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x00, 0x00, 0x5E, 0x5B, 0x7F, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00  /* 0.0.94.91.127.255 (Count) */
};

static const unsigned char bill_scalar_attr2_buf[] = {
    0x01, 0x01, 0x02, 0x06,             /* Array, 1 row, struct, 6 cols */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1E, /* Wh (0) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1F, /* VAh (0) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1B, /* Act MD (0, W) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1C, /* App MD (0, VA) */
    0x02, 0x02, 0x0F, 0xFE, 0x16, 0xFF, /* PF (-2, Unitless) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x06  /* Count (0, Months) */
};

/* --- DAILY LOAD SCALAR ARRAYS (2 COLUMNS) --- */
static const unsigned char daily_scalar_capture_objs[] = {
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1.0.1.8.0.255 (Wh) */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00  /* 1.0.9.8.0.255 (VAh) */
};

static const unsigned char daily_scalar_attr2_buf[] = {
    0x01, 0x01, 0x02, 0x02,             /* Array, 1 row, struct, 2 cols */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1E, /* Wh (0) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1F  /* VAh (0) */
};

/* --- Event Scalar Profile Capture Objects (4 Columns) --- */
static const unsigned char event_scalar_capture_objs[] = {
    /* 1. Active Current - WAS 1.0.94.91.14.255 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0B, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00,
    /* 2. Voltage */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0C, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00,
    /* 3. Power Factor */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0D, 0x07, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00,
    /* 4. Act Energy Wh */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00};

static const unsigned char block_scalar_capture_objs[] = {
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0C, 0x1B, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1.0.12.27.0.255 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x1D, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1.0.1.29.0.255 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x1D, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00, /* 1.0.9.29.0.255 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0B, 0x1B, 0x00, 0xFF, 0x0F, 0x03, 0x12, 0x00, 0x00  /* 1.0.11.27.0.255 */
};

/* --- Block Load Profile Capture Objects (5 Columns) --- */
static const unsigned char block_capture_objs[] = {
    0x02, 0x04, 0x12, 0x00, 0x08, 0x09, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 1. Clock */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0C, 0x1B, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 2. Vrms */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x1D, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 3. Act Power */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x1D, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 4. App Power */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0B, 0x1B, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00  /* 5. Current */
};

/* --- Daily Load Profile Capture Objects (3 Columns) --- */
static const unsigned char daily_capture_objs[] = {
    0x02, 0x04, 0x12, 0x00, 0x08, 0x09, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 1. Clock */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00, /* 2. Act Energy */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x09, 0x08, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00  /* 3. App Energy */
};

/* --- Event Profile Capture Objects (6 Columns) --- */
static const unsigned char event_capture_objs[] = {
    /* 1. Clock (Class 8) */
    0x02, 0x04, 0x12, 0x00, 0x08, 0x09, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 2. Event Code (Class 1) */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x60, 0x0B, 0x01, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 3. Active Current (Class 3) - WAS 0.0.94.91.14.255 */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0B, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 4. Voltage (Class 3) */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0C, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 5. Power Factor (Class 3) */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x0D, 0x07, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 6. Act Energy (Class 3) */
    0x02, 0x04, 0x12, 0x00, 0x03, 0x09, 0x06, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00};

/* --- Transaction Profile Capture Objects (2 Columns) --- */
static const unsigned char transaction_capture_objs[] = {
    /* 1. Clock (Class 8) */
    0x02, 0x04, 0x12, 0x00, 0x08, 0x09, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 2. Event Code (Class 1) - 0.0.96.11.3.255 */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x60, 0x0B, 0x03, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00};

/* --- POWER FAILURES (2 Columns) --- */
static const unsigned char pfail_capture_objs[] = {
    /* 1. Clock (Class 8) */
    0x02, 0x04, 0x12, 0x00, 0x08, 0x09, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00,
    /* 2. Event Code (Class 1) - 0.0.96.11.2.255 */
    0x02, 0x04, 0x12, 0x00, 0x01, 0x09, 0x06, 0x00, 0x00, 0x60, 0x0B, 0x02, 0xFF, 0x0F, 0x02, 0x12, 0x00, 0x00};

/* Pre-compiled Scalar Profile Buffers - ALIGNED WITH CLASS 3 */
static const unsigned char inst_scalar_attr2_buf[] = {
    0x01, 0x01, 0x02, 0x0C,             /* Array, 1 row, struct, 12 cols */
    0x02, 0x02, 0x0F, 0xFF, 0x16, 0x23, /* V (-1) */
    0x02, 0x02, 0x0F, 0xFD, 0x16, 0x21, /* A (-3) */
    0x02, 0x02, 0x0F, 0xFD, 0x16, 0x21, /* N-A (-3) */
    0x02, 0x02, 0x0F, 0xFD, 0x16, 0xFF, /* PF (-3, Unitless) */
    0x02, 0x02, 0x0F, 0xFF, 0x16, 0x2C, /* Hz (-1) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1C, /* VA (0) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1B, /* W (0) */
    0x02, 0x02, 0x0F, 0x01, 0x16, 0x1E, /* Wh (1) */
    0x02, 0x02, 0x0F, 0x01, 0x16, 0x1F, /* VAh (1) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1B, /* Act MD (0) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1C, /* App MD (0) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x06  /* Power ON Duration (0, Minutes) */
};

static const unsigned char block_scalar_attr2_buf[] = {
    0x01, 0x01, 0x02, 0x04,             /* Array, 1 row, struct, 4 cols */
    0x02, 0x02, 0x0F, 0xFF, 0x16, 0x23, /* V (-1) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1E, /* Wh (0) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1F, /* VAh (0) */
    0x02, 0x02, 0x0F, 0xFE, 0x16, 0x21  /* A (-2) */
};

static const unsigned char event_scalar_attr2_buf[] = {
    0x01, 0x01, 0x02, 0x04,             /* Array, 1 row, struct, 4 cols */
    0x02, 0x02, 0x0F, 0xFE, 0x16, 0x21, /* Active Current (-2, Amperes) */
    0x02, 0x02, 0x0F, 0xFF, 0x16, 0x23, /* V (-1) */
    0x02, 0x02, 0x0F, 0xFE, 0x16, 0xFF, /* PF (-2, Unitless) */
    0x02, 0x02, 0x0F, 0x00, 0x16, 0x1E  /* Wh (0) */
};
/* Handles all Class 7 (Profile Generic) GET requests: OBIS routing for
 * every profile (instantaneous, billing, block-load, daily-survey, event
 * log, nameplate), kicking off a dynamic block transfer for the ones
 * backed by history data. Returns which of three things ProcessGetRequest
 * should do next - reject, send *apdu_len_ptr normally, or nothing further
 * (a block transfer already sent its own first chunk). */
DLMS_Class7_Result_t DLMS_Meter_ProcessClass7Get(unsigned char client, unsigned char server, unsigned char invoke, unsigned char *obis, unsigned char attr, unsigned int *apdu_len_ptr)
{
    unsigned int apdu_len = *apdu_len_ptr;
    unsigned int i;
    unsigned int yr;
    unsigned long int register_val = 0;
    unsigned long int bill_cnt = 0;
    int j;
    unsigned long tmp_md_dt;
    unsigned int tmp_md_tm;
    unsigned char total_rows;
    const char *dev_id2 = DLMS_MANUFACTURER_STR;
    const char *fw_id = DLMS_FW_VERSION_STR;
    const char *curr_rating = DLMS_CURRENT_RATING_STR;

    /* Instantaneous Profile: 1.0.94.91.0.255 */
    if (obis[0] == 1 && obis[1] == 0 && obis[2] == 94 && obis[3] == 91 && obis[4] == 0)
    {
        if (attr == 2) /* Buffer (The actual values snapshot) */
        {
            dlms_apdu_buf[apdu_len++] = 0x01; /* Array */
            dlms_apdu_buf[apdu_len++] = 0x01; /* 1 Row */
            dlms_apdu_buf[apdu_len++] = 0x02; /* Structure */
            dlms_apdu_buf[apdu_len++] = 0x12; /* 18 Columns */

            /* 1. Clock */
            DLMS_Inject_DateTime(&apdu_len, 2000 + d_yr, d_mnth, d_day, t_hr, t_min, t_sec);

            /* 2 to 10. Live values (V, A, N-A, PF, Hz, VA, W, Wh, VAh) */
            // 2. Voltage (V)
            register_val = vrms_reg3;
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            // 3. Current (A)
            register_val = irms1_reg3;
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            // 4. Neutral Current (N-A)
            register_val = irms2_reg3;
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            // 5. Power Factor (PF)
            register_val = pf_reg3 * 10;
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            // 6. Frequency (Hz)
            register_val = freq_reg3;
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            // 7. Apparent Power (VA)
            register_val = kva_reg3;
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            // 8. Active Power (W)
            register_val = tot_kw_reg3;
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            // 9. Active Energy (Wh)
            register_val = load_val[0];
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            // 10. Apparent Energy (VAh)
            register_val = load_val[1];
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            /* 11 & 12. Act MD & Time */
            register_val = kwmd_val;
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            DLMS_Inject_EEPROM_Date(&apdu_len, register_val, kwmd_date, kwmd_time);

            /* 13 & 14. App MD & Time */
            register_val = kvamd_val;
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            DLMS_Inject_EEPROM_Date(&apdu_len, register_val, kvamd_date, kvamd_time);

            /* 15. Cumulative Power ON Duration (0.0.94.91.14.255) */
            register_val = Cum_Power_On_Dur; /* Total power on time in minutes */
            dlms_apdu_buf[apdu_len++] = 0x05;
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            /* 16. Cumulative Tamper Count (0.0.94.91.0.255) */
            register_val = all_tamper_cnt;
            dlms_apdu_buf[apdu_len++] = 0x06; /* Uint32 (Double Long Unsigned) */
            dlms_apdu_buf[apdu_len++] = (register_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (register_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = register_val & 0xFF;

            /* 17. Billing Counter (0.0.0.1.0.255) */
            bill_cnt = (unsigned long int)md_reset_cnt;
            dlms_apdu_buf[apdu_len++] = 0x06; /* Uint32 (Double Long Unsigned) */
            dlms_apdu_buf[apdu_len++] = (bill_cnt >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (bill_cnt >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (bill_cnt >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = bill_cnt & 0xFF;

            /* 18. Config Program Changes (0.0.96.2.0.255) */
            dlms_apdu_buf[apdu_len++] = 0x12; /* Uint16 */
            dlms_apdu_buf[apdu_len++] = (Cum_Prog_Count >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = Cum_Prog_Count & 0xFF;
        }
        else if (attr == 6) /* Sort Object (Clock) */
        {
            dlms_apdu_buf[apdu_len++] = 0x02;
            dlms_apdu_buf[apdu_len++] = 0x04;
            dlms_apdu_buf[apdu_len++] = 0x12;
            dlms_apdu_buf[apdu_len++] = 0x00;
            dlms_apdu_buf[apdu_len++] = 0x08;
            dlms_apdu_buf[apdu_len++] = 0x09;
            dlms_apdu_buf[apdu_len++] = 0x06;
            dlms_apdu_buf[apdu_len++] = 0x00;
            dlms_apdu_buf[apdu_len++] = 0x00;
            dlms_apdu_buf[apdu_len++] = 0x01;
            dlms_apdu_buf[apdu_len++] = 0x00;
            dlms_apdu_buf[apdu_len++] = 0x00;
            dlms_apdu_buf[apdu_len++] = 0xFF;
            dlms_apdu_buf[apdu_len++] = 0x0F;
            dlms_apdu_buf[apdu_len++] = 0x02;
            dlms_apdu_buf[apdu_len++] = 0x12;
            dlms_apdu_buf[apdu_len++] = 0x00;
            dlms_apdu_buf[apdu_len++] = 0x00;
        }
        else
        {
            /* Uses the universal helper for Attr 1, 3, 4, 5, 7, 8 */
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x12, inst_capture_objs, sizeof(inst_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }

    /* Instantaneous Scalar Profile: 1.0.94.91.3.255 */
    else if (obis[0] == 1 && obis[1] == 0 && obis[2] == 94 && obis[3] == 91 && obis[4] == 3)
    {
        if (attr == 2) /* Buffer */
        {
            for (i = 0; i < sizeof(inst_scalar_attr2_buf); i++)
            {
                dlms_apdu_buf[apdu_len++] = inst_scalar_attr2_buf[i];
            }
        }
        else
        {
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x0C, inst_scalar_capture_objs, sizeof(inst_scalar_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }
    /* Billing Profile: 1.0.98.1.0.255 */
    else if (obis[0] == 1 && obis[1] == 0 && obis[2] == 98 && obis[3] == 1 && obis[4] == 0)
    {
        if (attr == 2) /* Buffer (Historical Data) */
        {
            /* Calculate up to 13 months based on billing resets */
            tx_billing_total_rows = (md_reset_cnt >= HISTORY_SIZE) ? HISTORY_SIZE : (md_reset_cnt + 1);

            /* Initialize Dynamic Engine */
            tx_billing_curr_row = 0;
            tx_is_dynamic = 1;
            tx_blk_num = 1;
            tx_invoke_id = invoke;
            tx_req_client = client;
            tx_req_server = server;

            /* Kick off the first chunk */
            DLMS_Generate_Billing_Block();
            *apdu_len_ptr = apdu_len;
            return DLMS_CLASS7_RESULT_ALREADY_SENT;
        }
        else if (attr == 7 || attr == 8)
        {
            total_rows = (md_reset_cnt >= HISTORY_SIZE) ? HISTORY_SIZE : (md_reset_cnt + 1);
            if (total_rows > 3)
                total_rows = 3;               /* Matches the RAM safety cap in attr 2 */
            dlms_apdu_buf[apdu_len++] = 0x06; /* uint32 */
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = total_rows;
        }
        else
        {
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x19, bill_capture_objs, sizeof(bill_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }
    /* Block Load Scalar Profile: 1.0.94.91.4.255 */
    else if (obis[0] == 1 && obis[1] == 0 && obis[2] == 94 && obis[3] == 91 && obis[4] == 4)
    {
        if (attr == 2)
        {
            for (i = 0; i < sizeof(block_scalar_attr2_buf); i++)
            {
                dlms_apdu_buf[apdu_len++] = block_scalar_attr2_buf[i];
            }
        }
        else
        {
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x04, block_scalar_capture_objs, sizeof(block_scalar_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }

    /* Billing Scalar Profile: 1.0.94.91.6.255*/
    else if (obis[0] == 1 && obis[1] == 0 && obis[2] == 94 && obis[3] == 91 && obis[4] == 6)
    {
        if (attr == 2)
        {
            for (i = 0; i < sizeof(bill_scalar_attr2_buf); i++)
            {
                dlms_apdu_buf[apdu_len++] = bill_scalar_attr2_buf[i];
            }
        }
        else
        {
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x06, bill_scalar_capture_objs, sizeof(bill_scalar_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }

    /* Daily Load Scalar Profile: 1.0.94.91.5.255 */
    else if (obis[0] == 1 && obis[1] == 0 && obis[2] == 94 && obis[3] == 91 && obis[4] == 5)
    {
        if (attr == 2)
        {
            for (i = 0; i < sizeof(daily_scalar_attr2_buf); i++)
            {
                dlms_apdu_buf[apdu_len++] = daily_scalar_attr2_buf[i];
            }
        }
        else
        {
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x02, daily_scalar_capture_objs, sizeof(daily_scalar_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }
    /* ========================================================= */
    /* Name Plate Profile: 0.0.94.91.10.255 */
    else if (obis[0] == 0 && obis[1] == 0 && obis[2] == 94 && obis[3] == 91 && obis[4] == 10)
    {
        if (attr == 2) /* Buffer Payload */
        {
            unsigned long int temp_sn;
            unsigned char sn_str_dyn[8];

            dlms_apdu_buf[apdu_len++] = 0x01; /* Array */
            dlms_apdu_buf[apdu_len++] = 0x01; /* 1 Row */
            dlms_apdu_buf[apdu_len++] = 0x02; /* Structure */
            dlms_apdu_buf[apdu_len++] = 0x08; /* 8 Columns */

            /* 1. Clock */
            DLMS_Inject_DateTime(&apdu_len, 2000 + d_yr, d_mnth, d_day, t_hr, t_min, t_sec);

            /* 2. Meter Serial Number */
            temp_sn = meter_no;
            for (j = 6; j >= 0; j--)
            {
                sn_str_dyn[j] = (temp_sn % 10) + '0';
                temp_sn = temp_sn / 10;
            }
            dlms_apdu_buf[apdu_len++] = 0x09; /* Octet-string */
            dlms_apdu_buf[apdu_len++] = 7;    /* Length 7 */
            for (i = 0; i < 7; i++)
                dlms_apdu_buf[apdu_len++] = sn_str_dyn[i];

            /* 3. Manufacturer Name (Dev ID 2) */
            dlms_apdu_buf[apdu_len++] = 0x09;
            dlms_apdu_buf[apdu_len++] = DLMS_MANUFACTURER_LEN;
            for (i = 0; i < DLMS_MANUFACTURER_LEN; i++)
                dlms_apdu_buf[apdu_len++] = dev_id2[i];

            /* 4. Firmware Version */
            dlms_apdu_buf[apdu_len++] = 0x09;
            dlms_apdu_buf[apdu_len++] = DLMS_FW_VERSION_LEN;
            for (i = 0; i < DLMS_FW_VERSION_LEN; i++)
                dlms_apdu_buf[apdu_len++] = fw_id[i];

            /* 5. Meter Type (Code 5) */
            dlms_apdu_buf[apdu_len++] = 0x12; /* uint16 */
            dlms_apdu_buf[apdu_len++] = 0x00;
            dlms_apdu_buf[apdu_len++] = DLMS_METER_TYPE_CODE;

            /* 6. Meter Category (C3) */
            dlms_apdu_buf[apdu_len++] = 0x09;
            dlms_apdu_buf[apdu_len++] = 2;
            dlms_apdu_buf[apdu_len++] = DLMS_METER_CATEGORY_STR[0];
            dlms_apdu_buf[apdu_len++] = DLMS_METER_CATEGORY_STR[1];

            /* 7. Current Rating (5-30A) */
            dlms_apdu_buf[apdu_len++] = 0x09;
            dlms_apdu_buf[apdu_len++] = DLMS_CURRENT_RATING_LEN;
            for (i = 0; i < DLMS_CURRENT_RATING_LEN; i++)
                dlms_apdu_buf[apdu_len++] = curr_rating[i];

            /* 8. Year of Manufacture */
            yr = 2000 + d_yr;
            dlms_apdu_buf[apdu_len++] = 0x12; /* uint16 */
            dlms_apdu_buf[apdu_len++] = (yr >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = yr & 0xFF;
        }
        else if (attr == 7 || attr == 8) /* Entries In Use & Profile Entries */
        {
            dlms_apdu_buf[apdu_len++] = 0x06; /* double-long-unsigned */
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = 1; /* Nameplate is always 1 static entry */
        }
        else /* Metadata Builder */
        {
            /* Pass 0 for capture_period as Nameplate is a static table */
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x08, nameplate_capture_objs, sizeof(nameplate_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }

    /* Event Scalar Profile: 1.0.94.91.7.255 */
    else if (obis[0] == 1 && obis[1] == 0 && obis[2] == 94 && obis[3] == 91 && obis[4] == 7)
    {
        if (attr == 2)
        {
            for (i = 0; i < sizeof(event_scalar_attr2_buf); i++)
            {
                dlms_apdu_buf[apdu_len++] = event_scalar_attr2_buf[i];
            }
        }
        else
        {
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x04, event_scalar_capture_objs, sizeof(event_scalar_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }

    /* ---------------------------------------------------
     * ACTUAL DATA PROFILES (Returns 1 row of current data)
     * --------------------------------------------------- */

    /* Block Load Profile: 1.0.99.1.0.255 */
    else if (obis[0] == 1 && obis[1] == 0 && obis[2] == 99 && obis[3] == 1 && obis[4] == 0)
    {
        if (attr == 2)
        {
            tx_block_total_rows = g_Class07_Blockload_EntriesInUse;
            tx_block_curr_row = 0;
            tx_is_dynamic = 4; /* 4 = Block Load Profile Engine */
            tx_blk_num = 1;
            tx_invoke_id = invoke;
            tx_req_client = client;
            tx_req_server = server;

            DLMS_Generate_BlockLoad_Block();
            *apdu_len_ptr = apdu_len;
            return DLMS_CLASS7_RESULT_ALREADY_SENT;
        }
        else if (attr == 7 || attr == 8)
        {
            /* Attr 8 = Max Capacity, Attr 7 = Current Entries In Use */
            unsigned long return_val = (attr == 8) ? g_Class07_Blockload_MaxEntries : g_Class07_Blockload_EntriesInUse;
            if (return_val == 0)
                return_val = 1; /* Safety fallback */

            dlms_apdu_buf[apdu_len++] = 0x06; /* uint32 */
            dlms_apdu_buf[apdu_len++] = (return_val >> 24) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (return_val >> 16) & 0xFF;
            dlms_apdu_buf[apdu_len++] = (return_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = return_val & 0xFF;
        }
        else
        {
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 1800, 0x05, block_capture_objs, sizeof(block_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }

    /* Daily Load Profile: 1.0.99.2.0.255 */
    else if (obis[0] == 1 && obis[1] == 0 && obis[2] == 99 && obis[3] == 2 && obis[4] == 0)
    {
        if (attr == 2) /* Buffer */
        {
            unsigned long last_date = from_eeprom(LOAD_DATE_LOC + (364 * 3), 3);
            tx_daily_total_rows = (last_date != 0) ? 365 : day_pos;
            tx_daily_curr_row = 0;
            tx_is_dynamic = 2; /* 2 = Daily Profile Engine */
            tx_blk_num = 1;
            tx_invoke_id = invoke;
            tx_req_client = client;
            tx_req_server = server;

            DLMS_Generate_Daily_Block();
            *apdu_len_ptr = apdu_len;
            return DLMS_CLASS7_RESULT_ALREADY_SENT;
        }
        else if (attr == 7 || attr == 8) /* Entries In Use & Max Capacity */
        {
            unsigned long last_date = from_eeprom(LOAD_DATE_LOC + (364 * 3), 3);
            unsigned int t_rows = (last_date != 0) ? 365 : day_pos;
            unsigned int return_val;

            if (t_rows == 0)
                t_rows = 1;

            return_val = (attr == 8) ? 365 : t_rows;

            dlms_apdu_buf[apdu_len++] = 0x06; /* double-long-unsigned */
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = (return_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = return_val & 0xFF;
        }
        else
        {
            if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 86400, 0x03, daily_capture_objs, sizeof(daily_capture_objs), 1))
            {
                return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }

    /* Event Profiles: 0.0.99.98.x.255 */
    else if (obis[0] == 0 && obis[1] == 0 && obis[2] == 99 && obis[3] == 98)
    {
        unsigned char valid_count;
        unsigned int loc;
        unsigned int return_val;
        unsigned char is_active;
        unsigned int total_dlms_rows = 0;
        unsigned char dlms_prof = obis[4];
        unsigned char evt;

        if (dlms_prof == 3)
        {
            tx_event_type = 100;
            valid_count = 0;
            for (i = 0; i < CONFIG_EVENT_SIZE; i++)
            {
                loc = CONFIG_EVENT_LOC + (i * 26);
                if (read_eeprom(loc + 1) != 0 && read_eeprom(loc + 1) != 0xFF)
                    valid_count++;
            }
            total_dlms_rows = valid_count;
        }
        else if (dlms_prof == 2)
        {
            tx_event_type = 200;
            valid_count = 0;
            for (i = 0; i < DLMS_PFAIL_EVENT_SIZE; i++)
            {
                loc = DLMS_PFAIL_EVENT_LOC + (i * 6);
                if (read_eeprom(loc + 1) != 0 && read_eeprom(loc + 1) != 0xFF)
                    valid_count++;
            }
            total_dlms_rows = valid_count;
        }
        else if (dlms_prof == 5)
        {
            tx_event_type = 251;
            /* Cover open uses a single 8-byte buffer in EEPROM */
            if (read_eeprom(CUOPEN_LOC) == 1 || from_eeprom(CUOPEN_LOC + 1, 3) != 0)
                total_dlms_rows = 1;
            else
                total_dlms_rows = 0;
        }
        else if (dlms_prof == 0 || dlms_prof == 1 || dlms_prof == 4)
        {
            tx_event_type = dlms_prof;
            total_dlms_rows = 0;
            for (j = 0; j < 3; j++)
            {
                unsigned char int_type = dlms_to_internal[dlms_prof][j];
                if (int_type == 0xFF)
                    continue;

                valid_count = 0;
                for (i = 0; i < TAMPER_SIZE; i++)
                {
                    loc = TAMPER_LOC + (int_type * 520) + (i * 26);
                    if (read_eeprom(loc) != 0 && read_eeprom(loc) != 0xFF)
                        valid_count++;
                }
                is_active = (store_tamper_stat & (1 << int_type)) ? 1 : 0;
                total_dlms_rows += (valid_count * 2) + is_active;
            }
        }
        else
        {
            tx_event_type = 99;
            total_dlms_rows = 0;
        }

        evt = tx_event_type; /* <--- Sync old 'evt' code with 'tx_event_type' */

        if (attr == 2)
        {
            tx_event_total_rows = total_dlms_rows;
            tx_event_curr_row = 0;
            tx_event_type = evt;

            tx_is_dynamic = 3; /* 3 = Event Profile Engine */
            tx_blk_num = 1;
            tx_invoke_id = invoke;
            tx_req_client = client;
            tx_req_server = server;

            DLMS_Generate_Event_Block();
            *apdu_len_ptr = apdu_len;
            return DLMS_CLASS7_RESULT_ALREADY_SENT;
        }
        else if (attr == 7 || attr == 8)
        {
            if (evt == 100)
            {
                return_val = (attr == 8) ? CONFIG_EVENT_SIZE : total_dlms_rows;
            }
            else if (evt == 200)
            {
                return_val = (attr == 8) ? DLMS_PFAIL_EVENT_SIZE : total_dlms_rows;
            }
            else
            {
                return_val = (attr == 8) ? (TAMPER_SIZE * 2) : total_dlms_rows;
            }

            dlms_apdu_buf[apdu_len++] = 0x06; /* double-long-unsigned */
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = 0;
            dlms_apdu_buf[apdu_len++] = (return_val >> 8) & 0xFF;
            dlms_apdu_buf[apdu_len++] = return_val & 0xFF;
        }
        else
        {
            if (evt == 100)
            {
                if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x02, transaction_capture_objs, sizeof(transaction_capture_objs), 1))
                    return DLMS_CLASS7_RESULT_REJECT;
            }
            else if (evt == 200)
            {
                if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x02, pfail_capture_objs, sizeof(pfail_capture_objs), 1))
                    return DLMS_CLASS7_RESULT_REJECT;
            }
            else
            {
                if (!DLMS_Build_Class7_Metadata(&apdu_len, attr, 0, 0x06, event_capture_objs, sizeof(event_capture_objs), 1))
                    return DLMS_CLASS7_RESULT_REJECT;
            }
        }
    }
    else
    {
        /* Prevent CTT Timeout for unsupported profiles like 1.0.94.91.5.255 */
        dlms_apdu_buf[apdu_len++] = 0xC4;
        dlms_apdu_buf[apdu_len++] = 0x01; // Get Response Normal
        dlms_apdu_buf[apdu_len++] = invoke;
        dlms_apdu_buf[apdu_len++] = 0x01; // Data Access Error
        dlms_apdu_buf[apdu_len++] = 0x03; // Object Undefined
    }
    *apdu_len_ptr = apdu_len;
    return DLMS_CLASS7_RESULT_SEND;
}