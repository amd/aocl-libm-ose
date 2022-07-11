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

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/iface.h>
#include <libm/entry_pt.h>
#include <libm/arch/all.h>


static const
struct alm_arch_funcs __arch_funcs_cexp = {
    .def_arch = ALM_UARCH_VER_DEFAULT,
    .funcs = {
        [ALM_UARCH_VER_DEFAULT] = {
            [ALM_FUNC_SCAL_SP_CMPLX] = &ALM_PROTO_ARCH_AVX2(cexpf),
            [ALM_FUNC_SCAL_DP_CMPLX] = &ALM_PROTO_ARCH_AVX2(cexp),
        },

        [ALM_UARCH_VER_ZEN] = {
            [ALM_FUNC_SCAL_SP_CMPLX] = &ALM_PROTO_ARCH_ZN(cexpf),
            [ALM_FUNC_SCAL_DP_CMPLX] = &ALM_PROTO_ARCH_ZN(cexp),
        },

        [ALM_UARCH_VER_ZEN2] = {
            [ALM_FUNC_SCAL_SP_CMPLX] = &ALM_PROTO_ARCH_ZN2(cexpf),
            [ALM_FUNC_SCAL_DP_CMPLX] = &ALM_PROTO_ARCH_ZN2(cexp),
        },

        [ALM_UARCH_VER_ZEN3] = {
            [ALM_FUNC_SCAL_SP_CMPLX] = &ALM_PROTO_ARCH_ZN3(cexpf),
            [ALM_FUNC_SCAL_DP_CMPLX] = &ALM_PROTO_ARCH_ZN3(cexp),
        },
    },
};

void
LIBM_IFACE_PROTO(cexp)(void *arg) {
    alm_ep_wrapper_t g_entry_cexp = {
       .g_ep = {
           [ALM_FUNC_SCAL_SP_CMPLX] = &G_ENTRY_PT_PTR(cexpf),
           [ALM_FUNC_SCAL_DP_CMPLX] = &G_ENTRY_PT_PTR(cexp),
        },
    };

    alm_iface_fixup(&g_entry_cexp, &__arch_funcs_cexp);
}

