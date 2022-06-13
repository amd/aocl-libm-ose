/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stddef.h>                     /* for NULL */

#include <libm/cpu_features.h>
#include <libm/entry_pt.h>
#include <libm/iface.h>


struct entry_pt_interface entry_pt_initializers[C_AMD_LAST_ENTRY] = {
    [C_AMD_ACOS]       = {LIBM_IFACE_PROTO(acos), NULL},
    [C_AMD_ACOSH]      = {LIBM_IFACE_PROTO(acosh), NULL},
    [C_AMD_ASIN]       = {LIBM_IFACE_PROTO(asin), NULL},
    [C_AMD_ASINH]      = {LIBM_IFACE_PROTO(asinh), NULL},
    [C_AMD_ATAN2]      = {LIBM_IFACE_PROTO(atan2), NULL},
    [C_AMD_ATAN]       = {LIBM_IFACE_PROTO(atan), NULL},
    [C_AMD_ATANH]      = {LIBM_IFACE_PROTO(atanh), NULL},
    [C_AMD_CBRT]       = {LIBM_IFACE_PROTO(cbrt), NULL},
    [C_AMD_CEIL]       = {LIBM_IFACE_PROTO(ceil), NULL},
    [C_AMD_COPYSIGN]   = {LIBM_IFACE_PROTO(copysign), NULL},
    [C_AMD_COS]        = {LIBM_IFACE_PROTO(cos), NULL},
    [C_AMD_COSH]       = {LIBM_IFACE_PROTO(cosh), NULL},
    [C_AMD_COSPI]      = {LIBM_IFACE_PROTO(cospi), NULL},
    [C_AMD_EXP10]      = {LIBM_IFACE_PROTO(exp10), NULL},
    [C_AMD_EXP2]       = {LIBM_IFACE_PROTO(exp2), NULL},
    [C_AMD_EXP]        = {LIBM_IFACE_PROTO(exp), NULL},
    [C_AMD_EXPM1]      = {LIBM_IFACE_PROTO(expm1), NULL},
    [C_AMD_FABS]       = {LIBM_IFACE_PROTO(fabs), NULL},
    [C_AMD_FDIM]       = {LIBM_IFACE_PROTO(fdim), NULL},
    [C_AMD_FLOOR]      = {LIBM_IFACE_PROTO(floor), NULL},
    [C_AMD_FMA]        = {LIBM_IFACE_PROTO(fma), NULL},
    [C_AMD_FMAX]       = {LIBM_IFACE_PROTO(fmax), NULL},
    [C_AMD_FMIN]       = {LIBM_IFACE_PROTO(fmin), NULL},
    [C_AMD_FMOD]       = {LIBM_IFACE_PROTO(fmod), NULL},
    [C_AMD_FREXP]      = {LIBM_IFACE_PROTO(frexp), NULL},
    [C_AMD_HYPOT]      = {LIBM_IFACE_PROTO(hypot), NULL},
    [C_AMD_LDEXP]      = {LIBM_IFACE_PROTO(ldexp), NULL},
    [C_AMD_LOG10]      = {LIBM_IFACE_PROTO(log10), NULL},
    [C_AMD_LOG1P]      = {LIBM_IFACE_PROTO(log1p), NULL},
    [C_AMD_LOG2]       = {LIBM_IFACE_PROTO(log2), NULL},
    [C_AMD_LOGB]       = {LIBM_IFACE_PROTO(logb), NULL},
    [C_AMD_LOG]        = {LIBM_IFACE_PROTO(log), NULL},
    [C_AMD_MODF]       = {LIBM_IFACE_PROTO(modf), NULL},
    [C_AMD_NAN]        = {LIBM_IFACE_PROTO(nan), NULL},
    [C_AMD_NEARBYINT]  = {LIBM_IFACE_PROTO(nearbyint), NULL},
    [C_AMD_NEXTAFTER]  = {LIBM_IFACE_PROTO(nextafter), NULL},
    [C_AMD_NEXTTOWARD] = {LIBM_IFACE_PROTO(nexttoward), NULL},
    [C_AMD_POW]        = {LIBM_IFACE_PROTO(pow), NULL},
    [C_AMD_REMAINDER]  = {LIBM_IFACE_PROTO(remainder), NULL},
    [C_AMD_REMQUO]     = {LIBM_IFACE_PROTO(remquo), NULL},
    [C_AMD_RINT]       = {LIBM_IFACE_PROTO(rint), NULL},
    [C_AMD_ROUND]      = {LIBM_IFACE_PROTO(round), NULL},
    [C_AMD_SCALBLN]    = {LIBM_IFACE_PROTO(scalbln), NULL},
    [C_AMD_SCALBN]     = {LIBM_IFACE_PROTO(scalbn), NULL},
    [C_AMD_SIN]        = {LIBM_IFACE_PROTO(sin), NULL},
    [C_AMD_SINH]       = {LIBM_IFACE_PROTO(sinh), NULL},
    [C_AMD_SINPI]      = {LIBM_IFACE_PROTO(sinpi), NULL},
    [C_AMD_SQRT]       = {LIBM_IFACE_PROTO(sqrt), NULL},
    [C_AMD_TAN]        = {LIBM_IFACE_PROTO(tan), NULL},
    [C_AMD_TANH]       = {LIBM_IFACE_PROTO(tanh), NULL},
    [C_AMD_TANPI]      = {LIBM_IFACE_PROTO(tanpi), NULL},
    [C_AMD_TRUNC]      = {LIBM_IFACE_PROTO(trunc), NULL},
    [C_AMD_ERF]      =   {LIBM_IFACE_PROTO(erf), NULL},

    [C_AMD_SINCOS]     = {LIBM_IFACE_PROTO(sincos), NULL},

    /* Integer variants */
    [C_AMD_FINITE]     = {LIBM_IFACE_PROTO(finite), NULL},
    [C_AMD_ILOGB]      = {LIBM_IFACE_PROTO(ilogb), NULL},
    [C_AMD_LRINT]      = {LIBM_IFACE_PROTO(lrint), NULL},
    [C_AMD_LROUND]     = {LIBM_IFACE_PROTO(lround), NULL},
    [C_AMD_LLRINT]     = {LIBM_IFACE_PROTO(llrint), NULL},
    [C_AMD_LLROUND]    = {LIBM_IFACE_PROTO(llround), NULL},

    /*complex*/
    [C_AMD_CEXP]       = {LIBM_IFACE_PROTO(cexp), NULL},
    [C_AMD_CPOW]       = {LIBM_IFACE_PROTO(cpow), NULL},
    [C_AMD_CLOG]       = {LIBM_IFACE_PROTO(clog), NULL},
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


static alm_func_t
alm_iface_fixup_one(const struct alm_arch_funcs *alm_funcs,
                    alm_uarch_ver_t arch_ver, int idx)
{
    alm_func_t ret = (alm_func_t)NULL;

    if (!alm_funcs)
        return  ret;

    /* if invalid arg, fixup with defaults */
    if (ALM_UARCH_MAX - arch_ver >=
        ALM_UARCH_MAX - ALM_UARCH_VER_DEFAULT)
        arch_ver = alm_funcs->def_arch;

    /* if corrupted arch_version */
    if (ALM_UARCH_MAX - arch_ver >=
        ALM_UARCH_MAX - ALM_UARCH_VER_DEFAULT)
        arch_ver = ALM_UARCH_VER_DEFAULT;

    for (int i = arch_ver; i >=0 ; i--) {
        /* Keep searching till we find default version */
        if (alm_funcs->funcs[i][idx]) {
            ret = alm_funcs->funcs[i][idx];
            break;
        }
    }

    /* or NULL */
    return ret;
}

static alm_uarch_ver_t
alm_get_uach(void)
{
    alm_uarch_ver_t arch_ver;

    if (alm_cpu_arch_is_zen4())
        arch_ver = ALM_UARCH_VER_ZEN4;
    else if (alm_cpu_arch_is_zen3())
        arch_ver = ALM_UARCH_VER_ZEN3;
    else if (alm_cpu_arch_is_zen2())
        arch_ver = ALM_UARCH_VER_ZEN2;
    else if (alm_cpu_arch_is_zen())
        arch_ver = ALM_UARCH_VER_ZEN;
    else
        arch_ver = ALM_UARCH_VER_DEFAULT;

    return arch_ver;
}

void
alm_iface_fixup(alm_ep_wrapper_t *g_ep_wrapper,
                const struct alm_arch_funcs *alm_funcs)
{
    static alm_uarch_ver_t arch_ver;

    if (!alm_funcs)
        return;

    arch_ver = alm_get_uach();

    for (int i = ((int)ALM_FUNC_VAR_MAX-1); i >=0 ; i--) {
        alm_ep_func_t *gptr = g_ep_wrapper->g_ep[i];

        if (gptr) {
            /* Overwrite only if find valid function */
            alm_ep_func_t f = alm_iface_fixup_one(alm_funcs, arch_ver, i);
            if (f)
                *gptr = f;
        }
    }
}
