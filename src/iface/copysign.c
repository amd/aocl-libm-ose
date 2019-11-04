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
LIBM_IFACE_PROTO(copysign)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(copysign) = &FN_PROTOTYPE_FMA3(copysign);

	/* Single */
	G_ENTRY_PT_PTR(copysignf) = &FN_PROTOTYPE_FMA3(copysignf);

	/* Vector Double */
	/* Vector Single */
}

