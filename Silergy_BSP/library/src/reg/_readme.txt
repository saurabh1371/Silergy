This directory has code to clear, save and restore
the meter's billing registers.  The billing registers
are stored in several copies to improve the reliability.
The demo code is designed to have a valid set of billing register
data from the time interrupts are enabled (i.e. power failure
detection is permitted), until power fails (it goes to sleep).

reg_eeprom.c saves and restores billing registers. It has multiple copies,
    in RAM, and in EEPROM, and tests them in several ways
    and if somehow all the copies are bad, it clears.
-------------
Ver. History :
-------------
 ver 1.00 - Ported from ZON version 1.08

reg_flash.c is designed to store billing registers in flash. It does
    wear levelling to get at least the same number of writes as a 
    typical EEPROM.
-------------
Ver. History :
-------------
 ver 1.00 - Ported from ZON version 1.08

$Id: _readme.txt 16026 2017-11-23 02:49:23Z rvandewa $
