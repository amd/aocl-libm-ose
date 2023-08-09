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
 *   v_f32x16_t vrs16_tanhf(v_f32x16_t x)
 *
 ******************************************
 * Implementation Notes
 * ----------------------
 * To compute vrs8_tanhf(v_f32x16_t x)
 * Split the 16-element vector into two 8-element vectors
 * For each 8-element vector, call vrs8_tanhf
 *
 * Algorithm to compute tanhf is as follows:
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
#include <immintrin.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>

#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <libm/arch/zen4.h>


v_f32x16_t
ALM_PROTO_ARCH_ZN4(vrs16_tanhf)(v_f32x16_t x)
{

    v_f32x8_t _x[2], op[2];

    /* Split the 8-element vector to two 4-element vectors */
    _x[0] = (v_f32x8_t){x[0], x[1], x[2], x[3],
                        x[4], x[5], x[6], x[7]};

    _x[1] = (v_f32x8_t){x[8], x[9], x[10], x[11],
                        x[12], x[13], x[14], x[15]};

    /* Call vrs8_tanhf for each 8-element vector */
    for(int i = 0; i < 2; i++)
    {
        op[i] = ALM_PROTO(vrs8_tanhf)(_x[i]);
    }

    /* Combine the results to one 8-element vector */
    v_f32x16_t result = _mm512_setr_ps(op[0][0], op[0][1], op[0][2], op[0][3], 
                                      op[0][4], op[0][5], op[0][6], op[0][7],
                                      op[1][0], op[1][1], op[1][2], op[1][3],
                                      op[1][4], op[1][5], op[1][6], op[1][7]);

    return result;


}

