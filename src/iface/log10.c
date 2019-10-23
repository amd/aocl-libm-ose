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
LIBM_IFACE_PROTO(log10)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(log10) = &FN_PROTOTYPE_FMA3(log10);

	/* Single */
	G_ENTRY_PT_PTR(log10f) = &FN_PROTOTYPE_FMA3(log10f);

	/* Vector Double */
	/* Vector Single */
}

