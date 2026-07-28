This directory has code that configures and starts the CE.

ce.c has CE interrupt setup, CE ISR and de-initialization routine.
-------------
Ver. History :
--------------
ver 2.00  - Mandatory change for version B of IC: Address of the CE code
    is written to the register CE->LCTN without shifting or masking.
ver 1.00  - Initial release.


An autocalibration stub is very useful when the autocalibration for a CE code
is not implemented or not known.
-------------
Ver. History :
--------------
ver 1.00  - Initial release.


$Id: _readme.txt 21397 2019-06-15 02:52:26Z rvandewa $
