;/**************************************************************************//**
; * @file     startup_SY7T_M11_boot_keil.s
; * @brief    CMSIS Core Device Startup File for
; *           SY7T211E Device
; * @version  V3.05
; * @date     09. November 2016
; *****************************************************************************/
;/* Copyright (c) 2011 - 2016 ARM LIMITED
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
;   --------------------------------------------------------------------------*/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> -------------------
;*/

;APP_START_ADDR EQU 0x00000600 ; APP START address

	
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000100

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp        ; Top of Stack
                DCD     Reset_Handler       ; Reset Handler
                DCD     _08_Handler         ; NMI Handler
                DCD     _0C_Handler         ; Hard Fault Handler
                DCD     _10_Handler         ; Reserved
                DCD     _14_Handler         ; Reserved
                DCD     _18_Handler         ; Reserved
                DCD     _1C_Handler         ; Reserved
                DCD     _20_Handler         ; Reserved
                DCD     _24_Handler         ; Reserved
                DCD     _28_Handler         ; Reserved
                DCD     _2C_Handler         ; SVCall Handler
                DCD     _30_Handler         ; Reserved
                DCD     _34_Handler         ; Reserved
                DCD     _38_Handler         ; PendSV Handler
                DCD     _3C_Handler         ; SysTick Handler

                ; External Interrupts
                DCD     _40_Handler         ; 0 UART 0 RX Handler
                DCD     _44_Handler         ; 1 UART 0 TX Handler
                DCD     _48_Handler         ; 2 UART 1 RX Handler
                DCD     _4C_Handler         ; 3 UART 1 TX Handler
                DCD     _50_Handler         ; 4 UART 2 RX Handler
                DCD     _54_Handler         ; 5 UART 2 TX Handler
                DCD     _58_Handler         ; 6 GPIO Port 0 Combined Handler
                DCD     _5C_Handler         ; 7 GPIO Port 1 Combined Handler
                DCD     _60_Handler         ; 8 TIMER 0 Handler
                DCD     _64_Handler         ; 9 TIMER 1 Handler
                DCD     _68_Handler         ; 10 Dual timer Handler
                DCD     _6C_Handler         ; 11 Computing Engine Handler (SY7T_M11)
                DCD     _70_Handler         ; 12 UART 0 Overflow Handler
                DCD     _74_Handler         ; 13 UART 1 Overflow Handler
                DCD     _78_Handler         ; 14 UART 2 Overflow Handler
                DCD     _7C_Handler         ; 15 I2C handler (AMll)
                DCD     _80_Handler         ; 16 GPIO Port Handler
                DCD     _84_Handler         ; 17 FUSE Parity Handler (SY7T_M11)
                DCD     _88_Handler         ; 18 RTC interrupts (SY7T_M11)
                DCD     _8C_Handler         ; 19 VSTAT Handler (SY7T_M11)
                DCD     _90_Handler         ; 20 Slave SPIHandler (SY7T_M11)
                                            ; 21 - 31 Unused (SY7T_M11)
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

                ALIGN

; Reset Handler
_start
Reset_Handler   PROC
                EXPORT  _start                    [WEAK]
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  boot
                IMPORT  app_start_addr
                ; Delay until the RAM clock is full speed. (Bugzilla 2765)
                LDR     R0, =30
ram_wait        SUBS    R0, R0, #1
                BGE     ram_wait      
                LDR    R3, =boot      ; Call main() of bootloader
                BLX    R3
Start_App
                EXPORT  Start_App                 [WEAK]
                LDR    R3, =app_start_addr
                LDR    R2, [R3]
                LDR    R3, [R2]
                MOV    SP, R3
                LDR    R2, [R2, #4]
                LDR    R3, =0xffffffff
                CMP    R2, R3
                BEQ    _start         ; If erased, try again.
                BLX    R2
                B      _start         ; If it returns, restart.
                ENDP


; Pass-through exception Handlers.
_08_Handler
_0C_Handler
_10_Handler
_14_Handler
_18_Handler
_1C_Handler
_20_Handler
_24_Handler
_28_Handler
_2C_Handler
_30_Handler
_34_Handler
_38_Handler
_3C_Handler
_40_Handler
_44_Handler
_48_Handler
_4C_Handler
_50_Handler
_54_Handler
_58_Handler
_5C_Handler
_60_Handler
_64_Handler
_68_Handler
_6C_Handler
_70_Handler
_74_Handler
_78_Handler
_7C_Handler
_80_Handler
_84_Handler
_88_Handler
_8C_Handler
_90_Handler
; R3 is saved automatically by the CPU; Leave R0-R2, LR unchanged. 
        MRS  R3, IPSR       ; Get the vector number 
        LSLS R3, R3, #2     ; Multiply by 4
        LDR  R2, =app_start_addr    ; application base address, if no App then it is 0x0000
        LDR  R2, [R2]       ; Read value located at R2 to R2
        ADDS R3, R3, R2     ; R3 = R3 + R2 (vector addr in app)
        LDR  R3, [R3]       ; interrupt handler located at R3
        BX   R3             ; Run the vector; LR is unchanged
			
        ALIGN


; User Initial Stack & Heap

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ALIGN


                END
