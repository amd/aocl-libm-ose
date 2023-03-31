/*
 * Copyright (C) 2018-2020, Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __AMD_LIBM_IFACE_H__
#define __AMD_LIBM_IFACE_H__


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
    C_AMD_ERF,
    C_AMD_SINCOS,
    C_AMD_ADD,

    /*
     * INTEGER VARIANTS
     */
    C_AMD_FINITE,
    C_AMD_ILOGB,
    C_AMD_LRINT,
    C_AMD_LROUND,
    C_AMD_LLRINT,
    C_AMD_LLROUND,

    /* complex */
    C_AMD_CEXP,
    C_AMD_CPOW,
    C_AMD_CLOG,

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
extern void LIBM_IFACE_PROTO(sincos)(void *arg);
extern void LIBM_IFACE_PROTO(exp10)(void *arg);
extern void LIBM_IFACE_PROTO(exp2)(void *arg);
extern void LIBM_IFACE_PROTO(exp)(void *arg);
extern void LIBM_IFACE_PROTO(expm1)(void *arg);
extern void LIBM_IFACE_PROTO(fabs)(void *arg);
extern void LIBM_IFACE_PROTO(finite)(void *arg);
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
extern void LIBM_IFACE_PROTO(lrint)(void *arg);
extern void LIBM_IFACE_PROTO(llrint)(void *arg);
extern void LIBM_IFACE_PROTO(lround)(void *arg);
extern void LIBM_IFACE_PROTO(llround)(void *arg);
extern void LIBM_IFACE_PROTO(ilogb)(void *arg);
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
extern void LIBM_IFACE_PROTO(erf)(void *arg);
extern void LIBM_IFACE_PROTO(cexp)(void *arg);
extern void LIBM_IFACE_PROTO(clog)(void *arg);
extern void LIBM_IFACE_PROTO(cpow)(void *arg);
extern void LIBM_IFACE_PROTO(add)(void *arg);

struct entry_pt_interface {
    void (*epi_init)(void *arg);
    void *epi_arg;
};

extern struct entry_pt_interface entry_pt_initializers[C_AMD_LAST_ENTRY];

/*
 * NOTE: The offsets to be maintained in src/iface/ *.c files
 *       will break otherwise
 */
enum ALM_FUNC_VARIANTS {
    ALM_FUNC_SCAL_SP,
    ALM_FUNC_SCAL_DP,
    ALM_FUNC_VECT_SP_4,  /* Single Precision */
    ALM_FUNC_VECT_SP_8,
    ALM_FUNC_VECT_DP_2,  /* Double precision */
    ALM_FUNC_VECT_DP_4,

    /*vector array variants*/
    ALM_FUNC_VECT_SP_ARR,
    ALM_FUNC_VECT_DP_ARR,

    ALM_FUNC_SCAL_HP,
    ALM_FUNC_VECT_HP_8,  /* Half Precision */
    ALM_FUNC_VECT_HP_16,
    ALM_FUNC_VECT_HP_32,

    ALM_FUNC_VECT_SP_16,
    ALM_FUNC_VECT_DP_8,

    ALM_FUNC_SCAL_SP_CMPLX, /* Complex variant */
    ALM_FUNC_SCAL_DP_CMPLX,

    ALM_FUNC_VAR_MAX,                   /* should be last, always */
};
typedef enum ALM_FUNC_VARIANTS alm_func_var_t;

enum ALM_UARCH_VERSIONS {
    ALM_UARCH_VER_DEFAULT,
    ALM_UARCH_VER_BASE64,
    ALM_UARCH_VER_FMA3,
    ALM_UARCH_VER_ZEN,
    ALM_UARCH_VER_ZENPLUS = ALM_UARCH_VER_ZEN,
    ALM_UARCH_VER_ZEN2,
    ALM_UARCH_VER_ZEN3,
    ALM_UARCH_VER_ZEN4,

    ALM_UARCH_MAX,                       /* should be last, always */
};
typedef enum ALM_UARCH_VERSIONS alm_uarch_ver_t;

#ifndef alm_func_t
typedef void (*alm_func_t)(void);
#endif

struct alm_ep_wrapper {
    alm_func_t * g_ep[ALM_FUNC_VAR_MAX];
};
typedef struct alm_ep_wrapper alm_ep_wrapper_t;

struct alm_arch_funcs {
    alm_uarch_ver_t  def_arch;          /* Default version to choose */
    void *           funcs[ALM_UARCH_MAX][ALM_FUNC_VAR_MAX]; /* function array */
};

void alm_iface_fixup(alm_ep_wrapper_t *g_ep_wrapper,
                     const struct alm_arch_funcs *alm_funcs);

#endif  /* __AMD_LIBM_IFACE_H__ */
