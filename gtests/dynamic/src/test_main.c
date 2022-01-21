/* Copyright (C) 2020, Advanced Micro Devices. All rights reserved
 * Author: Pranoy Jayaraj <pranoy.jayaraj@amd.com>
 * An example code for exercising dynamic loading of the amdlibm
   library
 */

#include "libm_dynamic_load.h"

int main(int argc, char* argv[]) {
    void* handle;
    char* libm_path = NULL;

   if (argc <= 1) {
        printf("Error! Provide .so file path as argument\n");
        return 1;
    }

    libm_path = argv[1];

    #if defined(_WIN64) || defined(_WIN32)
      handle = LoadLibrary("libalm.dll");
      if (!handle) {
          printf("Error! %s\n", GetLastError());
          exit(EXIT_FAILURE);
      }
    #else
      handle = dlopen(libm_path, RTLD_LAZY);
      if (!handle) {
          printf("Error! %s\n", dlerror());
          exit(EXIT_FAILURE);
      }
    #endif

    /* exp log pow */
    test_exp(handle);
    test_exp10(handle);
    test_exp2(handle);
    test_expm1(handle);

    test_log(handle);
    test_log2(handle);
    test_logb(handle);
    test_log10(handle);
    test_log1p(handle);

    test_pow(handle);

    /* Trigonometric */
    test_sin(handle);
    test_cos(handle);
    test_tan(handle);

    /* Inverse trig */
    test_acos(handle);
    test_asin(handle);
    test_atan(handle);
    test_atan2(handle);

    /* Hyperbolic */
    test_sinh(handle);
    test_cosh(handle);
    test_tanh(handle);

    /* Inverse hyperbolic */
    test_asinh(handle);
    test_acosh(handle);
    test_atanh(handle);

    test_sqrt(handle);
    test_nextafter(handle);
    test_lround(handle);
    test_floor(handle);
    test_remainder(handle);
    test_nearbyint(handle);
    test_nexttoward(handle);
    test_nan(handle);
    test_lrint(handle);
    test_llrint(handle);
    test_hypot(handle);
    test_ceil(handle);
    test_scalbln(handle);
    test_ilogb(handle);
    test_modf(handle);
    test_remquo(handle);
    test_ldexp(handle);
    test_frexp(handle);
    test_scalbn(handle);
    test_cbrt(handle);
    test_trunc(handle);
    test_copysign(handle);

    test_fabs(handle);
    test_fdim(handle);
    test_fmax(handle);
    test_fmin(handle);
    test_fmod(handle);
    test_finite(handle);

    test_sincos(handle);

    /*complex*/
    test_cexp(handle);

    #if defined(_WIN64) || defined(_WIN32)
      FreeLibrary(handle);
    #else
      dlclose(handle);
    #endif
    return 0;
}
