
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

/********************************************
 * ---------------------
 * Signature
 * ---------------------
 * void vrsa_cosf(int length, float *input, float *result)
 *
 * vrsa_cosf() computes the cosine values for 'length' number of elements
 * present in the 'input' array.
 * The corresponding ouput is stored in the 'result' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * For any given length,
 *     If length is greater than 8:
 *         Pack 8 elements of input array into a 256-bit register
 *             call vrs8_cosf()
 *         Store the output into result array.
 *         Repeat
 *
 *         For the remaining element/s,
 *         Pack the last 8 elements of input array into a 256-bit register,
 *             call vrs8_cosf()
 *         Store the output into result array.
 *     Return
 *
 *     If length is lesser than 8:
 *         Pack the elements of input array into a 256-bit register
 *         Mask the inputs which are not needed to be computed with a 0.
 *             call vrs8_cosf()
 *         Store the output of unmasked elements into result array.
 * Return
 */


#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <stdio.h>
#include <libm_util_amd.h>

void ALM_PROTO_OPT(vrsa_cosf)(int length, float *input, float *result)
{
    int j = 0;
    if(likely(length >= FLOAT_ELEMENTS_256_BIT))
    {
        for (j = 0; j <= length - FLOAT_ELEMENTS_256_BIT; j += FLOAT_ELEMENTS_256_BIT)
        {
            __m256 ip8 = _mm256_loadu_ps(&input[j]);
            __m256 op8 = ALM_PROTO(vrs8_cosf)(ip8);
            _mm256_storeu_ps(&result[j], op8);
        }
        if (length - j)
        {
            __m256 ip8 = _mm256_loadu_ps(&input[length - FLOAT_ELEMENTS_256_BIT]);
            __m256 op8 = ALM_PROTO(vrs8_cosf)(ip8);
            _mm256_storeu_ps(&result[length - FLOAT_ELEMENTS_256_BIT], op8);
        }
        return;
    }
    __m256i mask = GET_MASK_FLOAT_256_BIT(length);
    __m256 ip8 = _mm256_maskload_ps(&input[j], mask);
    __m256 op8 = ALM_PROTO(vrs8_cosf)(ip8);
    _mm256_maskstore_ps(&result[j], mask, op8);
}
