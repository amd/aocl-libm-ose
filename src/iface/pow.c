/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/iface.h>
#include <libm/entry_pt.h>
#include <libm/cpu_features.h>

typedef double (*amd_pow_t)(double, double);
typedef float (*amd_powf_t)(float, float) ;
typedef __m128d (*amd_pow_v2d_t)(__m128d, __m128d);
typedef __m256d (*amd_pow_v4d_t)(__m256d, __m256d);
typedef __m128  (*amd_powf_v4s_t)(__m128, __m128);
typedef __m256  (*amd_powf_v8s_t)(__m256, __m256);

void
LIBM_IFACE_PROTO(pow)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */

    amd_pow_t  fn_d = NULL;
    amd_powf_t fn_s = NULL;
    amd_pow_v4d_t fn_v4d = NULL;
    amd_pow_v2d_t fn_v2d = NULL;
    amd_powf_v4s_t fn_v4s = NULL;
    amd_powf_v8s_t fn_v8s = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    fn_d = &FN_PROTOTYPE_FMA3(pow);
    fn_s = &FN_PROTOTYPE_FMA3(powf);
    fn_v4d = &FN_PROTOTYPE_FMA3(vrd4_pow);
    fn_v2d = &FN_PROTOTYPE_FMA3(vrd2_pow);
    fn_v4s = &FN_PROTOTYPE_OPT(vrs4_powf);
    fn_v8s = &FN_PROTOTYPE_OPT(vrs8_powf);

    if (CPU_HAS_AVX2(features) &&
        CPU_FEATURE_AVX2_USABLE(features)) {
        fn_d = &FN_PROTOTYPE_OPT(pow);
        fn_s = &FN_PROTOTYPE_OPT(powf);
        fn_v4d = &FN_PROTOTYPE_OPT(vrd4_pow);
        fn_v2d = &FN_PROTOTYPE_OPT(vrd2_pow);
    } else if (CPU_HAS_SSSE3(features) &&
               CPU_FEATURE_SSSE3_USABLE(features)) {
        fn_d = &FN_PROTOTYPE_BAS64(pow);
    } else if (CPU_HAS_AVX(features) &&
               CPU_FEATURE_AVX_USABLE(features)) {
        fn_d = &FN_PROTOTYPE_BAS64(pow);
    }

    /*
     * Template:
     *     override with any micro-architecture-specific
     *     implementations
     */
    if (mfg_info->mfg_type == CPU_MFG_AMD) {
        switch(mfg_info->family) {
        case 0x15:                      /* Naples */
            break;
        case 0x17:                      /* Rome */
            break;
        case 0x19:                      /* Milan */
            break;
        }
    }

    G_ENTRY_PT_PTR(pow) = fn_d;
    G_ENTRY_PT_PTR(powf) = fn_s;
    G_ENTRY_PT_PTR(vrd4_pow) = fn_v4d;
    G_ENTRY_PT_PTR(vrd2_pow) = fn_v2d;
    G_ENTRY_PT_PTR(vrs4_powf) = fn_v4s;
    G_ENTRY_PT_PTR(vrs8_powf) = fn_v8s;
}
