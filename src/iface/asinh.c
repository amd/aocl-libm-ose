/*
 * Copyright (C) 2018, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <libm_macros.h>
#include <libm/cpu_features.h>
#include <libm/entry_pt.h>
#include <libm/iface.h>
#include <libm/amd_funcs_internal.h>    /* Contains all implementations */


void
LIBM_IFACE_PROTO(asinh)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(asinh) = &FN_PROTOTYPE_FMA3(asinh);

	/* Single */
	G_ENTRY_PT_PTR(asinhf) = &FN_PROTOTYPE_FMA3(asinhf);

	/* Vector Double */
	/* Vector Single */
}

