/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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

/*
 * Template for AVX2 double-precision array wrappers (vrda_*)
 *
 * TEMPLATE COMPILATION:
 * This file is compiled multiple times with different -DALM_FUNC=<name>
 * definitions to generate vrda_sin, vrda_cos, vrda_exp, etc. The build
 * system (CMake/SCons) loops over a list of functions and compiles this
 * template once per function, passing -DALM_FUNC=sin, -DALM_FUNC=cos, etc.
 * This avoids duplicating nearly identical code for each array wrapper.
 */

#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>
#include <libm/compiler.h>

#ifndef ALM_FUNC
#error "ALM_FUNC must be defined (e.g., -DALM_FUNC=sin)"
#endif

/*
 * ALM_PASTE macro: Token-pasting helper for building function names at compile time.
 * Two levels of indirection are required because the C preprocessor doesn't
 * expand macro arguments when they appear next to ## operator. ALM_PASTE_(a,b)
 * does the actual concatenation, while ALM_PASTE(a,b) forces argument expansion.
 *
 * Example: With -DALM_FUNC=sin, ALM_PASTE(vrda_, ALM_FUNC) expands to vrda_sin.
 */
#define ALM_PASTE_(a, b) a##b
#define ALM_PASTE(a, b)  ALM_PASTE_(a, b)

void ALM_PROTO_OPT(ALM_PASTE(vrda_, ALM_FUNC))(int length, const double* input, double* result)
{
    /*
     * Early exit for zero-length or negative length to avoid undefined behavior
     * when callers pass NULL pointers for zero-length operations.
     */
    if (unlikely(length <= 0))
        return;

    /*
     * TWO CODE PATHS:
     * 1. length >= 4 (DOUBLE_ELEMENTS_256_BIT): Process full vectors in a loop,
     *    handle remainder by overlapping the last vector.
     * 2. length < 4: Use masked load/store to process only valid elements.
     */
    if (likely(length >= DOUBLE_ELEMENTS_256_BIT))
    {
        /*
         * PRE-LOAD LAST VECTOR: Load the last 4 elements before entering the
         * main loop. This allows us to handle the remainder (length % 4 != 0)
         * by simply processing and storing this vector at the end. The overlap
         * with already-processed elements is harmless (we just recompute them)
         * and avoids the need for masked operations in the common case.
         */
        __m256d last_ip4 = _mm256_loadu_pd(&input[length - DOUBLE_ELEMENTS_256_BIT]);

        for (int j = 0; j <= length - DOUBLE_ELEMENTS_256_BIT; j += DOUBLE_ELEMENTS_256_BIT)
        {
            __m256d ip4 = _mm256_loadu_pd(&input[j]);
            __m256d op4 = ALM_PROTO_OPT(ALM_PASTE(vrd4_, ALM_FUNC))(ip4);
            _mm256_storeu_pd(&result[j], op4);
        }

        if ((length % DOUBLE_ELEMENTS_256_BIT) != 0)
        {
            __m256d op4 = ALM_PROTO_OPT(ALM_PASTE(vrd4_, ALM_FUNC))(last_ip4);
            _mm256_storeu_pd(&result[length - DOUBLE_ELEMENTS_256_BIT], op4);
        }
    }
    else
    {
        /*
         * MASKED PATH: For arrays smaller than the vector width, use AVX2
         * masked load/store to process only the valid elements. The mask is
         * computed from the length using GET_MASK_DOUBLE_256_BIT macro.
         */
        __m256i mask = GET_MASK_DOUBLE_256_BIT(length);
        __m256d ip4 = _mm256_maskload_pd(&input[0], mask);
        __m256d op4 = ALM_PROTO_OPT(ALM_PASTE(vrd4_, ALM_FUNC))(ip4);
        _mm256_maskstore_pd(&result[0], mask, op4);
    }
}
