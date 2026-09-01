/* File: vim878.h
 * This file is normally included in main\options.h
 * Defines icons, line sizes and positions, etc. for this display
 * Note, this is for a second revision, in which pin 5 of the LCD
 * should be attached to VPULSE.
 */

/* --- Auto Mode: full scroll list per approved display spec (PCB: P05_B_Gujarat_Big_LCD) ---
 * NOTE: indices are used directly as the LCD_DisplayParm state, in on-screen order.
 * Auto mode free-runs (no timeout): after DISP_AUTO_TOTAL_SCREENS it wraps back to
 * DISP_AUTO_SEG_CHECK and the "AUTO" banner is shown again before the next pass.
 * H1 comes from get_bill_data(1) -> stBilling_Profile (see asdaq_app.c). */
enum DipSeqAuto
{
    DISP_AUTO_SEG_CHECK = 0, // 1. LCD segment check (lamp test)
    DISP_KWH,                // 2. KWh (6+1)
    DISP_KVAH,               // 3. KVAh (6+1)
    DISP_MD,                 // 4. Current month MD in KW (3 decimal)
    DISP_MD_KVA,             // 5. Current month MD in KVA (3 decimal)
    DISP_IL_KW,              // 6. Instant Load in KW (3 decimal)
    DISP_IL_KVA,             // 7. Instant Load in KVA (3 decimal)
    DISP_PM_KWH,             // 8. Previous month KWh, H1 (5+1, Bill)
    DISP_PM_KVAH,            // 9. Previous month KVAh, H1 (5+1, Bill)
    DISP_PM_MD_KW,           // 10. Previous month MD KW, H1 (3 decimal, Bill)
    DISP_PM_MD_KVA,          // 11. Previous month MD KVA, H1 (3 decimal, Bill)
    DISP_PM_ONHR,            // 12. Previous month power-on hours, H1 (Bill)

    DISP_AUTO_TOTAL_SCREENS // Sentinel - total screen count, NOT a screen
};

/* --- Push-button mode: full auto-scrolling sequence (10 s/screen, one cycle then timeout) ---
 * NOTE: indices are used directly as the LCD_PushButton_Parm state, in on-screen order.
 * H1..H6 come from get_bill_data(1..6) -> stBilling_Profile (see asdaq_app.c).
 */
enum PBDispSeq
{
    PB_SEG_CHECK = 0, // LCD segment check (lamp test)
    PB_DATE,          // Real date
    PB_TIME,          // Real time
    PB_SERIAL_NO,     // Serial number (7 digits)
    PB_KWH,           // KWh (6+1)
    PB_KVAH,          // KVAh (6+1)

    PB_H1_KWH,
    PB_H2_KWH,
    PB_H3_KWH,
    PB_H4_KWH,
    PB_H5_KWH,
    PB_H6_KWH, // Prev months KWh (5+1, H1-H6, Bill)

    PB_H1_MD_KW,
    PB_H1_MD_KW_TIME,
    PB_H1_MD_KW_DATE, // Prev month MD KW w/ date & time (3 dec, H1-H3, Bill)
    PB_H2_MD_KW,
    PB_H2_MD_KW_TIME,
    PB_H2_MD_KW_DATE,
    PB_H3_MD_KW,
    PB_H3_MD_KW_TIME,
    PB_H3_MD_KW_DATE,

    PB_H1_KVAH,
    PB_H2_KVAH,
    PB_H3_KVAH,
    PB_H4_KVAH,
    PB_H5_KVAH,
    PB_H6_KVAH, // Prev months KVAh (5+1, H1-H6, Bill)

    PB_H1_MD_KVA,
    PB_H1_MD_KVA_TIME,
    PB_H1_MD_KVA_DATE, // Prev month MD KVA w/ date & time (3 dec, H1-H3, Bill)
    PB_H2_MD_KVA,
    PB_H2_MD_KVA_TIME,
    PB_H2_MD_KVA_DATE,
    PB_H3_MD_KVA,
    PB_H3_MD_KVA_TIME,
    PB_H3_MD_KVA_DATE,

    PB_HIRES_KWH,  // High resolution KWh (2+4)
    PB_HIRES_KVAH, // High resolution KVAh (2+4)
    PB_IL_KW,      // Instant Load in KW (3 decimal)
    PB_IL_KVA,     // Instant Load in KVA (3 decimal)
    PB_INST_V,     // Instant Voltage
    PB_INST_I,     // Phase Current
    PB_INST_IN,    // Neutral Current
    PB_INST_PF,    // Instant PF
    PB_ONHR,       // Power-on hours, current month
    PB_AVG_PF,     // Average PF

    PB_TOTAL_SCREENS // Sentinel - total screen count, NOT a screen
};

/* --- Battery-backup mode: shorter list, auto-scroll + manual (button) scroll --- */
enum BatDispSeq
{
    BAT_KWH = 0,   // KWh (6+1)
    BAT_KVAH,      // KVAh (6+1)
    BAT_MD_KW,     // Current month MD in KW (3 decimal)
    BAT_MD_KVA,    // Current month MD in KVA (3 decimal)
    BAT_PM_KWH,    // Previous month KWh (5+1, H1, Bill)
    BAT_PM_KVAH,   // Previous month KVAh (5+1, H1, Bill)
    BAT_PM_MD_KW,  // Previous month MD in KW (3 decimal, H1, Bill)
    BAT_PM_MD_KVA, // Previous month MD in KVa (3 decimal, H1, Bill)
    BAT_PM_ONHR,   // Previous month power-on hours (P, on, Bill)

    BAT_TOTAL_SCREENS // Sentinel - total screen count
};

/* Seconds each screen is held. Auto mode value (10 s) is set by the existing
 * NoOfSeconds counter in TaskAutoScroll(); battery mode uses its own, slower
 * interval to save power - adjust to match the approved power budget. */
#define BATTERY_DISP_INTERVAL_SEC 5

/* How long (in seconds, i.e. TaskAutoScroll ticks) the "AUTO" / "PUSH" mode
 * banner is held before a scroll cycle begins. Spec calls for 1-2 sec. */
#define MODE_BANNER_DURATION_SEC 2

extern uint8_t BAT_DisplayParm;
void BatteryModeTask(void);       // call once/sec on battery backup - auto-scrolls
void BatteryModeManualNext(void); // call on a button press while on battery backup

#define KWh_Icon       \
    lcd_put_icon(T20); \
    lcd_put_icon(T14); \
    lcd_put_icon(T17);

#define KW_Icon        \
    lcd_put_icon(T20); \
    lcd_put_icon(T14);

#define KVAh_Icon      \
    lcd_put_icon(T20); \
    lcd_put_icon(T15); \
    lcd_put_icon(T16); \
    lcd_put_icon(T17);

#define KVA_Icon       \
    lcd_put_icon(T20); \
    lcd_put_icon(T15); \
    lcd_put_icon(T16);

#define V_Icon lcd_put_icon(T15);

#define A_Icon lcd_put_icon(T16);

// UPPCL
const char test_str[][15] =
    {
        // 0    1    2    3    4   5   6     7   8   9       10        11   12      13    14     15      16     17       18   19     20
        "PF", "LG", "LD", "LAB", "P", "N", "EE", "V", "SP", "NP", "SILERG5", "COMM", "CLr2", "CLr3", "CLr1", "UnLoc", "CLrc", "rESEt", "SLno", "CAL", "HZ",
        // 21    22    23    24    25    26    27    28    29      30      31
        "H1", "H2", "H3", "H4", "H5", "H6", "ON", "IL", "HI", "AUTO", "PUSH"
        /* 21-26: history-entry tags H1-H6 (used only where 2 spare digits are
         *        free, e.g. the MD screens - see .c file for exact placement),
         * 27: "PON" power-on-hours tag, 28: "IL" instant-load tag,
         * 29: "HI" high-resolution tag, 30: "AUTO" mode-entry banner,
         * 31: "PUSH" mode-entry banner (see MODE_BANNER_DURATION_SEC).
         * TODO(bench): these are new tags, not part of the original approved
         * artwork - confirm they fit/read cleanly on the real glass, and
         * consider dedicated icons instead if spare segments exist. */
};

// Set-up values for the LCD hardware on Maxim's India PCB.
#define LCD_MAPL_DEF 0x000783FF // 0x00078BFF
#define LCD_MAPH_DEF 0x000000F0
#define LCD_MODE_DEF 0xFF000048 // 0xfD000008

// Minus character.
#define LCD_MINUS (36)

/* load_val[] (KWh/KVAh accumulators) and stBilling_Profile.Cumm_Energy_KWh/KVAh
 * are stored in units of 0.01 kWh / kVAh (hundredths) - see load_func() in
 * asdaq_app.c ("10 watt*ctr" comment). The (6+1) and (5+1) energy screens on
 * this glass only have 1 decimal digit, so divide by 10 (tenths) before
 * writing them with lcd_put_num(). Do NOT apply this to the (2+4) high-
 * resolution screens, which display the hundredths value directly. */
#define ENERGY_TENTHS(raw) ((raw) / 10)

/* kwmd_val / kvamd_val / stBilling_Profile.MD_KW / stBilling_Profile.MD_KVA
 * (all fed from store_md_data(), asdaq_app.c) are now stored in units of
 * 0.001 kW / kVA (thousandths) - store_md_data() was changed from *60 to
 * *600 to get this 3rd decimal digit of real resolution. The MD screens
 * are a 4-digit "X.XXX" field and take the raw value directly - this macro
 * is now a passthrough, kept so the call sites stay self-documenting and
 * so a future rescale only needs to change this one line. */
#define MD_THOUSANDTHS(raw) (raw)

// The following data define the BQ09193A glass, and need not change on
// different PCBs.

// Define the digits in the display.
#define DIGIT_CNT 8 // 8 digits, total.

/* Icon mapping, indexes arrays in the glass file for segment and common.  */
#define LCD_FIRST_PERIOD 1 // 1st character from left
#define LCD_LAST_PERIOD 6  // 6th character from left
enum icon
{
    P4,
    T6,
    T1,
    P2,
    T5,
    P1,
    T4,
    T3,
    T2,
    P3,
    T10,
    T11,
    T12,
    T13,
    T15,
    T16,
    T17,
    T7,
    T8,
    T9,
    T20,
    T14,
    T18,
    T19

    , // T6
    // EARTH,
    // REV,
    // NM,
    // MAG,
    // BILL,
    // MD,
    // T2,
    // T3,
    //_h,
    // TOD,
    // COPEN,
    // TIME_Icon,
    // ON,
    // OK,
    // P1,
    // COL,
    // T1,
    //_v,
    //_k,
    // P4,
    // P3,
    // COMM,
    // P2
    //
};

// This driver has a debug routine to make debug output
// when the meter is in an extreme low power mode.
// It puts out two numbers that flash alternately and automatically.
// "two" has a decimal point on the right.
void lcd_debug(int one_1, int two_2);
/* File: vim878.C */
/* Include your LCD file here as derived in xl sheet */

/* List of icon segment indexes, 2nd Column from Icon map */
const unsigned char icon_seg_8COM[] =
    {
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        1,
        1

};

/* Commons connected to icons, Last Column from Icon map  */
const unsigned char icon_common_8COM[] =
    {
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        2,
        4,
        8,
        16,
        32,
        64,
        128,
        2,
        4,
        8,
        16,
        32,
        64,
        128

};

/* find segments for characters 1 to 6. */
const unsigned char char_seg_8COM[] =
    {
        /* find 0 segments (A,B,C,D) for characters 1 to 8. */
        0,
        7,
        6,
        5,
        4,
        3,
        2,
        9,
        8,
        0};

// font 0..9A..Z-; Numbers are "commons", a bit
// pattern written to a segment register in LCDINDADDR to make
// part of a 14-segment character.
const unsigned char font_8COM[] =
    {
        /* font[0][], commons for font for characters 0 to 9 and A to Z, '-'  */
        238,
        72,
        186,
        218,
        92,
        214,
        246,
        74,
        254,
        94,
        126,
        244,
        166,
        248,
        182,
        54,
        230,
        124,
        36,
        200,
        180,
        164,
        110,
        110,
        238,
        62,
        94,
        48,
        214,
        180,
        236,
        236,
        236,
        124,
        220,
        186,
        18};

/* end of file */

/* end of file */