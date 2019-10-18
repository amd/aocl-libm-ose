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
LIBM_IFACE_PROTO(exp)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */
    G_ENTRY_PT_PTR(exp) = &FN_PROTOTYPE_BAS64(exp);
    G_ENTRY_PT_PTR(expf) = &FN_PROTOTYPE_BAS64(expf);
}
