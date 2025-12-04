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

#define ALM_OVERRIDE 1
#include <libm/arch/zen4.h>

#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>


/********************************************
 * ---------------------
 * Signature
 * ---------------------
 * void vrda_round(int length, double *input, double *result)
 *
 * vrda_round() computes the round values for 'length' number of elements
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
 *             Load 8 elements from input array into a 512-bit register
 *             Call vrd8_round()
 *             Store the output into result array
 *         Repeat until all complete chunks are processed
 *
 *         For the remaining elements (if any):
 *             Use the pre-saved last 8 elements
 *             Call vrd8_round()
 *             Store the output at the last 8 positions in result array
 *     Return
 *
 *     If length is less than 8:
 *         Create a mask for the actual number of elements
 *         Load elements using masked load
 *         Call vrd8_round()
 *         Store the output using masked store
 *     Return
 */

void ALM_PROTO_ARCH_ZN4(vrda_round)(int length, const double *input, double *result)
{
    if (likely(length >= DOUBLE_ELEMENTS_512_BIT))
    {
        /* Save the last 8 elements before processing. This avoids errors when the
           operation is in-place */
        __m512d last_ip8 = _mm512_loadu_pd(&input[length - DOUBLE_ELEMENTS_512_BIT]);

        int j = 0;

        // Process complete chunks of 8 (n*8 elements)
        for (j = 0; j <= length - DOUBLE_ELEMENTS_512_BIT; j += DOUBLE_ELEMENTS_512_BIT)
        {
            __m512d ip8 = _mm512_loadu_pd(&input[j]);
            __m512d op8 = ALM_PROTO(vrd8_round)(ip8);
            _mm512_storeu_pd(&result[j], op8);
        }

        // Handle remaining elements using the pre-saved last 8 elements
        if (length - j)
        {
            __m512d op8 = ALM_PROTO(vrd8_round)(last_ip8);
            _mm512_storeu_pd(&result[length - DOUBLE_ELEMENTS_512_BIT], op8);
        }
        return;
    }

    // For length < 8, use masked operations
    __m512d zero = _mm512_set1_pd(0);
    __mmask8 mask = (__mmask8)(0xFF >> (8 - length));
    __m512d ip8 = _mm512_mask_loadu_pd(zero, mask, &input[0]);
    __m512d op8 = ALM_PROTO(vrd8_round)(ip8);
    _mm512_mask_storeu_pd(&result[0], mask, op8);
}

