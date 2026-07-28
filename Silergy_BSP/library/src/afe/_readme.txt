This directory has the analog front-end interface code.
This is the high-level math that manages numbers from a low-level 
compute engine driver in <device>\drivers\non-cmsis\ce*

afe_*.c .h is an analog front end interface, a stub is provided.
afe_*_stub.c is a stub for the analog front end.  It's handy for testing
    output routines and meter operation without high voltages.

The equations are for different metering drops, implemented assuming a
CE code that can handle up to 3 phases:

afe_0_1.c is for CE code that does Wh = VA * IA, or Wh = 0.5*VA*(IA - IB);
Neutral current values (WhN, VARhN, etc.) are calculated using IB and VB. 
VA and VB should both measure the single voltage.
Designed for shunt on IA and CT on IB. May use shunts on both.
(with remote for IB) or CTs on both.
Wh = VA * IA can use a 1, 2, or 3-phase CE code with a 3-phase autocal.
Wh = 0.5*VA*(IA - IB) needs a special differential autocal for "equation 1."

-------------
Ver. History :
-------------
 ver 3.00 - Corrected temperature compensation logic.
            Fixed voltage logic in the multiphase AFE logic.
            Added afe_2, 3p3w, W=VA*IA+VB*IB, 3-phase delta.
 ver 1.05 - Revised total harmonic distortion to take the square root.
            Changed creep value of VPULSE_SUM and WPULSE_SUM to 0xA000_0000,
            helpful with some meter specifications.
            Autoranges the scaling variable for converting neutral units.
 ver 1.04 - Revised total harmonic distortion for current and voltage to
            utilize the squared-values, i.e. power, and take the sqrt().
            Fixed a defect in the completion-detection so that it always
            indicates completion when called by the interrupt.
 ver 1.03 - Added code to compensate AFEs using CE with 64-bit booth multiply.
            Also added total harmonic distortion calculations to narrowband.
 ver 1.02 - Added code to calculate I1SCALE. Requires Cinsq to exist
            in ce.h's CE data structure.
 ver 1.01 - Separated afe_start() into afe_init(), and afe_enable()
            so that the calibration could copy default hardware state,
            e.g., the preamp value.
            Also changed afe_stop() to afe_disable() to pair the names.
            This change requires a change to main().
            In the 3-phase, corrected some errors that did not provide
            correct values.
 ver 1.00 - Ported from ZON version 1.08a (from ZON M1 xtal_fail_code)


afe_5.c is for CE code that does Wh = VA*IA+VB*IB+VC*IC;
-------------
Ver. History : See above; These are parallel pieces of code.
-------------


afe_2.c is for CE code that does Wh = VA*IA+VB*IB;
-------------
Ver. History : See above; These are parallel pieces of code.
-------------

afe_*_stub.c simulate the meter equations;
-------------
Ver. History : See above; These are parallel pieces of code.
-------------

$Id: _readme.txt 24035 2020-03-25 22:17:15Z rvandewa $
