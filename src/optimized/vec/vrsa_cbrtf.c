
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
 * void vrsa_cbrtf(int length, float *input, float *result)
 *
 * vrsa_cbrtf() computes the cube root values for 'length' number of elements
 * present in the 'input' array.
 * The corresponding ouput is stored in the 'result' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * For any given length,
 *     If length is greater than 4:
 *         Pack 4 elements of input array into a 128-bit register
 *             call vrs4_cbrtf()
 *         Store the output into result array.
 *         Repeat
 *
 *         For the remaining element/s,
 *         Pack the last 4 elements of input array into a 128-bit register,
 *             call vrs4_cbrtf()
 *         Store the output into result array.
 *     Return
 *
 *     If length is lesser than 4:
 *         For each element in the input array,
 *             call cbrtf()
 *         Store the output into result array.
 * Return
 * 
 * TODO: The above mentioned logic can be enhanced with vrs8_cbrtf() API, once it is implemented. 
 */

#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <stdio.h>
#include <libm_util_amd.h>

void ALM_PROTO_OPT(vrsa_cbrtf)(int length, float *input, float *result)
{
    int j = 0;
    if(likely(length >= FLOAT_ELEMENTS_128_BIT))
    {
        for (j = 0; j <= length - FLOAT_ELEMENTS_128_BIT; j += FLOAT_ELEMENTS_128_BIT)
        {
            __m128 ip4 = _mm_loadu_ps(&input[j]);
            __m128 op4 = ALM_PROTO(vrs4_cbrtf)(ip4);
            _mm_storeu_ps(&result[j], op4);
        }
        if (length - j)
        {
            __m128 ip4 = _mm_loadu_ps(&input[length - FLOAT_ELEMENTS_128_BIT]);
            __m128 op4 = ALM_PROTO(vrs4_cbrtf)(ip4);
            _mm_storeu_ps(&result[length - FLOAT_ELEMENTS_128_BIT], op4);
        }
        return;
    }
    for (j = 0; j < length; ++j)
    {
        result[j] = ALM_PROTO(cbrtf)(input[j]);
    }
}