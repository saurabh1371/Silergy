CD .\\Objects
srec_cat.exe -Disable_Sequence_Warnings SY7T108_1p2w_shunt_ct_dlms_1.hex -Intel -fill 0xff 0x0000 0x200000 -o SY7T108_1p2w_shunt_ct_dlms_1_temp.hex -Intel --address_length=4 --line_length=44 -output_block_size=16
srec_cat.exe -Disable_Sequence_Warnings SY7T108_1p2w_shunt_ct_dlms_1_temp.hex -Intel -unfill 0xFF 16 -o SY7T108_1p2w_shunt_ct_dlms_1_aligned.hex -Intel --address_length=4 --line_length=44 -output_block_size=16
del SY7T108_1p2w_shunt_ct_dlms_1_temp.hex


