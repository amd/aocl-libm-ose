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


