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


void
LIBM_IFACE_PROTO(log2)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(log2) = &FN_PROTOTYPE_FMA3(log2);

	/* Single */
	G_ENTRY_PT_PTR(log2f) = &FN_PROTOTYPE_FMA3(log2f);

	/* Vector Double */
	/* Vector Single */
}

