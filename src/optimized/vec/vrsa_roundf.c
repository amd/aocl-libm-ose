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

#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>

 /********************************************
 * ---------------------
 * Signature
 * ---------------------
 * void vrsa_roundf(int length, const float *input, float *result)
 *
 * vrsa_roundf() computes the roundf values for 'length' number of elements
 * present in the 'input' array.
 * The corresponding output is stored in the 'result' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * The implementation uses a unified approach that handles both in-place
 * and out-of-place operations:
 *
 *     If length is greater than or equal to 8:
 *         Save the last 8 elements from input array before processing
 *         Process elements in chunks of 8 (n*8 complete elements):
 *             Load 8 elements from input array into a 256-bit register
 *             Call vrs8_roundf()
 *             Store the output into result array
 *         Repeat until all complete chunks are processed
 *
 *         For the remaining elements (if any):
 *             Use the pre-saved last 8 elements
 *             Call vrs8_roundf()
 *             Store the output at the last 8 positions in result array
 *     Return
 *
 *     If length is less than 8:
 *         Create a mask for the actual number of elements
 *         Load elements using masked load
 *         Call vrs8_roundf()
 *         Store the output using masked store
 *     Return
 */

void ALM_PROTO_OPT(vrsa_roundf)(int length, const float *input, float *result)
{
    if (likely(length >= FLOAT_ELEMENTS_256_BIT))
    {
        /* Save the last 8 elements before processing. This avoids errors when the
           operation is in-place */
        __m256 last_ip8 = _mm256_loadu_ps(&input[length - FLOAT_ELEMENTS_256_BIT]);

        int j = 0;

        // Process complete chunks of 8 (n*8 elements)
        for (j = 0; j <= length - FLOAT_ELEMENTS_256_BIT; j += FLOAT_ELEMENTS_256_BIT)
        {
            __m256 ip8 = _mm256_loadu_ps(&input[j]);
            __m256 op8 = ALM_PROTO(vrs8_roundf)(ip8);
            _mm256_storeu_ps(&result[j], op8);
        }

        // Handle remaining elements using the pre-saved last 8 elements
        if (length - j)
        {
            __m256 op8 = ALM_PROTO(vrs8_roundf)(last_ip8);
            _mm256_storeu_ps(&result[length - FLOAT_ELEMENTS_256_BIT], op8);
        }
        return;
    }

    // For length < 8, use masked operations
    __m256i mask = GET_MASK_FLOAT_256_BIT(length);
    __m256 ip8 = _mm256_maskload_ps(&input[0], mask);
    __m256 op8 = ALM_PROTO(vrs8_roundf)(ip8);
    _mm256_maskstore_ps(&result[0], mask, op8);
}