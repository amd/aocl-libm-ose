/*
 * Copyright (C) 2018-2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/iface.h>
#include <libm/entry_pt.h>
#include <libm/cpu_features.h>

typedef double (*amd_exp_t)(double);
typedef float (*amd_expf_t)(float);
typedef __m128d (*amd_exp_v2d_t)(__m128d);
typedef __m256d (*amd_exp_v4d_t)(__m256d);
typedef __m128  (*amd_exp_v4s_t)(__m128);
typedef __m256  (*amd_exp_v8s_t)(__m256);

void
LIBM_IFACE_PROTO(exp)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */
    amd_exp_t  fn_d = NULL;
    amd_expf_t fn_s = NULL;
    amd_exp_v4d_t fn_v4d = NULL;
    amd_exp_v4s_t fn_v4s = NULL;
    amd_exp_v8s_t fn_v8s = NULL;
    amd_exp_v2d_t fn_v2d = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    fn_d = &FN_PROTOTYPE_FMA3(exp);
    fn_s = &FN_PROTOTYPE_FMA3(expf);
    fn_v4d = &FN_PROTOTYPE_FMA3(vrd4_exp);
    fn_v4s = &FN_PROTOTYPE_FMA3(vrs4_expf);
    fn_v8s = &FN_PROTOTYPE_OPT(vrs8_expf);
    fn_v2d = &FN_PROTOTYPE_OPT(vrd2_exp);

    if (CPU_HAS_AVX2(features) &&
        CPU_FEATURE_AVX2_USABLE(features)) {
	    fn_d = &FN_PROTOTYPE_OPT(exp);
	    fn_s = &FN_PROTOTYPE_OPT(expf);
        fn_v4s = &FN_PROTOTYPE_OPT(vrs4_expf);
        fn_v8s = &FN_PROTOTYPE_OPT(vrs8_expf);
        fn_v2d = &FN_PROTOTYPE_OPT(vrd2_exp);
    } else if (CPU_HAS_SSSE3(features) &&
               CPU_FEATURE_SSSE3_USABLE(features)) {
	    fn_d = &FN_PROTOTYPE_BAS64(exp);
    } else if (CPU_HAS_AVX(features) &&
               CPU_FEATURE_AVX_USABLE(features)) {
	    fn_d = &FN_PROTOTYPE_BAS64(exp);
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

    G_ENTRY_PT_PTR(exp) = fn_d;
    G_ENTRY_PT_PTR(expf) = fn_s;
    G_ENTRY_PT_PTR(vrd4_exp) = fn_v4d;
    G_ENTRY_PT_PTR(vrs4_expf) = fn_v4s;
    G_ENTRY_PT_PTR(vrs8_expf) = fn_v8s;
    G_ENTRY_PT_PTR(vrd2_exp) = fn_v2d;
}

