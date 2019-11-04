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
LIBM_IFACE_PROTO(llrint)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(llrint) = &FN_PROTOTYPE_FMA3(llrint);

	/* Single */
	G_ENTRY_PT_PTR(llrintf) = &FN_PROTOTYPE_FMA3(llrintf);

	/* Vector Double */
	/* Vector Single */
}

