/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef LIBM_DYNAMIC_LOAD_H_INCLUDED
#define LIBM_DYNAMIC_LOAD_H_INCLUDED

/* select function to dynamically load symbols (lin/win) */
#if defined(_WIN64) || defined(_WIN32)
  #include <Windows.h>
  #define FUNC_LOAD GetProcAddress
#else
  #include <dlfcn.h>
  #define FUNC_LOAD dlsym
#endif
#include <stdlib.h>
#include <stdio.h>
#include "func_types.h"
#include "utils.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

//test functions
int test_exp        (void*);
int test_log        (void*);
int test_pow        (void*);
int test_sin        (void*);
int test_cos        (void*);
int test_tan        (void*);
int test_fabs       (void*);
int test_asin       (void*);
int test_acos       (void*);
int test_atan       (void*);
int test_sinh       (void*);
int test_cosh       (void*);
int test_tanh       (void*);
int test_asinh      (void*);
int test_acosh      (void*);
int test_atanh      (void*);
int test_atan2      (void*);
int test_sqrt       (void*);
int test_nextafter  (void*);
int test_exp10      (void*);
int test_remainder  (void*);
int test_nan        (void*);
int test_floor      (void*);
int test_round      (void*);
int test_rint       (void*);
int test_lround     (void*);
int test_llround    (void*);
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
int test_fdim       (void*);
int test_fmax       (void*);
int test_fmin       (void*);
int test_sincos     (void*);

/*complex*/
int test_cexp       (void*);

int test_erf      (void*);

#endif
