This directory contains LCD driver code.

lcd.c writes data to an LCD. When it initializes, it scans the LCD tables,
    and sets-up every pin that is used.  It selects the clock rate and mode.
    After a design is frozen, the values of the LCD set-up registers can
    be copied to constant defines to make smaller init code.
-------------
Ver. History :
--------------
ver 2.00  - Initial release, SY7T_M11 version B. Order of bytes in the LCD's
    DMA system is changed to native little-endian.
ver 1.00  - Initial release, IC version A.

lcd_stub.c is a stub driver. 
    Useful when an LCD is not needed.  Just replace lcd.c with it.
-------------
Ver. History :
--------------
ver 1.01  - Added functional version of lcd_map() to support drivers when
            they need to remap a pin.
ver 1.00  - Initial release.

$Id: _readme.txt 23558 2020-03-09 21:04:44Z rvandewa $
