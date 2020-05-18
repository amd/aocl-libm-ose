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
LIBM_IFACE_PROTO(tanh)(void *arg)
{
	/* Double */
	G_ENTRY_PT_PTR(tanh) = &FN_PROTOTYPE_REF(tanh);

	/* Single */
	G_ENTRY_PT_PTR(tanhf) = &FN_PROTOTYPE_REF(tanhf);

	/* Vector Double */
	/* Vector Single */
}

