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
LIBM_IFACE_PROTO(atan)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(atan) = &FN_PROTOTYPE_REF(atan);

	/* Single */
	G_ENTRY_PT_PTR(atanf) = &FN_PROTOTYPE_REF(atanf);

	/* Vector Double */
	/* Vector Single */
}

