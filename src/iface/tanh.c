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
#include <libm/arch/zen3.h>

typedef float (*amd_tanhf_t)(float);

void
LIBM_IFACE_PROTO(tanh)(void *arg)
{
    amd_tanhf_t fn_s = NULL;

    static struct cpu_features *features = NULL;

    if(!features)   {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    fn_s = &FN_PROTOTYPE_REF(tanhf);

    if (CPU_HAS_AVX2(features) &&
        CPU_FEATURE_AVX2_USABLE(features)) {
        fn_s = &FN_PROTOTYPE_OPT(tanhf);
    }

    if (mfg_info->mfg_type == CPU_MFG_AMD) {
        switch(mfg_info->family) {
            case 0x15:                      /* Naples */
                        break;
            case 0x17:                      /* Rome */
                        fn_s = &ALM_PROTO_ARCH_ZN2(tanhf);
                        break;
            case 0x19:                      /* Milan */
                        fn_s = &ALM_PROTO_ARCH_ZN3(tanhf);
                        break;
        }
    }

	/* Double */
	G_ENTRY_PT_PTR(tanh) = &FN_PROTOTYPE_REF(tanh);

	/* Single */
	G_ENTRY_PT_PTR(tanhf) = fn_s;

	/* Vector Double */
	/* Vector Single */
}

