systick.c drives an ARM systick timer in several ways.
1. The classic interrupting timer.
2. A one-shot timer, handy for state machines with timeouts.
3. Measuring an interval. Handy because the ARM Cortex M0 has no cycle
counter to measure code's timing.
4. Precision delay when interrupts are disabled.
Inputs and measurements are in counts of cycles.
The .h file includes code to convert to and from counts of cycles.

-------------
Ver. History :
--------------
ver 1.00  - Initial release.

$Id$
