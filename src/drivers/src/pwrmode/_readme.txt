This directory has the power mode code for battery-powered and
voltage-line cut modes.

pwrmode.c is the power mode control interface.

-------------
Ver. History :
--------------
ver 3.00  - brownout is detected from the power-up state machine.
          - A brief 1ms delay is after each power mode command, to let device
            operate.
          - If all power is failing, (v3p3d_nok = 1), the power failure
            interrupt enters sleep immediately to prevent a BADVDD condition 
            in the power up state machine.
ver 2.00  - Unchanged for v2.0
ver 1.00  - Initial release.


pwrmode_stub.c is a fake power mode control.  When you don't want it to work.
-------------
Ver. History :
--------------
ver 3.00  - Matches API of v3.0
ver 2.00  - Unchanged for v2.0
ver 1.00  - Initial release.

$Id: _readme.txt 24708 2020-04-29 20:36:29Z rvandewa $
