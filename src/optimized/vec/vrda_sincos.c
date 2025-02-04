/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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
 * void vrda_sincos(int length, double *input, double *sin, double *cos)
 *
 * vrda_sincos() computes the sine and cosine values for 'length' number of elements
 * present in the 'input' array.
 * The corresponding sine ouput is stored in the 'sin' array.
 * The corresponding cosine ouput is stored in the 'cos' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * For any given length,
 *     If length is greater than 4:
 *         Pack 4 elements of input arrays into 256-bit registers
 *             call vrd4_sincos()
 *         Store the output into result array.
 *         Repeat
 *
 *     If length is lesser than 4:
 *         Pack the elements of input arrays into 256-bit registers
 *         Mask the inputs which are not needed to be computed with a 0.
 *             call vrd4_sincos()
 *         Store the output of unmasked elements into result array.
 * Return
 */

#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>

void ALM_PROTO_OPT(vrda_sincos)(int length, double *x, double *sin, double *cos)
{
    int j = 0;
    __m256d opsin, opcos;

    if(likely(length >= DOUBLE_ELEMENTS_256_BIT))
    {
        for (j = 0; j <= length - DOUBLE_ELEMENTS_256_BIT; j += DOUBLE_ELEMENTS_256_BIT)
        {
            __m256d ip4 = _mm256_loadu_pd(&x[j]);
            ALM_PROTO(vrd4_sincos)(ip4, &opsin, &opcos);
            _mm256_storeu_pd(&sin[j], opsin);
            _mm256_storeu_pd(&cos[j], opcos);
        }
    }
    int remainder = length -j;
    if(remainder)
    {
        __m256i mask = GET_MASK_DOUBLE_256_BIT(remainder);
        __m256d ip4 = _mm256_maskload_pd(&x[j], mask);
        ALM_PROTO(vrd4_sincos)(ip4, &opsin, &opcos);
        _mm256_maskstore_pd(&sin[j], mask, opsin);
        _mm256_maskstore_pd(&cos[j], mask, opcos);
    }
}
