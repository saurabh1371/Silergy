/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: System and clock initialization.
 *
 * Copyright 2017, Silergy Corp.   All rights reserved.
 * $Id: system_SY7T_M11.c 24401 2020-04-14 04:30:50Z rvandewa $
 ****************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SY7_device.h"
#include "irq.h"
#include "flash.h"
#include "delay.h"

#if 0 // Testing preprocessor labels for compiler-dependent code
#if __ARMCC_VERSION // Keil only
#warning ARMCC switch enabled.
#endif
#if __arm__ // Keil and GCC
#warning __arm__ switch enabled.
#endif
#if __GNUC__ // GCC only
#warning __GNUC__ switch enabled.
#endif
#endif

#define RTC_ALARM_CALLBACK_IDX 0
#define TEMP_RANGE_CALLBACK_IDX 1
#define VSTAT_CALLBACK_IDX 2
#define FUSE_FAIL_CALLBACK_IDX 3
#define CRYSTAL_FAIL_CALLBACK_IDX 4

/*----------------------------------------------------------------------------
  System Core Clock Variables
 *----------------------------------------------------------------------------*/
/* System Clock Frequency (Core Clock)*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;
/* Clock's Divisor from fastest possible frequency */
uint32_t SystemCoreClockDivisor = 1;


/*----------------------------------------------------------------------------
  System interrupt handler callback array.
 *----------------------------------------------------------------------------*/
void (*sys_handler_ary[5])(void);   // Addresses of system callbacks.


/****************************************************************************
* Description:
*       Initialize ST7xxx core clock variables, devices and interrupts.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
******************************************************************************/
void SystemInit (void)
{
    int i;
    unsigned int ui;

    // Wait for the clock's PLL to stabilize.
    // This is absolutely required before the write to the flash controller,
    // below.
    while(0 == SYS->STAT_b.pll_ok)
        ;

    FLASH->CNTL = 0;                    // Clear any pass code.

    // Power modes switch at more accurate voltages if the Vref is on.
    ADC->VREF_CNTL_b.vref_en = 1; // Enable the voltage reference.

    // Switch the temperature measurement to mains.
    sys_shadow_busy_wait();             // Wait till the RTC is free

    SYS->WAKE_SRC = SHADOW_UNLOCK;      // Enable writing.

    // Disable the RTC's alarm.
    RTC->ALARM = 0x1f3f3f;      // Set to 31:63:63, an impossible time.

    // Disable the temperature range detection.
    TEMP->ALARM = 0x7fff8000;   // Set to widest possible temp. range.

    // The RTC's compensation polynomial is recalculated when a new temperature
    // measurement occurs.
    SYS->WAKE_b.temp_pwr = 1;   // Temp. measured using mains power.
    SYS->PWR_MOD_b.temp_per = TEMP_PER_DEFAULT; // Temp. measurement period.
    SYS->PWR_MOD_b.tper_dis = 0;        // Measure automatically.

    // Enable brownout if there is a battery on the PCB.
    // If brownout is disabled, then the SoC sleeps if V3P3SYS falls.
    SYS->PWR_MOD_b.brn_en = 1;  // Enable battery modes by default.

    SYS->WAKE_SRC = 0;          // Relock the shadow registers.
    sys_shadow_busy_wait();     // wait until set.

    // Initialize the SoC's clock rate for low noise mission mode.
    // Update the clock rate variable.
	//0,0==>10.223616MHz   	//0,1==>786.432kHz
	//1,0==>5.11808MHz      //1,1==>393.216kHz
	//2,0==>2.555904MHz     //2,1==>196.608kHz
	//3,0==>851.968kHz      //3,1==>196.608kHz
	//4,0==>425.984kHz      //4,1==>196.608kHz
    sys_set_mpuclk(4, 0);

    // Let accidental writes to NVRAM go to the last, hopefully unused cell.
    NVRAM->ADDR = NVRAM_LAST_CELL;    // Protect NVRAM from EMI.

    PWM->CNTL = 0;          // All PWMs (pulse width modulators) off.
    OPT->TX_CNTL = 0;       // Also the IRDA hardware (which are PWMs).
    OPT->RX_CNTL = 0;       // Disable these before UART2

    CE->PLS_CNTL = 0x000001ff;  // Make pulses too narrow and low Hz to occur.

    // Disable and Clear interrupts on all devices.

    // Clear SYS interrupts after clearing RTC and temp alarms.
    // i.e. xtal fail, fuse fail, voltage status, temperature range
    // and rtc alarm
    SYS->STAT = 0x80001F1F; // Clear and disable the system interrupts.

    SysTick->CTRL = 0;      // SysTick

if(!(SYS->WAKE_SRC_b.ws_pb))
{
		UART0->CNTL = 0;        // UART0
    UART0->STATE = 0x0C;    // Clear buffer overrun states
    UART0->INT = 0x0F;      // Clear all interrupt flags

    UART1->CNTL = 0;        // UART1
    UART1->STATE = 0x0C;    // Clear buffer overrun states
    UART1->INT = 0x0F;      // Clear all interrupt flags

    UART2->CNTL = 0;        // UART2
    UART2->STATE = 0x0C;    // Clear buffer overrun states
    UART2->INT = 0x0F;      // Clear all interrupt flags

    // By default, no pins are LCD pins.
    LCD->MAPH = 0;
    LCD->MAPL = 0;
    LCD->CNTL = 0;          // Disable the LCD.

    TIMER0->CNTL  = 0;      // Stop timer , Disable timer interrupt.
    TIMER0->INT = 1;        // Clear timer interrupt.

    TIMER1->CNTL  = 0;      // Stop timer , Disable timer interrupt.
    TIMER1->INT = 1;        // Clear timer interrupt.

    DTIMER0->CNTL = 0;      // Disable dual-Timer 
    DTIMER0->INT = 1;       // Clear DTIMER Interrupt.

    DTIMER1->CNTL = 0;      // Disable dual-Timer 
    DTIMER1->INT = 1;       // Clear DTIMER Interrupt.

    CE->INT_CNTL = 0;           // Disable all CE interrupts.
    CE->INT_CNTL = 0x000000FF;  // Clear all CE interrupts.

    SSI->EEX = 0;               // disable synch. serial master
    SSI->INT = 0;               // disable synch. serial interrupt master
    SSI->INT = 1;               // clear interrupt

    SSPI->CNTL = 0x00000400;    // Disable Slave SPI & Interrupt; Safe mode
    SSPI->CNTL = 0x00000401;    // Clear SSPI Interrupt; Still safe mode

    // All output devices should be disabled before the pin-interrupts; 
    // otherwise they can cause pin interrupts.
    for(i = 0; i <= LAST_DIO; ++i) // All the external pin interrupts
    {
        // Note, this reprograms the slave SPI pins (DIO[9]...DIO[12])
        // to be DIOs. This is intentional safety code to help prevent
        // writes to RAM.  The SSPI driver reinitializes them.
        LCD->DIO[i] = 0;        // Make them all inputs. Disable the interrupt.
        LCD->DIO_b[i].flag = 1; // Clear the interrupts.
    }

    // In a Cortex M0, all priorities preempt, so no need to set that.
    // Why do this? In a soft reset, prevent hangs from unserviced interrupts.
    for(i = 0; i <= (int)LAST_IRQ; ++i)
    {   // Disable and clear the non-core interrupts.
        NVIC_DisableIRQ( (IRQn_Type) i );
        NVIC_ClearPendingIRQ( (IRQn_Type) i );
        // Set all to lowest priority. (Power up default = highest.)
        NVIC_SetPriority( (IRQn_Type) i, LOWEST_PRIORITY );
    }

    // The fuse-fail interrupt handler refreshes fuses if they are corrupted.
    sys_int_enable(FUSE_FAIL_INT, NULL);
	}
    // Wait for the RTC's MPU interface registers to finish reset, and change.
    // If the xtal fails, exit because the clock can't change.
    ui = RTC->SUB;
    while(ui == RTC->SUB && 0 == SYS->STAT_b.xfail_i)
        ;
}


/****************************************************************************
* Description:
*       Wait until the shadow registers unlock.
* Input Parameters: none
* Output:
* Return Status:
*       true = timeout
*****************************************************************************/
int32_t sys_shadow_busy_wait(void)
{
    int32_t cnt = SystemCoreClock/128; // >1/256 S

    while (--cnt > 0
            && (SYS->WAKE_SRC_b.rtc_busy
                || SYS->WAKE_SRC_b.rtc_rd
                || SYS->WAKE_SRC_b.rtc_wr))
    {
    }

    return (0 == cnt) ? true : false; // Return failure if timeout.
}


/****************************************************************************
* Description:
*       Wait until a temperature measurement is complete.
* Input Parameters: none
* Output:
* Return Status:
*       true = timeout
*****************************************************************************/
int32_t sys_tmp_busy_wait(void)
{
    int32_t cnt = SystemCoreClock/128; // >1/256 S

    while (--cnt > 0 && TEMP->CNTL_b.temp_start) 
    {
    }

    return (0 == cnt) ? true : false; // Return failure if timeout.
}

/***************************************************************************
* Description:
*       Updates the MPU clock, VLS settings then updates SystemCoreClock.
* Parameters:
*         None.
*   Input:
*         mpuspd: PLL clock div. setting
*					vls: Very low speed setting
*   Output:
*         Setting mpuspd divides the PLL clock 
*					to adjust the MPU clock as follows:
*						mpuspd | 	PLL clock div.
*						000b			PLL clock divided by 2
*						001b			PLL clock divided by 4
*						010b			PLL clock divided by 8
*						011b			PLL clock divided by 24
*						100b			PLL clock divided by 48
*					Setting vls bit divides the PLL output by factor of 13. 
* Return Status:
*         None
***************************************************************************/
void sys_set_mpuclk(int32_t mpuspd, bool vls)
{
    SYS->MPU_CLK_b.mpu_spd = mpuspd; // Update mpu_spd
    ADC->CNTL_b.adc_vls  = vls; // Update adc_vls
    SystemCoreClockUpdate(); // Update Core Clock Frequency
}

/***************************************************************************
* Description:
*       Updates the variable SystemCoreClock by reading the register.
* Parameters:
*         None.
*   Input:
*         The clock control register.
*   Output:
*         The clock speed, in the variable SystemCoreClock.
* Return Status:
*         None
***************************************************************************/
void SystemCoreClockUpdate (void)            /* Update Core Clock Frequency      */
{

    switch(SYS->MPU_CLK_b.mpu_spd)
    {
        default:
            // Execution falls through here.
        case 0:
            if(ADC->CNTL_b.adc_vls)
                SystemCoreClockDivisor = 2*13;
            else
                SystemCoreClockDivisor = 2;
        break;

        case 1:
            if(ADC->CNTL_b.adc_vls)
                SystemCoreClockDivisor = 4*13;
            else
                SystemCoreClockDivisor = 4;
        break;

        case 2:
            if(ADC->CNTL_b.adc_vls)
                SystemCoreClockDivisor = (8*13);
            else
                SystemCoreClockDivisor = 8;
        break;

        case 3: // If VLS is set then SystemCoreClockDivisor = (8*13);
            if(ADC->CNTL_b.adc_vls)
                SystemCoreClockDivisor = (8*13);
            else
                SystemCoreClockDivisor = 24;
        break;

        case 4: // If VLS is set then SystemCoreClockDivisor = (8*13);
            if(ADC->CNTL_b.adc_vls)
                SystemCoreClockDivisor = (8*13);
            else					
                SystemCoreClockDivisor = 48;
        break;
    }

    SystemCoreClock = PLL_CLOCK/SystemCoreClockDivisor;
}


/***************************************************************************
* Description:
*       Sets system interrupt callback and enables or disables the interrupt.
*       One oddity: It will not disable the fuse-fail interrupt.
*       The fuse fail interrupt is supposed to help recover from EMI,
*       and so it should always be enabled.
* Parameters:
*   Input:
*       int_bit is the interrupt bit to enable.
*       fn_ptr is a callback routine to call if the interrupt occurs.
*   Output:
*       None.
* Return Status:
*       None
***************************************************************************/
void sys_int_enable(Sys_int_t int_bit, void (*fn_ptr)(void) )
{
    IRQn_Type irq_type = Reset_IRQn;
    int priority = LOWEST_PRIORITY;
    int idx = -1;

    switch(int_bit)
    {
        case RTC_ALARM_INT:
            SYS->STAT = 0x80000101; // Clear the flag and disable...
            irq_type = RTC_IRQn;
            priority = LOWEST_PRIORITY;
            idx = RTC_ALARM_CALLBACK_IDX;
        break;

        case TEMP_RANGE_INT:
            SYS->STAT = 0x80000202; // Clear the flag and disable...
            irq_type = RTC_IRQn;
            priority = LOWEST_PRIORITY;
            idx = TEMP_RANGE_CALLBACK_IDX;
        break;

        case VSTAT_INT:
            SYS->STAT = 0x80000404; // Clear the flag and disable...
            irq_type = VSTAT_IRQn;
            priority = HIGHEST_PRIORITY;
            idx = VSTAT_CALLBACK_IDX;
        break;

        case FUSE_FAIL_INT:
            SYS->STAT = 0x80000808; // Clear the flag and disable...
            irq_type = FUSE_IRQn;
            priority = HIGH_PRIORITY;
            idx = FUSE_FAIL_CALLBACK_IDX;
        break;

        case CRYSTAL_FAIL_INT:
            SYS->STAT = 0x80001010; // Clear the flag and disable...
            irq_type = FUSE_IRQn;
            priority = HIGH_PRIORITY;
            idx = CRYSTAL_FAIL_CALLBACK_IDX;
        break;

        default:
        break;
    }

    // If the function exists, or it is a fuse-fail interrupt.
    if(NULL != fn_ptr || FUSE_FAIL_INT == int_bit)
    {
        if(0 <= idx) // if the interrupt mask is valid.
        {
            uint32_t sys_stat;
            IRQ_DEFINES;

            sys_handler_ary[idx] = fn_ptr;  // Set the handler's address.

            IRQ_DISABLE();
            sys_stat = SYS->STAT;
            // Preserve interrupt bits. Discard power state bits.
            sys_stat &= 0xffff;
            sys_stat |= int_bit << 8;       // Set the interrupt bit.
            SYS->STAT = sys_stat;           // Enable the interrupt.
            IRQ_ENABLE();

            NVIC_EnableIRQ(irq_type);
            NVIC_SetPriority(irq_type, priority);
        }
    } else {
        if(0 <= idx) // if the interrupt mask is valid.
        {

            NVIC_DisableIRQ(irq_type);
            SYS->STAT = 0x80000000 | (int_bit << 8);  // Disable the interrupt.
            NVIC_SetPriority(irq_type, LOWEST_PRIORITY);
            sys_handler_ary[idx] = NULL;  // Clear the handler's address.
        }
    }
} // sys_int_enable()


/***************************************************************************
* Description:
*       Clears system interrupt callback and disables the interrupt.
*       One oddity: It will not disable the fuse-fail interrupt.
*       The fuse fail interrupt is supposed to help recover from EMI,
*       and so it should always be enabled.
* Parameters:
*   Input:
*       Interrupt flag mask.
*   Output:
*       None.
* Return Status:
*       None
***************************************************************************/
void sys_int_disable(Sys_int_t int_bit)
{
    sys_int_enable(int_bit, NULL);
}


/***************************************************************************
* Description:
*        Fuse Interrupt Service routine.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
***************************************************************************/
void FUSE_Handler(void)
{
    int stat = SYS->STAT;
    unsigned int flash_prot;

    // Mask interrupt bits that are not enabled,
    // so that disabled interrupt bits cannot cause spurious interrupts.
    // Interrupt enable bits are in 0xff00 of STAT.
    // Interrupt bits are in 0x00ff of STAT.
    stat &= stat >> 8;

    if(stat & (int)FUSE_FAIL_INT) // Fuses failed interrupt?
    {
        SYS->STAT = 0x80000008; // Clear the flag...

        if(0 == SV->VERSION_b.test) // Do not hang the device in test mode.
        {
            flash_prot = FLASH->PROT;
            flash_prot |= (1<<8); // set to 1 to allow write/erase flash transactions
            FLASH->PROT = flash_prot;
            // Command the SoC to reread fuse registers from the infoblock.
            FLASH->CNTL = (FLASH_UNLOCK | FLASH_FUSE_REFRESH); 

            flash_prot = FLASH->PROT;
            flash_prot &= ~(1<<8); // disable write/erase flash transactions
            FLASH->PROT = flash_prot;	

            // The handler can reenable it...
            if(NULL != sys_handler_ary[FUSE_FAIL_CALLBACK_IDX])
                (*sys_handler_ary[FUSE_FAIL_CALLBACK_IDX])();
        }
    }

    if(stat & (int)CRYSTAL_FAIL_INT) // Crystal failed interrupt?
    {
        // Prevent the next interrupt. Interrupt is level sensitive.
        SYS->STAT = 0x80001010; // Clear the flag...

        if(NULL != sys_handler_ary[CRYSTAL_FAIL_CALLBACK_IDX])
            (*sys_handler_ary[CRYSTAL_FAIL_CALLBACK_IDX])();
    }
} // FUSE_Handler()


/***************************************************************************
* Description:
*        RTC Interrupt Service routine.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
***************************************************************************/
void RTC_Handler(void)
{
    int stat = SYS->STAT;

    // Mask interrupt bits that are not enabled,
    // so that disabled interrupt bits cannot cause spurious interrupts.
    // Interrupt enable bits are in 0xff00 of STAT.
    // Interrupt bits are in 0x00ff of STAT.
    stat &= stat >> 8;

    if(stat & (int)RTC_ALARM_INT) // Alarm interrupt?
    {
        // Disable the RTC's alarm.
        RTC->ALARM = 0x1f3f3f;      // Set to 31:63:63, an impossible time.

        // Prevent the next interrupt. Interrupt is level sensitive.
        SYS->STAT = 0x80000101; // Clear the flag and disable the interrupt...

        if(NULL != sys_handler_ary[RTC_ALARM_CALLBACK_IDX])
            (*sys_handler_ary[RTC_ALARM_CALLBACK_IDX])();
    }

    if(stat & (int)TEMP_RANGE_INT) // Temperature range interrupt?
    {
        // Prevent the next interrupt. Interrupt is level sensitive.
        SYS->STAT = 0x80000202; // Clear the flag and disable the interrupt...

        if(NULL != sys_handler_ary[TEMP_RANGE_CALLBACK_IDX])
            (*sys_handler_ary[TEMP_RANGE_CALLBACK_IDX])();
    }
} // RTC_Handler()


/***************************************************************************
* Description:
*        Power status Interrupt Service routine.
* Parameters:
*   Input:
*         None.
*   Output:
*         None.
* Return Status:
*         None
***************************************************************************/
void VSTAT_Handler(void)
{
    int stat = SYS->STAT;

    // Mask interrupt bits that are not enabled,
    // so that disabled interrupt bits cannot cause spurious interrupts.
    // Interrupt enable bits are in 0xff00 of STAT.
    // Interrupt bits are in 0x00ff of STAT.
    stat &= stat >> 8;

    if(stat & (int)VSTAT_INT) // Voltage status changed interrupt?
    {
        SYS->STAT = 0x80000004; // Clear the flag...

        if(NULL != sys_handler_ary[VSTAT_CALLBACK_IDX])
            (*sys_handler_ary[VSTAT_CALLBACK_IDX])();
    }
} // VSTAT_Handler()


/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

