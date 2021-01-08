#
# Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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
from scripts.libm import bin64, double_to_hex, hex_to_double

class Log(object):
    def __init__(self, table_size = 256):
        self.table_size = table_size

    def GenTable(self):
        #newsize = self.table_size +  1
        newsize = self.table_size << 1
        for j in range(0, newsize): # 0 to 256
            with precision(256):
                f = bigfloat.log(1+j/newsize)
                f_leadint = bin64(f).hex1int() & 0xfffffffff0000000
                f_tail = f - hex_to_double(f_leadint)

            #print("%x"%f_leadint, double_to_hex(float(f_tail)))
            print("{", "0x%x, %s"%(f_leadint, double_to_hex(f_tail)), "},")

    def GenInvTable(self):
        total = 2 * self.table_size
        for j in range(0, total+1, 2):
            with precision(256):
                f = 2.0 * (total / ( j + total) )
                print(double_to_hex(f))

    def GenLn2(self):
        with precision(128):
            ln2 = bigfloat.log(2.0)
            l = bin64(ln2).hex1int() & 0xffffffffe0000000   
            ld = bin64(hex_to_double(l))
            print(".ln_lead = ", double_to_hex(ld), ld.hex())

        with precision(128):
            t = ln2 - ld
            tail = bin64(t)
            print(".ln_tail = ", double_to_hex(tail), tail.hex())


    def GenPoly(self):
        pass


