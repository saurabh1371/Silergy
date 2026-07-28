This directory has API include files for unshared driver code.
By convention, to make higher-level code portable to another IC,
these files should be included by a global include file with a standard name.
Usually a PCB description file (map*.h) is included in  the most global file.
The name of the global file is somewhat standardized by Silergy (e.g.
SY7_device.h for driver-level code that only depends on an IC model, 
options.h in system or application code that requires an exact environment.)

+ autocal.h is an autocalibration API used by all analog front ends.
+ cexxxyyy.h CE image and ADC/CE register configuration include file.
   xxx is the analog front end (AFE) design or version of a part.
   Different parts can have compatible AFEs that run the same CE code.
   yyy is a version of CE code. Different CE versions have different
   features, described by a CE reference manual.
+ SY7_device.h is included by shared device drivers to adapt them to this IC.
+ <device>.h describes the hardware registers of <device>
+ <device>.sfr Is the debugger file to watch the hardware registers of 
  <device>, as used in Keil debuggers. (A Keil .svd file is available on
  request.)
+ system_<device>.h describes the start-up code of <device>

$Id: _readme.txt 24752 2020-04-30 00:28:49Z rvandewa $

