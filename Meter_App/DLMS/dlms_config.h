#ifndef DLMS_CONFIG_H
#define DLMS_CONFIG_H

/*******************************************************************************
 * dlms_config.h
 * Compile-time tuning for the portable DLMS/COSEM stack. Every constant
 * here is a protocol/timing knob, not meter data - safe to carry unchanged
 * into a new controller port, or override per-project.
 ******************************************************************************/

/* Largest HDLC/APDU frame the stack will build or accept */
#define DLMS_MAX_FRAME_SIZE 512

/* Ticks of inactivity (driven by DLMS_HDLC_Tick(), normally called once a
 * second from the meter's RTC/system tick) before an idle association is
 * dropped */
#define HDLC_INACTIVITY_TICKS 20

/* Upper bound on the negotiated max-info-field size the stack will ever
 * advertise, regardless of what a client asks for. Protects small targets
 * and works around head-end tool quirks (e.g. the CTT's frame-size cap) */
#define HDLC_SAFE_MAX_INFO 200

#endif /* DLMS_CONFIG_H */
