
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
 * void vrda_pow(int length, double *input1, double *input2, double *result)
 *
 * vrda_pow() computes the pow values for 'length' number of elements as
 * values in the 'input1' array raised to 'input2' array.
 * The corresponding ouput is stored in the 'result' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * For any given length,
 *     If length is greater than 4:
 *         Pack 4 elements of input arrays into 256-bit registers
 *             call vrd4_pow()
 *         Store the output into result array.
 *         Repeat
 *
 *         For the remaining element/s,
 *         Pack the last 4 elements of input arrays into 256-bit registers,
 *             call vrd4_pow()
 *         Store the output into result array.
 *     Return
 *
 *     If length is lesser than 4:
 *         Pack the elements of input arrays into 256-bit registers
 *         Mask the inputs which are not needed to be computed with a 0.
 *             call vrd4_pow()
 *         Store the output of unmasked elements into result array.
 * Return
 */
#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>
void ALM_PROTO_OPT(vrda_pow)(int length, double *input1, double *input2, double *result)
{
    int j = 0;
    if(likely(length >= DOUBLE_ELEMENTS_256_BIT))
    {
        for (j = 0; j <= length - DOUBLE_ELEMENTS_256_BIT; j += DOUBLE_ELEMENTS_256_BIT)
        {
            __m256d ip14 = _mm256_loadu_pd(&input1[j]);
            __m256d ip24 = _mm256_loadu_pd(&input2[j]);
            __m256d op4 = ALM_PROTO(vrd4_pow)(ip14, ip24);
            _mm256_storeu_pd(&result[j], op4);
        }
        if (length - j)
        {
            __m256d ip14 = _mm256_loadu_pd(&input1[length - DOUBLE_ELEMENTS_256_BIT]);
            __m256d ip24 = _mm256_loadu_pd(&input2[length - DOUBLE_ELEMENTS_256_BIT]);
            __m256d op4 = ALM_PROTO(vrd4_pow)(ip14, ip24);
            _mm256_storeu_pd(&result[length - DOUBLE_ELEMENTS_256_BIT], op4);
        }
        return;
    }
    __m256i mask = GET_MASK_DOUBLE_256_BIT(length);
    __m256d ip14 = _mm256_maskload_pd(&input1[j], mask);
    __m256d ip24 = _mm256_maskload_pd(&input2[j], mask);
    __m256d op4 = ALM_PROTO(vrd4_pow)(ip14, ip24);
    _mm256_maskstore_pd(&result[j], mask, op4);
}
