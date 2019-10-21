/*
 * Copyright (C) 2018-2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <stddef.h>                     /* for NULL */

#include <libm/entry_pt.h>
#include <libm/iface.h>

struct entry_pt_interface entry_pt_initializers[C_AMD_LAST_ENTRY] = {
    [C_AMD_EXP] = {LIBM_IFACE_PROTO(exp), NULL},
    [C_AMD_EXP2] = {LIBM_IFACE_PROTO(exp2), NULL},
    [C_AMD_LOG] = {LIBM_IFACE_PROTO(log), NULL},
    [C_AMD_POW] = {LIBM_IFACE_PROTO(pow), NULL},
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

void
libm_iface_init(void)
{
    int n = ARRAY_SIZE(entry_pt_initializers);
    struct entry_pt_interface *ptr;

    for (int i = 0; i < n; i++) {
        ptr = &entry_pt_initializers[i];
        if (ptr && ptr->epi_init) {
            ptr->epi_init(ptr->epi_arg);
        }
    }
}

