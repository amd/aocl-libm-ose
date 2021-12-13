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

import struct

# define double_to_hex (or float_to_hex)
def double_to_hex(f):
        return hex(struct.unpack('<Q', struct.pack('<d', f))[0])

def hex_to_double(h):
        return float(struct.unpack("<d",struct.pack("<Q",h))[0])

class bin64(float):
	def hex1(self):
    		#return hex(struct.unpack('<Q', struct.pack('<d', self))[0])
    		return double_to_hex(self)

	def hex1int(self):
    		return int(self.hex1(),16)

# Veltkamp’s algorithm or the Veltkamp-Dekker algorithm
# 'delta' is at least half of precision length, for double it is 26 or 27 (considering 52-bit precision)
# void Split ( const double x , const int delta , double * x_h , double * x_l )
# { // No unsafe optimizations !
#	double c = ( double )((1 UL << delta ) + 1);
#	* x_h = ( c * x ) + ( x - ( c * x ));
#	* x_l = x - x_h ;
#	return ;
# }
def split_double(x, Scale=26):
    c = (1 << Scale) + 1
    xh = c * x + (x - (c * x))
    xm = x - (c * x)
    xl = x - xh

    return x, xh, xl

