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

typedef float  (*amd_coshf_t)(float);
typedef __m128  (*amd_cosh_v4s_t)(__m128);

void
LIBM_IFACE_PROTO(cosh)(void *arg)
{

    amd_coshf_t fn_s = NULL;
    amd_cosh_v4s_t fn_v4s = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

	/* Double */
	G_ENTRY_PT_PTR(cosh) = &FN_PROTOTYPE_REF(cosh);

	/* Single */
	G_ENTRY_PT_PTR(coshf) = &FN_PROTOTYPE_OPT(coshf);

	/* Vector Double */
	/* Vector Single */
    fn_v4s = &FN_PROTOTYPE_OPT(vrs4_coshf);
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
                        fn_s = &ALM_PROTO_ARCH_ZN2(coshf);
                        fn_v4s = &ALM_PROTO_ARCH_ZN2(vrs4_coshf);
                        break;
            case 0x19:                      /* Milan */
                        fn_s = &ALM_PROTO_ARCH_ZN3(coshf);
                        fn_v4s = &ALM_PROTO_ARCH_ZN3(vrs4_coshf);
                        break;
        }
    }

     /* Single */
    G_ENTRY_PT_PTR(coshf) = fn_s;
    G_ENTRY_PT_PTR(vrs4_coshf) = fn_v4s;
}

