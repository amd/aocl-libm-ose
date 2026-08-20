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

#include <libm_macros.h>
#include <libm/compiler.h>
#include <libm_util_amd.h>
#include <libm/types.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

#include "../../vrd8_exp_kernel.h"

#define ARG_MAX          v8_exp_kernel_data.exp_max
#define MASK             v8_exp_kernel_data.mask

#define SCALAR_EXP ALM_PROTO_OPT(exp)


/*
 * C implementation of exp double precision 512-bit vector version (v8d)
 *
 * Implementation Notes
 * ----------------------
 * 1. Argument Reduction:
 *      e^x = 2^(x/ln2) = 2^(x*(64/ln(2))/64)     --- (1)
 *
 *      Choose 'n' and 'f', such that
 *      x * 64/ln2 = n + f                        --- (2) | n is integer
 *                            | |f| <= 0.5
 *     Choose 'm' and 'j' such that,
 *      n = (64 * m) + j                          --- (3)
 *
 *     From (1), (2) and (3),
 *      e^x = 2^((64*m + j + f)/64)
 *          = (2^m) * (2^(j/64)) * 2^(f/64)
 *          = (2^m) * (2^(j/64)) * e^(f*(ln(2)/64))
 *
 * 2. Table Lookup
 *      Values of (2^(j/64)) are precomputed, j = 0, 1, 2, 3 ... 63
 *
 * 3. Polynomial Evaluation
 *   From (2),
 *     f = x*(64/ln(2)) - n
 *   Let,
 *     r  = f*(ln(2)/64) = x - n*(ln(2)/64)
 *
 * 4. Reconstruction
 *      Thus,
 *        e^x = (2^m) * (2^(j/64)) * e^r
 *
 */

v_f64x8_t
ALM_PROTO_OPT(vrd8_exp)(v_f64x8_t x)
{

    v_u64x8_t vx = as_v8_u64_f64(x);

    // Get absolute value
    vx = vx & MASK;

    // Check if -709 < vx < 709
    v_u64x8_t cond = (vx > ARG_MAX );

    // Branch-free, domain-check-free reconstruction
    v_f64x8_t ret = vrd8_exp_fastpath(x);

    if(unlikely(any_v8_u64_avx512(cond))) {

        return (v_f64x8_t) {
            (cond[0]) ? SCALAR_EXP(x[0]):ret[0],
            (cond[1]) ? SCALAR_EXP(x[1]):ret[1],
            (cond[2]) ? SCALAR_EXP(x[2]):ret[2],
            (cond[3]) ? SCALAR_EXP(x[3]):ret[3],
            (cond[4]) ? SCALAR_EXP(x[4]):ret[4],
            (cond[5]) ? SCALAR_EXP(x[5]):ret[5],
            (cond[6]) ? SCALAR_EXP(x[6]):ret[6],
            (cond[7]) ? SCALAR_EXP(x[7]):ret[7],
        };

    }

    return ret;
}
