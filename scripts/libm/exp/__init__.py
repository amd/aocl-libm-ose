#!/usr/bin/env python3

#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

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
