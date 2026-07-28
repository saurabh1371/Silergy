This directory has code to find daylight savings time and time-zone
abbreviations.
The clock in this system keeps local time in the hardware registers.
This approach enables meter software to have quick, simple access to the time.
But, some may need to calculate daylight savings time and universal time.
    
dst_<country code> performs daylight savings time calculations (DST).
    Usually, the calibration gmt_offset (minutes from UTC) must be set.
    After the clock and calendar is set, the meter will change to and from
    DST, and display the correct time zone strings.
    Countries without DST can still get correct time-zone strings.
    Use dst_stub() if this is not useful.
    dst_us.c has been tested. dst_ca.c uses the same logic.
    dst_eu.c has been informally tested, others are untested.
-------------
Ver. History :
-------------
 ver 1.01 - Added Samoa time to dst_us.c
 ver 1.00 - Initial release
    

$Id: _readme.txt 24060 2020-03-26 23:34:10Z rvandewa $
