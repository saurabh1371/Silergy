COPY .\\Output\\SY7T_M11_bootloader.hex .\\hex2c
CD .\\hex2c
hex2c.exe Bootloader SY7T_M11_bootloader.hex bl_array.c
COPY bl_array.c ..\\..\\System_Files
echo "Done"