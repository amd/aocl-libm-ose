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
LIBM_IFACE_PROTO(sinh)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(sinh) = &FN_PROTOTYPE_REF(sinh);

	/* Single */
	G_ENTRY_PT_PTR(sinhf) = &FN_PROTOTYPE_REF(sinhf);

	/* Vector Double */
	/* Vector Single */
}

