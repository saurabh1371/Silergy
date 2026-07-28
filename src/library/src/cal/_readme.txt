This directory has code for the default calibration and to save and restore
the calibration.

cal*.c .h saves and restores calibrations to and from EEPROM. It tests them in several ways
    and if the saved calibration is invalid, it uses a factory default 
    calibration.
cal_flash.c saves and restores calibrations to and from Flash.  It is currently excluded from the build.

-------------
Ver. History :
-------------
 ver 1.01 - Modified to perform preamp save and restore.
 ver 1.00 - Ported from ZON version 1.08

$Id: _readme.txt 21397 2019-06-15 02:52:26Z rvandewa $
