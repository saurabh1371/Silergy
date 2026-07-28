This directory has code to run an integrated inter-IC bus an I2C Master.
As the master, it generates the clock.
There are three implementations: i2c_master and i2c_master_int uses the specialized hardware.
The other uses GPIOs, and bit-bangs the logic.
Since they all do the same thing, they have identical APIs, and
are accessed by an identical .h file.

i2c_bitbang.c
Uses the GPIOs.  This can be moved to any pins that can be GPIOs.

-------------
Ver. History :
--------------
ver 1.00  - Initial release.


i2c_master.c
Uses the hardware I/O device.

-------------
Ver. History :
--------------
ver 1.00  - Initial release.


$Id: _readme.txt 24519 2020-04-22 16:49:54Z ptanwar $
