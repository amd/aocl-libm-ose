/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 */

#include <libm_macros.h>
#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>
#include <libm/iface.h>
#include <libm/entry_pt.h>

void
LIBM_IFACE_PROTO(expm1)(void *arg)
{
    /*
     * Should setup all variants,
     * single, double, and vectors (also complex if available)
     */
    G_ENTRY_PT_PTR(expm1) = &FN_PROTOTYPE_BAS64(expm1);
    G_ENTRY_PT_PTR(expm1f) = &FN_PROTOTYPE_BAS64(expm1f);

    /*vector versions*/
    G_ENTRY_PT_PTR(vrd4_expm1) = &FN_PROTOTYPE(vrd4_expm1);
}
