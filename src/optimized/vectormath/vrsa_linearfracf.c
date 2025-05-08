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

/*
C implementation of Linearfrac

Signature:
    void vrsa_linearfracf(int length, float *a, float *b, float *scalea, float *shifta, float *scaleb, float *shiftb, float *result)

Implementation notes:

    Linearfrac function performs a linear fraction transformation of vector a by vector b
    with scalar parameters
    y[i] = (scalea.a[i]+shifta)/(scaleb.b[i]+shiftb)

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

void ALM_PROTO_OPT(vrsa_linearfracf)(int length, float *a, float *b, float scalea, float shifta, float scaleb, float shiftb, float *result)
{
    int remainder, j = 0;
    uint32_t scaleb_u = asuint32(scaleb);
    uint32_t shiftb_u = asuint32(shiftb);

    /* Special case where scaleb = 0 and shiftb = 1
       In this case,
       Output = (scalea * a + shifta) / (scaleb * b + shiftb)
              = (scalea * a + shifta) / (0 * b + 1)
              = (scalea * a + shifta) / 1
              = (scalea * a + shifta)
    */
    if(((scaleb_u & ~SIGNBIT_SP32) == 0) && (shiftb_u == POS_ONE_F32))
    {
        /* Broadcast scalar values scalea and scaleb to vectors */
        v_f32x8_t scalea_v = _mm256_broadcast_ss(&scalea);
        v_f32x8_t shifta_v = _mm256_broadcast_ss(&shifta);
        v_f32x8_t a_v, transa;

        if(likely(length >= FLOAT_ELEMENTS_256_BIT))
        {
            for (j = 0; j <= length - FLOAT_ELEMENTS_256_BIT; j += FLOAT_ELEMENTS_256_BIT)
            {
                a_v = _mm256_loadu_ps(&a[j]);
                /* transa = (a * scalea) + shifta */
                transa = _mm256_fmadd_ps(scalea_v, a_v, shifta_v);
                _mm256_storeu_ps(&result[j], transa);
            }
        }    
        remainder = length - j;
        if (remainder)
        {
            __m256i mask = GET_MASK_FLOAT_256_BIT(remainder);
            a_v = _mm256_maskload_ps(&a[j], mask);
            /* transa = (a * scalea) + shifta */
            transa = _mm256_fmadd_ps(scalea_v, a_v, shifta_v);
            _mm256_maskstore_ps(&result[j], mask, transa);
        }
    }
    else
    {
        /* Broadcast scalar values scalea, scaleb, shifta and shiftb to vectors*/
        v_f32x8_t scalea_v = _mm256_broadcast_ss(&scalea);
        v_f32x8_t scaleb_v = _mm256_broadcast_ss(&scaleb);
        v_f32x8_t shifta_v = _mm256_broadcast_ss(&shifta);
        v_f32x8_t shiftb_v = _mm256_broadcast_ss(&shiftb);
        v_f32x8_t a_v, b_v, transa, transb, result_v;

        if(likely(length >= FLOAT_ELEMENTS_256_BIT))
        {
            
            for (j = 0; j <= length - FLOAT_ELEMENTS_256_BIT; j += FLOAT_ELEMENTS_256_BIT)
            {
                a_v = _mm256_loadu_ps(&a[j]);
                b_v = _mm256_loadu_ps(&b[j]);
                /* transa = (a * scalea) + shifta */
                transa = _mm256_fmadd_ps(scalea_v, a_v, shifta_v);
                /* transb = (b * scaleb) + shiftb */
                transb = _mm256_fmadd_ps(scaleb_v, b_v, shiftb_v);
                /* result = (transa / transb) = ((a * scalea) + shifta) / ((b * scaleb) + shiftb)*/
                result_v = _mm256_div_ps(transa, transb);
                _mm256_storeu_ps(&result[j], result_v);
            }
        }
        remainder = length - j;
        if (remainder)
        {
            __m256i mask = GET_MASK_DOUBLE_256_BIT(length);
            a_v = _mm256_maskload_ps(&a[j], mask);
            b_v = _mm256_maskload_ps(&b[j], mask);
            /* transa = (a * scalea) + shifta */
            transa = _mm256_fmadd_ps(scalea_v, a_v, shifta_v);
            /* transb = (b * scaleb) + shiftb */
            transb = _mm256_fmadd_ps(scaleb_v, b_v, shiftb_v);
            /* result = (transa / transb) = ((a * scalea) + shifta) / ((b * scaleb) + shiftb)*/
            result_v = _mm256_div_ps(transa, transb);
            _mm256_maskstore_ps(&result[j], mask, result_v);
        }
    }

}
