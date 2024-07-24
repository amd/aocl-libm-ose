
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
//
#include <libm/arch/all.h>
static const
struct alm_arch_funcs __arch_funcs_fmin = {
    .def_arch = ALM_UARCH_VER_DEFAULT,
    .funcs = {
        [ALM_UARCH_VER_DEFAULT] = {
            [ALM_FUNC_SCAL_SP]     = &ALM_PROTO_ARCH_AVX2(fminf),
            [ALM_FUNC_SCAL_DP]     = &ALM_PROTO_ARCH_AVX2(fmin),
            [ALM_FUNC_VECT_SP_ARR] = &ALM_PROTO_ARCH_AVX2(vrsa_fminf),
            [ALM_FUNC_VECT_DP_ARR] = &ALM_PROTO_ARCH_AVX2(vrda_fmin),
        },
        [ALM_UARCH_VER_ZEN] = {
            [ALM_FUNC_SCAL_SP]     = &ALM_PROTO_ARCH_ZN(fminf),
            [ALM_FUNC_SCAL_DP]     = &ALM_PROTO_ARCH_ZN(fmin),
            [ALM_FUNC_VECT_SP_ARR] = &ALM_PROTO_ARCH_ZN(vrsa_fminf),
            [ALM_FUNC_VECT_DP_ARR] = &ALM_PROTO_ARCH_ZN(vrda_fmin),
        },
        [ALM_UARCH_VER_ZEN2] = {
            [ALM_FUNC_SCAL_SP]     = &ALM_PROTO_ARCH_ZN2(fminf),
            [ALM_FUNC_SCAL_DP]     = &ALM_PROTO_ARCH_ZN2(fmin),
            [ALM_FUNC_VECT_SP_ARR] = &ALM_PROTO_ARCH_ZN2(vrsa_fminf),
            [ALM_FUNC_VECT_DP_ARR] = &ALM_PROTO_ARCH_ZN2(vrda_fmin),
        },
        [ALM_UARCH_VER_ZEN3] = {
            [ALM_FUNC_SCAL_SP]     = &ALM_PROTO_ARCH_ZN3(fminf),
            [ALM_FUNC_SCAL_DP]     = &ALM_PROTO_ARCH_ZN3(fmin),
            [ALM_FUNC_VECT_SP_ARR] = &ALM_PROTO_ARCH_ZN3(vrsa_fminf),
            [ALM_FUNC_VECT_DP_ARR] = &ALM_PROTO_ARCH_ZN3(vrda_fmin),
        },
        [ALM_UARCH_VER_ZEN4] = {
            [ALM_FUNC_SCAL_SP]     = &ALM_PROTO_ARCH_ZN4(fminf),
            [ALM_FUNC_SCAL_DP]     = &ALM_PROTO_ARCH_ZN4(fmin),
            [ALM_FUNC_VECT_SP_ARR] = &ALM_PROTO_ARCH_ZN4(vrsa_fminf),
            [ALM_FUNC_VECT_DP_ARR] = &ALM_PROTO_ARCH_ZN4(vrda_fmin),
        },
        [ALM_UARCH_VER_ZEN5] = {
            [ALM_FUNC_SCAL_SP]     = &ALM_PROTO_ARCH_ZN5(fminf),
            [ALM_FUNC_SCAL_DP]     = &ALM_PROTO_ARCH_ZN5(fmin),
            [ALM_FUNC_VECT_SP_ARR] = &ALM_PROTO_ARCH_ZN5(vrsa_fminf),
            [ALM_FUNC_VECT_DP_ARR] = &ALM_PROTO_ARCH_ZN5(vrda_fmin),
        },
    },
};
void
LIBM_IFACE_PROTO(fmin)(void *arg) {
    alm_ep_wrapper_t g_entry_fmin = {
       .g_ep = {
            [ALM_FUNC_SCAL_SP]     = &G_ENTRY_PT_PTR(fminf),
            [ALM_FUNC_SCAL_DP]     = &G_ENTRY_PT_PTR(fmin),
            [ALM_FUNC_VECT_SP_ARR] = &G_ENTRY_PT_PTR(vrsa_fminf),
            [ALM_FUNC_VECT_DP_ARR] = &G_ENTRY_PT_PTR(vrda_fmin),
        },
    };
    alm_iface_fixup(&g_entry_fmin, &__arch_funcs_fmin);
}
