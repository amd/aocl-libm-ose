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
extern int test_cosh(void*);
extern int test_tanh(void*);
extern int test_atan2(void*);

#endif
