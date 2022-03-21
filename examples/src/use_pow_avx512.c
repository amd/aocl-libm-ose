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

#define AMD_LIBM_VEC_EXPERIMENTAL
#include <stdio.h>
#include "amdlibm.h"
#include "amdlibm_vec.h"
#include <immintrin.h>

int use_pow_avx512() {
    #if defined (__AVX512__)
    int i=0;
    printf ("\nUsing vrd8 (Double Precision vector 8 variant) of AMD pow()\n");
    __m512d input_vrd8, result_pow_vrd8;
    double input_array_vrd8[8] = {2.3, 4.5, 56.5, 43.4, 45.0, 44.3, 32.3, 21.2};
    double output_array_vrd8[8];
    input_vrd8 = _mm512_loadu_pd(input_array_vrd8);
    result_pow_vrd8 = amd_vrd8_pow(input_vrd8, input_vrd8);
    _mm512_storeu_pd(output_array_vrd8, result_pow_vrd8);
    printf ("Input: {");
    for(i=0; i<8; i++) {
        printf ("%lf,", input_array_vrd8[i]);
    }
    printf ("}, Output: {");
    for (i=0; i<8; i++) {
        printf ("%lf,", output_array_vrd8[i]);
    }

    printf ("\nUsing vrs16 (Single precision vector 16 element variant of AMD powf()\n");
    __m512 input_vrs16, result_pow_vrs16;
    float input_array_vrs16[16] = {1.2, 0.0, 2.3, 3.4, 5.6, 7.8, 8.9, 1.0,
                                   1.2, 0.0, 2.3, 3.4, 5.6, 7.8, 8.9, 1.0};
    float output_array_vrs16[16];
    input_vrs16 = _mm512_loadu_ps(input_array_vrs16);
    result_pow_vrs16 = amd_vrs16_powf(input_vrs16, input_vrs16);
    _mm512_storeu_ps(output_array_vrs16, result_pow_vrs16);
    printf ("Input: {");
    for (i=0; i<16; i++) {
        printf ("%f,",input_array_vrs16[i]);
    }
    printf("}, Output: {");
    for (i=0; i<16; i++) {
        printf ("%f,", output_array_vrs16[i]);
    }

    #endif
    return 0;
}
