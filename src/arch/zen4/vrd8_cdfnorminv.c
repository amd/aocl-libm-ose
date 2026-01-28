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
 * C implementation of cdfnorminv double precision 512-bit vector version (vrd8)
 *
 * Signature:
 *    v_f64x8_t amd_vrd8_cdfnorminv(v_f64x8_t x)
 *
 * Initial implementation: scalar fallback for functional correctness.
 * TODO: Replace with optimized vector implementation.
 */

#include <stdint.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/amd_funcs_internal.h>
#include <libm/arch/zen4.h>

#define SCALAR_CDFNORMINV ALM_PROTO_OPT(cdfnorminv)

v_f64x8_t
ALM_PROTO_ARCH_ZN4(vrd8_cdfnorminv)(v_f64x8_t _x) {
    v_f64x8_t result;
    for (uint64_t i = 0; i < 8; i++) {
        result[i] = SCALAR_CDFNORMINV(_x[i]);
    }
    return result;
}
