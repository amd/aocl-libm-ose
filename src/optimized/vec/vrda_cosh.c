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
 * void vrda_cosh(int length, double *input, double *output)
 *
 * vrda_cosh() computes the hyperbolic cosine values for 'length' number
 * of elements present in the 'input' array.
 * The ouput is stored in the 'output' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * For any given length,
 *     If length is greater than 2:
 *         Pack 2 elements of input array into a 128-bit register
 *             call vrd2_cosh()
 *         Store the output into output array.
 *         Repeat
 *     Return
 *
 *     If length is lesser than 2:
 *         Pack the elements of input array into a 128-bit register
 *         Mask the inputs which are not needed to be computed with a 0.
 *             call vrd2_cosh()
 *         Store the output of unmasked elements into result array.
 * Return
 */

#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>

void ALM_PROTO_OPT(vrda_cosh)(int length, double *input, double *output)
{
    int j = 0;
    __m128d  opcosh;

    if(likely(length >= DOUBLE_ELEMENTS_128_BIT))
    {
        for (j = 0; j <= length - DOUBLE_ELEMENTS_128_BIT; j += DOUBLE_ELEMENTS_128_BIT)
        {
            __m128d ip2 = _mm_loadu_pd(&input[j]);
            opcosh = ALM_PROTO(vrd2_cosh)(ip2);
            _mm_storeu_pd(&output[j], opcosh);
        }
    }
    int remainder = length -j;
    if(remainder == 1)
    {
        __m128i mask =  _mm_set_epi64x(0, -1);
        __m128d ip2 = _mm_maskload_pd(&input[j], mask);
        opcosh = ALM_PROTO(vrd2_cosh)(ip2);
        _mm_maskstore_pd(&output[j], mask, opcosh);
    }
}
