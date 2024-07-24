/*
 * Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
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
#include <libm/amd_funcs_internal.h>
#include <libm/iface.h>
#include <libm/entry_pt.h>


#include <libm/arch/all.h>


static const
struct alm_arch_funcs __arch_funcs_tanh = {
    .def_arch = ALM_UARCH_VER_DEFAULT,
    .funcs = {
        [ALM_UARCH_VER_DEFAULT] = {
            [ALM_FUNC_SCAL_SP] = &ALM_PROTO_ARCH_AVX2(tanhf),
            [ALM_FUNC_SCAL_DP] = &ALM_PROTO_ARCH_AVX2(tanh),
            [ALM_FUNC_VECT_SP_4] = &ALM_PROTO_ARCH_AVX2(vrs4_tanhf), /* v4s */
            [ALM_FUNC_VECT_SP_8] = &ALM_PROTO_ARCH_AVX2(vrs8_tanhf), /* v8s ? */
            [ALM_FUNC_VECT_SP_16] = &ALM_PROTO_ARCH_ZN4(vrs16_tanhf), /* v16s */
        },

        [ALM_UARCH_VER_ZEN] = {
            [ALM_FUNC_SCAL_SP] = &ALM_PROTO_ARCH_ZN(tanhf),
            [ALM_FUNC_SCAL_DP] = &ALM_PROTO_ARCH_ZN(tanh),
            [ALM_FUNC_VECT_SP_4] = &ALM_PROTO_ARCH_ZN(vrs4_tanhf),
            [ALM_FUNC_VECT_SP_8] = &ALM_PROTO_ARCH_ZN(vrs8_tanhf),
        },

        [ALM_UARCH_VER_ZEN2] = {
            [ALM_FUNC_SCAL_SP] = &ALM_PROTO_ARCH_ZN2(tanhf),
            [ALM_FUNC_SCAL_DP] = &ALM_PROTO_ARCH_ZN2(tanh),
            [ALM_FUNC_VECT_SP_4] = &ALM_PROTO_ARCH_ZN2(vrs4_tanhf),
            [ALM_FUNC_VECT_SP_8] = &ALM_PROTO_ARCH_ZN2(vrs8_tanhf),
        },

        [ALM_UARCH_VER_ZEN3] = {
            [ALM_FUNC_SCAL_SP] = &ALM_PROTO_ARCH_ZN3(tanhf),
            [ALM_FUNC_SCAL_DP] = &ALM_PROTO_ARCH_ZN3(tanh),
            [ALM_FUNC_VECT_SP_4] = &ALM_PROTO_ARCH_ZN3(vrs4_tanhf),
            [ALM_FUNC_VECT_SP_8] = &ALM_PROTO_ARCH_ZN3(vrs8_tanhf),
        },

        [ALM_UARCH_VER_ZEN4] = {
            [ALM_FUNC_SCAL_SP] = &ALM_PROTO_ARCH_ZN4(tanhf),
            [ALM_FUNC_SCAL_DP] = &ALM_PROTO_ARCH_ZN4(tanh),
            [ALM_FUNC_VECT_SP_4] = &ALM_PROTO_ARCH_ZN4(vrs4_tanhf),
            [ALM_FUNC_VECT_SP_8] = &ALM_PROTO_ARCH_ZN4(vrs8_tanhf),
            [ALM_FUNC_VECT_SP_16] = &ALM_PROTO_ARCH_ZN4(vrs16_tanhf),
        },

        [ALM_UARCH_VER_ZEN5] = {
            [ALM_FUNC_SCAL_SP] = &ALM_PROTO_ARCH_ZN5(tanhf),
            [ALM_FUNC_SCAL_DP] = &ALM_PROTO_ARCH_ZN5(tanh),
            [ALM_FUNC_VECT_SP_4] = &ALM_PROTO_ARCH_ZN5(vrs4_tanhf),
            [ALM_FUNC_VECT_SP_8] = &ALM_PROTO_ARCH_ZN5(vrs8_tanhf),
            [ALM_FUNC_VECT_SP_16] = &ALM_PROTO_ARCH_ZN5(vrs16_tanhf),
        },

    },
};

void
LIBM_IFACE_PROTO(tanh)(void *arg) {
    alm_ep_wrapper_t g_entry_tanh = {
       .g_ep = {
          [ALM_FUNC_SCAL_SP]   = &G_ENTRY_PT_PTR(tanhf),
          [ALM_FUNC_SCAL_DP]   = &G_ENTRY_PT_PTR(tanh),
          [ALM_FUNC_VECT_SP_4] = &G_ENTRY_PT_PTR(vrs4_tanhf),
          [ALM_FUNC_VECT_SP_8] = &G_ENTRY_PT_PTR(vrs8_tanhf),
          [ALM_FUNC_VECT_SP_16] = &G_ENTRY_PT_PTR(vrs16_tanhf),
        },
    };

    alm_iface_fixup(&g_entry_tanh, &__arch_funcs_tanh);
}

