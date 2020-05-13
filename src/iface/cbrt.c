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
LIBM_IFACE_PROTO(cbrt)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(cbrt) = &FN_PROTOTYPE_FMA3(cbrt);

	/* Single */
	G_ENTRY_PT_PTR(cbrtf) = &FN_PROTOTYPE_FMA3(cbrtf);

	/* Vector Double */
	/* Vector Single */
}

