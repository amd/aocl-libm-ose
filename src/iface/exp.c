/*
 * Copyright (C) 2018-2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */
#include <stdio.h>

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

void
LIBM_IFACE_PROTO(exp)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */
    amd_exp_t  fn_d = NULL;
    amd_expf_t fn_s = NULL;

    //static struct cpu_features ftr = {};
    //struct cpu_features *features = &ftr;
    static struct cpu_features *features;

    if (!fn_d) {
        features = libm_cpu_get_features();

        struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

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

        fn_d = &FN_PROTOTYPE_FMA3(exp);

        if (CPU_HAS_AVX2(features) &&
            CPU_FEATURE_AVX2_USABLE(features)) {
            fn_d = &FN_PROTOTYPE_OPT(exp);
            fn_s = &FN_PROTOTYPE_OPT(expf);
        } else if (CPU_HAS_SSSE3(features) &&
                   CPU_FEATURE_SSSE3_USABLE(features)) {
            fn_d = &FN_PROTOTYPE_BAS64(exp);
        } else if (CPU_HAS_AVX(features) &&
                   CPU_FEATURE_AVX_USABLE(features)) {
            fn_d = &FN_PROTOTYPE_BAS64(exp);
	}

    G_ENTRY_PT_PTR(exp) = fn_d;
    G_ENTRY_PT_PTR(expf) = fn_s;
}
