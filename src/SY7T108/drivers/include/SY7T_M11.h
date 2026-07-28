
/****************************************************************************************************//**
 * @file     SY7T_M11.h
 *
 * @brief    CMSIS Cortex-M0 Peripheral Access Layer Header File for
 *           SY7T_M11 from Silergy Technology.
 *
 * @version  V1.0
 * @date     29. April 2020
 *
 * @note     Generated with SVDConv V2.87l 
 *           from CMSIS SVD File 'SY7T_M11.svd' Version 1.0,
 *
 * @par      ARM Limited (ARM) is supplying this software for use with Cortex-M
 *           processor based microcontroller, but can be equally used for other
 *           suitable processor architectures. This file can be freely distributed.
 *           Modifications to this file shall be clearly marked.
 *           
 *           THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *           OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *           MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *           ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *           CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *******************************************************************************************************/



/** @addtogroup Silergy Technology
  * @{
  */

/** @addtogroup SY7T_M11
  * @{
  */

#ifndef SY7T_M11_H
#define SY7T_M11_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
/* -------------------  Cortex-M0 Processor Exceptions Numbers  ------------------- */
  Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
  SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
  PendSV_IRQn                   =  -2,              /*!<  14  Pendable request for system service                              */
  SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
/* ---------------------  SY7T_M11 Specific Interrupt Numbers  -------------------- */
  UARTRX0_IRQn                  =   0,              /*!<   0  UART 0 Receive Interrupt                                         */
  UARTTX0_IRQn                  =   1,              /*!<   1  UART 0 Transmit Interrupt                                        */
  UARTRX1_IRQn                  =   2,              /*!<   2  UART 1 Receive Interrupt                                         */
  UARTTX1_IRQn                  =   3,              /*!<   3  UART 1 Transmit Interrupt                                        */
  UARTRX2_IRQn                  =   4,              /*!<   4  UART 2 Receive Interrupt                                         */
  UARTTX2_IRQn                  =   5,              /*!<   5  UART 2 Transmit Interrupt                                        */
  PORT0_IRQn                    =   6,              /*!<   6  PORT 0 Interrupt                                                 */
  PORT1_IRQn                    =   7,              /*!<   7  PORT 1 Interrupt                                                 */
  TIMER0_IRQn                   =   8,              /*!<   8  Timer 0 Interrupt                                                */
  TIMER1_IRQn                   =   9,              /*!<   9  Timer 1 Interrupt                                                */
  DTIMER_IRQn                   =  10,              /*!<  10  Dual Timer Interrupt                                             */
  CE_IRQn                       =  11,              /*!<  11  CE Interrupt                                                     */
  UARTOVF0_IRQn                 =  12,              /*!<  12  UART 0 Overflow Interrupt                                        */
  UARTOVF1_IRQn                 =  13,              /*!<  13  UART 1 Overflow Interrupt                                        */
  UARTOVF2_IRQn                 =  14,              /*!<  14  UART 2 Overflow Interrupt                                        */
  I2C_IRQn                      =  15,              /*!<  15  I2C Interrupt                                                    */
  EXT_IRQn                      =  16,              /*!<  16  External Interrupt                                               */
  FUSE_IRQn                     =  17,              /*!<  17  FUSE and XTAL fail Interrupt                                     */
  RTC_IRQn                      =  18,              /*!<  18  RTC alarm and temperature range Interrupt                        */
  VSTAT_IRQn                    =  19,              /*!<  19  Voltage status change Interrupt                                  */
  SSPI_IRQn                     =  20               /*!<  20  Slave SPI Interrupt                                              */
} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the Cortex-M0 Processor and Core Peripherals---------------- */
#define __CM0_REV                 0x0000            /*!< Cortex-M0 Core Revision                                               */
#define __MPU_PRESENT                  0            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               2            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm0.h"                               /*!< Cortex-M0 processor and core peripherals                              */
#include "system_SY7T_M11.h"                        /*!< SY7T_M11 System                                                       */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */


/** @addtogroup Device_Peripheral_Registers
  * @{
  */


/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif



/* ================================================================================ */
/* ================                     TIMER0                     ================ */
/* ================================================================================ */


/**
  * @brief Timer 0 (TIMER0)
  */

typedef struct {                                    /*!< TIMER0 Structure                                                      */
  
  union {
    __IO uint32_t  CNTL;                            /*!< Timer enable and control                                              */
    
    struct {
      __IO uint32_t  timer_en   :  1;               /*!< Timer Enable                                                          */
      __IO uint32_t  ext_in     :  1;               /*!< External Input as TimerEnable                                         */
      __IO uint32_t  ext_clk    :  1;               /*!< External Input as Clock                                               */
      __IO uint32_t  timer_ie   :  1;               /*!< Timer Interrupt Enable                                                */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  __IO uint32_t  VAL;                               /*!< Current Timer Counter Value                                           */
  __IO uint32_t  RELOAD;                            /*!< Counter Reload Value                                                  */
  
  union {
    __IO uint32_t  INT;                             /*!< Timer Interrupt register                                              */
    
    struct {
      __IO uint32_t  timer_i    :  1;               /*!< Timer Interrupt flag                                                  */
    } INT_b;                                        /*!< BitSize                                                               */
  };
} TIMER0_Type;


/* ================================================================================ */
/* ================                     DTIMER0                    ================ */
/* ================================================================================ */


/**
  * @brief Dual Timer 0 (DTIMER0)
  */

typedef struct {                                    /*!< DTIMER0 Structure                                                     */
  __IO uint32_t  LOAD;                              /*!< Counter load Value                                                    */
  __I  uint32_t  VAL;                               /*!< Counter Current Value                                                 */
  
  union {
    __IO uint32_t  CNTL;                            /*!< Dual Timer control                                                    */
    
    struct {
      __IO uint32_t  count_mode :  1;               /*!< Selects Counter mode                                                  */
      __IO uint32_t  timer_size :  1;               /*!< Selects 16-bit or 32-bit counter size                                 */
      __IO uint32_t  timer_pre  :  2;               /*!< Timer prescale                                                        */
           uint32_t             :  1;
      __IO uint32_t  timer_ie   :  1;               /*!< Timer interrupt enable                                                */
      __IO uint32_t  timer_mode :  1;               /*!< Timer Mode                                                            */
      __IO uint32_t  timer_en   :  1;               /*!< Timer Enable                                                          */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  __O  uint32_t  INT;                               /*!< Dual Timer interrupt clear                                            */
  
  union {
    __I  uint32_t  RSTAT;                           /*!< Dual Timer raw interrupt status                                       */
    
    struct {
      __I  uint32_t  timer_i    :  1;               /*!< Dual Timer raw interrupt status                                       */
    } RSTAT_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  STAT;                            /*!< Dual Timer mask interrupt status                                      */
    
    struct {
      __I  uint32_t  timer_i    :  1;               /*!< Dual timer Interrupt status                                           */
    } STAT_b;                                       /*!< BitSize                                                               */
  };
  __IO uint32_t  BGLOAD;                            /*!< Dual Timer Background Load Register                                   */
} DTIMER0_Type;


/* ================================================================================ */
/* ================                      UART0                     ================ */
/* ================================================================================ */


/**
  * @brief UART 0 (UART0)
  */

typedef struct {                                    /*!< UART0 Structure                                                       */
  
  union {
    __IO uint32_t  DATA;                            /*!< Receive and Transmit Data Value                                       */
    
    struct {
      __IO uint32_t  data       :  8;               /*!< Data Value                                                            */
    } DATA_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  STATE;                           /*!< UART State                                                            */
    
    struct {
      __I  uint32_t  txbf       :  1;               /*!< Tx Buffer Full                                                        */
      __I  uint32_t  rxbf       :  1;               /*!< Rx Buffer Full                                                        */
      __IO uint32_t  txov       :  1;               /*!< Tx Buffer Overrun (write 1 to clear)                                  */
      __IO uint32_t  rxov       :  1;               /*!< Rx Buffer Overrun (write 1 to clear)                                  */
    } STATE_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CNTL;                            /*!< UART Control Register                                                 */
    
    struct {
      __IO uint32_t  tx_en      :  1;               /*!< UART Tx enable                                                        */
      __IO uint32_t  rx_en      :  1;               /*!< UART Rx enable                                                        */
      __IO uint32_t  tx_ie      :  1;               /*!< Tx interrupt enable                                                   */
      __IO uint32_t  rx_ie      :  1;               /*!< RX interrupt enable                                                   */
      __IO uint32_t  txov_ie    :  1;               /*!< TX Overrun interrupt enable                                           */
      __IO uint32_t  rxov_ie    :  1;               /*!< RX Overrun interrupt enable                                           */
      __IO uint32_t  hstx       :  1;               /*!< High Speed Test Mode for TX only                                      */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INT;                             /*!< UART Interrupt Register                                               */
    
    struct {
      __IO uint32_t  tx_i       :  1;               /*!< TX Interrupt flag                                                     */
      __IO uint32_t  rx_i       :  1;               /*!< RX Interrupt flag                                                     */
      __IO uint32_t  txov_i     :  1;               /*!< TX Overrun Interrupt flag                                             */
      __IO uint32_t  rxov_i     :  1;               /*!< RX Overrun Interrupt flag                                             */
    } INT_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  BAUD;                            /*!< Baudrate Divider                                                      */
    
    struct {
      __IO uint32_t  baud_div   : 20;               /*!< Baud rate Divider                                                     */
    } BAUD_b;                                       /*!< BitSize                                                               */
  };
} UART0_Type;


/* ================================================================================ */
/* ================                       PWM                      ================ */
/* ================================================================================ */


/**
  * @brief PWM (PWM)
  */

typedef struct {                                    /*!< PWM Structure                                                         */
  
  union {
    __IO uint32_t  CNTL;                            /*!< PWM enable and control                                                */
    
    struct {
      __IO uint32_t  pwm0_en    :  1;               /*!< Enable PWM0 output                                                    */
      __IO uint32_t  pwm0_inv   :  1;               /*!< Invert PWM 0 output                                                   */
      __IO uint32_t  pwm1_en    :  1;               /*!< Enable PWM 1 output                                                   */
      __IO uint32_t  pwm1_inv   :  1;               /*!< Invert PWM 1 output                                                   */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PWM0;                            /*!< PWM 0 period and width                                                */
    
    struct {
      __IO uint32_t  width      : 16;               /*!< PWM 0 Width                                                           */
      __IO uint32_t  period     : 16;               /*!< PWM 0 Period                                                          */
    } PWM0_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PWM1;                            /*!< PWM 1 period and width                                                */
    
    struct {
      __IO uint32_t  width      : 16;               /*!< PWM 1 Width                                                           */
      __IO uint32_t  period     : 16;               /*!< PWM 1 Period                                                          */
    } PWM1_b;                                       /*!< BitSize                                                               */
  };
} PWM_Type;


/* ================================================================================ */
/* ================                       OPT                      ================ */
/* ================================================================================ */


/**
  * @brief IRDA/OPTICAL UART (OPT)
  */

typedef struct {                                    /*!< OPT Structure                                                         */
  
  union {
    __IO uint32_t  RX_CNTL;                         /*!< OPT Rx control                                                        */
    
    struct {
      __IO uint32_t  rx_en      :  1;               /*!< OPT Rx Enable                                                         */
      __IO uint32_t  rx_edge    :  1;               /*!< Input data falling/rising edge                                        */
           uint32_t             : 14;
      __IO uint32_t  low_counter: 14;               /*!< Number of clocks output low after the detected edge                   */
    } RX_CNTL_b;                                    /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED[3];
  
  union {
    __IO uint32_t  TX_CNTL;                         /*!< IRDA/Optical Tx control                                               */
    
    struct {
      __IO uint32_t  tx_en      :  1;               /*!< Optical Tx Enable output                                              */
      __IO uint32_t  tx_inv     :  1;               /*!< Invert the Optical Tx output                                          */
    } TX_CNTL_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  WP;                              /*!< Optical PWM period and width                                          */
    
    struct {
      __IO uint32_t  width      : 16;               /*!< Optical PWM Tx Width                                                  */
      __IO uint32_t  period     : 16;               /*!< Optical PWM Tx Period                                                 */
    } WP_b;                                         /*!< BitSize                                                               */
  };
} OPT_Type;


/* ================================================================================ */
/* ================                       RMT                      ================ */
/* ================================================================================ */


/**
  * @brief Remote (capacitively isolated) ADC ICs (RMT)
  */

typedef struct {                                    /*!< RMT Structure                                                         */
  
  union {
    __IO uint32_t  CNTL;                            /*!< Remote control                                                        */
    
    struct {
      __IO uint32_t  side_sel   :  5;               /*!< Select the decoded side channel data                                  */
      __IO uint32_t  rx_cmtest  :  1;               /*!< For test, allow the rd_cm signal to reach the tmux and register       */
      __IO uint32_t  rx_captest :  1;               /*!< Enable remote interface capacitor test                                */
      __I  uint32_t  rdcm       :  1;               /*!< Current value of the rdcm signal                                      */
      __IO uint32_t  xtra_10p   :  1;               /*!< Add an extra internal 10pF to the remote input pin pair               */
      __IO uint32_t  r2k_en     :  1;               /*!< Enable the internal 2Kohm load resistor on the remote input
                                                         pin pair                                                              */
      __IO uint32_t  swap       :  1;               /*!< 0:IAP is in phase, IAN out of phase; 1 reversed. Swap the input
                                                         pins                                                                  */
           uint32_t             :  5;
      __IO uint32_t  pad_en     :  1;               /*!< 1:Enable remote 0; 0:Analog ADC.                                      */
           uint32_t             :  3;
      __IO uint32_t  clk_en     :  1;               /*!< 1:Enable remote 0 clock                                               */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  SIDE_DATA;                       /*!< Side Data                                                             */
    
    struct {
      __IO uint32_t  side_dat   : 16;               /*!< Side data read in from the remote                                     */
      __I  uint32_t  side_flag  :  4;               /*!< Side data has been successfully clocked in                            */
      __IO uint32_t  parity     :  1;               /*!< Parity error on RMT 0                                                 */
    } SIDE_DATA_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FRAME;                           /*!< Frame size and mux frame error count                                  */
    
    struct {
      __IO uint32_t  size       :  9;               /*!< Remote frame size                                                     */
           uint32_t             :  3;
      __IO uint32_t  mframe_err_cnt:  3;            /*!< Mux frame error counter                                               */
    } FRAME_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ERR_CNT;                         /*!< Remote Error Counter                                                  */
    
    struct {
      __IO uint32_t  manch_err_cnt:  3;             /*!< Manchester error counter                                              */
           uint32_t             :  9;
      __IO uint32_t  sframe_err_cnt:  3;            /*!< Sync frame error counter                                              */
    } ERR_CNT_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  RAMP_REFA;                       /*!< Remote Ramp Registers                                                 */
    
    struct {
      __I  uint32_t  ramp_ref   : 11;               /*!< Current value of the remote FIR ramp                                  */
    } RAMP_REFA_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  RAMP_REFB;                       /*!< Remote Ramp Registers                                                 */
    
    struct {
      __I  uint32_t  ramp_ref   : 11;               /*!< Current value of the remote FIR ramp                                  */
    } RAMP_REFB_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  RAMP_REFC;                       /*!< Remote Ramp Registers                                                 */
    
    struct {
      __I  uint32_t  ramp_ref   : 11;               /*!< Current value of the remote FIR ramp                                  */
    } RAMP_REFC_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  RAMP_REFD;                       /*!< Remote Ramp Registers                                                 */
    
    struct {
      __I  uint32_t  ramp_ref   : 11;               /*!< Current value of the remote FIR ramp                                  */
    } RAMP_REFD_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  FLAGA;                           /*!< Remote flags                                                          */
    
    struct {
      __I  uint32_t  ram_flag   :  1;               /*!< RAM flag for interpolation algorithm                                  */
      __I  uint32_t  new_flag   :  1;               /*!< New flag for interpolation algorithm                                  */
    } FLAGA_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  FLAGB;                           /*!< Remote flags                                                          */
    
    struct {
      __I  uint32_t  ram_flag   :  1;               /*!< RAM flag for interpolation algorithm                                  */
      __I  uint32_t  new_flag   :  1;               /*!< New flag for interpolation algorithm                                  */
    } FLAGB_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  FLAGC;                           /*!< Remote flags                                                          */
    
    struct {
      __I  uint32_t  ram_flag   :  1;               /*!< RAM flag for interpolation algorithm                                  */
      __I  uint32_t  new_flag   :  1;               /*!< New flag for interpolation algorithm                                  */
    } FLAGC_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  FLAGD;                           /*!< Remote flags                                                          */
    
    struct {
      __I  uint32_t  ram_flag   :  1;               /*!< RAM flag for interpolation algorithm                                  */
      __I  uint32_t  new_flag   :  1;               /*!< New flag for interpolation algorithm                                  */
    } FLAGD_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CAP_BALA;                        /*!< Remote Capacitor balance                                              */
    
    struct {
      __IO uint32_t  cap_ap     :  5;               /*!< Capacitor trim for P pin                                              */
           uint32_t             :  3;
      __IO uint32_t  cap_an     :  5;               /*!< Capacitor trim for N pin                                              */
    } CAP_BALA_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CAP_BALB;                        /*!< Remote Capacitor balance                                              */
    
    struct {
      __IO uint32_t  cap_ap     :  5;               /*!< Capacitor trim for P pin                                              */
           uint32_t             :  3;
      __IO uint32_t  cap_an     :  5;               /*!< Capacitor trim for N pin                                              */
    } CAP_BALB_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CAP_BALC;                        /*!< Remote Capacitor balance                                              */
    
    struct {
      __IO uint32_t  cap_ap     :  5;               /*!< Capacitor trim for P pin                                              */
           uint32_t             :  3;
      __IO uint32_t  cap_an     :  5;               /*!< Capacitor trim for N pin                                              */
    } CAP_BALC_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CAP_BALD;                        /*!< Remote Capacitor balance                                              */
    
    struct {
      __IO uint32_t  cap_ap     :  5;               /*!< Capacitor trim for P pin                                              */
           uint32_t             :  3;
      __IO uint32_t  cap_an     :  5;               /*!< Capacitor trim for N pin                                              */
    } CAP_BALD_b;                                   /*!< BitSize                                                               */
  };
} RMT_Type;


/* ================================================================================ */
/* ================                       SYS                      ================ */
/* ================================================================================ */


/**
  * @brief System (SYS)
  */

typedef struct {                                    /*!< SYS Structure                                                         */
  
  union {
    __IO uint32_t  STAT;                            /*!< System Status                                                         */
    
    struct {
      __IO uint32_t  rtc_alarm_i:  1;               /*!< Interrupt flag for the RTC alarm                                      */
      __IO uint32_t  temp_r_i   :  1;               /*!< Interrupt flag for the temperature range                              */
      __IO uint32_t  vstat_i    :  1;               /*!< Interrupt flag for the voltage status                                 */
      __IO uint32_t  ffail_i    :  1;               /*!< Interrupt flag for the Fuse Fail condition                            */
      __IO uint32_t  xfail_i    :  1;               /*!< Interrupt flag for the crystal fail status                            */
           uint32_t             :  3;
      __IO uint32_t  rtc_alrm_ie:  1;               /*!< RTC alarm interrupt enable                                            */
      __IO uint32_t  temp_r_ie  :  1;               /*!< Temperature range interrupt enable                                    */
      __IO uint32_t  vstat_ie   :  1;               /*!< Voltage status interrupt enable                                       */
      __IO uint32_t  ffail_ie   :  1;               /*!< Fuse Fail interrupt enable                                            */
      __IO uint32_t  xfail_ie   :  1;               /*!< Crystal fail interrupt enable                                         */
           uint32_t             :  3;
      __I  uint32_t  pll_ok     :  1;               /*!< PLL OK flag                                                           */
      __I  uint32_t  xtal_fail  :  1;               /*!< Crystal failure detected                                              */
      __I  uint32_t  fuse_fail  :  1;               /*!< Fuse failure detected                                                 */
           uint32_t             :  1;
      __I  uint32_t  v3p3d_src_nvlct:  1;           /*!< 0:V3P3D is powered from VLCT pin.                                     */
      __I  uint32_t  v3p3d_src_nvbat:  1;           /*!< 0:V3P3D is powered from VBAT pin.                                     */
      __I  uint32_t  v3p3d_src_nvsys:  1;           /*!< 0:V3P3D is powered from V3P3SYS pin.                                  */
      __I  uint32_t  v3p3rtc_src_nvbat:  1;         /*!< 0:V3P3RTC is powered from VBAT pin.                                   */
      __I  uint32_t  v3a_nok    :  1;               /*!< 1:V3P3A is low.                                                       */
      __I  uint32_t  v3_nok     :  1;               /*!< 1:V3P3SYS is low.                                                     */
      __I  uint32_t  v3p3d_nok  :  1;               /*!< 1:V3P3D is low.                                                       */
      __I  uint32_t  vdd_nok    :  1;               /*!< 1:VDD (logic power at cap.) is too low.                               */
      __I  uint32_t  vbat_nok   :  1;               /*!< 1:VBAT pin's voltage is low.                                          */
      __I  uint32_t  vlct_nok   :  1;               /*!< 1:VLCT pin's voltage is low.                                          */
      __I  uint32_t  v3p3rtc_src_nvsys:  1;         /*!< 0:V3P3RTC is powered from V3P3SYS pin.                                */
      __I  uint32_t  v3p3rtc_src_nvrtc:  1;         /*!< 0:V3P3RTC is powered from VBAT_RTC pin.                               */
    } STAT_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MOD_CNTL;                        /*!< Power Mode Control                                                    */
    
    struct {
      __IO uint32_t  sleep      :  1;               /*!< Sleep mode                                                            */
      __IO uint32_t  lcd_only   :  1;               /*!< LCD only mode                                                         */
           uint32_t             : 29;
      __O  uint32_t  reset      :  1;               /*!< Reset                                                                 */
    } MOD_CNTL_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MPU_CLK;                         /*!< MPU Clock                                                             */
    
    struct {
      __IO uint32_t  mpu_spd    :  3;               /*!< MPU Speed                                                             */
    } MPU_CLK_b;                                    /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED[11];
  
  union {
    __IO uint32_t  PWR_MOD;                         /*!< Power Mode Control                                                    */
    
    struct {
      __IO uint32_t  temp_per   :  4;               /*!< Temperature measurement and RTC compensation period                   */
           uint32_t             :  1;
      __IO uint32_t  tper_dis   :  1;               /*!< Disable automatic temperature measurement                             */
           uint32_t             :  2;
      __IO uint32_t  brn_en     :  1;               /*!< Brown-out mode                                                        */
      __IO uint32_t  lct_en     :  1;               /*!< Line-cut power mode                                                   */
           uint32_t             :  6;
      __IO uint32_t  temp_filt  :  4;               /*!< Consecutive temperature measurements to trigger a wake event          */
    } PWR_MOD_b;                                    /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  WAKE;                            /*!< RTC Wake                                                              */
    
    struct {
      __IO uint32_t  wake_tmr   : 15;               /*!< Interval for the wake timer                                           */
           uint32_t             :  4;
      __IO uint32_t  ew_pb      :  1;               /*!< Enable wake-on Pushbutton                                             */
      __IO uint32_t  ew_wk0     :  1;               /*!< Enable wake-on wake 0                                                 */
      __IO uint32_t  ew_wk1     :  1;               /*!< Enable wake-on wake 1                                                 */
      __IO uint32_t  ew_rx0     :  1;               /*!< Enable wake-on RX0 (UART0 receive)                                    */
      __IO uint32_t  ew_tmr     :  1;               /*!< Enable wake-on timer                                                  */
      __IO uint32_t  ew_temp    :  1;               /*!< Enable wake-on temperature                                            */
           uint32_t             :  1;
      __IO uint32_t  ew_vlct    :  1;               /*!< Enable wake-on LCT power                                              */
           uint32_t             :  1;
      __IO uint32_t  temp_bat   :  1;               /*!< Temperature sensor also measures the battery                          */
      __IO uint32_t  temp_bcur  :  1;               /*!< Enable the battery current sink                                       */
      __IO uint32_t  temp_pwr   :  1;               /*!< Select the power source for the temperature measurement               */
      __IO uint32_t  temp_bsel  :  1;               /*!< Select which battery to measure                                       */
    } WAKE_b;                                       /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[4];
  
  union {
    __IO uint32_t  WAKE_SRC;                        /*!< CPU wake source identification                                        */
    
    struct {
      __IO uint32_t  rtc_wr     :  1;               /*!< RTC write enable                                                      */
      __IO uint32_t  rtc_rd     :  1;               /*!< RTC read enable                                                       */
      __I  uint32_t  rtc_busy   :  1;               /*!< RTC read or write is in progress                                      */
           uint32_t             :  5;
      __I  uint32_t  er_badvdd  :  1;               /*!< Bad Vdd was detected while CPU was in sleep                           */
      __I  uint32_t  er_oscfail :  1;               /*!< Oscillator failed while CPU was in sleep                              */
           uint32_t             :  1;
      __I  uint32_t  ws_wdof    :  1;               /*!< 1:Wake source is watchdog reset                                       */
      __I  uint32_t  ws_rstbit  :  1;               /*!< 1:Wake source is software-commanded reset                             */
      __I  uint32_t  ws_rst     :  1;               /*!< 1:Wake source is reset pin                                            */
      __I  uint32_t  ws_cstart  :  1;               /*!< 1:Wake source is cold start or battery insertion                      */
           uint32_t             :  4;
      __I  uint32_t  ws_pb      :  1;               /*!< Wake source is Pushbutton                                             */
      __I  uint32_t  ws_wk0     :  1;               /*!< Wake source is Wake 0                                                 */
      __I  uint32_t  ws_wk1     :  1;               /*!< Wake source is Wake 1                                                 */
      __I  uint32_t  ws_rx0     :  1;               /*!< Wake source is RX0 (UART0 receive)                                    */
      __I  uint32_t  ws_tmr     :  1;               /*!< 1:Wake source is the sleep timer                                      */
      __I  uint32_t  ws_temp    :  1;               /*!< 1:Wake source is temperature range                                    */
      __I  uint32_t  ws_vsys    :  1;               /*!< 1:Wake source is Vsys                                                 */
      __I  uint32_t  ws_vlct    :  1;               /*!< 1:Wake source is Vlct                                                 */
           uint32_t             :  4;
      __I  uint32_t  rtc_fail   :  1;               /*!< RTC failure occured                                                   */
    } WAKE_SRC_b;                                   /*!< BitSize                                                               */
  };
} SYS_Type;


/* ================================================================================ */
/* ================                       RTC                      ================ */
/* ================================================================================ */


/**
  * @brief Real Time Clock (RTC)
  */

typedef struct {                                    /*!< RTC Structure                                                         */
  
  union {
    __IO uint32_t  WDAY;                            /*!< RTC Weekday                                                           */
    
    struct {
      __IO uint32_t  rtc_wday   :  3;               /*!< Day of the week (1..7, 1=Sunday)                                      */
    } WDAY_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DATE;                            /*!< RTC date                                                              */
    
    struct {
      __IO uint32_t  rtc_mday   :  5;               /*!< Day of the month (1..28/29/30/31)                                     */
           uint32_t             :  3;
      __IO uint32_t  rtc_month  :  4;               /*!< Month of the year (1..12)                                             */
           uint32_t             :  4;
      __IO uint32_t  rtc_year   :  7;               /*!< Year of the century                                                   */
    } DATE_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TIME;                            /*!< RTC time                                                              */
    
    struct {
      __IO uint32_t  rtc_sec    :  6;               /*!< Second of minute (0..59)                                              */
           uint32_t             :  2;
      __IO uint32_t  rtc_min    :  6;               /*!< Minute of hour (0..59)                                                */
           uint32_t             :  2;
      __IO uint32_t  rtc_hour   :  5;               /*!< Hour of day (0..23)                                                   */
    } TIME_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  SUB;                             /*!< Subseconds                                                            */
    
    struct {
           uint32_t             : 24;
      __IO uint32_t  rtc_sub    :  8;               /*!< Subseconds of second (0..255)                                         */
    } SUB_b;                                        /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED;
  
  union {
    __IO uint32_t  ALARM;                           /*!< RTC Alarm                                                             */
    
    struct {
      __IO uint32_t  rtc_alrm_sec:  6;              /*!< Alarm seconds of minute (0..59)                                       */
           uint32_t             :  2;
      __IO uint32_t  rtc_alrm_min:  6;              /*!< Alarm minutes of hour (0..59)                                         */
           uint32_t             :  2;
      __IO uint32_t  rtc_alrm_hour:  5;             /*!< Alarm hour of day (0..23)                                             */
    } ALARM_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[4];
  
  union {
    __IO uint32_t  TC_AB;                           /*!< RTC Temperature compensation coefficient A,B                          */
    
    struct {
      __IO uint32_t  tc_b       : 16;               /*!< RTC Second order coefficient                                          */
      __IO uint32_t  tc_a       : 16;               /*!< RTC Third order coefficient                                           */
    } TC_AB_b;                                      /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  TC_CD;                           /*!< RTC temperature compensation coefficient C,D                          */
    
    struct {
      __IO uint32_t  tc_d       : 13;               /*!< Constant coefficient                                                  */
           uint32_t             :  3;
      __IO uint32_t  tc_c       : 16;               /*!< First order coefficient                                               */
    } TC_CD_b;                                      /*!< BitSize                                                               */
  };
} RTC_Type;


/* ================================================================================ */
/* ================                      TEMP                      ================ */
/* ================================================================================ */


/**
  * @brief Temperature Sensing (TEMP)
  */

typedef struct {                                    /*!< TEMP Structure                                                        */
  
  union {
    __IO uint32_t  BAT_TEMP;                        /*!< Battery and Temperature                                               */
    
    struct {
      __IO uint32_t  stemp      : 16;               /*!< Value of temperature measurement                                      */
           uint32_t             :  8;
      __IO uint32_t  bsense     :  8;               /*!< Value of battery measurement                                          */
    } BAT_TEMP_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ALARM;                           /*!< Temperature Alarm                                                     */
    
    struct {
      __IO uint32_t  temp_alarm_l: 16;              /*!< Temperature alarm low value                                           */
      __IO uint32_t  temp_alarm_h: 16;              /*!< Temperature Alarm high value                                          */
    } ALARM_b;                                      /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED[3];
  
  union {
    __IO uint32_t  CNTL;                            /*!< Temperature control                                                   */
    
    struct {
           uint32_t             :  8;
      __IO uint32_t  temp_start :  1;               /*!< Starts temperature or battery measurement                             */
           uint32_t             :  6;
      __I  uint32_t  temp_err   :  1;               /*!< Temperature error                                                     */
           uint32_t             :  8;
      __O  uint32_t  wd_clr     :  1;               /*!< Clears WatchDog                                                       */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  COMP;                            /*!< Temperature compensation test reg.                                    */
    
    struct {
      __I  uint32_t  n_max      : 16;               /*!< captured by temp. measurement.                                        */
      __I  uint32_t  rtc        : 16;               /*!< Signed result of RTC's cubic compensation equation.                   */
    } COMP_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  NBAT;                            /*!< Battery and temp. measurement test reg.                               */
    
    struct {
      __I  uint32_t  n_bat      : 16;               /*!< captured by battery measurement.                                      */
      __I  uint32_t  n_min      : 16;               /*!< captured by temperature measurement.                                  */
    } NBAT_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  XTAL;                            /*!< Battery and temp. measurement test reg.                               */
    
    struct {
      __I  uint32_t  t          : 16;               /*!< measurement time in xtal cycles.                                      */
      __I  uint32_t  na_min     : 16;               /*!< captured by temperature measurement.                                  */
    } XTAL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __I  uint32_t  AN_BAT;                          /*!< Test reg. for AN_BAT test mode.                                       */
    
    struct {
      __I  uint32_t  vco_cnt    : 16;               /*!< VCO cycles/period in AN_BAT test mode.                                */
    } AN_BAT_b;                                     /*!< BitSize                                                               */
  };
} TEMP_Type;


/* ================================================================================ */
/* ================                      NVRAM                     ================ */
/* ================================================================================ */


/**
  * @brief Non-volatile RAM (NVRAM)
  */

typedef struct {                                    /*!< NVRAM Structure                                                       */
  
  union {
    __IO uint32_t  ADDR;                            /*!< NVRAM Address                                                         */
    
    struct {
      __IO uint32_t  nvram_ad   :  5;               /*!< NVRAM address                                                         */
    } ADDR_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DATA;                            /*!< NVRAM Data                                                            */
    
    struct {
      __IO uint32_t  nvram_data : 32;               /*!< NVRAM Data                                                            */
    } DATA_b;                                       /*!< BitSize                                                               */
  };
} NVRAM_Type;


/* ================================================================================ */
/* ================                       LCD                      ================ */
/* ================================================================================ */


/**
  * @brief Liquid Crystal Display (LCD) on selected pins (LCD)
  */

typedef struct {                                    /*!< LCD Structure                                                         */
  
  union {
    __IO uint32_t  MODE;                            /*!< LCD Mode                                                              */
    
    struct {
      __IO uint32_t  on         :  1;               /*!< Test the LCD: All on                                                  */
      __IO uint32_t  blank      :  1;               /*!< Test the LCD: All off                                                 */
      __IO uint32_t  rst        :  1;               /*!< 1:Clear the LCD data                                                  */
      __IO uint32_t  lowf       :  1;               /*!< Sets low frequency/power LCD waveforms                                */
      __IO uint32_t  test       :  1;               /*!< LCD static test modes                                                 */
      __IO uint32_t  vmode      :  2;               /*!< Specifies how VLCD is generated                                       */
           uint32_t             :  1;
      __IO uint32_t  blinkA     :  8;               /*!< Selects segments to blink (SEG22)                                     */
      __IO uint32_t  blinkB     :  8;               /*!< Selects segments to blink (SEG23)                                     */
      __IO uint32_t  clk        :  2;               /*!< Sets the LCD clock frequency                                          */
      __IO uint32_t  y          :  1;               /*!< The blink rate                                                        */
      __IO uint32_t  all_com    :  1;               /*!< Shared COM pins are all COM                                           */
      __IO uint32_t  mode       :  3;               /*!< LCD states and bias levels.                                           */
      __IO uint32_t  en         :  1;               /*!< Enables LCD voltages and clock                                        */
    } MODE_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CNTL;                            /*!< LCD Control                                                           */
    
    struct {
      __IO uint32_t  dac        :  5;               /*!< LCD contrast-control DAC                                              */
           uint32_t             :  5;
      __IO uint32_t  page_dma_en:  1;               /*!< Enable page DMA                                                       */
           uint32_t             :  3;
      __IO uint32_t  test       :  1;               /*!< LCD-only factory test bit                                             */
      __IO uint32_t  short_qsec :  1;               /*!< Factory RTC test speed-up                                             */
      __IO uint32_t  page_addr  : 11;               /*!< Address of first DMA LCD image in RAM.                                */
           uint32_t             :  1;
      __IO uint32_t  num_pages  :  4;               /*!< Number of DMA LCD pages to display                                    */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED;
  
  union {
    __IO uint32_t  MAPL;                            /*!< LCD MAP, SEG/DIO pins 31..0 (1=LCD,0=DIO)                             */
    
    struct {
      __IO uint32_t  lcd_map    : 32;               /*!< Control LCD/DIO pin function                                          */
    } MAPL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  MAPH;                            /*!< LCD MAP High                                                          */
    
    struct {
      __IO uint32_t  lcd_map    :  8;               /*!< Control LCD/DIO pin function                                          */
    } MAPH_b;                                       /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED1[2];
  
  union {
    __IO uint32_t  DMA_PTIME;                       /*!< LCD DMA Page time control                                             */
    
    struct {
      __IO uint32_t  scroll     :  7;               /*!< Secs x 4 until the next page displays.                                */
           uint32_t             :  1;
      __IO uint32_t  init       :  7;               /*!< Secs x 4 until the first page displays after DMA Enable.              */
    } DMA_PTIME_b;                                  /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED2[120];
  
  union {
    __IO uint32_t  DIO[45];                         /*!< Control a SEG/DIO pin                                                 */
    
    struct {
      __IO uint32_t  data       :  1;               /*!< pin value in DIO mode                                                 */
      __IO uint32_t  dir        :  1;               /*!< pin direction in DIO mode                                             */
      __IO uint32_t  alt        :  1;               /*!< Alternate device (not DIO) or level interrupt if enabled              */
      __IO uint32_t  flag       :  1;               /*!< Interrupt flag                                                        */
      __IO uint32_t  pol        :  1;               /*!< Interrupt polarity                                                    */
      __IO uint32_t  mask       :  1;               /*!< Interrupt enable                                                      */
      __IO uint32_t  pull_up    :  1;               /*!< Pull up enable (down if pol=0 and mask=0)                             */
    } DIO_b[45];                                    /*!< BitSize                                                               */
  };
} LCD_Type;


/* ================================================================================ */
/* ================                      TMUX                      ================ */
/* ================================================================================ */


/**
  * @brief Test Multiplexer (TMUX)
  */

typedef struct {                                    /*!< TMUX Structure                                                        */
  
  union {
    __IO uint32_t  TMUXSEL;                         /*!< Test multiplexer control register                                     */
    
    struct {
      __IO uint32_t  tmux1_sel  :  9;               /*!< Test Multiplexer Output 1 Select                                      */
           uint32_t             :  7;
      __IO uint32_t  tmux0_sel  :  9;               /*!< Test Multiplexer Output Select                                        */
    } TMUXSEL_b;                                    /*!< BitSize                                                               */
  };
} TMUX_Type;


/* ================================================================================ */
/* ================                      SSPI                      ================ */
/* ================================================================================ */


/**
  * @brief Slave SPI (SSPI)
  */

typedef struct {                                    /*!< SSPI Structure                                                        */
  
  union {
    __IO uint32_t  CNTL;                            /*!< Slave SPI Control                                                     */
    
    struct {
      __IO uint32_t  sspi_i     :  1;               /*!< Slave SPI interrupt flag                                              */
      __I  uint32_t  sspi_stat  :  7;               /*!< SPI Status                                                            */
      __IO uint32_t  sspi_ie    :  1;               /*!< Slave SPI interrupt Enable                                            */
      __IO uint32_t  sspi_en    :  1;               /*!< Slave SPI Port Enable                                                 */
      __IO uint32_t  sspi_safe  :  1;               /*!< Limits SPI writes to SPI_CMD specific region in SRAM                  */
           uint32_t             :  5;
      __IO uint32_t  cmd        :  8;               /*!< The latest SPI command from a master.                                 */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
} SSPI_Type;


/* ================================================================================ */
/* ================                       CE                       ================ */
/* ================================================================================ */


/**
  * @brief Compute Engine (CE)
  */

typedef struct {                                    /*!< CE Structure                                                          */
  
  union {
    __IO uint32_t  RTM0;                            /*!< Real-Time Monitor 0                                                   */
    
    struct {
           uint32_t             :  2;
      __IO uint32_t  addr       : 11;               /*!< RTM address                                                           */
    } RTM0_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  RTM1;                            /*!< Real-Time Monitor 1                                                   */
    
    struct {
           uint32_t             :  2;
      __IO uint32_t  addr       : 11;               /*!< RTM address                                                           */
    } RTM1_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  RTM2;                            /*!< Real-Time Monitor 2                                                   */
    
    struct {
           uint32_t             :  2;
      __IO uint32_t  addr       : 11;               /*!< RTM address                                                           */
    } RTM2_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  RTM3;                            /*!< Real-Time Monitor 3                                                   */
    
    struct {
           uint32_t             :  2;
      __IO uint32_t  addr       : 11;               /*!< RTM address                                                           */
    } RTM3_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PLS_CNTL;                        /*!< Pulse timing control                                                  */
    
    struct {
      __IO uint32_t  pls_interval:  9;              /*!< Pulse interval                                                        */
           uint32_t             :  7;
      __IO uint32_t  pls_wid_max:  8;               /*!< Maximum pulse width                                                   */
    } PLS_CNTL_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CNTL;                            /*!< Meter and CE control                                                  */
    
    struct {
      __IO uint32_t  sum_samps  : 13;               /*!< Number of samples in XFER(Sum Cycle)                                  */
           uint32_t             :  3;
      __IO uint32_t  ce_en      :  1;               /*!< CE clock enable                                                       */
      __IO uint32_t  rtm_en     :  1;               /*!< RTM clock enable                                                      */
      __IO uint32_t  chop_en    :  2;               /*!< Chop mode                                                             */
      __IO uint32_t  fchop_en   :  1;               /*!< Fast chop enable                                                      */
      __IO uint32_t  prechop_en :  1;               /*!< Preamp chop enable                                                    */
      __IO uint32_t  bgchop_en  :  1;               /*!< Band-gap chop enable                                                  */
           uint32_t             :  1;
      __IO uint32_t  equ        :  3;               /*!< CE Equation                                                           */
           uint32_t             :  1;
      __IO uint32_t  invert_pls :  1;               /*!< Invert the CE pulse                                                   */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INT_CNTL;                        /*!< Meter interrupt control                                               */
    
    struct {
      __IO uint32_t  ce_busy_i  :  1;               /*!< CE Busy interrupt flag                                                */
      __IO uint32_t  xfer_n_i   :  1;               /*!< XFER interrupt flag                                                   */
      __IO uint32_t  vpls_i     :  1;               /*!< V Pulse interrupt flag                                                */
      __IO uint32_t  wpls_i     :  1;               /*!< W Pulse interrupt flag                                                */
      __IO uint32_t  xpls_i     :  1;               /*!< X Pulse interrupt flag                                                */
      __IO uint32_t  ypls_i     :  1;               /*!< Y Pulse interrupt flag                                                */
      __IO uint32_t  fir_done_i :  1;               /*!< FIR done interrupt flag                                               */
      __IO uint32_t  mux_sync_i :  1;               /*!< Mux sync interrupt flag                                               */
      __IO uint32_t  ce_busy_ie :  1;               /*!< CE Busy interrupt enable                                              */
      __IO uint32_t  xfer_n_ie  :  1;               /*!< XFER interrupt enable                                                 */
      __IO uint32_t  vpls_ie    :  1;               /*!< V Pulse interrupt enable                                              */
      __IO uint32_t  wpls_ie    :  1;               /*!< W Pulse interrupt enable                                              */
      __IO uint32_t  xpls_ie    :  1;               /*!< X Pulse interrupt enable                                              */
      __IO uint32_t  ypls_ie    :  1;               /*!< Y Pulse interrupt enable                                              */
      __IO uint32_t  fir_done_ie:  1;               /*!< FIR Done interrupt enable                                             */
      __IO uint32_t  mux_sync_ie:  1;               /*!< Mux sync interrupt enable                                             */
    } INT_CNTL_b;                                   /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED[6];
  __IO uint32_t  LCTN;                              /*!< CE Location                                                           */
} CE_Type;


/* ================================================================================ */
/* ================                       ADC                      ================ */
/* ================================================================================ */


/**
  * @brief ADC(Analog-to-digital Converter) (ADC)
  */

typedef struct {                                    /*!< ADC Structure                                                         */
  
  union {
    __IO uint32_t  VREF_CNTL;                       /*!< ADC voltage reference control                                         */
    
    struct {
      __IO uint32_t  adcs_en    :  1;               /*!< ADCS (Staring) clock enable                                           */
      __IO uint32_t  adcm_en    :  1;               /*!< ADCM (Multiplexed) clock enable                                       */
           uint32_t             :  6;
      __IO uint32_t  pre_en     :  1;               /*!< Preamp enable                                                         */
           uint32_t             :  7;
      __IO uint32_t  vref_en    :  1;               /*!< Voltage reference enable                                              */
           uint32_t             :  7;
      __IO uint32_t  fir_str    :  6;               /*!< F.I.R. length stretch for multiplexer slot 0                          */
    } VREF_CNTL_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  CNTL;                            /*!< ADC control                                                           */
    
    struct {
      __IO uint32_t  adcs_spd   :  2;               /*!< Staring ADC clock speed                                               */
      __IO uint32_t  adcm_spd   :  2;               /*!< Multiplexed ADC clock speed                                           */
      __IO uint32_t  mux_div    :  2;               /*!< Number of slots of each mux_frame.                                    */
           uint32_t             :  2;
      __IO uint32_t  adcs_bias  :  4;               /*!< ADCS (Staring) bias                                                   */
      __IO uint32_t  adcm_bias  :  4;               /*!< ADCM (Multiplex) bias                                                 */
      __IO uint32_t  low_bias   :  1;               /*!< Low bias mode                                                         */
           uint32_t             :  7;
      __IO uint32_t  adc_vls    :  1;               /*!< ADC very low speed, PLL/13                                            */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  SLOT_CNTL;                       /*!< ADC Slot control                                                      */
    
    struct {
      __IO uint32_t  slot0_sel  :  2;               /*!< Select ADC1 input for Slot0                                           */
           uint32_t             :  2;
      __IO uint32_t  slot1_sel  :  2;               /*!< Select ADC1 input for Slot1                                           */
    } SLOT_CNTL_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  FIR_LEN;                         /*!< ADC FIR Length                                                        */
    
    struct {
      __IO uint32_t  fir0_len   :  4;               /*!< Slot 0 F.I.R. length                                                  */
      __IO uint32_t  fir1_len   :  4;               /*!< Slot 1 F.I.R. length                                                  */
    } FIR_LEN_b;                                    /*!< BitSize                                                               */
  };
} ADC_Type;


/* ================================================================================ */
/* ================                      FLASH                     ================ */
/* ================================================================================ */


/**
  * @brief Flash (FLASH)
  */

typedef struct {                                    /*!< FLASH Structure                                                       */
  
  union {
    __IO uint32_t  CNTL;                            /*!< Flash Control                                                         */
    
    struct {
      __IO uint32_t  cmd        :  3;               /*!< Control for Flash Operation                                           */
      __IO uint32_t  refresh    :  1;               /*!< Fuse refresh                                                          */
      __I  uint32_t  trim_fsize :  1;               /*!< Flash size trim value                                                 */
      __I  uint32_t  out_of_range:  1;              /*!< Address is out of range                                               */
           uint32_t             :  1;
      __I  uint32_t  flash_idle :  1;               /*!< Flash idle flag                                                       */
      __IO uint32_t  code       : 24;               /*!< Code to enable writes to Flash controller                             */
    } CNTL_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ADDR;                            /*!< Flash Address                                                         */
    
    struct {
      __IO uint32_t  faddr      : 32;               /*!< Flash address                                                         */
    } ADDR_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DATA;                            /*!< Flash Data                                                            */
    
    struct {
      __IO uint32_t  fdata      : 32;               /*!< Flash Data                                                            */
    } DATA_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  PROT;                            /*!< Flash Protection                                                      */
    
    struct {
      __IO uint32_t  secure     :  1;               /*!< Lock out flash reads by SSPI                                          */
      __IO uint32_t  dbg_lock   :  1;               /*!< Lock out debugger                                                     */
           uint32_t             :  6;
      __IO uint32_t  unlock     :  1;               /*!< Allow Flash write and erase                                           */
           uint32_t             : 22;
      __I  uint32_t  preboot    :  1;               /*!< Preboot indicator                                                     */
    } PROT_b;                                       /*!< BitSize                                                               */
  };
} FLASH_Type;


/* ================================================================================ */
/* ================                       SSI                      ================ */
/* ================================================================================ */


/**
  * @brief Synchronous Serial Interface (SSI)
  */

typedef struct {                                    /*!< SSI Structure                                                         */
  
  union {
    union {
      __IO uint32_t  MSPI_CNTL;                     /*!< Master SPI Control                                                    */
      
      struct {
        __IO uint32_t  cnt      :  4;               /*!< Number of clocks to be issued                                         */
        __IO uint32_t  rd       :  1;               /*!< I2C read operation                                                    */
        __IO uint32_t  hiz      :  1;               /*!< Data bus floated (Hi-impedance) after the last clk                    */
        __I  uint32_t  busy     :  1;               /*!< Data bus is busy                                                      */
        __IO uint32_t  wfr      :  1;               /*!< Wait for ready                                                        */
      } MSPI_CNTL_b;                                /*!< BitSize                                                               */
    };
    
    union {
      __IO uint32_t  I2C_CNTL;                      /*!< Master I2C Control                                                    */
      
      struct {
        __IO uint32_t  cmd      :  4;               /*!< Command                                                               */
        __I  uint32_t  tx_ack   :  1;               /*!< Ack bit has been sent to device.                                      */
        __I  uint32_t  rx_ack   :  1;               /*!< Device sent an Ack.                                                   */
        __I  uint32_t  busy     :  1;               /*!< Data bus is busy                                                      */
        __I  uint32_t  err      :  1;               /*!< Error conditon due to illegal command                                 */
      } I2C_CNTL_b;                                 /*!< BitSize                                                               */
    };
  };
  
  union {
    __IO uint32_t  DATA;                            /*!< SSI Data                                                              */
    
    struct {
      __IO uint32_t  ee_data    :  8;               /*!< Device/EEPROM data                                                    */
    } DATA_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  EEX;                             /*!< Synchronous Serial I/O External EEPROM interface                      */
    
    struct {
      __IO uint32_t  dio_eex    :  2;               /*!< DIO interface                                                         */
    } EEX_b;                                        /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INT;                             /*!< SSI Interrupt                                                         */
    
    struct {
      __IO uint32_t  ssi_i      :  1;               /*!< SSI interrupt Flag                                                    */
           uint32_t             :  3;
      __IO uint32_t  ssi_ie     :  1;               /*!< SSI Interrupt Enable                                                  */
    } INT_b;                                        /*!< BitSize                                                               */
  };
} SSI_Type;


/* ================================================================================ */
/* ================                      PORT0                     ================ */
/* ================================================================================ */


/**
  * @brief GPIO PORT0 AHB (PORT0)
  */

typedef struct {                                    /*!< PORT0 Structure                                                       */
  
  union {
    __IO uint32_t  DATA;                            /*!< GPIO data pin value                                                   */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< GPIO data pin value                                                   */
    } DATA_b;                                       /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  DATAOUT;                         /*!< GPIO data output value                                                */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< GPIO data output value                                                */
    } DATAOUT_b;                                    /*!< BitSize                                                               */
  };
  __I  uint32_t  RESERVED[2];
  
  union {
    __IO uint32_t  OUTENSET;                        /*!< Set GPIO data output enable                                           */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Set GPIO data output enable                                           */
    } OUTENSET_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  OUTENCLR;                        /*!< Clear GPIO data output enable                                         */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Clear GPIO data output enable                                         */
    } OUTENCLR_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ALTFUNCSET;                      /*!< Set GPIO alternative function                                         */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Set GPIO alternative function                                         */
    } ALTFUNCSET_b;                                 /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  ALTFUNCCLR;                      /*!< Clear GPIO alternative function                                       */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Clear GPIO alternative function                                       */
    } ALTFUNCCLR_b;                                 /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INTENSET;                        /*!< Set Interrupt enable                                                  */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Set Interrupt enable                                                  */
    } INTENSET_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INTENCLR;                        /*!< Clear Interrupt enable                                                */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Clear Interrupt enable                                                */
    } INTENCLR_b;                                   /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INTTYPESET;                      /*!< Set Interrupt type                                                    */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Set Interrupt type                                                    */
    } INTTYPESET_b;                                 /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INTTYPECLR;                      /*!< Clear Interrupt type                                                  */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Clear Interrupt type                                                  */
    } INTTYPECLR_b;                                 /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INTPOLSET;                       /*!< Set Interrupt Polarity-level, edge configuration                      */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Set Interrupt Polarity-level, edge configuration                      */
    } INTPOLSET_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INTPOLCLR;                       /*!< Clear Interrupt Polarity-level, edge configuration                    */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Clear Interrupt Polarity-level, edge configuration                    */
    } INTPOLCLR_b;                                  /*!< BitSize                                                               */
  };
  
  union {
    __IO uint32_t  INTSTATUS;                       /*!< Interrupt status and clear                                            */
    
    struct {
      __IO uint32_t  value      : 16;               /*!< Interrupt status and clear                                            */
    } INTSTATUS_b;                                  /*!< BitSize                                                               */
  };
} PORT0_Type;


/* ================================================================================ */
/* ================                       SV                       ================ */
/* ================================================================================ */


/**
  * @brief Silicon Version (SV)
  */

typedef struct {                                    /*!< SV Structure                                                          */
  
  union {
    __I  uint32_t  VERSION;                         /*!< Silicon Version                                                       */
    
    struct {
      __I  uint32_t  minor      :  4;               /*!< Minor (numeric) field of silicon revision.                            */
      __I  uint32_t  major      :  4;               /*!< Major (letter) field of silicon revision.                             */
           uint32_t             : 23;
      __I  uint32_t  test       :  1;               /*!< Status of TEST_E pin.                                                 */
    } VERSION_b;                                    /*!< BitSize                                                               */
  };
} SV_Type;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler type
#endif




/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define TIMER0_BASE                     0x40000000UL
#define TIMER1_BASE                     0x40001000UL
#define DTIMER0_BASE                    0x40002000UL
#define DTIMER1_BASE                    0x40002020UL
#define UART0_BASE                      0x40004000UL
#define UART1_BASE                      0x40005000UL
#define UART2_BASE                      0x40006000UL
#define PWM_BASE                        0x40007000UL
#define OPT_BASE                        0x40007020UL
#define RMT_BASE                        0x40009000UL
#define SYS_BASE                        0x4000A000UL
#define RTC_BASE                        0x4000A020UL
#define TEMP_BASE                       0x4000A040UL
#define NVRAM_BASE                      0x4000A070UL
#define LCD_BASE                        0x4000C000UL
#define TMUX_BASE                       0x4000C008UL
#define SSPI_BASE                       0x4000C014UL
#define CE_BASE                         0x4000D000UL
#define ADC_BASE                        0x4000D020UL
#define FLASH_BASE                      0x4000E000UL
#define SSI_BASE                        0x4000F000UL
#define PORT0_BASE                      0x40010000UL
#define PORT1_BASE                      0x40011000UL
#define SV_BASE                         0x4000D038UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define TIMER0                          ((TIMER0_Type             *) TIMER0_BASE)
#define TIMER1                          ((TIMER0_Type             *) TIMER1_BASE)
#define DTIMER0                         ((DTIMER0_Type            *) DTIMER0_BASE)
#define DTIMER1                         ((DTIMER0_Type            *) DTIMER1_BASE)
#define UART0                           ((UART0_Type              *) UART0_BASE)
#define UART1                           ((UART0_Type              *) UART1_BASE)
#define UART2                           ((UART0_Type              *) UART2_BASE)
#define PWM                             ((PWM_Type                *) PWM_BASE)
#define OPT                             ((OPT_Type                *) OPT_BASE)
#define RMT                             ((RMT_Type                *) RMT_BASE)
#define SYS                             ((SYS_Type                *) SYS_BASE)
#define RTC                             ((RTC_Type                *) RTC_BASE)
#define TEMP                            ((TEMP_Type               *) TEMP_BASE)
#define NVRAM                           ((NVRAM_Type              *) NVRAM_BASE)
#define LCD                             ((LCD_Type                *) LCD_BASE)
#define TMUX                            ((TMUX_Type               *) TMUX_BASE)
#define SSPI                            ((SSPI_Type               *) SSPI_BASE)
#define CE                              ((CE_Type                 *) CE_BASE)
#define ADC                             ((ADC_Type                *) ADC_BASE)
#define FLASH                           ((FLASH_Type              *) FLASH_BASE)
#define SSI                             ((SSI_Type                *) SSI_BASE)
#define PORT0                           ((PORT0_Type              *) PORT0_BASE)
#define PORT1                           ((PORT0_Type              *) PORT1_BASE)
#define SV                              ((SV_Type                 *) SV_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group SY7T_M11 */
/** @} */ /* End of group Silergy Technology */

#ifdef __cplusplus
}
#endif


#endif  /* SY7T_M11_H */

