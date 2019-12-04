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

typedef double (*amd_log_t)(double);
typedef float (*amd_logf_t)(float);
typedef __m128d (*amd_log_v2d_t)(__m128d);
typedef __m256d (*amd_log_v4d_t)(__m256d);
typedef __m128  (*amd_log_v4s_t)(__m128);

extern float FN_PROTOTYPE_OPT(logf_v3)(float);

void
LIBM_IFACE_PROTO(log)(void *arg)
{

    amd_log_t  fn_d = NULL;
    amd_logf_t fn_s = NULL;
    amd_log_v4d_t fn_v4d = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    fn_d = &FN_PROTOTYPE_FMA3(log);
    fn_s = &FN_PROTOTYPE_FMA3(logf);
    fn_v4d = &FN_PROTOTYPE_FMA3(vrd4_log);

    if (CPU_HAS_AVX2(features) &&
        CPU_FEATURE_AVX2_USABLE(features)) {
            //fn_d = &FN_PROTOTYPE_OPT(log);	/* we dont have an optimized log yet */
	    fn_s = &FN_PROTOTYPE_OPT(logf);
    } else if (CPU_HAS_SSSE3(features) &&
               CPU_FEATURE_SSSE3_USABLE(features)) {
	    fn_d = &FN_PROTOTYPE_BAS64(log);
    } else if (CPU_HAS_AVX(features) &&
               CPU_FEATURE_AVX_USABLE(features)) {
	    fn_d = &FN_PROTOTYPE_BAS64(log);
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

    G_ENTRY_PT_PTR(log) = fn_d;
    G_ENTRY_PT_PTR(logf) = fn_s;
    G_ENTRY_PT_PTR(vrd4_log) = fn_v4d;

}

