This is the main directory for the logging code.

log_by_*.c .h has logging code to record conditions, events and errors.
    The error log code can accept error data before the module is initialized.
    Initialization unifies the nonvolatile data with the data from the
    current start-up.

log_by_bits.c Uses sticky bits. This code is very compact and fast.
log_by_record.c Uses an array of records in the billing register area.
    (This can be easily moved to another place.) It is periodically saved
    to nonvolatile RAM. 

-------------
Ver. History :
-------------
 ver 1.00 - initial release

$Id: _readme.txt 16026 2017-11-23 02:49:23Z rvandewa $

