EEPROM drivers have an identical API because they all do the same thing.
Using the same API permits the common integration of different EEPROM hardware.


eeprom_i2c.c: I2C eeprom driver code for ICs similar to the At24C1024.
-------------
Ver. History :
--------------
ver 1.00  - Initial release.


eeprom_spi.c: SPI eeprom driver code for ICs similar to the AT23C80.
-------------
Ver. History :
--------------
ver 1.01  - Uses the SPI driver.
ver 1.00  - Initial release.


sflash.c SPI serial flash driver code for ICs similar to the
GigaDevice- Uniform Sector Standard and Dual Serial Flash GD25WD40C/20C.
-------------
Ver. History :
--------------
ver 1.00  - Initial release for spi and spi1.

$Id: _readme.txt 23250 2020-02-17 03:04:35Z rvandewa $
