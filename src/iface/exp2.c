/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

//#include <libm/arch/zen.h>
#include <libm/arch/zen2.h>
#include <libm/arch/zen3.h>

static const
struct alm_arch_funcs __arch_funcs_exp2 = {
    .def_arch = ALM_UARCH_VER_DEFAULT,
    .funcs = {
        [ALM_UARCH_VER_DEFAULT] = {
            &FN_PROTOTYPE_FMA3(exp2f),
            &FN_PROTOTYPE_FMA3(exp2),
            &FN_PROTOTYPE_FMA3(vrs4_exp2f),
            NULL,                           /* vrs8 ? */
            &FN_PROTOTYPE_FMA3(vrd2_exp2),
            &FN_PROTOTYPE_FMA3(vrd4_exp2),
        },

        [ALM_UARCH_VER_ZEN2] = {
            NULL, 
            NULL, //&ALM_PROTO_ARCH_ZN2(exp2),
            NULL,                           /* vrs4 ? */
            NULL,                           /* vrs8 ? */
            NULL,//&ALM_PROTO_ARCH_ZN2(vrd2_exp2),
            NULL,//&ALM_PROTO_ARCH_ZN2(vrd4_exp2),
        },

        [ALM_UARCH_VER_ZEN3] = {
            NULL,
            NULL, //&ALM_PROTO_ARCH_ZN3(exp2),
            NULL,                           /* vrs4 ? */
            NULL,                           /* vrs8 ? */
            NULL,                           /* vrd2 ? */ 
            NULL,                           /* vrd4 ? */ 
        },
    },
};

void
LIBM_IFACE_PROTO(exp2)(void *arg)
{
    alm_ep_wrapper_t g_entry_exp2 = {
       .g_ep = {
        [ALM_FUNC_SCAL_SP]   = &G_ENTRY_PT_PTR(exp2f),
        [ALM_FUNC_SCAL_DP]   = &G_ENTRY_PT_PTR(exp2),
        //[ALM_FUNC_VECT_SP_4] = &G_ENTRY_PT_PTR(vrs4_exp2f),
        //[ALM_FUNC_VECT_SP_8] = &G_ENTRY_PT_PTR(vrs8_exp2f),
        [ALM_FUNC_VECT_DP_2] = &G_ENTRY_PT_PTR(vrd2_exp2),
        [ALM_FUNC_VECT_DP_4] = &G_ENTRY_PT_PTR(vrd4_exp2),
        },
    };

    alm_iface_fixup(&g_entry_exp2, &__arch_funcs_exp2);
}

