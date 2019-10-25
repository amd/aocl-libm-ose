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


void
LIBM_IFACE_PROTO(log)(void *arg)
{

    amd_log_t  fn_d = NULL;
    amd_logf_t fn_s = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    G_ENTRY_PT_PTR(log) = &FN_PROTOTYPE_BAS64(log);
    G_ENTRY_PT_PTR(logf) = &FN_PROTOTYPE_BAS64(logf);

    if (CPU_HAS_AVX2(features) &&
            CPU_FEATURE_AVX2_USABLE(features)) {
            fn_d = &FN_PROTOTYPE_OPT(log);
            fn_s = &FN_PROTOTYPE_OPT(logf);
        } else if (CPU_HAS_SSSE3(features) &&
                   CPU_FEATURE_SSSE3_USABLE(features)) {
            fn_d = &FN_PROTOTYPE_BAS64(log);
        } else if (CPU_HAS_AVX(features) &&
                   CPU_FEATURE_AVX_USABLE(features)) {
            fn_d = &FN_PROTOTYPE_BAS64(log);
	}

    G_ENTRY_PT_PTR(log) = fn_d;
    G_ENTRY_PT_PTR(logf) = fn_s;
}

