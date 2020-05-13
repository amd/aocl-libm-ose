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
LIBM_IFACE_PROTO(cosh)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(cosh) = &FN_PROTOTYPE_REF(cosh);

	/* Single */
	G_ENTRY_PT_PTR(coshf) = &FN_PROTOTYPE_REF(coshf);

	/* Vector Double */
	/* Vector Single */
}

