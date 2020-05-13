/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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
LIBM_IFACE_PROTO(lrint)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(lrint) = &FN_PROTOTYPE_REF(lrint);

	/* Single */
	G_ENTRY_PT_PTR(lrintf) = &FN_PROTOTYPE_REF(lrintf);

	/* Vector Double */
	/* Vector Single */
}

