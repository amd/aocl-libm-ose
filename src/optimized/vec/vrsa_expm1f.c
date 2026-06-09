/*
 * Copyright (C) 2024-2026 Advanced Micro Devices, Inc. All rights reserved.
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
 * void vrsa_expm1f(int length, const float *input, float *result)
 *
 * vrsa_expm1f() computes the expm1 values for 'length' number of elements
 * present in the 'input' array.
 * The corresponding ouput is stored in the 'result' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * The implementation uses a unified approach that handles both in-place
 * and out-of-place operations:
 *
 *     If length is greater than or equal to 4:
 *         Save the last 4 elements from input array before processing
 *         Process elements in chunks of 4 (n*4 complete elements):
 *             Load 4 elements from input array into a 128-bit register
 *             Call vrs4_expm1f()
 *             Store the output into result array
 *         Repeat until all complete chunks are processed
 *
 *         For the remaining elements (if any):
 *             Use the pre-saved last 4 elements
 *             Call vrs4_expm1f()
 *             Store the output at the last 4 positions in result array
 *     Return
 *
 *     If length is less than 4:
 *         Fallback to scalar: for each element call expm1f()
 *     Return
 *
 * TODO: The above mentioned logic can be enhanced with vrs8_expm1f() API, once it is implemented.
 */
#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <stdio.h>
#include <libm_util_amd.h>

void ALM_PROTO_OPT(vrsa_expm1f)(int length, const float *input, float *result)
{
    if (likely(length >= FLOAT_ELEMENTS_128_BIT))
    {
        /* Save the last 4 elements before processing. This avoids errors when the
           operation is in-place */
        __m128 last_ip4 = _mm_loadu_ps(&input[length - FLOAT_ELEMENTS_128_BIT]);

        int j = 0;

        // Process complete chunks of 4 (n*4 elements)
        for (j = 0; j <= length - FLOAT_ELEMENTS_128_BIT; j += FLOAT_ELEMENTS_128_BIT)
        {
            __m128 ip4 = _mm_loadu_ps(&input[j]);
            __m128 op4 = ALM_PROTO(vrs4_expm1f)(ip4);
            _mm_storeu_ps(&result[j], op4);
        }

        // Handle remaining elements using the pre-saved last 4 elements
        if (length - j)
        {
            __m128 op4 = ALM_PROTO(vrs4_expm1f)(last_ip4);
            _mm_storeu_ps(&result[length - FLOAT_ELEMENTS_128_BIT], op4);
        }
        return;
    }

    // For length < 4, fallback to scalar
    for (int j = 0; j < length; ++j)
    {
        result[j] = ALM_PROTO(expm1f)(input[j]);
    }
}
