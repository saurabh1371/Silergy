This directory has common driver code for the ARM demo code.
The header files are in the include directories.
If a chiop has a more specialized copy of the hardware, please use
the specialized driver in the device's driver folder.

+ bat has code to read the battery voltages.
+ ce has reusable code for the compute engine (DSP).
+ delay has a calibrated delay loop.
+ dtimer has a driver for the dual timer.
+ eeprom has the EEPROM drivers.
+ external_int has the code for pin-based interrupts.
+ flash has the driver for the SoC's flash memory.
+ gpio has the drivers for the SoC's general-purpose digital I/O pins.
+ i2c has the hardware and bit-banged I2C driver code.
+ info_block can read the trims, useful for calibration.
+ lcd has a driver for liquid crystal displays. (The SDK guide has
instructions to port to new LCD displays.)
+ nvram accesses the nonvolatile RAM.
+ opt sets up the optical (IRDA SIR) interface on the optical serial port.
+ pwm drives the pulse width modulation outputs.
+ pwrmode sets up and transitions to different power modes.
+ rmt has the driver and diagnostics for capacitively-isolated remote ADCs.
+ rtc has the driver for the real-time clock (it keeps time and date).
+ serial has the drivers for the UARTs to send and receive serial data.
+ spi is the driver for the master serial peripheral interface. (It make SCLK)
+ sspi is the driver for slave serial peripheral interface (It accepts SCLK)
+ systick is a driiver for a periodic interrupt, time measurement, delay
+ tiemr has the drivers for the timers.
+ tmp is the driver for the SoC's thermometer.
+ tmux is the driver for the test multiplexer pins. (Internal signals.)
+ wd is the driver for the SoC's software watchdog.

$Id: _readme.txt 24734 2020-04-29 22:54:07Z rvandewa $

