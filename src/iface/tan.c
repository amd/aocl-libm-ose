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

typedef double (*amd_tan_t)(double);
typedef float (*amd_tanf_t)(float);
typedef __m128d (*amd_tan_v2d_t)(__m128d);
typedef __m256d (*amd_tan_v4d_t)(__m256d);
typedef __m128  (*amd_tan_v4s_t)(__m128);
typedef __m256  (*amd_tan_v8s_t)(__m256);

void
LIBM_IFACE_PROTO(tan)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */
    amd_tan_t  fn_d = NULL;
    amd_tanf_t fn_s = NULL;
    //amd_tan_v4d_t fn_v4d = NULL;
    //amd_tan_v4s_t fn_v4s = NULL;
    //amd_tan_v8s_t fn_v8s = NULL;
    //amd_tan_v2d_t fn_v2d = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    fn_d = &FN_PROTOTYPE_REF(tan);
    fn_s = &FN_PROTOTYPE_REF(tanf);
    //fn_v4d = &FN_PROTOTYPE_FMA3(vrd4_tan);
    //fn_v4s = &FN_PROTOTYPE_FMA3(vrs4_tanf);
    //fn_v8s = &FN_PROTOTYPE_OPT(vrs8_tanf);
    //fn_v2d = &FN_PROTOTYPE_FMA3(vrd2_tan);

    if (CPU_HAS_AVX2(features) &&
        CPU_FEATURE_AVX2_USABLE(features)) {
	    //fn_d = &FN_PROTOTYPE_OPT(tan);
	    fn_s = &FN_PROTOTYPE_OPT(tanf);
        //fn_v4s = &FN_PROTOTYPE_OPT(vrs4_tanf);
        //fn_v8s = &FN_PROTOTYPE_OPT(vrs8_tanf);
        //fn_v2d = &FN_PROTOTYPE_OPT(vrd2_tan);
        //fn_v4d = &FN_PROTOTYPE_OPT(vrd4_tan);
     } else if (CPU_HAS_SSSE3(features) &&
               CPU_FEATURE_SSSE3_USABLE(features)) {
	    //fn_d = &FN_PROTOTYPE_BAS64(tan);
    } else if (CPU_HAS_AVX(features) &&
               CPU_FEATURE_AVX_USABLE(features)) {
	    //fn_d = &FN_PROTOTYPE_BAS64(tan);
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

    G_ENTRY_PT_PTR(tan) = fn_d;
    G_ENTRY_PT_PTR(tanf) = fn_s;
    //G_ENTRY_PT_PTR(vrd4_tan) = fn_v4d;
    //G_ENTRY_PT_PTR(vrs4_tanf) = fn_v4s;
    //G_ENTRY_PT_PTR(vrs8_tanf) = fn_v8s;
    //G_ENTRY_PT_PTR(vrd2_tan) = fn_v2d;
}

