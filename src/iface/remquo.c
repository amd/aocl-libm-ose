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
LIBM_IFACE_PROTO(remquo)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(remquo) = &FN_PROTOTYPE_REF(remquo);

	/* Single */
	G_ENTRY_PT_PTR(remquof) = &FN_PROTOTYPE_REF(remquof);

	/* Vector Double */
	/* Vector Single */
}

