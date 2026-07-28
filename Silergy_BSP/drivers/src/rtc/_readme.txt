This directory has the real-time clock code.
This is a register-level driver.  It has a bit of code to calculate
the day of year, used by some time-of-use systems.
In general, to save power, the current civil time is kept in the RTC hardware.

rtc.c is a real-time clock interface.
-------------
Ver. History :
--------------
ver 1.00  - Initial release.

rtc_stub.c is a stub. Use for testing only.
-------------
Ver. History :
--------------
ver 1.00  - Initial release.


$Id: _readme.txt 17328 2018-04-13 23:15:23Z rvandewa $
