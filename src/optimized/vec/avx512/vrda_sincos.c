/*
 * Copyright (C) 2025-2026 Advanced Micro Devices, Inc. All rights reserved.
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

/********************************************
 * ---------------------
 * Signature
 * ---------------------
 * void vrda_sincos(int length, const double *x, double *sin, double *cos)
 *
 * vrda_sincos() computes the sine and cosine values for 'length' number of elements
 * present in the 'x' array.
 * The corresponding sine ouput is stored in the 'sin' array.
 * The corresponding cosine ouput is stored in the 'cos' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * For any given length,
 *     If length is greater than 8:
 *         Pack 8 elements of input arrays into 512-bit registers
 *             call vrd8_sincos()
 *         Store the output into result array.
 *         Repeat
 *
 *     If length is lesser than 8:
 *         Pack the elements of input arrays into 512-bit registers
 *         Mask the inputs which are not needed to be computed with a 0.
 *             call vrd8_sincos()
 *         Store the output of unmasked elements into result array.
 * Return
 */

#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>

void ALM_PROTO_OPT(vrda_sincos)(int length, const double *x, double *sin, double *cos)
{
    int j = 0;
    __m512d opsin, opcos;

    if(likely(length >= DOUBLE_ELEMENTS_512_BIT))
    {
        for (j = 0; j <= length - DOUBLE_ELEMENTS_512_BIT; j += DOUBLE_ELEMENTS_512_BIT)
        {
            __m512d ip8 = _mm512_loadu_pd(&x[j]);
            ALM_PROTO_OPT(vrd8_sincos)(ip8, &opsin, &opcos);
            _mm512_storeu_pd(&sin[j], opsin);
            _mm512_storeu_pd(&cos[j], opcos);
        }
    }
    int remainder = length -j;
    if(remainder)
    {
        __m512d zero = _mm512_set1_pd(0);
        __mmask8 mask = (__mmask8)(0xFF >> ( 8 - remainder ));
        __m512d ip8 = _mm512_mask_loadu_pd(zero, mask, &x[j]);
        ALM_PROTO_OPT(vrd8_sincos)(ip8, &opsin, &opcos);
        _mm512_mask_storeu_pd(&sin[j], mask, opsin);
        _mm512_mask_storeu_pd(&cos[j], mask, opcos);
    }
}
