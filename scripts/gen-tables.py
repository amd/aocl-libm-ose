#!/usr/bin/env python3

import struct
import bigfloat
from bigfloat import precision as preci

# define double_to_hex (or float_to_hex)
def double_to_hex(f):
        return hex(struct.unpack('<Q', struct.pack('<d', f))[0])

def hex_to_double(h):
        return float(struct.unpack("<d",struct.pack("<Q",h))[0])

table_size = 1024

for i in range(0, table_size):
        idx = float(i)/table_size
        h1 = 0.0
        with preci(80):
                h1 = bigfloat.pow(2, idx)

        myhex = double_to_hex(h1)
        print("%s"%myhex, end='')

        b = int(myhex, base=16) & ~( (1 << 28) - 1)
        print("\t0x%x\t"%b, end='')

        d = h1 - hex_to_double(b)
        print("%s"%double_to_hex(d))

