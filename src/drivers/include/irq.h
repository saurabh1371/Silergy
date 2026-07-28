//**************************************************************************
//  DESCRIPTION: Implement a dual-priority monitor.
//  This implements two priorities: preemptible, and non-preemptible.
//  Since there's only two, priority inversion is impossible.
//  Since the code runs to completion, hangs are impossible
//  Since there's only one locking flag, and thus one order
//  to get it, deadlocks are impossible.
//  Therefore, it's the start of a very reliable exokernel.
//  Critical regions have to be kept brief, though! 100 microseconds
//  is a recommended maximum.
//  
//  Disabling a single device's interrupt can cause priority inversion. 
//  E.g. a serial interrupt may be disabled in the main loop.  
//  Another device's (a timer...) interrupt preempts the CPU because
//  its interrupt is not locked.
//  The preempting interrupt may be lower priority... and cause
//  the serial interrupt to be delayed so much that data is lost.
//**************************************************************************
#ifndef IRQ_H
#define IRQ_H 1

// Read the current interrupt state of a Cortx M0.
#define IRQ_DEFINES   int primask = __get_PRIMASK();

// Disable the interrupt state. 
#define IRQ_DISABLE() __set_PRIMASK(1)

// Restore the interrupt state.
#define IRQ_ENABLE()  __set_PRIMASK(primask)

// Enable interrupts
#define IRQ_GLOBAL_ENABLE()  __set_PRIMASK(0)

#ifndef INC_FREERTOS_H  // If no FreeRTOS, control interrupts with same API

// Disable interrupts.
#define taskENTER_CRITICAL()  __set_PRIMASK(1)

// Enable interrupts.
#define taskEXIT_CRITICAL()   __set_PRIMASK(0)
#endif


/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif 
