/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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

#include <stdio.h>
extern int use_exp();
extern int use_pow();
extern int use_log();
extern int use_fabs();
extern int use_atan();
extern int use_sin();
extern int use_cos();
extern int use_tan();
extern int use_cosh();
extern int use_tanh();
extern int use_sinh();
extern int use_exp2();
extern int use_log2();
extern int use_asin();
extern int use_acos();
extern int use_asinh();
extern int use_fmin();

/* trigonometric */
extern int use_cexp();
extern int use_clog();
extern int use_cpow();

/* erf */
extern int use_erf();

/* avx512 */
#if defined (__AVX512__)
extern int use_exp_avx512();
extern int use_log_avx512();
extern int use_log10_avx512();
extern int use_exp2_avx512();
extern int use_pow_avx512();
extern int use_atan_avx512();
extern int use_asin_avx512();
#endif

int main()  {
    printf("Illustration of AOCL LibM functions\n");
    use_exp();
    use_pow();
    use_log();
    use_fabs();
    use_atan();
    use_sin();
    use_cos();
    use_tan();
    use_cosh();
    use_tanh();
    use_sinh();
    use_exp2();
    use_log2();
    use_asin();
    use_acos();
    use_asinh();
    use_cexp();
    use_clog();
    use_cpow();
    use_erf();
    use_fmin();

    #if defined (__AVX512__)
    use_exp_avx512();
    use_exp2_avx512();
    use_pow_avx512();
    use_log_avx512();
    use_log10_avx512();
    use_atan_avx512();
    use_asin_avx512();
    #endif

    return 0;
}
