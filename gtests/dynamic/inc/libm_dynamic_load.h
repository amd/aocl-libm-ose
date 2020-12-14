#ifndef INCLUDE_H_INCLUDED
#define INCLUDE_H_INCLUDED

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

//test functions
extern int test_exp(void*);
extern int test_log(void*);
extern int test_pow(void*);
extern int test_sin(void*);
extern int test_cos(void*);
extern int test_tan(void*);
extern int test_fabs(void*);
extern int test_atan(void*);
extern int test_sinh(void*);
extern int test_cosh(void*);
extern int test_tanh(void*);
extern int test_atan2(void*);
extern int test_sqrt(void*);
extern int test_nextafter(void*);
extern int test_exp10(void*);
extern int test_remainder(void*);
extern int test_asinh(void*);
extern int test_floor(void*);
extern int test_lround(void*);
extern int test_nearbyint(void*);
extern int test_exp2(void*);
extern int test_fmod(void*);
extern int test_finite(void*);

#endif
