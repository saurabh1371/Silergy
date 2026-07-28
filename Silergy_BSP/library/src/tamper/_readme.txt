This directory has code for Tamper Detection.

tamper*.c is tamper detection. It tests for intrusion in whatever
    ways are defined in the PCB's definition file.
    If a GPIO is defined, it's polled.
    If an interrupt is defined, it's enabled.
    If a wake is defined, the meter wakes.
The stub file disables tamper detection.
-------------
Ver. History :
-------------
 ver 1.01 - Detects unhandled fuse failure.
 ver 1.00 - Ported from ZON version 1.08

$Id: _readme.txt 24039 2020-03-25 23:44:25Z rvandewa $
