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
    [C_AMD_ACOS]       = {LIBM_IFACE_PROTO(acos), NULL},
    [C_AMD_ACOSH]      = {LIBM_IFACE_PROTO(acosh), NULL},
    [C_AMD_ASIN]       = {LIBM_IFACE_PROTO(asin), NULL},
    //[C_AMD_ASINH]      = {LIBM_IFACE_PROTO(asinh), NULL},
    [C_AMD_ATAN2]      = {LIBM_IFACE_PROTO(atan2), NULL},
    [C_AMD_ATAN]       = {LIBM_IFACE_PROTO(atan), NULL},
    [C_AMD_ATANH]      = {LIBM_IFACE_PROTO(atanh), NULL},
    [C_AMD_CBRT]       = {LIBM_IFACE_PROTO(cbrt), NULL},
    [C_AMD_CEIL]       = {LIBM_IFACE_PROTO(ceil), NULL},
    [C_AMD_COS]        = {LIBM_IFACE_PROTO(cos), NULL},
    [C_AMD_EXP10]      = {LIBM_IFACE_PROTO(exp10), NULL},
    [C_AMD_EXP2]       = {LIBM_IFACE_PROTO(exp2), NULL},
    [C_AMD_EXP]        = {LIBM_IFACE_PROTO(exp), NULL},
    [C_AMD_EXPM1]      = {LIBM_IFACE_PROTO(expm1), NULL},
    [C_AMD_FABS]       = {LIBM_IFACE_PROTO(fabs), NULL},
    [C_AMD_FMA]        = {LIBM_IFACE_PROTO(fma), NULL},
    [C_AMD_FREXP]      = {LIBM_IFACE_PROTO(frexp), NULL},
    [C_AMD_HYPOT]      = {LIBM_IFACE_PROTO(hypot), NULL},
    [C_AMD_LDEXP]      = {LIBM_IFACE_PROTO(ldexp), NULL},
    [C_AMD_LOG10]      = {LIBM_IFACE_PROTO(log10), NULL},
    [C_AMD_LOG1P]      = {LIBM_IFACE_PROTO(log1p), NULL},
    [C_AMD_LOG2]       = {LIBM_IFACE_PROTO(log2), NULL},
    [C_AMD_LOG]        = {LIBM_IFACE_PROTO(log), NULL},
    [C_AMD_NAN]        = {LIBM_IFACE_PROTO(nan), NULL},
    [C_AMD_NEARBYINT]  = {LIBM_IFACE_PROTO(nearbyint), NULL},
    [C_AMD_NEXTAFTER]  = {LIBM_IFACE_PROTO(nextafter), NULL},
    [C_AMD_NEXTTOWARD] = {LIBM_IFACE_PROTO(nexttoward), NULL},
    [C_AMD_POW]        = {LIBM_IFACE_PROTO(pow), NULL},
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

