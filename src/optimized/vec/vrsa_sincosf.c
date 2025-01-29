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
 * void vrsa_sincosf(int length, float *input, float *sin, float *cos)
 *
 * vrsa_sincosf() computes the sine and cosine values for 'length' number of elements
 * present in the 'input' array.
 * The corresponding sine ouput is stored in the 'sin' array.
 * The corresponding cosine ouput is stored in the 'cos' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * For any given length,
 *     If length is greater than 8:
 *         Pack 8 elements of input array into a 256-bit register
 *             call vrs8_sincosf()
 *         Store the output into sin and cos array.
 *         Repeat
 *     Return
 *
 *     If length is lesser than 8:
 *         Pack the elements of input array into a 256-bit register
 *         Mask the inputs which are not needed to be computed with a 0.
 *             call vrs8_sincosf()
 *         Store the output of unmasked elements into result array.
 * Return
 */

#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>

void ALM_PROTO_OPT(vrsa_sincosf)(int length, float *x, float *sin, float *cos)
{
    int j = 0;
    __m256 opsin, opcos;

    if(likely(length >= FLOAT_ELEMENTS_256_BIT))
    {
        for (j = 0; j <= length - FLOAT_ELEMENTS_256_BIT; j += FLOAT_ELEMENTS_256_BIT)
        {
            __m256 ip8 = _mm256_loadu_ps(&x[j]);
            ALM_PROTO(vrs8_sincosf)(ip8, &opsin, &opcos);
            _mm256_storeu_ps(&sin[j], opsin);
            _mm256_storeu_ps(&cos[j], opcos);
        }
    }
    int remainder = length -j;
    if(remainder)
    {
        __m256i mask = GET_MASK_FLOAT_256_BIT(remainder);
        __m256 ip8 = _mm256_maskload_ps(&x[j], mask);
        ALM_PROTO(vrs8_sincosf)(ip8, &opsin, &opcos);
        _mm256_maskstore_ps(&sin[j], mask, opsin);
        _mm256_maskstore_ps(&cos[j], mask, opcos);
    }
}
