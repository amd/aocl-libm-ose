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
LIBM_IFACE_PROTO(tan)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(tan) = &FN_PROTOTYPE_FMA3(tan);

	/* Single */
	G_ENTRY_PT_PTR(tanf) = &FN_PROTOTYPE_FMA3(tanf);

	/* Vector Double */
	/* Vector Single */
}
