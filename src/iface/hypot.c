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
LIBM_IFACE_PROTO(hypot)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(hypot) = &FN_PROTOTYPE_REF(hypot);

	/* Single */
	G_ENTRY_PT_PTR(hypotf) = &FN_PROTOTYPE_REF(hypotf);

	/* Vector Double */
	/* Vector Single */
}

