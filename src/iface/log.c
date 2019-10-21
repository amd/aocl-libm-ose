/*
 * Copyright (C) 2018-2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/iface.h>
#include <libm/entry_pt.h>

void
LIBM_IFACE_PROTO(log)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */
    G_ENTRY_PT_PTR(log) = &FN_PROTOTYPE_BAS64(log);
    G_ENTRY_PT_PTR(logf) = &FN_PROTOTYPE_BAS64(logf);
}
