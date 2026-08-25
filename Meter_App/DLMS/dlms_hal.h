#ifndef DLMS_HAL_H
#define DLMS_HAL_H

/*******************************************************************************
 * dlms_hal.h
 * Hardware Abstraction Contract for the portable DLMS/COSEM stack.
 *
 * This is the ONLY hardware-facing contract dlms_hdlc.c depends on. To port
 * the stack to a new controller, implement the two functions below
 * somewhere in that controller's own driver / glue code - nothing in
 * dlms_hdlc.c or dlms_cosem.c needs to change.
 ******************************************************************************/

/* --- Must be implemented by the target's UART driver --- */
/* Sends one raw byte out the optical/comm port. Blocking or buffered - the
 * stack does not care, as long as bytes go out in the order sent. */
void HAL_UART_SendByte(unsigned char data);

/* --- Must be implemented by the meter's own glue code (display, LEDs,
 * wake timers, etc). The stack calls this on protocol-level events so
 * dlms_hdlc.c never has to touch a meter's display/timer globals directly.
 * Provide an empty function body if the target has no UI to update. --- */
typedef enum
{
    DLMS_HDLC_EVENT_VALID_FRAME, /* Any well-formed HDLC frame accepted (keep comm/backlight alive) */
    DLMS_HDLC_EVENT_DISCONNECT   /* DISC received / link torn down */
} DLMS_HDLC_Event_t;

void DLMS_HAL_OnFrameActivity(DLMS_HDLC_Event_t event);

#endif /* DLMS_HAL_H */
