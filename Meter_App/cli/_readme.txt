This directory folder has code for a simple command line interface,
or "CLI" through a UART. The CLI is a compromise between firmware 
simplicity and minimal PC code. A standard terminal program works.
Information about using or removing it is below.

cli.c, .h is the command line interface.
cli_display_x_phase_iso.c is the the front panel simulated on the serial line.
cli_autoreport.c dumps data to the CLI device, ^x=on, ^y=off
clistubs.c, cli_integration_stubs.c have temporary routines not yet implemented.
help.c, .h contains help text for the CLI.
version.c .h is the system's software version string.

The CLI is >15K with help.  It is not needed in a meter that 
is calibrated by automatic factory equipment or autocalibration.

To remove the cli from the build, remove the build's /CLI and /serial files,
except version.c (do not delete them from the disk until you are sure),
and then include cli/cli_stub.c.  To remove the last traces, remove the calls
that other code makes to cli_stub.c  These are in meter/meter.c and dev/crt0.asm
version.c is used to test meter data and display software versions. Please
feel free to use your own software version or part numbers in version.c

The cli commands are documented in the parameter document.

To use the CLI in your code:
char *line_str;
...
ser2_init();
...
// Command line interface.
line_str = ser2_get_line ();  // Get the line as a string.
if (NULL != line_str) // Is a line complete?
{
   set_port (2, line_str, 80);  // Attach the CLI I/O to this port.
   cli ();                      // Interpret the line. (queue output)
   unset_port ();               // Detach the CLI I/O from this port.
}


$Id: _readme.txt 25648 2020-06-10 11:00:29Z phuddar $

