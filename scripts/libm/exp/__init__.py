#!/usr/bin/env python3
import bigfloat
from bigfloat import precision

from scripts.libm import bin64,hex_to_double,split_double,double_to_hex

class Exp(object):
    def __init__(self, table_size=64):
        self.table_size = table_size

    def GenTable(self):
        for i in range(0, self.table_size):
            idx = float(i)/self.table_size
            h1 = 0.0
            with precision(200):
                h1 = bigfloat.pow(2.0, idx)
            fm,fh,fl = split_double(h1)
            #print("{", fh.hex(), ", ", fl.hex(), "},\n", endl='')
            print("{",
                  double_to_hex(fm), ", ",
                  double_to_hex(fh), ", ",
                  double_to_hex(fl),
                  "},")
            if i % 16 == 0:
                print("// i = %d"%i)

    def GenLn2(self):
        with precision(128):
            ln2 = bigfloat.log(2.0)
            h = -ln2 / self.table_size
            hint = bin64(h).hex1int() & 0xffffffffffff0000
            hd = bin64(hex_to_double(hint))
            print(".ln2by_tblsz_head = ", hd.hex())

        with precision(128):
            t = h - hd;
            tail = bin64(t)
            print(".ln2by_tblsz_tail = ", tail.hex())


if __name__ == "__main__":
    import os               # Miscellaneous OS interfaces.
    import sys              # System-specific parameters and functions.

    if len(sys.argv) > 1:
            sys.stdout = open(sys.argv[1], "w+")

    parser = argparse.ArgumentParser(description='Exp2 Table Generation')
    parser.add_argument('-l', '--length',
                        action='store_true', help='Table Size')

    exp2 = Exp2(length)
    try:
            exp2.Generate()
    except StatisticsException, mesg:
            print(); print("Exception caught: %s" % mesg); sys.exit(0)

    sys.exit(0)
