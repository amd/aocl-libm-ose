#include <stdio.h>

#include <print.h>

static const char *binary_table [] = {
    "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", /* 0 - 7 */
    "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111", /* 8 - F */

};

void _print_byte(uint8_t v)
{
    printf("%s%s", binary_table[(v >> 4) & 0xf], binary_table[v & 0xf]);

}

void print_bits(const void *a, int size, bool newline)
{
    unsigned char *v = a;

    while(size >= 0) {
        print_byte(v[size]);
        size--;
    }
    if (newline)
        printf("\n");
}

void print_bits_32(uint32_t *v)
{
    print_bits(v, sizeof(*v));
}

void print_bits_64(uint64_t *v)
{
    print_bits(v, sizeof(*v));
}

void print_float(float f)
{
    print_bits_32(&f);
}
void print_double(double d)
{
    print_bits_64(&d);
}
