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

