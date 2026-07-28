;/**************************************************************************//**
; * @file     SVC_Handler.s
; * @brief    Decode a service call exception
; * @version  V3.05
; * @date     20. March 2019
; ******************************************************************************/
;/* Copyright (c) 2019 Silergy Corp.
;
;   All rights reserved.
;   Redistribution and use in source and binary forms, with or without
;   modification, are permitted provided that the following conditions are met:
;   - Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;   - Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;   - Neither the name of ARM nor the names of its contributors may be used
;     to endorse or promote products derived from this software without
;     specific prior written permission.
;   *
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
;   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;   POSSIBILITY OF SUCH DAMAGE.
;   ---------------------------------------------------------------------------*/
                AREA    |.text|, CODE, READONLY
; Decode a service call exception.
SVC_Handler     PROC
                EXPORT SVC_Handler
                ; Find the correct stack pointer.
                MOVS r1, #4
                MOV r0, lr
                TST r0, r1      ; Which stack pointer? (0=main stack pointer)
                BEQ sd_sp_msp
                MRS r1, PSP     ; Process stack pointer.
                B sd_sp_exit
sd_sp_msp
                MRS r1, MSP     ; Main stack pointer.
sd_sp_exit
                ; SVCs have a number, used to decode them.
                ; The SVC no. is 0xff & *(((uint16_t*)sp[6]) - 1)
                ; Recover the SVC's no.
                LDR r0,[r1, #0x18]  ; Get the address after the SVC from stack.
                SUBS r0,r0,#2       ; Get the preceding instruction, the SVC.
                LDRH r0,[r0]    
                LSLS r0,r0,#24      ; Discard the upper 24 bits.
                LSRS r0,r0,#24       
                ; SVC no. is now in r0.  
                
                ; sp, in r1, is the stack pointer and has other parameters.
                ; sp[0] = r0, sp[1] = r1, sp[2] = r2, sp[3] = r3
                ; sp[4] = r12, sp[5] = r14/lr, sp[6] = address after the SVC
                ; sp[6] = the PSR
                
                ; Jump to C function:
                ; SVC_Main(uint32_t svc-no, uint32_t *sp);
                LDR r2, =SVC_Main
                BX r2
                ;SVC #0x12  ; Test with debugger.
                ENDP


; Stubbed SVC decoder: SVC_Main(uint32_t svc-no, uint32_t *sp);
SVC_Main        PROC
                EXPORT SVC_Main [WEAK]
                B .
                ENDP

                END     ; End of file

