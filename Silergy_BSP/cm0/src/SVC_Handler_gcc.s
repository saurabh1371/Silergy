/******************************************************************************
 * @file     SVC_Handler_gcc.s
 * @brief    Device Startup File for
 *           SY7T301 Device, for the GCC assembler, gas
 * @version  V1.00
 * @date     20. March 2019
 ******************************************************************************
 * Copyright (c) 2019 Silergy Corp.

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of Silergy nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
 
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   --------------------------------------------------------------------------*/

    .text

    .align 4

/* Decode a service call exception. */
    .func SVC_Handler
    .global SVC_Handler
    .thumb_func
SVC_Handler:
    /* Find the correct stack pointer. */
    movs %r1, #4
    mov %r0, %lr
    tst %r0, %r1    /* Which stack pointer? (0=main stack pointer) */
    beq sd_sp_msp
    mrs %r1, psp    /* Process stack pointer. (Called from app. code.) */
    b sd_sp_exit
sd_sp_msp:
    mrs %r1, msp    /* Main stack pointer. (Called from system code.) */
sd_sp_exit:

    /* SVCs have a number, used to decode them.
     * The SVC no. is 0xff & *(((uint16_t*)sp[6]) - 1)
     * Recover the SVC's no. */
    ldr %r0, [%r1, #0x18]   /* Get the address after the SVC from stack. */
    subs %r0, %r0, #2       /* Get the preceding instruction, the SVC. */
    ldrh %r0, [%r0]
    lsls %r0, %r0, #24      /* Discard the upper 24 bits. */
    lsrs r0, %r0, #24
    /* SVC no. is now in r0. */

    /* sp, in r1, is the stack pointer and has other parameters.
     * sp[0] = r0, sp[1] = r1, sp[2] = r2, sp[3] = r3
     * sp[4] = r12, sp[5] = r14/lr, sp[6] = address after the SVC
     * sp[6] = the PSR */

    /* Jump to C function SVC_Handler(uint32_t svc_no, uint32_t *sp). */
    ldr %r2, =SVC_Main
    bx  %r2
    /*svc #0x12   /+ Test with debugger. */
    .endfunc

    /* Stubbed SVC decoder: SVC_Main(uint32_t svc-no, uint32_t *sp); */
    .func SVC_Main
    .weak SVC_Main
    .thumb_func
SVC_Main:
    b .
    .endfunc

    .align 4

    .end    /* End of file */

