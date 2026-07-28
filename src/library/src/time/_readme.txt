This directory has much of the higher-level (non-driver) clock code.
The clock in this system keeps local time in the hardware registers.
This approach enables meter software to have quick, simple access to the time.
But, some may need to calculate julian seconds (i.e. for calendar conversion)
daylight savings time and universal time.

time.c  does system-level time management: Recovery from NVRAM, validation,
    finding the day of week, day of year, iso week of year, etc.
    It also calculates times and dates using julian seconds: 00:00 2017-1-1 = 0
    It has logic to switch in and out of daylight savings time.
    The logic calls code from dst\dst_<country code>.
-------------
Ver. History :
-------------
 ver 1.00 - Initial release

time_stub.c  does system-level time management, however, it has just enough
    logic to set and read the clock. It lacks NVRAM backup, Julian seconds, 
    DST, GMT, support for calculating the day of week, etc. (The day of week 
    must be set!) Why use it? It's about 1K smaller.
-------------
Ver. History :
-------------
 ver 1.00 - Initial release

$Id: _readme.txt 21231 2019-05-30 22:39:55Z rvandewa $
