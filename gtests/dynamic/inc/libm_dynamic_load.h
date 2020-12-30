#ifndef INCLUDE_H_INCLUDED
#define INCLUDE_H_INCLUDED

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

/*utlity functions*/
float  GenerateRangeFloat   (float  min,  float  max);
double GenerateRangeDouble  (double min,  double max);

//test functions
int test_exp        (void*);
int test_log        (void*);
int test_pow        (void*);
int test_sin        (void*);
int test_cos        (void*);
int test_tan        (void*);
int test_fabs       (void*);
int test_atan       (void*);
int test_sinh       (void*);
int test_cosh       (void*);
int test_tanh       (void*);
int test_atan2      (void*);
int test_sqrt       (void*);
int test_nextafter  (void*);
int test_exp10      (void*);
int test_remainder  (void*);
int test_nan        (void*);
int test_asinh      (void*);
int test_floor      (void*);
int test_lround     (void*);
int test_nearbyint  (void*);
int test_exp2       (void*);
int test_fmod       (void*);
int test_finite     (void*);
int test_nexttoward (void*);
int test_lrint      (void*);
int test_llrint     (void*);
int test_hypot      (void*);
int test_ceil       (void*);
int test_log2       (void*);
int test_logb       (void*);
int test_log10      (void*);
int test_scalbln    (void*);
int test_ilogb      (void*);
int test_log1p      (void*);
int test_modf       (void*);
int test_remquo     (void*);
int test_ldexp      (void*);
int test_frexp      (void*);
int test_scalbn     (void*);
int test_cbrt       (void*);
int test_expm1      (void*);
int test_trunc      (void*);
int test_copysign   (void*);

#endif
