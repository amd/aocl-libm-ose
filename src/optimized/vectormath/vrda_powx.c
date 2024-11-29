/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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

/*
C implementation of vector array version of powx

Signature:
    void vrda_powx(int length, double *x, double y, double *result)

Implementation notes:

    A variation of power function - Computes vector raised to a scalar power
    powx(x,y) = e^(y * log(x))

*/

#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>
#include <stdio.h>

void ALM_PROTO_OPT(vrda_powx)(int length, double *x, double y, double *result)
{
    int j = 0;
    int remainder = length % DOUBLE_ELEMENTS_256_BIT;

    if(likely(length >= DOUBLE_ELEMENTS_256_BIT))
    {
        for (j = 0; j <= length - DOUBLE_ELEMENTS_256_BIT; j += DOUBLE_ELEMENTS_256_BIT)
        {
            __m256d ip41 = _mm256_loadu_pd(&x[j]);
            __m256d op4 = ALM_PROTO(vrd4_powx)(ip41, y);
            _mm256_storeu_pd(&result[j], op4);
        }
    }
    remainder = length -j;

    if(remainder)
    {
        __m256i mask = GET_MASK_DOUBLE_256_BIT(remainder);
        __m256d ip41 = _mm256_maskload_pd(&x[j], mask);
        __m256d op4 = ALM_PROTO(vrd4_powx)(ip41, y);
        _mm256_maskstore_pd(&result[j], mask, op4);
    }
}
