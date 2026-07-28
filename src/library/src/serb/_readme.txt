This directory has the binary serial I/O often used for automatic
meter reading.

serbx.c sends and receives binary on the UART number x.
There are three modules, one for each uart.
It utilizes the low-level serial driver serx.c.
serb2.c optionally initializes optical interfaces.
-------------
Ver. History :
--------------
ver 1.00  - Initial release.


$Id: _readme.txt 16212 2017-12-08 00:31:28Z ptanwar $
