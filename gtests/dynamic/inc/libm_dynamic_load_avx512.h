#ifndef LIBM_DYNAMIC_LOAD_AVX512_H_INCLUDED
#define LIBM_DYNAMIC_LOAD_AVX512_H_INCLUDED

#include <immintrin.h>

typedef __m512d (*func_v8d)    (__m512d);
typedef __m512d (*func_v8d_2)  (__m512d, __m512d);

typedef __m512  (*func_v16s)   (__m512);
typedef __m512  (*funcf_v16s_2)   (__m512, __m512);

#endif
