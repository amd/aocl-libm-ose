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
LIBM_IFACE_PROTO(exp2)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */
    G_ENTRY_PT_PTR(exp2) = &FN_PROTOTYPE_BAS64(exp2);
    G_ENTRY_PT_PTR(exp2f) = &FN_PROTOTYPE_BAS64(exp2f);

    /*vector versions*/
    G_ENTRY_PT_PTR(vrd2_exp2) = &FN_PROTOTYPE_FMA3(vrd2_exp2);
    G_ENTRY_PT_PTR(vrd4_exp2) = &FN_PROTOTYPE_FMA3(vrd4_exp2);
}
