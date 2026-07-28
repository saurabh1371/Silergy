;/**************************************************************************//**
; * @file     startup_SY7T_M11_keil.s
; * @brief    CMSIS Core Device Startup File for
; *           SY7T_M11 Device
; * @version  V3.05
; * @date     4. March 2020
; *****************************************************************************/
;/* Copyright (c) 2011 - 2020 ARM LIMITED, 2020 Silergy Corp.
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

__Vectors       DCD     __initial_sp       ; Top of Stack; Loaded at reset.
                DCD     Reset_Handler      ; -15 Reset Handler; Started at reset
                DCD     NMI_Handler        ; -14 NMI Handler; No NMI hardware
                DCD     HardFault_Handler  ; -13 Hard Fault Handler
                DCD     Default_Handler    ; All default handlers are reserved
                DCD     Default_Handler    ; and unused.
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     SVC_Handler        ; -5 Supervisor exception
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     PendSV_Handler     ; -2 Supervisor interrupt
                DCD     SysTick_Handler    ; -1 Periodic interrupt

                ; External Interrupts
                DCD     UARTRX0_Handler    ; 0 UART 0 RX Handler
                DCD     UARTTX0_Handler    ; 1 UART 0 TX Handler
                DCD     UARTRX1_Handler    ; 2 UART 1 RX Handler
                DCD     UARTTX1_Handler    ; 3 UART 1 TX Handler
                DCD     UARTRX2_Handler    ; 4 UART 2 RX Handler
                DCD     UARTTX2_Handler    ; 5 UART 2 TX Handler
                DCD     PORT0_Handler      ; 6 GPIO Port 0
                DCD     PORT1_Handler      ; 7 GPIO Port 1
                DCD     TIMER0_Handler     ; 8 TIMER 0 Handler
                DCD     TIMER1_Handler     ; 9 TIMER 1 Handler
                DCD     DTIMER_Handler     ; 10 Dual timer Handler
                DCD     CE_Handler         ; 11 Computing Engine Handler
                DCD     UARTOVF0_Handler   ; 12 UART 0 Overflow Handler
                DCD     UARTOVF1_Handler   ; 13 UART 1 Overflow Handler
                DCD     UARTOVF2_Handler   ; 14 UART 2 Overflow Handler
                DCD     I2C_Handler        ; 15 I2C handler
                DCD     EXT_Handler        ; 16 GPIO Port Handler
                DCD     FUSE_Handler       ; 17 FUSE Parity Handler
                DCD     RTC_Handler        ; 18 RTC interrupts
                DCD     VSTAT_Handler      ; 19 VSTAT Handler
                DCD     SSPI_Handler       ; 20 Slave SPI Handler
                                           ; 21 - 31 Unused, Reserved
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY
                ALIGN


; Reset Handler
; It disables interrupts and sets the stack pointer so that background
; (noninterrupt) code can call it to restart the system.
; The long branches (LDR/BLX) avoid link errors if a routine is far.
_start
                EXPORT  _start                    [WEAK]
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main

                ; Why disable interrupts?
                ; So this start-up code can be called from non-interrupt code.
                CPSID   I               ; Disable interrupts
                DSB                     ; Data storage barrier
                ISB                     ; Synchronization barrier, instruction

                ; Delay until the RAM clock is full speed. 
                LDR     R0, =30
ram_wait        SUBS    R0, R0, #1
                BGE     ram_wait      

                ; Use this code's stack address even with boot code.
                LDR     R0, =__initial_sp   ;  Get the Stack address
                MOV     SP, R0              ;  Set the Main Stack Pointer(MSP)

                LDR     R0, =SystemInit ; Call set-up of hardware.
                BLX     R0

                LDR     R0, =__main     ; Call library to set-up and call main()
                BLX     R0

                ; Run library exit() if main() returns, to meet C standard.
                LDR     R0, =exit
                BX      R0              ; Branch; Doesn't return.
                ENDP                    ; End of procedure


; Default Handler. External code can override it to detect
; and respond to hard faults and spurious interrupts.
; See the hardware reference manual for the SYS_MOD_CNTL register.
SYS_MOD_CNTL    EQU     0x4000A004    	; Register address
RESET_BIT       EQU     0x80000000      ; Commands it to reset.
SV_VERSION      EQU     0x4000D038    	; Register address
TEST_BIT        EQU     0x80000000      ; Indicates test mode.

; If this handler is defined externally, the external handler replaces this.
Default_Handler PROC
                EXPORT  Default_Handler [WEAK]
; Recover quickly and thoroughly from an error by forcing a reset.
1               CPSID   I               ; Disable interrupts
                DSB                     ; Data storage barrier
                ISB                     ; Synchronization barrier.. instruction
                LDR     R1, =SV_VERSION ; Read the test mode bit.
                LDR     R1, [R1]        ; Read the register
                LDR     R0, =TEST_BIT   ; Check the test mode bit.
                ANDS    R1, R0
                BNE  	%1				; If it is set, loop. If not, reset.
                LDR     R0, =RESET_BIT  ; The bit to reset the SoC.
                LDR     R1, =SYS_MOD_CNTL ; I/O register to control the SoC.
                STR     R0, [R1]        ; Reset core: Store bit in register.
                B    	%1				; Retry; Reset by watchdog...
                ENDP                    ; End of procedure


; Handle a hard fault by recovering from the error.
; This has a separate entry point to aid debugging.
; If this handler is defined externally, the external one replaces this.
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler [WEAK]
;2               B       %2             ; Loop; For debug.
; Uses the default_handler to recover the system.
; The long branch (LDR/BX) avoids link errors if it is far.
                LDR     R0, =Default_Handler
                BX      R0              ; Branch; Doesn't return.
                ENDP                    ; End of procedure


; Handle all possible spurious interrupts by recovering from the error.
; This has a separate entry point to aid debugging.
; If any of these handlers are defined externally, they replace this.
; This is not a procedure, so the linker does not remove this code.
                EXPORT exit                       [WEAK]
                EXPORT NMI_Handler                [WEAK]
                EXPORT SVC_Handler                [WEAK]
                EXPORT PendSV_Handler             [WEAK]
                EXPORT SysTick_Handler            [WEAK]
                EXPORT UARTRX0_Handler            [WEAK]
                EXPORT UARTTX0_Handler            [WEAK]
                EXPORT UARTRX1_Handler            [WEAK]
                EXPORT UARTTX1_Handler            [WEAK]
                EXPORT UARTRX2_Handler            [WEAK]
                EXPORT UARTTX2_Handler            [WEAK]
                EXPORT PORT0_Handler	          [WEAK]
                EXPORT PORT1_Handler         	  [WEAK]
                EXPORT TIMER0_Handler             [WEAK]
                EXPORT TIMER1_Handler             [WEAK]
                EXPORT DTIMER_Handler             [WEAK]
                EXPORT CE_Handler 			      [WEAK]					
                EXPORT UARTOVF0_Handler           [WEAK]
                EXPORT UARTOVF1_Handler           [WEAK]
                EXPORT UARTOVF2_Handler           [WEAK]
                EXPORT I2C_Handler                [WEAK]
                EXPORT EXT_Handler            	  [WEAK]
                EXPORT FUSE_Handler            	  [WEAK]
                EXPORT RTC_Handler            	  [WEAK]
                EXPORT VSTAT_Handler           	  [WEAK]
                EXPORT SSPI_Handler            	  [WEAK]
exit
NMI_Handler
SVC_Handler
PendSV_Handler
SysTick_Handler
UARTRX0_Handler
UARTTX0_Handler
UARTRX1_Handler
UARTTX1_Handler
UARTRX2_Handler
UARTTX2_Handler
PORT0_Handler
PORT1_Handler
TIMER0_Handler
TIMER1_Handler
DTIMER_Handler
CE_Handler
UARTOVF0_Handler
UARTOVF1_Handler
UARTOVF2_Handler
I2C_Handler
EXT_Handler
FUSE_Handler
RTC_Handler
VSTAT_Handler
SSPI_Handler
;3               B       %3                   ; Loop; For debug.
; Uses the default_handler to recover the system.
; The long branch (LDR/BX) avoids link errors if it is far.
                LDR     R0, =Default_Handler ; Can call an external handler.
                BX      R0                   ; Branch; Doesn't return.

_Copyright
                EXPORT  _Copyright                [WEAK]
                DCB     "Copyright (c) 2020 Silergy Corp. "
                DCB     "All Rights Reserved.",0

                ALIGN
; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, = Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END     ; End of file

