This directory has the serial I/O for the command line interface.

sercx.c is the "cooked" serial logic for UARTX
There is a driver for each UART.
That is, it implements backspace, CR, etc. manages buffering, etc.
It utilizes the low-level serial driver serx.c.
serc2.c optionally initializes the optical interfaces used for IRDA.
-------------
Ver. History :
--------------
ver 1.01  - Pins are remapped from LCD use. (Demo PCBs use many pins for LCD).
ver 1.00  - Initial release.


$Id: _readme.txt 24036 2020-03-25 23:01:04Z rvandewa $
