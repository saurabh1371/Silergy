#ifndef DLMS_API_H
#define DLMS_API_H

#define DLMS_RESULT_SUCCESS 0x00
#define DLMS_RESULT_HARDWARE_FAULT 0x01
#define DLMS_RESULT_TEMPORARY_FAILURE 0x02
#define DLMS_RESULT_READ_WRITE_DENIED 0x03
#define DLMS_RESULT_OBJECT_UNDEFINED 0x04
#define DLMS_RESULT_OBJECT_CLASS_INCONSIST 0x09
#define DLMS_RESULT_OBJECT_UNAVAILABLE 0x0B
#define DLMS_RESULT_TYPE_UNMATCHED 0x0C
#define DLMS_RESULT_SCOPE_OF_ACCESS_VIOL 0x0D
#define DLMS_RESULT_DATA_BLOCK_UNAVAILABLE 0x0E

#define DLMS_LDN_STR "SLRG-DLMS1"
#define DLMS_LDN_LEN 10
#define DLMS_MANUFACTURER_STR "Silergy Corp."
#define DLMS_MANUFACTURER_LEN 13
#define DLMS_FW_VERSION_STR "V1.00"
#define DLMS_FW_VERSION_LEN 5
#define DLMS_METER_CATEGORY_STR "C3"
#define DLMS_CURRENT_RATING_STR "5-30A"
#define DLMS_CURRENT_RATING_LEN 5
#define DLMS_METER_TYPE_CODE 0x05

/* Access Rights Bitmasks */
#define ACCESS_READ_C16 0x01  /* Public/MR Client */
#define ACCESS_READ_C32 0x02  /* AMR Client */
#define ACCESS_READ_C48 0x04  /* Utility/CMRI Client */
#define ACCESS_WRITE_C48 0x08 /* Utility Write */
#define ACCESS_METHOD 0x10    /* Action Trigger */

/* Callback Function Signatures */
/* Read: Injects data into dlms_apdu_buf and updates apdu_len */
typedef void (*DLMS_ReadHandler)(unsigned char attr, unsigned int *apdu_len);

/* Write: Parses incoming data, applies it, and returns DLMS Result Code (0x00 = Success) */
typedef unsigned char (*DLMS_WriteHandler)(unsigned char attr, unsigned char *data_ptr, unsigned int len);

/* Method: Executes an action (e.g. MD Reset) */
typedef unsigned char (*DLMS_MethodHandler)(unsigned char method_id);

/* The Object Dictionary Structure */
typedef struct
{
    unsigned int class_id;
    unsigned char obis[6];
    unsigned char access_rights;
    DLMS_ReadHandler read_cb;
    DLMS_WriteHandler write_cb;
    DLMS_MethodHandler method_cb;
} DLMS_Object_t;

/* Global Dictionary defined in your meter code */
extern const DLMS_Object_t DLMS_Object_Dictionary[];
extern const unsigned int DLMS_Object_Count;

/* Helper functions for the callbacks to use */
void DLMS_Inject_Type05_Uint32(unsigned int *len_ptr, unsigned long val);
void DLMS_Inject_Type12_Uint16(unsigned int *len_ptr, unsigned int val);
unsigned int DLMS_Get_Encoded_Length(unsigned char *data);
void DLMS_Inject_DateTime(unsigned int *len_ptr, unsigned int yr, unsigned char mo, unsigned char dy, unsigned char hr, unsigned char mn, unsigned char sec);
void DLMS_Inject_Dummy_DateTime(unsigned int *len_ptr);
void DLMS_Inject_EEPROM_Date(unsigned int *len_ptr, unsigned long check_val, unsigned long eeprom_date, unsigned int eeprom_time);

/* Meter-specific event-code resolver (implemented in dlms_cosem.c, used by
 * both the profile-generic event log and the meter_dlms_objects.c read
 * callbacks for the 0.0.96.11.x.255 "latest event" family) */
unsigned char get_is15959_event_code(unsigned char internal_type, unsigned char is_rest);

#endif /* DLMS_API_H */
