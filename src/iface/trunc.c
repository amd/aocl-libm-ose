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
LIBM_IFACE_PROTO(trunc)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(trunc) = &FN_PROTOTYPE_BAS64(trunc);

	/* Single */
	G_ENTRY_PT_PTR(truncf) = &FN_PROTOTYPE_BAS64(truncf);

	/* Vector Double */
	/* Vector Single */
}

