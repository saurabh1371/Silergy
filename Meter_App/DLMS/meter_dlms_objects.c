/*******************************************************************************
 * meter_dlms_objects.c
 *
 * This file is the "object dictionary" for the meter's DLMS/COSEM model.
 * Every register a DLMS client can Read/Write/Action lives here as one
 * small callback, and DLMS_Object_Dictionary[] at the bottom maps each
 * callback to its class ID + OBIS code + access rights.
 *
 * dlms_cosem.c looks up this table by OBIS code and calls whichever
 * callback matches - this file never talks to the wire protocol directly.
 ******************************************************************************/
#include <stddef.h>
#include "asdaq_variables.h"
#include "dlms_eeprom.h"
#include "dlms_stack.h"
#include "dlms_api.h"

// --- SILERGY-TO-RENESAS TRANSLATION BRIDGE ---
#define vrms_reg3          inst_voltage
#define irms1_reg3         inst_phase_current
#define irms2_reg3         inst_neutral_current
#define freq_reg3          inst_freq
#define pf_reg3            inst_pf
#define tot_kw_reg3        inst_kw
#define kva_reg3           inst_kva
#define ontime             reset_on_time
// #define tamper_cnt         all_tamper_cnt

// Memory and Config mappings
// #define config_change_cnt  scratch
#define config_event_pos   scratch2
#define CONFIG_EVENT_SIZE  10
#define CONFIG_EVENT_LOC   0
#define DLMS_PFAIL_EVENT_SIZE 10
#define DLMS_PFAIL_EVENT_LOC 0
#define DLMS_PFAIL_EVENT_POS_LOC 0
#define tamper_pos         event_pos
#define TAMPER_SIZE        EVENT_SIZE
#define TAMPER_LOC         VOLT_EVENT_LOC
#define INST_TAMPER_LOC    VOLT_EVENT_LOC
#define HIST_BILL_TIME_LOC LAST_BILL_TIME_LOC

// TOD and Billing mappings
#define passive_num_of_zone num_of_zone
#define passive_t_zone      t_zone
#define PASSIVE_NUM_ZONE_LOC NUM_ZONE_LOC
#define tod_activation_time activate_time
#define TOD_ACTIVATION_TIME_LOC ACTIVATE_TIME_LOC
#define spec_bill_active    ep_clear_stat
#define spec_bill_hr        single_billing_hr
#define spec_bill_mn        single_billing_min
#define billing_hour        billing_hr
#define billing_minute      billing_min
#define spec_bill_yr        single_billing_year
#define spec_bill_mo        single_billing_month
#define spec_bill_dy        single_billing_day
#define BILL_HOUR_LOC       BILL_TIME_LOC
#define BILL_MINUTE_LOC     (BILL_TIME_LOC+1)
#define SPEC_BILL_LOC       SCHEDULED_BILL_DAY_LOC
#define last_stored_tod_kwh_val last_hr_load_val[0]
#define last_stored_tod_kvah_val last_hr_load_val[1]

// Profile Generic mappings
#define LOAD_DATE_LOC       DAILY_SURVEY_LOC
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
typedef struct {
    struct { unsigned char year_high, year_low, month, day_of_month, hour, minute; } clock_value;
    unsigned int voltage_value, kWh_value, kVAh_value, current_value;
} class07_blockload_entry_t;
extern class07_blockload_entry_t g_Class07_BlockLoadBuffer;
#endif
// ---------------------------------------------

/* Running write-position for the power-fail event log in EEPROM (wraps at
 * DLMS_PFAIL_EVENT_SIZE). Kept with the other module state up top. */
unsigned int dlms_pfail_event_pos = 0;

/* =========================================================
 * 1. READ CALLBACKS
 * ========================================================= */

/* Class 3 (Register) - live RMS voltage, attr 2 = value, attr 3 = scaler/unit */
static void Read_Voltage(unsigned char attr, unsigned int *apdu_len)
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, vrms_reg3); // Send Live Voltage
    }
    else if (attr == 3)
    {
        // Scalar & Unit: -1 Scaler (0xFF), 35 = Voltage (0x23)
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x23;
    }
}

/* Class 1 (Data) - 1.0.0.8.0.255 Demand Integration Period, in seconds.
 * Shares md_intgr_val with the write side below. */
static void Read_MD_Integration_Period(unsigned char attr, unsigned int *apdu_len)
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, (md_intgr_val * 60));
    }
}

/* Class 1 (Data) - 1.0.0.8.4.255 Load Survey Capture Period, in seconds. */
static void Read_Profile_Capture_Period(unsigned char attr, unsigned int *apdu_len)
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, (survey_intgr_val * 60));
    }
}

/* ---------------------------------------------------------------------
 * Class 1 (Data) - static/device-info objects. Each only answers attr 2;
 * anything else falls through to Get_Reject (see dlms_cosem.c dispatcher).
 * ------------------------------------------------------------------- */

static void Read_LDN(unsigned char attr, unsigned int *apdu_len) /* 0.0.42.0.0.255 */
{
    unsigned char i;
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = DLMS_LDN_LEN;
        for (i = 0; i < DLMS_LDN_LEN; i++)
            dlms_apdu_buf[(*apdu_len)++] = DLMS_LDN_STR[i];
    }
}

static void Read_MeterNo(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.1.0.255 */
{
    unsigned char i;
    unsigned long temp_sn;
    unsigned char sn_str_dyn[7];
    if (attr == 2)
    {
        temp_sn = meter_no;
        for (i = 7; i > 0; i--)
        {
            sn_str_dyn[i - 1] = (temp_sn % 10) + '0';
            temp_sn = temp_sn / 10;
        }
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 7;
        for (i = 0; i < 7; i++)
            dlms_apdu_buf[(*apdu_len)++] = sn_str_dyn[i];
    }
}

static void Read_ManufacturerName(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.1.1.255 */
{
    unsigned char i;
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = DLMS_MANUFACTURER_LEN;
        for (i = 0; i < DLMS_MANUFACTURER_LEN; i++)
            dlms_apdu_buf[(*apdu_len)++] = DLMS_MANUFACTURER_STR[i];
    }
}

static void Read_FWVersion(unsigned char attr, unsigned int *apdu_len) /* 1.0.0.2.0.255 */
{
    unsigned char i;
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = DLMS_FW_VERSION_LEN;
        for (i = 0; i < DLMS_FW_VERSION_LEN; i++)
            dlms_apdu_buf[(*apdu_len)++] = DLMS_FW_VERSION_STR[i];
    }
}

static void Read_MeterType(unsigned char attr, unsigned int *apdu_len) /* 0.0.94.91.9.255 */
{
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = DLMS_METER_TYPE_CODE;
    }
}

static void Read_MeterCategory(unsigned char attr, unsigned int *apdu_len) /* 0.0.94.91.11.255 */
{
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 2;
        dlms_apdu_buf[(*apdu_len)++] = DLMS_METER_CATEGORY_STR[0];
        dlms_apdu_buf[(*apdu_len)++] = DLMS_METER_CATEGORY_STR[1];
    }
}

static void Read_CurrentRating(unsigned char attr, unsigned int *apdu_len) /* 0.0.94.91.12.255 */
{
    unsigned char i;
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = DLMS_CURRENT_RATING_LEN;
        for (i = 0; i < DLMS_CURRENT_RATING_LEN; i++)
            dlms_apdu_buf[(*apdu_len)++] = DLMS_CURRENT_RATING_STR[i];
    }
}

static void Read_YearOfManufacture(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.1.4.255 */
{
    unsigned int yr;
    if (attr == 2)
    {
        yr = 2000 + d_yr;
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = (yr >> 8) & 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = yr & 0xFF;
    }
}

static void Read_ProfileBufferDuration(unsigned char attr, unsigned int *apdu_len) /* 1.0.0.8.5.255, fixed 86400 sec */
{
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x06;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x51;
        dlms_apdu_buf[(*apdu_len)++] = 0x80;
    }
}

static void Read_TamperCountCumulative(unsigned char attr, unsigned int *apdu_len) /* 0.0.94.91.0.255 */
{
    unsigned long register_val;
    if (attr == 2)
    {
        register_val = all_tamper_cnt;
        dlms_apdu_buf[(*apdu_len)++] = 0x06; /* Double-Long-Unsigned */
        dlms_apdu_buf[(*apdu_len)++] = (register_val >> 24) & 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = (register_val >> 16) & 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = (register_val >> 8) & 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = register_val & 0xFF;
    }
}

static void Read_BillingCounter(unsigned char attr, unsigned int *apdu_len) /* 0.0.0.1.0.255 */
{
    unsigned long bill_cnt;
    if (attr == 2)
    {
        bill_cnt = (unsigned long)md_reset_cnt;
        dlms_apdu_buf[(*apdu_len)++] = 0x06; /* Double-Long-Unsigned */
        dlms_apdu_buf[(*apdu_len)++] = (bill_cnt >> 24) & 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = (bill_cnt >> 16) & 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = (bill_cnt >> 8) & 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = bill_cnt & 0xFF;
    }
}

static void Read_ConfigChangeCount(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.2.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, (unsigned int)Cum_Prog_Count);
    }
}

/* Shared resolver for the 0.0.96.11.x.255 "latest event code" family below -
 * target_evt picks which slot, mirroring the mapping the original inline
 * code used (100 = last config/transaction event, <TAMPER_TYPE = a tamper
 * slot, anything else = always-0 / not wired up). */
static void Read_EventCode_Common(unsigned int *apdu_len, unsigned char target_evt)
{
    unsigned int latest_ev_code = 0;
    unsigned int h_idx;
    unsigned int l_loc;

    if (target_evt == 100)
    {
        h_idx = (config_event_pos == 0) ? (CONFIG_EVENT_SIZE - 1) : (config_event_pos - 1);
        l_loc = CONFIG_EVENT_LOC + (h_idx * 26);
        if (read_eeprom(l_loc + 1) != 0 && read_eeprom(l_loc + 1) != 0xFF)
        {
            latest_ev_code = (read_eeprom(l_loc) << 8) | read_eeprom(l_loc + 1);
        }
    }
    else if (target_evt < TAMPER_TYPE)
    {
        if ((store_tamper_stat & (1 << target_evt)) != 0)
        {
            latest_ev_code = get_is15959_event_code(target_evt, 0);
        }
        else
        {
            h_idx = (tamper_pos[target_evt] == 0) ? (TAMPER_SIZE - 1) : (tamper_pos[target_evt] - 1);
            l_loc = TAMPER_LOC + (target_evt * 520) + (h_idx * 26);
            if (from_eeprom(l_loc + 1, 4) != 0)
            {
                latest_ev_code = (from_eeprom(l_loc + 14, 4) != 0)
                                     ? get_is15959_event_code(target_evt, 1)
                                     : get_is15959_event_code(target_evt, 0);
            }
        }
    }
    /* target_evt == 99 (unmapped slots 2 and 6): falls through, reports 0 - matches original */

    dlms_apdu_buf[(*apdu_len)++] = 0x12;
    dlms_apdu_buf[(*apdu_len)++] = (latest_ev_code >> 8) & 0xFF;
    dlms_apdu_buf[(*apdu_len)++] = latest_ev_code & 0xFF;
}

static void Read_EventCode_E0(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.11.0.255 */
{
    if (attr == 2)
        Read_EventCode_Common(apdu_len, 3);
}
static void Read_EventCode_E1(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.11.1.255 */
{
    if (attr == 2)
        Read_EventCode_Common(apdu_len, 1);
}
static void Read_EventCode_E2(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.11.2.255 - not wired to a slot, always reports 0 */
{
    if (attr == 2)
        Read_EventCode_Common(apdu_len, 99);
}
static void Read_EventCode_E3(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.11.3.255 - last config/transaction event */
{
    if (attr == 2)
        Read_EventCode_Common(apdu_len, 100);
}
static void Read_EventCode_E4(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.11.4.255 */
{
    if (attr == 2)
        Read_EventCode_Common(apdu_len, 0);
}
static void Read_EventCode_E5(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.11.5.255 */
{
    if (attr == 2)
        Read_EventCode_Common(apdu_len, 2);
}
static void Read_EventCode_E6(unsigned char attr, unsigned int *apdu_len) /* 0.0.96.11.6.255 - not wired to a slot, always reports 0 */
{
    if (attr == 2)
        Read_EventCode_Common(apdu_len, 99);
}

static void Read_BillingTimestamp(unsigned char attr, unsigned int *apdu_len) /* 0.0.0.1.2.255 */
{
    unsigned long tmp_md_dt;
    unsigned int h_idx;
    unsigned int yr;
    unsigned char mo, dy, hr, mn;

    if (attr != 2)
        return;

    if (md_reset_cnt > 0)
    {
        h_idx = (mnth_pos == 0) ? (HISTORY_SIZE - 1) : (mnth_pos - 1);
        tmp_md_dt = from_eeprom(KWMD_LOC + (h_idx * 21), 3);

        if (tmp_md_dt == 0)
        {
            DLMS_Inject_Dummy_DateTime(apdu_len);
        }
        else
        {
            yr = 2000 + (tmp_md_dt % 100);
            mo = (tmp_md_dt / 100) % 100;
            dy = (tmp_md_dt / 10000) % 100;

            hr = read_eeprom(HIST_BILL_TIME_LOC + (h_idx * 2));
            mn = read_eeprom(HIST_BILL_TIME_LOC + (h_idx * 2) + 1);
            if (hr > 23)
                hr = 0;
            if (mn > 59)
                mn = 0;

            DLMS_Inject_DateTime(apdu_len, yr, mo, dy, hr, mn, 0);
        }
    }
    else
    {
        DLMS_Inject_Dummy_DateTime(apdu_len);
    }
}

static void Read_FWSignature(unsigned char attr, unsigned int *apdu_len) /* 1.0.0.2.8.255 */
{
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 5;
        dlms_apdu_buf[(*apdu_len)++] = 'S';
        dlms_apdu_buf[(*apdu_len)++] = 'I';
        dlms_apdu_buf[(*apdu_len)++] = 'G';
        dlms_apdu_buf[(*apdu_len)++] = '0';
        dlms_apdu_buf[(*apdu_len)++] = '1';
    }
}

static void Read_InternalStatus(unsigned char attr, unsigned int *apdu_len) /* 1.0.96.5.0.255 */
{
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x06; /* 0 = Healthy */
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
}

static void Read_ActiveMD(unsigned char attr, unsigned int *apdu_len) /* 1.0.1.6.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type05_Uint32(apdu_len, kwmd_val);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x00; /* scaler 0 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x1B; /* W */
    }
    else if (attr == 4)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x05;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
    else if (attr == 5)
    {
        DLMS_Inject_Dummy_DateTime(apdu_len);
    }
}

static void Read_ApparentMD(unsigned char attr, unsigned int *apdu_len) /* 1.0.9.6.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type05_Uint32(apdu_len, kvamd_val);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x00; /* scaler 0 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x1C; /* VA */
    }
    else if (attr == 4)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x05;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
    else if (attr == 5)
    {
        DLMS_Inject_Dummy_DateTime(apdu_len);
    }
}

/* Shared handler for the four Class 15 (Association LN) instances -
 * 0.0.40.0.{0,1,2,3}.255. assoc_id mirrors the original obis[4]: 0 = current
 * association (resolved from dlms_get_client_sap), 1/2/3 = a specific
 * client's fixed association. attr 2 (object list) never reaches here - the
 * existing block-transfer path in DLMS_COSEM_ProcessGetRequest serves it
 * directly and returns before the object-dictionary lookup runs. */
static void Read_Association_Common(unsigned char attr, unsigned int *apdu_len, unsigned char assoc_id)
{
    if (attr == 3) /* associated_partners_id */
    {
        unsigned char client_sap;

        if (assoc_id == 1)
            client_sap = 0x10;
        else if (assoc_id == 2)
            client_sap = 0x20;
        else if (assoc_id == 3)
            client_sap = 0x30;
        else
            client_sap = (dlms_get_client_sap == 0x61) ? 0x30 : ((dlms_get_client_sap == 0x41) ? 0x20 : 0x10);

        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = client_sap;
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
    }
    else if (attr == 4) /* application_context_name */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x07;
        dlms_apdu_buf[(*apdu_len)++] = 0x60;
        dlms_apdu_buf[(*apdu_len)++] = 0x85;
        dlms_apdu_buf[(*apdu_len)++] = 0x74;
        dlms_apdu_buf[(*apdu_len)++] = 0x05;
        dlms_apdu_buf[(*apdu_len)++] = 0x08;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
    }
    else if (attr == 5) /* xDLMS_context_info */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x06;
        dlms_apdu_buf[(*apdu_len)++] = 0x04;
        dlms_apdu_buf[(*apdu_len)++] = 0x18;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x10;
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x11;
        dlms_apdu_buf[(*apdu_len)++] = 0x06;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
    else if (attr == 6) /* authentication_mechanism_name */
    {
        unsigned char auth_mech;

        if (assoc_id == 1)
            auth_mech = 0x00;
        else if (assoc_id == 2)
            auth_mech = 0x01;
        else if (assoc_id == 3)
            auth_mech = 0x02;
        else
            auth_mech = (dlms_get_client_sap == 0x61) ? 0x02 : ((dlms_get_client_sap == 0x41) ? 0x01 : 0x00);

        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x07;
        dlms_apdu_buf[(*apdu_len)++] = 0x60;
        dlms_apdu_buf[(*apdu_len)++] = 0x85;
        dlms_apdu_buf[(*apdu_len)++] = 0x74;
        dlms_apdu_buf[(*apdu_len)++] = 0x05;
        dlms_apdu_buf[(*apdu_len)++] = 0x08;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = auth_mech;
    }
    else if (attr == 8) /* association_status */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
    }
    else if (attr == 9) /* security_setup_reference */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
}

static void Read_Association_Current(unsigned char attr, unsigned int *apdu_len) /* 0.0.40.0.0.255 */
{
    Read_Association_Common(attr, apdu_len, 0);
}
static void Read_Association_Client16(unsigned char attr, unsigned int *apdu_len) /* 0.0.40.0.1.255 */
{
    Read_Association_Common(attr, apdu_len, 1);
}
static void Read_Association_Client32(unsigned char attr, unsigned int *apdu_len) /* 0.0.40.0.2.255 */
{
    Read_Association_Common(attr, apdu_len, 2);
}
static void Read_Association_Client48(unsigned char attr, unsigned int *apdu_len) /* 0.0.40.0.3.255 */
{
    Read_Association_Common(attr, apdu_len, 3);
}

static void Read_Clock(unsigned char attr, unsigned int *apdu_len) /* 0.0.1.0.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_DateTime(apdu_len, 2000 + d_yr, d_mnth, d_day, t_hr, t_min, t_sec);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x10;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x4A;
    }
    else if (attr == 4)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x11;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
    else if (attr == 5 || attr == 6)
    {
        /* Daylight Savings Begin (5) / End (6): standard wildcard datetime */
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x0C;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0x80;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
    }
    else if (attr == 7)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
    else if (attr == 8)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x03;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
    else if (attr == 9)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
    }
}

static void Read_ActivityCalendar(unsigned char attr, unsigned int *apdu_len) /* 0.0.13.0.0.255 */
{
    unsigned char i;

    if (attr == 2 || attr == 6) /* calendar_name active/passive */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x03;
        dlms_apdu_buf[(*apdu_len)++] = 'T';
        dlms_apdu_buf[(*apdu_len)++] = 'O';
        dlms_apdu_buf[(*apdu_len)++] = 'D';
    }
    else if (attr == 3 || attr == 7) /* season_profile active/passive */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x03;
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 'S';
        dlms_apdu_buf[(*apdu_len)++] = '1';
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x0C;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0x80;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 'W';
        dlms_apdu_buf[(*apdu_len)++] = '1';
    }
    else if (attr == 4 || attr == 8) /* week_profile active/passive */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x08;
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 'W';
        dlms_apdu_buf[(*apdu_len)++] = '1';
        for (i = 0; i < 7; i++)
        {
            dlms_apdu_buf[(*apdu_len)++] = 0x11;
            dlms_apdu_buf[(*apdu_len)++] = 0x01;
        }
    }
    else if (attr == 5 || attr == 9) /* day_profile active/passive */
    {
        unsigned char target_num;
        unsigned int *target_zone_ptr;
        unsigned char out_hr, out_mn;

        if (attr == 5)
        {
            target_num = num_of_zone;
            target_zone_ptr = t_zone;
        }
        else
        {
            target_num = passive_num_of_zone;
            target_zone_ptr = passive_t_zone;
        }

        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x11;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;

        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = TOD_SIZE;

        for (i = 0; i < TOD_SIZE; i++)
        {
            dlms_apdu_buf[(*apdu_len)++] = 0x02;
            dlms_apdu_buf[(*apdu_len)++] = 0x03;

            dlms_apdu_buf[(*apdu_len)++] = 0x09;
            dlms_apdu_buf[(*apdu_len)++] = 0x04;

            if (i < target_num)
            {
                out_hr = (unsigned char)(target_zone_ptr[i] / 100);
                out_mn = (unsigned char)(target_zone_ptr[i] % 100);
                if (out_hr > 23)
                    out_hr = 0;
                if (out_mn > 59)
                    out_mn = 0;
                dlms_apdu_buf[(*apdu_len)++] = out_hr;
                dlms_apdu_buf[(*apdu_len)++] = out_mn;
            }
            else
            {
                dlms_apdu_buf[(*apdu_len)++] = 0x00;
                dlms_apdu_buf[(*apdu_len)++] = 0x00;
            }

            dlms_apdu_buf[(*apdu_len)++] = 0x00;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;

            dlms_apdu_buf[(*apdu_len)++] = 0x09;
            dlms_apdu_buf[(*apdu_len)++] = 0x06;
            dlms_apdu_buf[(*apdu_len)++] = 0x00;
            dlms_apdu_buf[(*apdu_len)++] = 0x00;
            dlms_apdu_buf[(*apdu_len)++] = 0x0A;
            dlms_apdu_buf[(*apdu_len)++] = 0x00;
            dlms_apdu_buf[(*apdu_len)++] = 0x64;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;

            dlms_apdu_buf[(*apdu_len)++] = 0x12;
            dlms_apdu_buf[(*apdu_len)++] = 0x00;

            if (i < target_num)
                dlms_apdu_buf[(*apdu_len)++] = (i + 1);
            else
                dlms_apdu_buf[(*apdu_len)++] = 0x00;
        }
    }
    else if (attr == 10) /* activation_time */
    {
        if (tod_activation_time == 0)
        {
            dlms_apdu_buf[(*apdu_len)++] = 0x09;
            dlms_apdu_buf[(*apdu_len)++] = 0x0C;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0x80;
            dlms_apdu_buf[(*apdu_len)++] = 0x00;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        }
        else
        {
            unsigned long temp = tod_activation_time;
            unsigned char mn, hr, dy, mo;
            unsigned int yr;

            mn = temp & 0x3F;
            hr = (temp >> 6) & 0x1F;
            dy = (temp >> 11) & 0x1F;
            mo = (temp >> 16) & 0x0F;
            yr = 2000 + ((temp >> 20) & 0xFF);

            if (mn > 59)
                mn = 0;
            if (hr > 23)
                hr = 0;

            dlms_apdu_buf[(*apdu_len)++] = 0x09;
            dlms_apdu_buf[(*apdu_len)++] = 0x0C;
            dlms_apdu_buf[(*apdu_len)++] = (yr >> 8) & 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = yr & 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = mo;
            dlms_apdu_buf[(*apdu_len)++] = dy;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = hr;
            dlms_apdu_buf[(*apdu_len)++] = mn;
            dlms_apdu_buf[(*apdu_len)++] = 0x00;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0x80;
            dlms_apdu_buf[(*apdu_len)++] = 0x00;
            dlms_apdu_buf[(*apdu_len)++] = 0x00;
        }
    }
}

static void Read_BillingSchedule(unsigned char attr, unsigned int *apdu_len) /* 0.0.15.0.0.255 */
{
    if (attr == 2) /* executed_script */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x06;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x0A;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x64;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
    }
    else if (attr == 3) /* type */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
    }
    else if (attr == 4) /* execution_time */
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;

        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x04;
        if (spec_bill_active == 1)
        {
            dlms_apdu_buf[(*apdu_len)++] = spec_bill_hr;
            dlms_apdu_buf[(*apdu_len)++] = spec_bill_mn;
        }
        else
        {
            dlms_apdu_buf[(*apdu_len)++] = billing_hour;
            dlms_apdu_buf[(*apdu_len)++] = billing_minute;
        }
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;

        dlms_apdu_buf[(*apdu_len)++] = 0x09;
        dlms_apdu_buf[(*apdu_len)++] = 0x05;
        if (spec_bill_active == 1)
        {
            dlms_apdu_buf[(*apdu_len)++] = (spec_bill_yr >> 8) & 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = spec_bill_yr & 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = spec_bill_mo;
            dlms_apdu_buf[(*apdu_len)++] = spec_bill_dy;
        }
        else
        {
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = 0xFF;
            dlms_apdu_buf[(*apdu_len)++] = billing_day;
        }
        dlms_apdu_buf[(*apdu_len)++] = 0xFF;
    }
}

static void Read_HDLC_Setup(unsigned char attr, unsigned int *apdu_len) /* 0.0.22.0.0.255 */
{
    if (attr == 2)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x05;
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x11;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
    }
    else if (attr == 4)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x11;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
    }
    else if (attr == 5)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
    else if (attr == 6)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
    }
    else if (attr == 7)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x00;
        dlms_apdu_buf[(*apdu_len)++] = 0x64; /* 100 */
    }
    else if (attr == 8)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x13;
        dlms_apdu_buf[(*apdu_len)++] = 0x88; /* 5000 */
    }
    else if (attr == 9)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x12;
        dlms_apdu_buf[(*apdu_len)++] = 0x01;
        dlms_apdu_buf[(*apdu_len)++] = 0x00; /* 256 */
    }
}

static void Read_ActiveEnergy(unsigned char attr, unsigned int *apdu_len) /* 1.0.1.8.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type05_Uint32(apdu_len, load_val[0]);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x01; /* scaler +1 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x1E; /* Wh */
    }
}

static void Read_ApparentEnergy(unsigned char attr, unsigned int *apdu_len) /* 1.0.9.8.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type05_Uint32(apdu_len, load_val[1]);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x01; /* scaler +1 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x1F; /* VAh */
    }
}

static void Read_Current1(unsigned char attr, unsigned int *apdu_len) /* 1.0.11.7.0.255 (and .27.) */
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, irms1_reg3);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0xFD; /* scaler -3 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x21; /* A */
    }
}

static void Read_Current2(unsigned char attr, unsigned int *apdu_len) /* 1.0.91.7.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, irms2_reg3);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0xFD; /* scaler -3 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x21; /* A */
    }
}

static void Read_Frequency(unsigned char attr, unsigned int *apdu_len) /* 1.0.14.7.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, freq_reg3);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF; /* scaler -1 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x2C; /* Hz */
    }
}

static void Read_InstPF(unsigned char attr, unsigned int *apdu_len) /* 1.0.13.7.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, pf_reg3 * 10);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0xFD; /* scaler -3 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF; /* unitless */
    }
}

static void Read_AvgPF(unsigned char attr, unsigned int *apdu_len) /* 1.0.13.0.0.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, avg_pf);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0xFE; /* scaler -2 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF; /* unitless */
    }
}

static void Read_ActivePower(unsigned char attr, unsigned int *apdu_len) /* 1.0.1.7.0.255 (and .29.) */
{
    if (attr == 2)
    {
        DLMS_Inject_Type05_Uint32(apdu_len, tot_kw_reg3);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x00; /* scaler 0 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x1B; /* W */
    }
}

static void Read_ApparentPower(unsigned char attr, unsigned int *apdu_len) /* 1.0.9.7.0.255 (and .29.) */
{
    if (attr == 2)
    {
        DLMS_Inject_Type05_Uint32(apdu_len, kva_reg3);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x00; /* scaler 0 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x1C; /* VA */
    }
}

static void Read_PowerOnDuration(unsigned char attr, unsigned int *apdu_len) /* 0.0.94.91.14.255 - minutes (ontime is 6-min blocks) */
{
    if (attr == 2)
    {
        DLMS_Inject_Type05_Uint32(apdu_len, Cum_Power_On_Dur);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0xFF; /* scaler -1 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x06; /* Minutes */
    }
}

static void Read_ActiveCurrentAlt(unsigned char attr, unsigned int *apdu_len) /* 1.0.94.91.14.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, irms1_reg3);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0xFE; /* scaler -2 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x21; /* A */
    }
}

static void Read_BillDailyCount(unsigned char attr, unsigned int *apdu_len) /* 0.0.94.91.127.255 */
{
    if (attr == 2)
    {
        DLMS_Inject_Type05_Uint32(apdu_len, md_reset_cnt);
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x00; /* scaler 0 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x06; /* Months */
    }
}

static void Read_DIP_Register(unsigned char attr, unsigned int *apdu_len) /* 1.0.0.8.0.255 as a Class 3 register */
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, (md_intgr_val * 60));
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x00; /* scaler 0 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x07; /* Seconds */
    }
}

static void Read_PCP_Register(unsigned char attr, unsigned int *apdu_len) /* 1.0.0.8.4.255 as a Class 3 register */
{
    if (attr == 2)
    {
        DLMS_Inject_Type12_Uint16(apdu_len, (survey_intgr_val * 60));
    }
    else if (attr == 3)
    {
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x02;
        dlms_apdu_buf[(*apdu_len)++] = 0x0F;
        dlms_apdu_buf[(*apdu_len)++] = 0x00; /* scaler 0 */
        dlms_apdu_buf[(*apdu_len)++] = 0x16;
        dlms_apdu_buf[(*apdu_len)++] = 0x07; /* Seconds */
    }
}

/* Class 8 (Clock) attr 2 - sets the RTC from a DLMS date-time octet string.
 * Tag/length must be 0x09 0x0C (octet-string, 12 bytes) per the DLMS spec. */
static unsigned char Write_RTC(unsigned char attr, unsigned char *data, unsigned int len)
{
    if (attr == 2)
    {
        if (data[0] == 0x09 && data[1] == 0x0C)
        {
            unsigned int yr = (data[2] << 8) | data[3];
            unsigned char mo = data[4];
            unsigned char dy = data[5];
            unsigned char hr = data[7];
            unsigned char mn = data[8];
            unsigned char sc = data[9];

            unsigned char bcd_sec = ((sc / 10) << 4) | (sc % 10);
            unsigned char bcd_min = ((mn / 10) << 4) | (mn % 10);
            unsigned char bcd_hr = ((hr / 10) << 4) | (hr % 10);
            unsigned char bcd_day = ((dy / 10) << 4) | (dy % 10);
            unsigned char bcd_mo = ((mo / 10) << 4) | (mo % 10);
            unsigned char bcd_yr = (((yr % 100) / 10) << 4) | ((yr % 100) % 10);

            set_rtc(bcd_sec, bcd_min, bcd_hr, bcd_day, bcd_mo, bcd_yr);
            store_log_dt(1);
            log_config_change_event(151);

            return DLMS_RESULT_SUCCESS;
        }
        return DLMS_RESULT_TYPE_UNMATCHED;
    }
    return DLMS_RESULT_OBJECT_UNDEFINED;
}

/* =========================================================
 * 3. METHOD CALLBACKS (Action Triggers)
 * ========================================================= */

/* Class 9 (Script Table), method 1 - triggers an MD reset on next cycle */
static unsigned char Method_MD_Reset(unsigned char method_id)
{
    if (method_id == 1)
    {
        md_reset_b = 1;
        log_config_change_event(166);
        return DLMS_RESULT_SUCCESS;
    }
    return DLMS_RESULT_READ_WRITE_DENIED;
}

/* =========================================================
 * ACTIVITY CALENDAR WRITE (Client 48)
 * Parses incoming day_profile_passive and updates TOD EEPROM
 * ========================================================= */
static unsigned char Write_Activity_Calendar(unsigned char attr, unsigned char *data, unsigned int len)
{
    unsigned int offset = 0;
    unsigned char num_zones = 0;
    unsigned char i, dp;
    unsigned int new_t_zone[TOD_SIZE];
    unsigned char num_day_profiles, day_id_type, sel_type;
    unsigned char hr, mn;
    unsigned int yr;
    unsigned char mo, dy, hr_t, mn_t;
    unsigned long int trigger_utc;

    if (attr == 9)
    {
        if (data[offset++] != 0x01)
            return DLMS_RESULT_TYPE_UNMATCHED;
        num_day_profiles = data[offset++];
        if (num_day_profiles == 0)
            return DLMS_RESULT_SUCCESS;

        for (dp = 0; dp < num_day_profiles; dp++)
        {
            if (data[offset++] != 0x02)
                return DLMS_RESULT_TYPE_UNMATCHED;
            if (data[offset++] != 0x02)
                return DLMS_RESULT_TYPE_UNMATCHED;

            day_id_type = data[offset++];
            if (day_id_type == 0x06 || day_id_type == 0x05)
                offset += 4;
            else if (day_id_type == 0x12 || day_id_type == 0x10)
                offset += 2;
            else if (day_id_type == 0x11 || day_id_type == 0x0F || day_id_type == 0x16)
                offset += 1;
            else
                return DLMS_RESULT_TYPE_UNMATCHED;

            if (data[offset++] != 0x01)
                return DLMS_RESULT_TYPE_UNMATCHED;
            num_zones = data[offset++];
            if (num_zones > TOD_SIZE)
                return DLMS_RESULT_TYPE_UNMATCHED;

            for (i = 0; i < num_zones; i++)
            {
                if (data[offset++] != 0x02)
                    return DLMS_RESULT_TYPE_UNMATCHED;
                if (data[offset++] != 0x03)
                    return DLMS_RESULT_TYPE_UNMATCHED;
                if (data[offset++] != 0x09)
                    return DLMS_RESULT_TYPE_UNMATCHED;
                if (data[offset++] != 0x04)
                    return DLMS_RESULT_TYPE_UNMATCHED;

                hr = data[offset++];
                mn = data[offset++];
                offset += 2;

                new_t_zone[i] = (hr * 100) + mn;

                if (data[offset++] != 0x09)
                    return DLMS_RESULT_TYPE_UNMATCHED;
                if (data[offset++] != 0x06)
                    return DLMS_RESULT_TYPE_UNMATCHED;
                offset += 6;

                sel_type = data[offset++];
                if (sel_type == 0x06 || sel_type == 0x05)
                    offset += 4;
                else if (sel_type == 0x12 || sel_type == 0x10)
                    offset += 2;
                else if (sel_type == 0x11 || sel_type == 0x0F || sel_type == 0x16)
                    offset += 1;
                else
                    return DLMS_RESULT_TYPE_UNMATCHED;
            }
        }

        if (num_zones > 0)
        {
            for (i = 0; i < num_zones; i++)
            {
                passive_t_zone[i] = new_t_zone[i];
                to_eeprom(PASSIVE_TOD_LOC + (i * 2), passive_t_zone[i], 2);
            }
            passive_num_of_zone = num_zones;
            write_eeprom(PASSIVE_NUM_ZONE_LOC, passive_num_of_zone);
            log_config_change_event(155);
        }
        return DLMS_RESULT_SUCCESS;
    }
    else if (attr == 10)
    {
        if (len < 14)
            return DLMS_RESULT_TYPE_UNMATCHED;
        if (data[0] != 0x09 || data[1] != 0x0C)
            return DLMS_RESULT_TYPE_UNMATCHED;

        yr = (data[2] << 8) | data[3];
        mo = data[4];
        dy = data[5];
        hr_t = data[7];
        mn_t = data[8];

        trigger_utc = ((unsigned long)(yr % 100) << 20) |
                      ((unsigned long)mo << 16) |
                      ((unsigned long)dy << 11) |
                      ((unsigned long)hr_t << 6) |
                      (mn_t);

        tod_activation_time = trigger_utc;
        to_eeprom(TOD_ACTIVATION_TIME_LOC, tod_activation_time, 4);

        return DLMS_RESULT_SUCCESS;
    }
    else if (attr == 6 || attr == 7 || attr == 8)
    {
        return DLMS_RESULT_SUCCESS;
    }
    return DLMS_RESULT_OBJECT_UNDEFINED;
}

/* Class 20 (Activity Calendar), method 1 - promotes the passive calendar
 * (written above) to active and clears the pending activation time */
static unsigned char Method_Activity_Calendar(unsigned char method_id)
{
    if (method_id == 1)
    {
        unsigned char i;
        num_of_zone = passive_num_of_zone;
        write_eeprom(NUM_ZONE_LOC, num_of_zone);

        for (i = 0; i < TOD_SIZE; i++)
        {
            t_zone[i] = passive_t_zone[i];
            to_eeprom(TOD_LOC + (i * 2), t_zone[i], 2);
        }

        tod_activation_time = 0;
        to_eeprom(TOD_ACTIVATION_TIME_LOC, 0, 4);

        return DLMS_RESULT_SUCCESS;
    }
    return DLMS_RESULT_READ_WRITE_DENIED;
}

/* =========================================================
 * INTEGRATION / CAPTURE PERIOD WRITE (Class 1, attr 2)
 * ========================================================= */
/* Class 1 (Data) - EVENT 152 - MD Integration Period.
 * Independent of Profile Capture Period below: only touches md_intgr_val /
 * MD_INTGR_LOC / PREV_MIN_SLOT_LOC. Does NOT reset load survey data. */
static unsigned char Write_MD_Integration_Period(unsigned char attr, unsigned char *data, unsigned int len)
{
    if (attr == 2)
    {
        unsigned long intgr_sec = 0;
        unsigned int new_min = 0;

        if (data[0] == 0x06 || data[0] == 0x05)
            intgr_sec = ((unsigned long)data[1] << 24) | ((unsigned long)data[2] << 16) | ((unsigned long)data[3] << 8) | data[4];
        else if (data[0] == 0x12 || data[0] == 0x10)
            intgr_sec = ((unsigned int)data[1] << 8) | data[2];
        else if (data[0] == 0x11 || data[0] == 0x0F)
            intgr_sec = data[1];
        else
            return DLMS_RESULT_TYPE_UNMATCHED;

        new_min = intgr_sec / 60;

        if (new_min == 15 || new_min == 30)
        {
            md_intgr_val = new_min;
            write_eeprom(MD_INTGR_LOC, md_intgr_val);

            log_config_change_event(152);

            write_eeprom(PREV_MIN_SLOT_LOC, (t_min / md_intgr_val));

            return DLMS_RESULT_SUCCESS;
        }
        return DLMS_RESULT_TYPE_UNMATCHED;
    }
    return DLMS_RESULT_OBJECT_UNDEFINED;
}

/* Class 1 (Data) - EVENT 153 - Load Profile Capture Period.
 * Independent of MD Integration Period above: only touches survey_intgr_val /
 * SURVEY_INTGR_LOC / PREV_SURVEY_MIN_SLOT_LOC, and resets the load survey
 * (which is specifically what changing the capture period must do). */
static unsigned char Write_Profile_Capture_Period(unsigned char attr, unsigned char *data, unsigned int len)
{
    if (attr == 2)
    {
        unsigned long intgr_sec = 0;
        unsigned int new_min = 0;

        if (data[0] == 0x06 || data[0] == 0x05)
            intgr_sec = ((unsigned long)data[1] << 24) | ((unsigned long)data[2] << 16) | ((unsigned long)data[3] << 8) | data[4];
        else if (data[0] == 0x12 || data[0] == 0x10)
            intgr_sec = ((unsigned int)data[1] << 8) | data[2];
        else if (data[0] == 0x11 || data[0] == 0x0F)
            intgr_sec = data[1];
        else
            return DLMS_RESULT_TYPE_UNMATCHED;

        new_min = intgr_sec / 60;

        if (new_min == 15 || new_min == 30)
        {
            survey_intgr_val = new_min;
            write_eeprom(SURVEY_INTGR_LOC, survey_intgr_val);

            log_config_change_event(153);

            reset_ls_data();

            to_eeprom(HRPOS_LOC, 0, 2);
            to_eeprom(HRCNT_LOC, 0, 2);
            write_eeprom(PREV_SURVEY_MIN_SLOT_LOC, (t_min / survey_intgr_val));

            return DLMS_RESULT_SUCCESS;
        }
        return DLMS_RESULT_TYPE_UNMATCHED;
    }
    return DLMS_RESULT_OBJECT_UNDEFINED;
}

/* =========================================================
 * BILLING SCHEDULE WRITE (Class 22) - DUAL MODE
 * ========================================================= */
static unsigned char Write_Billing_Schedule(unsigned char attr, unsigned char *data, unsigned int len)
{
    if (attr == 4) /* execution_time */
    {
        unsigned int offset = 0;
        unsigned char num_elements, i;
        unsigned char yr_high = 0xFF, yr_low = 0xFF, mo = 0xFF, dy = 1;
        unsigned char hr = 0, mn = 0;
        unsigned int full_year;
        unsigned char date_found = 0;

        if (data[offset++] != 0x01)
            return DLMS_RESULT_TYPE_UNMATCHED;
        num_elements = data[offset++];
        if (num_elements == 0)
            return DLMS_RESULT_SUCCESS;

        if (data[offset++] != 0x02)
            return DLMS_RESULT_TYPE_UNMATCHED;
        if (data[offset++] != 0x02)
            return DLMS_RESULT_TYPE_UNMATCHED;

        /* Safely extract Time and Date, regardless of array order */
        for (i = 0; i < 2; i++)
        {
            if (data[offset] == 0x09) /* Octet String */
            {
                unsigned char str_len = data[offset + 1];
                if (str_len == 0x04) /* Time Format */
                {
                    hr = data[offset + 2];
                    mn = data[offset + 3];
                    offset += (2 + str_len);
                }
                else if (str_len == 0x05) /* Date Format */
                {
                    yr_high = data[offset + 2];
                    yr_low = data[offset + 3];
                    mo = data[offset + 4];
                    dy = data[offset + 5];
                    date_found = 1;
                    offset += (2 + str_len);
                }
                else
                    return DLMS_RESULT_TYPE_UNMATCHED;
            }
            else
                return DLMS_RESULT_TYPE_UNMATCHED;
        }

        if (!date_found)
            return DLMS_RESULT_TYPE_UNMATCHED;

        /* --------------------------------------------------
           LOGIC: Decide if it is Recurring or Specific
           -------------------------------------------------- */
        if (yr_high == 0xFF && yr_low == 0xFF && mo == 0xFF)
        {
            /* RECURRING SCHEDULE */
            if (dy == 0xFE || dy == 0xFD)
                dy = 31; /* Handle 'Last Day' wildcard */
            if (dy >= 1 && dy <= 31)
            {
                billing_day = dy;
                billing_hour = hr;
                billing_minute = mn;

                write_eeprom(BILL_DAY_LOC, billing_day);
                write_eeprom(BILL_HOUR_LOC, billing_hour);
                write_eeprom(BILL_MINUTE_LOC, billing_minute);

                /* Disable specific bill and save to EEPROM */
                spec_bill_active = 0;
                write_eeprom(SPEC_BILL_LOC, 0);

                log_config_change_event(154); /* 154 = Single-action Schedule for Billing Dates */

                return DLMS_RESULT_SUCCESS;
            }
        }
        else
        {
            /* SPECIFIC DATE (One-Off Bill) */
            full_year = ((unsigned int)yr_high << 8) | yr_low;

            spec_bill_yr = full_year;
            spec_bill_mo = mo;
            spec_bill_dy = dy;
            spec_bill_hr = hr;
            spec_bill_mn = mn;
            spec_bill_active = 1;

            /* Save to EEPROM for Power Failures (7 Bytes total) */
            write_eeprom(SPEC_BILL_LOC, 1);                /* Active Flag */
            to_eeprom(SPEC_BILL_LOC + 1, spec_bill_yr, 2); /* Year (2 Bytes) */
            write_eeprom(SPEC_BILL_LOC + 3, spec_bill_mo); /* Month */
            write_eeprom(SPEC_BILL_LOC + 4, spec_bill_dy); /* Day */
            write_eeprom(SPEC_BILL_LOC + 5, spec_bill_hr); /* Hour */
            write_eeprom(SPEC_BILL_LOC + 6, spec_bill_mn); /* Minute */

            log_config_change_event(154); /* 154 = Single-action Schedule for Billing Dates */

            return DLMS_RESULT_SUCCESS;
        }
        return DLMS_RESULT_TYPE_UNMATCHED;
    }
    return DLMS_RESULT_OBJECT_UNDEFINED;
}

/* =========================================================
 * POWER FAILURE EVENT LOGGER
 * Writes 101 (Occurrence) and 102 (Restoration)
 * ========================================================= */
void log_power_event(unsigned int ev_code, unsigned long dt_stamp)
{
    unsigned int loc;

    loc = DLMS_PFAIL_EVENT_LOC + (dlms_pfail_event_pos * 6);

    write_eeprom(loc, (ev_code >> 8) & 0xFF);
    write_eeprom(loc + 1, ev_code & 0xFF);
    to_eeprom(loc + 2, dt_stamp, 4);

    dlms_pfail_event_pos++;
    if (dlms_pfail_event_pos >= DLMS_PFAIL_EVENT_SIZE)
    {
        dlms_pfail_event_pos = 0;
    }
    to_eeprom(DLMS_PFAIL_EVENT_POS_LOC, dlms_pfail_event_pos, 2);
}

/* =========================================================
 * MASTER DICTIONARY
 * Every row: { class_id, obis[6], access_rights, read_cb, write_cb, method_cb }
 * ========================================================= */
const DLMS_Object_t DLMS_Object_Dictionary[] = {
    // 1. Clock (Class 8)
    {8, {0, 0, 1, 0, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48 | ACCESS_WRITE_C48, Read_Clock, Write_RTC, NULL},

    // -- Class 4 (Extended Register - MD) --
    {4, {1, 0, 1, 6, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ActiveMD, NULL, NULL},
    {4, {1, 0, 9, 6, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ApparentMD, NULL, NULL},

    // -- Class 15 (Association LN) --
    {15, {0, 0, 40, 0, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Association_Current, NULL, NULL},
    {15, {0, 0, 40, 0, 1, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Association_Client16, NULL, NULL},
    {15, {0, 0, 40, 0, 2, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Association_Client32, NULL, NULL},
    {15, {0, 0, 40, 0, 3, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Association_Client48, NULL, NULL},

    // -- Class 23 (HDLC Setup / Utility Table) --
    {23, {0, 0, 22, 0, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_HDLC_Setup, NULL, NULL},

    // -- Class 1 (Data) - static / device info --
    {1, {0, 0, 42, 0, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_LDN, NULL, NULL},
    {1, {0, 0, 96, 1, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_MeterNo, NULL, NULL},
    {1, {0, 0, 96, 1, 1, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ManufacturerName, NULL, NULL},
    {1, {1, 0, 0, 2, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_FWVersion, NULL, NULL},
    {1, {0, 0, 94, 91, 9, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_MeterType, NULL, NULL},
    {1, {0, 0, 94, 91, 11, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_MeterCategory, NULL, NULL},
    {1, {0, 0, 94, 91, 12, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_CurrentRating, NULL, NULL},
    {1, {0, 0, 96, 1, 4, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_YearOfManufacture, NULL, NULL},
    {1, {1, 0, 0, 8, 5, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ProfileBufferDuration, NULL, NULL},
    {1, {0, 0, 94, 91, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_TamperCountCumulative, NULL, NULL},
    {1, {0, 0, 0, 1, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_BillingCounter, NULL, NULL},
    {1, {0, 0, 96, 2, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ConfigChangeCount, NULL, NULL},
    {1, {0, 0, 96, 11, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_EventCode_E0, NULL, NULL},
    {1, {0, 0, 96, 11, 1, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_EventCode_E1, NULL, NULL},
    {1, {0, 0, 96, 11, 2, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_EventCode_E2, NULL, NULL},
    {1, {0, 0, 96, 11, 3, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_EventCode_E3, NULL, NULL},
    {1, {0, 0, 96, 11, 4, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_EventCode_E4, NULL, NULL},
    {1, {0, 0, 96, 11, 5, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_EventCode_E5, NULL, NULL},
    {1, {0, 0, 96, 11, 6, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_EventCode_E6, NULL, NULL},
    {1, {0, 0, 0, 1, 2, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_BillingTimestamp, NULL, NULL},
    {1, {1, 0, 0, 2, 8, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_FWSignature, NULL, NULL},
    {1, {1, 0, 96, 5, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_InternalStatus, NULL, NULL},

    // -- Class 3 (Register) --
    {3, {1, 0, 12, 7, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Voltage, NULL, NULL},
    {3, {1, 0, 12, 27, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Voltage, NULL, NULL},
    {3, {1, 0, 1, 8, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ActiveEnergy, NULL, NULL},
    {3, {1, 0, 9, 8, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ApparentEnergy, NULL, NULL},
    {3, {1, 0, 11, 7, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Current1, NULL, NULL},
    {3, {1, 0, 11, 27, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Current1, NULL, NULL},
    {3, {1, 0, 91, 7, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Current2, NULL, NULL},
    {3, {1, 0, 14, 7, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_Frequency, NULL, NULL},
    {3, {1, 0, 13, 7, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_InstPF, NULL, NULL},
    {3, {1, 0, 13, 0, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_AvgPF, NULL, NULL},
    {3, {1, 0, 1, 7, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ActivePower, NULL, NULL},
    {3, {1, 0, 1, 29, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ActivePower, NULL, NULL},
    {3, {1, 0, 9, 7, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ApparentPower, NULL, NULL},
    {3, {1, 0, 9, 29, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ApparentPower, NULL, NULL},
    {3, {0, 0, 94, 91, 14, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_PowerOnDuration, NULL, NULL},
    {3, {1, 0, 94, 91, 14, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_ActiveCurrentAlt, NULL, NULL},
    {3, {0, 0, 94, 91, 127, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_BillDailyCount, NULL, NULL},
    {3, {1, 0, 0, 8, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_DIP_Register, NULL, NULL},
    {3, {1, 0, 0, 8, 4, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48, Read_PCP_Register, NULL, NULL},

    // 3. MD Integration Period (Class 1)
    {1, {1, 0, 0, 8, 0, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48 | ACCESS_WRITE_C48, Read_MD_Integration_Period, Write_MD_Integration_Period, NULL},

    // 4. Profile Capture Period (Class 1)
    {1, {1, 0, 0, 8, 4, 255}, ACCESS_READ_C16 | ACCESS_READ_C32 | ACCESS_READ_C48 | ACCESS_WRITE_C48, Read_Profile_Capture_Period, Write_Profile_Capture_Period, NULL},

    // 5. MD Reset Script (Class 9)
    {9, {0, 0, 10, 0, 1, 255}, ACCESS_METHOD, NULL, NULL, Method_MD_Reset},
    // 6. Activity Calendar (Class 20)
    {20, {0, 0, 13, 0, 0, 255}, ACCESS_READ_C32 | ACCESS_READ_C48 | ACCESS_WRITE_C48 | ACCESS_METHOD, Read_ActivityCalendar, Write_Activity_Calendar, Method_Activity_Calendar},

    // 7. Billing Schedule (Class 22)
    {22, {0, 0, 15, 0, 0, 255}, ACCESS_READ_C32 | ACCESS_READ_C48 | ACCESS_WRITE_C48, Read_BillingSchedule, Write_Billing_Schedule, NULL}};

const unsigned int DLMS_Object_Count = sizeof(DLMS_Object_Dictionary) / sizeof(DLMS_Object_t);