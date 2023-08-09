/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
 * Signature:
 *   v_f32x8_t vrs8_tanhf(v_f32x8_t x)
 *
 ******************************************
 * Implementation Notes
 * ----------------------
 * To compute vrs8_tanhf(v_f32x8_t x)
 * Split the 8-element vector into two 4-element vectors
 * For each 4-element vector, call vrs4_tanhf
 *
 * vrs4_tanhf works as follows:
 * Let y = |x|
 * If 0 <= y < 0x1.154246p3
 *    Let z = e^(-2.0 * y) - 1      -(1)
 *
 *    Using (1), tanhf(y) can be calculated as,
 *    tanhf(y) = -z / (z + 2.0)
 *
 * For other cases, call scalar tanhf()
 *
 * If x < 0, then we use the identity
 *       tanhf(-x) = -tanhf(x)
 *
 * Max ULP of current implementation: 1
 *
 */

#include <stdint.h>
#include <emmintrin.h>

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>

#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>


v_f32x8_t
ALM_PROTO_OPT(vrs8_tanhf)(v_f32x8_t x)
{

    v_f32x4_t _x[2], op[2];

    /* Split the 8-element vector to two 4-element vectors */
    _x[0] = _mm256_extractf128_ps(x, 0);
    _x[1] = _mm256_extractf128_ps(x, 1);

    /* Call vrs4_tanhf for each 4-element vector */
    for(int i = 0; i < 2; i++)
    {
        op[i] = ALM_PROTO(vrs4_tanhf)(_x[i]);
    }

    /* Combine the results to one 8-element vector */
    v_f32x8_t result = _mm256_set_m128(op[1], op[0]);

    return result;


}

