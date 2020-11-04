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

#include <stdio.h>
#include <libm_macros.h>
#include <libm/cpu_features.h>
#include <libm/entry_pt.h>
#include <libm/iface.h>
#include <libm/amd_funcs_internal.h>    /* Contains all implementations */
#include <libm/arch/zen2.h>
#include <libm/arch/zen3.h>

typedef double (*amd_sin_t)(double);
typedef float (*amd_sinf_t)(float);
typedef __m128d (*amd_sin_v2d_t)(__m128d);
typedef __m256d (*amd_sin_v4d_t)(__m256d);
typedef __m128  (*amd_sin_v4s_t)(__m128);
typedef __m256  (*amd_sin_v8s_t)(__m256);

void
LIBM_IFACE_PROTO(sin)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */
    amd_sin_t  fn_d = NULL;
    amd_sinf_t fn_s = NULL;
    amd_sin_v4s_t fn_v4s = NULL;
    amd_sin_v8s_t fn_v8s = NULL;
    amd_sin_v2d_t fn_v2d = NULL;
    amd_sin_v4d_t fn_v4d = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    if (CPU_HAS_AVX2(features) &&
        CPU_FEATURE_AVX2_USABLE(features)) {

        fn_d = &FN_PROTOTYPE_OPT(sin);
        fn_s = &FN_PROTOTYPE_OPT(sinf);
        fn_v4s = &FN_PROTOTYPE_OPT(vrs4_sinf);
        fn_v8s = &FN_PROTOTYPE_OPT(vrs8_sinf);
        fn_v2d = &FN_PROTOTYPE_OPT(vrd2_sin);
        fn_v4d = &FN_PROTOTYPE_OPT(vrd4_sin);

     } else if (CPU_HAS_SSSE3(features) &&
        CPU_FEATURE_SSSE3_USABLE(features)) {

        fn_d = &FN_PROTOTYPE_BAS64(sin);
        fn_s = &FN_PROTOTYPE_BAS64(sinf);

    } else if (CPU_HAS_AVX(features) &&
        CPU_FEATURE_AVX_USABLE(features)) {

        fn_d = &FN_PROTOTYPE_BAS64(sin);
        fn_s = &FN_PROTOTYPE_BAS64(sinf);

    }

     /*
     * Template:
     *     override with any micro-architecture-specific
     *     implementations
     */
    if (mfg_info->mfg_type == CPU_MFG_AMD) {
        switch(mfg_info->family) {
        case 0x15:                                      /* Naples */
            break;
        case 0x17: fn_d   = &ALM_PROTO_ARCH_ZN2(sin);   /* Rome */
                   fn_s   = &ALM_PROTO_ARCH_ZN2(sinf);
                   fn_v4s = &ALM_PROTO_ARCH_ZN2(vrs4_sinf);
                   fn_v8s = &ALM_PROTO_ARCH_ZN2(vrs8_sinf);
                   fn_v2d = &ALM_PROTO_ARCH_ZN2(vrd2_sin);
                   fn_v4d = &ALM_PROTO_ARCH_ZN2(vrd4_sin);
            break;
        case 0x19: fn_d   = &ALM_PROTO_ARCH_ZN3(sin);   /* Milan */
                   fn_s   = &ALM_PROTO_ARCH_ZN3(sinf);
                   fn_v4s = &ALM_PROTO_ARCH_ZN3(vrs4_sinf);
                   fn_v8s = &ALM_PROTO_ARCH_ZN3(vrs8_sinf);
                   fn_v2d = &ALM_PROTO_ARCH_ZN3(vrd2_sin);
                   fn_v4d = &ALM_PROTO_ARCH_ZN3(vrd4_sin);
            break;
        }
    }

    /* Double */
    G_ENTRY_PT_PTR(sin) = fn_d;

    /* Single */
    G_ENTRY_PT_PTR(sinf) = fn_s;

    /* Vector Double */
    G_ENTRY_PT_PTR(vrd2_sin) = fn_v2d;

    G_ENTRY_PT_PTR(vrd4_sin) = fn_v4d;

    /* Vector Single */
    G_ENTRY_PT_PTR(vrs4_sinf) = fn_v4s;

    G_ENTRY_PT_PTR(vrs8_sinf) = fn_v8s;
}

