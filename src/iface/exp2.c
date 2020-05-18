/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/iface.h>
#include <libm/entry_pt.h>
#include <libm/cpu_features.h>

typedef double (*amd_exp2_t)(double);
typedef float (*amd_exp2f_t)(float);
typedef __m256d (*amd_exp2_v4d_t)(__m256d);

void
LIBM_IFACE_PROTO(exp2)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */

    amd_exp2_t  fn_d = NULL;
    amd_exp2f_t fn_s = NULL;
    amd_exp2_v4d_t fn_v4d = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    //struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    fn_d = &FN_PROTOTYPE_FMA3(exp2);
    fn_s = &FN_PROTOTYPE_FMA3(exp2f);
    fn_v4d = &FN_PROTOTYPE_FMA3(vrd4_exp2);

    if (CPU_HAS_AVX2(features) &&
        CPU_FEATURE_AVX2_USABLE(features)) {
            //fn_d = &FN_PROTOTYPE_OPT(exp2);
            //fn_s = &FN_PROTOTYPE_OPT(exp2f);
    }

    G_ENTRY_PT_PTR(exp2) = fn_d;
    G_ENTRY_PT_PTR(exp2f) = fn_s;

    /*vector versions*/
    G_ENTRY_PT_PTR(vrd2_exp2) = &FN_PROTOTYPE_FMA3(vrd2_exp2);
    G_ENTRY_PT_PTR(vrd4_exp2) = fn_v4d;
}

