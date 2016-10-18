#!/usr/bin/env python

import sys
import re

rom_base = 0x0
rom_len = 0x8000

src_file_name = sys.argv[1]
dst_file_name = src_file_name + '.spicode'
f = open(src_file_name, 'r')
lines = f.readlines()
f.close()

hex_list = {}
header = ':'
length = 0
addr = 0
type = 0
line_num = 0
for ln in lines:
    m = re.search('(\S{1})(\S{2})(\S{4})(\S{2})', ln)
    if m is not None:
        header = m.group(1)
        length = int(m.group(2), 16)
        addr = int(m.group(3), 16)
        type = int(m.group(4), 16)
        #if line_num == 0:
        #    if type != 4:
        #        raise Exception, ('The first line type is ERROR')
        #    else:
        #        print 'The hex to text start'
        #else:
        if type != 0:
            print 'The %d line is %d type, the hex data in this line isnot trans to text' % (line_num, type)
        else:
            for i in range(length):
                hex_list[addr+i] = str(ln[i*2+9:i*2+11])
    line_num += 1
    
if line_num == 0:
    raise Exception, ('ERROR: The source file is empty')
else:
    if type != 1:
        raise Exception, ('The last line %d type is ERROR' % type)

for i in range(rom_base, rom_len):
    if i not in hex_list.keys():
        hex_list[i] = '00'

f = open(dst_file_name, 'w')
f.write('item: spi_download_code\n')
f.write('call spi_pmu_write(&h7fff, &h0000);\n')
for i in range(rom_base, rom_len, 2):
    f.write('call spi_pmu_write(&h'+str(hex(i)[2:]).zfill(4)+', &h'+str(hex_list[i+1])+str(hex_list[i])+');\n')

f.close()
print 'The hex to text is success'


