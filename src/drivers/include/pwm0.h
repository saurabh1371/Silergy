/***************************************************************************
 * This code and information is provided "as is" without warranty of any 
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: PWM0 header file
 *
 * Copyright 2017, Silergy Corp. All rights reserved.
 *
 * $Id: pwm0.h 15428 2017-09-28 22:05:38Z rvandewa $
 ****************************************************************************/
#ifndef PWM0_H
#define PWM0_H 1

// Firmware Device Driver version.
extern const char fw_ver_pwm0[]; 

// Starts PWM output at desired frequency and Duty Cycle.
int pwm0_start(uint32_t freq_hz, uint32_t duty_cycle);

// Stops PWM output.
void pwm0_stop(void);

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
 
#endif // PWM0_H
