/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 */

#include <libm_macros.h>
#include <libm/cpu_features.h>
#include <libm/entry_pt.h>
#include <libm/iface.h>
#include <libm/amd_funcs_internal.h>    /* Contains all implementations */

#include <libm/arch/zen2.h>

typedef double (*amd_cos_t)(double);
typedef float  (*amd_cosf_t)(float);
typedef __m128d (*amd_cos_v2d_t)(__m128d);
typedef __m128  (*amd_cos_v4s_t)(__m128);

void
LIBM_IFACE_PROTO(cos)(void *arg)
{
    amd_cos_t  fn_d = NULL;
    amd_cosf_t fn_s = NULL;
    amd_cos_v4s_t fn_v4s = NULL;
    amd_cos_v2d_t fn_v2d = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    fn_d = &FN_PROTOTYPE_FMA3(cos);
    fn_s = &FN_PROTOTYPE_FMA3(cosf);
    fn_v4s = &FN_PROTOTYPE_FMA3(vrs4_cosf);
    fn_v2d = &FN_PROTOTYPE_FMA3(vrd2_cos);

    if (CPU_HAS_AVX2(features) &&
        CPU_FEATURE_AVX2_USABLE(features)) {
        //fn_d = &FN_PROTOTYPE_OPT(cos);
        fn_s = &FN_PROTOTYPE_OPT(cosf);
        fn_v4s = &FN_PROTOTYPE_OPT(vrs4_cosf); 
    } else if (CPU_HAS_SSSE3(features) &&
               CPU_FEATURE_SSSE3_USABLE(features)) {
        fn_d = &FN_PROTOTYPE_BAS64(cos);
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
                        fn_s = &ALM_PROTO_ARCH_ZN2(cosf);
                        fn_v4s = &ALM_PROTO_ARCH_ZN2(vrs4_cosf);
                        break;
            case 0x19:                      /* Milan */
                        fn_s = &ALM_PROTO_ARCH_ZN2(cosf);
                        fn_v4s = &ALM_PROTO_ARCH_ZN2(vrs4_cosf);
                        break;
        }
    }

    /* Double */
    G_ENTRY_PT_PTR(cos) = fn_d;

    /* Single */
    G_ENTRY_PT_PTR(cosf) = fn_s;

    /* Vector Double */
    G_ENTRY_PT_PTR(vrd2_cos) = fn_v2d;

    /* Vector Single */
    G_ENTRY_PT_PTR(vrs4_cosf) = fn_v4s;
}

