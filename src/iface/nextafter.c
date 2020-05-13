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
LIBM_IFACE_PROTO(nextafter)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(nextafter) = &FN_PROTOTYPE_REF(nextafter);

	/* Single */
	G_ENTRY_PT_PTR(nextafterf) = &FN_PROTOTYPE_REF(nextafterf);

	/* Vector Double */
	/* Vector Single */
}

