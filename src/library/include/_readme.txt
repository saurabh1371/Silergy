This directory has the header files for the ARM demo code.
The header files have function declarations and extern variable definitions shared between the source files.

+ afe.h calculates usable data from the analog front end (CE driver's) outputs.
+ cal.h  is the calibration access include file.
+ cli_buf.h is the CLI's (command line interface's) buffered serial I/O.
+ cli_buf_str.h are the C strings the CLI expects to have.
+ crc.h is CCITT CRC-16 and CRC-32 code include file.
+ dst.h is an API for daylight savings time.
+ hash_str.h makes a data-dependent byte from a string.
+ log.h logs events.
+ mmath.h is special math used by metering.
+ parity.h calculates 8-bit parity
+ reg.h  defines the meter's billing register data structures and functions.
+ serb*.h defines binary serial interfaces
  It uses the register-level drivers ser*.c
+ serc*.h defines the "cooked" serial interfaces for the command line
  interface. It uses the register-level drivers ser*.c
+ sspi*.h defines a slave SPI interface that can run the command line
interface.
+ stm.h is the software timer module API include file.
+ tamper.h is the generic tamper detection logic.
+ time.h defines start-up and calendar logic for the real-time clock.

$Id: _readme.txt 24736 2020-04-29 23:18:16Z rvandewa $

