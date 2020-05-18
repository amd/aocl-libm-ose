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
LIBM_IFACE_PROTO(cos)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(cos) = &FN_PROTOTYPE_FMA3(cos);

	/* Single */
	G_ENTRY_PT_PTR(cosf) = &FN_PROTOTYPE_FMA3(cosf);

	/* Vector Double */
	/* Vector Single */
}

