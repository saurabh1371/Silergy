The folder contains the following files

scfu.exe                               Silergy CM0 Flash Utility executable.
hidapi.dll                             Windows library/DLL file used by scfu.exe
release note.pdf                       Release notes
.\flash_drivers\SY7T108F_64K.FLM       Flash driver file for SY7T108F device
.\flash_drivers\SY7T108G_128K.FLM      Flash driver file for SY7T108G device
.\flash_drivers\SY7x2xxH_256K.FLM      Flash driver file for SY7x2xxH device(s)
readme.txt                             This file

Instructions to the user:

1) Copy and unzip the package SCFU_V0202.zip to a folder of your choice.

2) Open a command line prompt and cd to the unzipped folder 
  (DO NOT DOUBLE CLICK THE scfu.exe from Windows Explorer).

3) Type scfu.exe -h for a list of command line options.

4) Make sure the CMSIS-DAP device is connected to Silergy’s Device and the 
   CMSIS-DAP device’s USB driver is properly enumerated with the Windows PC 
   where it is plugged in. An error similar to this message: 
   “Target Error, Unexpected DAP Response” will occur if SCFU cannot communicated
   to the CMSIS-DAP device.  Refer to the Silergy Device’s starting guide for 
   troubleshooting.

5) Type the command line options as specified below.

Command line options:

1) scfu.exe -h - lists the usage string with command line options as follows -

##########################################################################
Silergy CM0 Flash Utility Version = 2.02
##########################################################################


Usage:
scfu.exe {-h}{-x}{-p -n [1 | 2 | 3 | 4 | 5] -f filename}{-d -n [1 | 2 | 3 | 4 | 5] -f filename}

-h = help
-x = erase entire Flash
-p = program flash
-d = dump flash contents starting from 0x00
    -n = DUT
        1 = SY7T108F(64K)
        2 = SY7T108G(128K)
        3 = SY7T243HS(256K)
        4 = SY7M211H(256K)
        5 = SY7M213H(256K)
    -f = output/input filename to dump/program the flash contents

2) With -d option, the contents of the flash will be displayed onto STDOUT 
   if -f option is omitted

3) -x, -d and -p are exclusive. User has to provide only one of the -d or -x 
   options. 
   It is an error if 2 or more options are given in a single session
  
4) A typical flash dump session with -n option is as follows  

Example: > scfu.exe -d -n1 -f test.hex

##########################################################################
Silergy CM0 Flash Utility Version = 2.02
##########################################################################

Product String: Keil ULINK2 CMSIS-DAP.



IDCode = 0B B1 14 77

Flash Dump:
Start address  = 0x00000000
Size in Kbytes = 64
output file    = test.hex

..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
..............................................................................
............................

Flash contents stored successfully in file test.hex.

5) Note that the dots displayed here is only indicative.
   If -f option is not selected, the dots will not be diplayed. Instead the file 
   contents will be displayed onto stdout.
   
6) It takes approximately 16 seconds to dump 128 Kbytes of flash contents

7) A typical flash programming and verification session is as follows  
   It takes approximately 27 seconds to complete program and verify 128kbytes of
   flash data.
   
E:\test_folder\SCFU_V0200>scfu.exe -p -n3 -f testfiles\test.hex


##########################################################################
Silergy CM0 Flash Utility Version = 2.02
##########################################################################

Debugger String: Keil ULINK-ME CMSIS-DAP.


IDCode = 0B B1 14 77

Loading flash driver.............................................................Done

Loading test.hex...Done

Chip Erase...Done

Programming.............................................................................................................
........................................................................................................................
........................................................................................................................
........................................................................................................................
........................................................................................................................
........................................................................................................................
........................................................................................................................
........................................................................................................................
............................................Done
Verification............................................................................................................
........................................................................................................................
........................................................................................................................
........................................................................................................................
........................................................................................................................
........................................................................................................................
.................................................................................................Done
