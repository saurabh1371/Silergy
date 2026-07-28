This directory has the header files for some shared drivers of ARM demo code.
The header files have function declarations and extern variable definitions shared between the source files.
If a device has some specialized drive, those will be under the device's
folder. The specialized versions should replace these, but have similar IO.

+ bat.h reads battery voltages.
+ bitdef.h defines names for bits, e.g. "1" is Bit0 ;
+ ce.h has reusable code for the compute engine (DSP CPU).
+ delay.h is a calibrated delay loop.
+ dtimer is the dual timer driver.
+ eeprom.h, eeprom_common.h provide access to EEPROM drivers.
+ external_int*.h provides access to pin-based interrupts.
+ flash.h provides access to the SoC's flash memory.
+ gpio*.h provides access to the SoC's general-purpose digital I/O pins.
+ i2c*_master.h provides access to the i2c master.
+ info_block.h permits code to read the IC's trims. (Useful in calibration.)
+ irq.h can disable and enable interupts.
+ lcd.h provides access to the SoC's LCD hardware. The fonts help.
+ lcd_font*.h are the fonts for LCDs, 7-segment,and 14-segment.
+ nvram.h is for accessing the nonvolatile RAM.
+ opt.h is for the optical serial interface (IRDA SIR) of the UART.
+ pwm*.h is the pulse-width modulation drivers.
+ pwrmode.h is the include file to access battery modes.
+ rmt.h provides access to the capacitively-isolated remote ADCs.
+ rtc.h provides access to the real-time clock (it keeps time and date).
+ serX.h accesses the UARTs to send and receive serial data.
+ sflash*.h drives serial flash ICs.
+ spi*.h runs a master (output) serial peripheral interface (SPI).
+ sspi.h runs the (input) slave SPI.
+ systick.h does timing using the systick timer.
+ timer*.h provides access to the timers.
+ tmp.h accesses the SoC's thermometer.
+ tmux.h provides access to the SoC's test-signal multiplexer.
+ wd.h services the SoC's software watchdog.

$Id: _readme.txt 24733 2020-04-29 22:49:09Z rvandewa $

