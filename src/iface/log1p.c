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
LIBM_IFACE_PROTO(log1p)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(log1p) = &FN_PROTOTYPE_FMA3(log1p);

	/* Single */
	G_ENTRY_PT_PTR(log1pf) = &FN_PROTOTYPE_FMA3(log1pf);

	/* Vector Double */
	/* Vector Single */
}

