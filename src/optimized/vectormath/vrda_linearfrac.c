/*
 * Copyright (C) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
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
 * void vrda_linearfrac(int length, const double *a, const double *b, double scalea, 
 *                      double shifta, double scaleb, double shiftb, double *result)
 *
 * vrda_linearfrac() computes the linear fraction transformation for 'length' 
 * number of elements present in the 'a' and 'b' arrays.
 * The corresponding output is stored in the 'result' array.
 *
 * Computation: result[i] = (scalea * a[i] + shifta) / (scaleb * b[i] + shiftb)
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * The implementation uses a unified approach that handles both in-place
 * and out-of-place operations. It also optimizes for a special case.
 *
 * Special Case Optimization (when scaleb = 0 and shiftb = 1):
 *     In this case, the denominator becomes (0 * b + 1) = 1
 *     So the computation simplifies to: result[i] = (scalea * a[i] + shifta)
 *
 *     Broadcast scalar values scalea and shifta to 256-bit vectors
 *
 *     If length is greater than or equal to 4:
 *         Save the last 4 elements from array 'a' before processing
 *         Process elements in chunks of 4 (n*4 complete elements):
 *             Load 4 elements from array 'a' into a 256-bit register
 *             Compute transa = (a * scalea) + shifta using FMA instruction
 *             Store the output into result array
 *         Repeat until all complete chunks are processed
 *
 *         For the remaining elements (if any):
 *             Use the pre-saved last 4 elements from array 'a'
 *             Compute transa = (a * scalea) + shifta using FMA instruction
 *             Store the output at the last 4 positions in result array
 *     Return
 *
 *     If length is less than 4:
 *         Create a mask for the actual number of elements
 *         Load elements from array 'a' using masked load
 *         Compute transa = (a * scalea) + shifta using FMA instruction
 *         Store the output using masked store
 *     Return
 *
 * General Case (when the special case condition is not met):
 *     Broadcast scalar values scalea, shifta, scaleb, and shiftb to 256-bit vectors
 *
 *     If length is greater than or equal to 4:
 *         Save the last 4 elements from both arrays 'a' and 'b' before processing
 *         Process elements in chunks of 4 (n*4 complete elements):
 *             Load 4 elements from array 'a' into a 256-bit register
 *             Load 4 elements from array 'b' into a 256-bit register
 *             Compute transa = (a * scalea) + shifta using FMA instruction
 *             Compute transb = (b * scaleb) + shiftb using FMA instruction
 *             Compute result = transa / transb using division instruction
 *             Store the output into result array
 *         Repeat until all complete chunks are processed
 *
 *         For the remaining elements (if any):
 *             Use the pre-saved last 4 elements from arrays 'a' and 'b'
 *             Compute transa = (a * scalea) + shifta using FMA instruction
 *             Compute transb = (b * scaleb) + shiftb using FMA instruction
 *             Compute result = transa / transb using division instruction
 *             Store the output at the last 4 positions in result array
 *     Return
 *
 *     If length is less than 4:
 *         Create a mask for the actual number of elements
 *         Load elements from array 'a' using masked load
 *         Load elements from array 'b' using masked load
 *         Compute transa = (a * scalea) + shifta using FMA instruction
 *         Compute transb = (b * scaleb) + shiftb using FMA instruction
 *         Compute result = transa / transb using division instruction
 *         Store the output using masked store
 *     Return
 *
 */
#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>

void ALM_PROTO_OPT(vrda_linearfrac)(int length, const double *a, const double *b, double scalea, double shifta, double scaleb, double shiftb, double *result)
{
    int j = 0;
    uint64_t scaleb_u = asuint64(scaleb);
    uint64_t shiftb_u = asuint64(shiftb);

    /* Special case where scaleb = 0 and shiftb = 1
       In this case,
       Output = (scalea * a + shifta) / (scaleb * b + shiftb)
              = (scalea * a + shifta) / (0 * b + 1)
              = (scalea * a + shifta) / 1
              = (scalea * a + shifta)
    */
    if(((scaleb_u & ~SIGNBIT_DP64) == 0) && (shiftb_u == POS_ONE_F64))
    {
        /* Broadcast scalar values scalea and shifta to vectors */
        v_f64x4_t scalea_v = _mm256_broadcast_sd(&scalea);
        v_f64x4_t shifta_v = _mm256_broadcast_sd(&shifta);
        v_f64x4_t a_v, transa;

        if(likely(length >= DOUBLE_ELEMENTS_256_BIT))
        {
            /* Save the last 4 elements before processing. This avoids errors when the
               operation is in-place */
            __m256d last_a = _mm256_loadu_pd(&a[length - DOUBLE_ELEMENTS_256_BIT]);
            
            // Process complete chunks of 4 (n*4 elements)
            for (j = 0; j <= length - DOUBLE_ELEMENTS_256_BIT; j += DOUBLE_ELEMENTS_256_BIT)
            {
                a_v = _mm256_loadu_pd(&a[j]);
                /* transa = (a * scalea) + shifta */
                transa = _mm256_fmadd_pd(scalea_v, a_v, shifta_v);
                _mm256_storeu_pd(&result[j], transa);
            }
            
            // Handle remaining elements using the pre-saved last 4 elements
            if (length - j)
            {
                /* transa = (a * scalea) + shifta */
                transa = _mm256_fmadd_pd(scalea_v, last_a, shifta_v);
                _mm256_storeu_pd(&result[length - DOUBLE_ELEMENTS_256_BIT], transa);
            }
            return;
        }
        
        // For length < 4, use masked operations
        __m256i mask = GET_MASK_DOUBLE_256_BIT(length);
        a_v = _mm256_maskload_pd(&a[0], mask);
        /* transa = (a * scalea) + shifta */
        transa = _mm256_fmadd_pd(scalea_v, a_v, shifta_v);
        _mm256_maskstore_pd(&result[0], mask, transa);
    }
    else
    {
        /* Broadcast scalar values scalea, scaleb, shifta and shiftb to vectors*/
        v_f64x4_t scalea_v = _mm256_broadcast_sd(&scalea);
        v_f64x4_t scaleb_v = _mm256_broadcast_sd(&scaleb);
        v_f64x4_t shifta_v = _mm256_broadcast_sd(&shifta);
        v_f64x4_t shiftb_v = _mm256_broadcast_sd(&shiftb);

        if(likely(length >= DOUBLE_ELEMENTS_256_BIT))
        {
            /* Save the last 4 elements from both arrays before processing. This avoids errors
               when the operation is in-place */
            __m256d last_a = _mm256_loadu_pd(&a[length - DOUBLE_ELEMENTS_256_BIT]);
            __m256d last_b = _mm256_loadu_pd(&b[length - DOUBLE_ELEMENTS_256_BIT]);

            v_f64x4_t a_v, b_v, transa, transb, result_v;

            // Process complete chunks of 4 (n*4 elements)
            for (j = 0; j <= length - DOUBLE_ELEMENTS_256_BIT; j += DOUBLE_ELEMENTS_256_BIT)
            {
                a_v = _mm256_loadu_pd(&a[j]);
                b_v = _mm256_loadu_pd(&b[j]);
                /* transa = (a * scalea) + shifta */
                transa = _mm256_fmadd_pd(scalea_v, a_v, shifta_v);
                /* transb = (b * scaleb) + shiftb */
                transb = _mm256_fmadd_pd(scaleb_v, b_v, shiftb_v);
                /* result = (transa / transb) = ((a * scalea) + shifta) / ((b * scaleb) + shiftb)*/
                result_v = _mm256_div_pd(transa, transb);
                _mm256_storeu_pd(&result[j], result_v);
            }
            
            // Handle remaining elements using the pre-saved last 4 elements
            if (length - j)
            {
                /* transa = (a * scalea) + shifta */
                transa = _mm256_fmadd_pd(scalea_v, last_a, shifta_v);
                /* transb = (b * scaleb) + shiftb */
                transb = _mm256_fmadd_pd(scaleb_v, last_b, shiftb_v);
                /* result = (transa / transb) = ((a * scalea) + shifta) / ((b * scaleb) + shiftb)*/
                result_v = _mm256_div_pd(transa, transb);
                _mm256_storeu_pd(&result[length - DOUBLE_ELEMENTS_256_BIT], result_v);
            }
            return;
        }
        
        // For length < 4, use masked operations
        __m256i mask = GET_MASK_DOUBLE_256_BIT(length);
        v_f64x4_t a_v = _mm256_maskload_pd(&a[0], mask);
        v_f64x4_t b_v = _mm256_maskload_pd(&b[0], mask);
        /* transa = (a * scalea) + shifta */
        v_f64x4_t transa = _mm256_fmadd_pd(scalea_v, a_v, shifta_v);
        /* transb = (b * scaleb) + shiftb */
        v_f64x4_t transb = _mm256_fmadd_pd(scaleb_v, b_v, shiftb_v);
        /* result = (transa / transb) = ((a * scalea) + shifta) / ((b * scaleb) + shiftb)*/
        v_f64x4_t result_v = _mm256_div_pd(transa, transb);
        _mm256_maskstore_pd(&result[0], mask, result_v);
    }
}