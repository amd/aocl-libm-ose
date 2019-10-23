#ifndef __AMD_LIBM_IFACE_H__
#define __AMD_LIBM_IFACE_H__

/*
 * Copyright (C) 2018-2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#if defined(__cplusplus)
extern "C" {
#endif


enum {
    /* We leave '0', */
    C_AMD_ACOS = 1,
    C_AMD_ACOSH,
    C_AMD_ASIN,
    C_AMD_ASINH,
    C_AMD_ATAN2,
    C_AMD_ATAN,
    C_AMD_ATANH,
    C_AMD_CBRT,
    C_AMD_CEIL,
    C_AMD_COPYSIGN,
    C_AMD_COS,
    C_AMD_COSH,
    C_AMD_COSPI,
    C_AMD_EXP10,
    C_AMD_EXP2,
    C_AMD_EXP,
    C_AMD_EXPM1,
    C_AMD_FABS,
    C_AMD_FDIM,
    C_AMD_FLOOR,
    C_AMD_FMA,
    C_AMD_FMAX,
    C_AMD_FMIN,
    C_AMD_FMOD,
    C_AMD_FREXP,
    C_AMD_HYPOT,
    C_AMD_LDEXP,
    C_AMD_LOG10,
    C_AMD_LOG1P,
    C_AMD_LOG2,
    C_AMD_LOGB,
    C_AMD_LOG,
    C_AMD_MODF,
    C_AMD_NAN,
    C_AMD_NEARBYINT,
    C_AMD_NEXTAFTER,
    C_AMD_NEXTTOWARD,
    C_AMD_POW,
    C_AMD_REMAINDER,
    C_AMD_REMQUO,
    C_AMD_RINT,
    C_AMD_ROUND,
    C_AMD_SCALBLN,
    C_AMD_SCALBN,
    C_AMD_SIN,
    C_AMD_SINH,
    C_AMD_SINPI,
    C_AMD_SQRT,
    C_AMD_TAN,
    C_AMD_TANH,
    C_AMD_TANPI,
    C_AMD_TRUNC,

    /*
     * INTEGER VARIANTS
     */
    C_AMD_FINITE,
    C_AMD_ILOGB,
    C_AMD_LRINT,
    C_AMD_LROUND,
    C_AMD_LLRINT,
    C_AMD_LLROUND,

    /*
     * This one needs to be last one, REALLY !!!
     */
    C_AMD_LAST_ENTRY,
};

#if defined(__cplusplus)
}
#endif

#define LIBM_IFACE_PROTO(fn) libm_iface_amd_##fn

/* INIT function for libm */
extern void libm_iface_init(void);

/* function specific initilizer */
extern void LIBM_IFACE_PROTO(acos)(void *arg);
extern void LIBM_IFACE_PROTO(acosh)(void *arg);
extern void LIBM_IFACE_PROTO(asin)(void *arg);
extern void LIBM_IFACE_PROTO(asinh)(void *arg);
extern void LIBM_IFACE_PROTO(atan2)(void *arg);
extern void LIBM_IFACE_PROTO(atan)(void *arg);
extern void LIBM_IFACE_PROTO(atanh)(void *arg);
extern void LIBM_IFACE_PROTO(cbrt)(void *arg);
extern void LIBM_IFACE_PROTO(ceil)(void *arg);
extern void LIBM_IFACE_PROTO(copysign)(void *arg);
extern void LIBM_IFACE_PROTO(cos)(void *arg);
extern void LIBM_IFACE_PROTO(cosh)(void *arg);
extern void LIBM_IFACE_PROTO(cospi)(void *arg);
extern void LIBM_IFACE_PROTO(exp10)(void *arg);
extern void LIBM_IFACE_PROTO(exp2)(void *arg);
extern void LIBM_IFACE_PROTO(exp)(void *arg);
extern void LIBM_IFACE_PROTO(expm1)(void *arg);
extern void LIBM_IFACE_PROTO(fabs)(void *arg);
extern void LIBM_IFACE_PROTO(fastpow)(void *arg);
extern void LIBM_IFACE_PROTO(fdim)(void *arg);
extern void LIBM_IFACE_PROTO(floor)(void *arg);
extern void LIBM_IFACE_PROTO(fma)(void *arg);
extern void LIBM_IFACE_PROTO(fmax)(void *arg);
extern void LIBM_IFACE_PROTO(fmin)(void *arg);
extern void LIBM_IFACE_PROTO(fmod)(void *arg);
extern void LIBM_IFACE_PROTO(frexp)(void *arg);
extern void LIBM_IFACE_PROTO(hypot)(void *arg);
extern void LIBM_IFACE_PROTO(ldexp)(void *arg);
extern void LIBM_IFACE_PROTO(log10)(void *arg);
extern void LIBM_IFACE_PROTO(log1p)(void *arg);
extern void LIBM_IFACE_PROTO(log2)(void *arg);
extern void LIBM_IFACE_PROTO(logb)(void *arg);
extern void LIBM_IFACE_PROTO(log)(void *arg);
extern void LIBM_IFACE_PROTO(modf)(void *arg);
extern void LIBM_IFACE_PROTO(nan)(void *arg);
extern void LIBM_IFACE_PROTO(nearbyint)(void *arg);
extern void LIBM_IFACE_PROTO(nextafter)(void *arg);
extern void LIBM_IFACE_PROTO(nexttoward)(void *arg);
extern void LIBM_IFACE_PROTO(pow)(void *arg);
extern void LIBM_IFACE_PROTO(remainder)(void *arg);
extern void LIBM_IFACE_PROTO(remquo)(void *arg);
extern void LIBM_IFACE_PROTO(rint)(void *arg);
extern void LIBM_IFACE_PROTO(round)(void *arg);
extern void LIBM_IFACE_PROTO(scalbln)(void *arg);
extern void LIBM_IFACE_PROTO(scalbn)(void *arg);
extern void LIBM_IFACE_PROTO(sin)(void *arg);
extern void LIBM_IFACE_PROTO(sinh)(void *arg);
extern void LIBM_IFACE_PROTO(sinpi)(void *arg);
extern void LIBM_IFACE_PROTO(sqrt)(void *arg);
extern void LIBM_IFACE_PROTO(tan)(void *arg);
extern void LIBM_IFACE_PROTO(tanh)(void *arg);
extern void LIBM_IFACE_PROTO(tanpi)(void *arg);
extern void LIBM_IFACE_PROTO(trunc)(void *arg);

struct entry_pt_interface {
    void (*epi_init)(void *arg);
    void *epi_arg;
};

extern struct entry_pt_interface entry_pt_initializers[C_AMD_LAST_ENTRY];

#endif
