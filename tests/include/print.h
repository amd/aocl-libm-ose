#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdint.h>

void print_bits_32(uint32_t);
void print_bits_64(uint64_t);

#ifdef __GNUC__
#define print_bits(x) _Generic((x), 			\
			int:	  print_bits_32,	\
			long int: print_bits_32,	\
			uint32_t: print_bits_32,	\
			float:	  print_bits_32, 	\
			double:   print_bits_64, 	\
			uint64_t: print_bits_64,	\
			long long:print_bits_64)((x))
#endif

#endif
