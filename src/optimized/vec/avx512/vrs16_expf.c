/*
 * Copyright (C) 2019-2026, Advanced Micro Devices. All rights reserved.
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
 * C implementation of exp single precision 512-bit vector version (v16s)
 *
 * Implementation Notes
 * ----------------------
 * 1. Argument Reduction:
 *      e^x = 2^(x/ln2)                          --- (1)
 *
 *      Let x/ln(2) = z                          --- (2)
 *
 *      Let z = n + r , where n is an integer    --- (3)
 *                      |r| <= 1/2
 *
 *     From (1), (2) and (3),
 *      e^x = 2^z
 *          = 2^(N+r)
 *          = (2^N)*(2^r)                        --- (4)
 *
 * 2. Polynomial Evaluation
 *   From (4),
 *     r   = z - N
 *     2^r = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4 + C6*r^5
 *
 * 4. Reconstruction
 *      Thus,
 *        e^x = (2^N) * (2^r)
 *
 *
 */

#include <libm_macros.h>
#include <libm/compiler.h>
#include <libm_util_amd.h>
#include <libm/types.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

#include "vrs16_expf.h"

/*
 * Single definition of the vrs16 expf degree-5 coefficient table (declared
 * extern in vrs16_expf.h).  Defining it here once avoids a duplicate copy in
 * every translation unit that includes the header (e.g. vrs16_coshf.c).
 * Coefficients obtained using the Remez algorithm.
 */
const v_f32x16_t ALM_PROTO_OPT(v16_expf_poly)[5] = {
    _MM512_SET1_PS16(0x1p0f),
    _MM512_SET1_PS16(0x1.fffdc4p-2f),
    _MM512_SET1_PS16(0x1.55543cp-3f),
    _MM512_SET1_PS16(0x1.573aecp-5f),
    _MM512_SET1_PS16(0x1.126bb6p-7f),
};

#define SCALAR_EXPF ALM_PROTO_OPT(expf)

v_f32x16_t
ALM_PROTO_OPT(vrs16_expf)(v_f32x16_t _x)
{
    /* Lanes outside the valid range are fixed up with scalar expf below. */
    v_u32x16_t cond = vrs16_expf_special_mask(_x);

    v_f32x16_t result = vrs16_expf_fastpath(_x);

    /*
     * If input value is outside valid range, call scalar expf(value)
     * Else, return the above computed result
     */

    if(unlikely(any_v16_u32_avx512(cond))) {

        result = call_v16_f32(SCALAR_EXPF, _x, result, cond);

    }

    return result;

}
