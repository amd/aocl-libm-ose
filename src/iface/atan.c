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
LIBM_IFACE_PROTO(atan)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(atan) = &FN_PROTOTYPE_REF(atan);

	/* Single */
	G_ENTRY_PT_PTR(atanf) = &FN_PROTOTYPE_REF(atanf);

	/* Vector Double */
	/* Vector Single */
}

