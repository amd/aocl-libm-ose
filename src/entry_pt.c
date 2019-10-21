/*
 * Copyright (C) 2018-2019 AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/cpu_features.h>
#include <libm/iface.h>
#include <libm/entry_pt.h>

#ifdef __cplusplus
extern "C"
{
#endif

double G_ENTRY_PT(acos)(double x);
double G_ENTRY_PT(acosh)(double x);
double G_ENTRY_PT(asin)(double x);
double G_ENTRY_PT(asinh)(double x);
double G_ENTRY_PT(atan2)(double x, double y);
double G_ENTRY_PT(atan)(double x);
double G_ENTRY_PT(atanh)(double x);
double G_ENTRY_PT(cbrt)(double x);
double G_ENTRY_PT(ceil)(double x);
double G_ENTRY_PT(copysign)(double x, double y);
double G_ENTRY_PT(cos)(double x);
double G_ENTRY_PT(cosh)(double x);
double G_ENTRY_PT(cospi)(double x);
double G_ENTRY_PT(exp10)(double x);
double G_ENTRY_PT(exp2)(double x);
double G_ENTRY_PT(exp)(double x);
double G_ENTRY_PT(expm1)(double x);
double G_ENTRY_PT(fabs)(double x);
double G_ENTRY_PT(fastpow)(double x, double y);
double G_ENTRY_PT(fdim)(double x, double y);
double G_ENTRY_PT(floor)(double x);
double G_ENTRY_PT(fma)(double x, double y, double z);
double G_ENTRY_PT(fmax)(double x, double y);
double G_ENTRY_PT(fmin)(double x, double y);
double G_ENTRY_PT(fmod)(double x, double y);
double G_ENTRY_PT(frexp)(double value, int *exp);
double G_ENTRY_PT(hypot)(double x, double y);
double G_ENTRY_PT(ldexp)(double x, int exp);
double G_ENTRY_PT(log10)(double x);
double G_ENTRY_PT(log1p)(double x);
double G_ENTRY_PT(log2)(double x);
double G_ENTRY_PT(logb)(double x);
double G_ENTRY_PT(log)(double x);
double G_ENTRY_PT(modf)(double x, double *iptr);
double G_ENTRY_PT(nan)(const char *tagp);
double G_ENTRY_PT(nearbyint)(double x);
double G_ENTRY_PT(nextafter)(double x, double y);
double G_ENTRY_PT(nexttoward)(double x, long double y);
double G_ENTRY_PT(pow)(double x, double y);
double G_ENTRY_PT(remainder)(double x, double y);
double G_ENTRY_PT(remquo) (double x, double y, int *quo);
double G_ENTRY_PT(rint)(double x);
double G_ENTRY_PT(round)(double f);
double G_ENTRY_PT(scalbln)(double x, long int n);
double G_ENTRY_PT(scalbn)(double x, int n);
double G_ENTRY_PT(sin)(double x);
double G_ENTRY_PT(sinh)(double x);
double G_ENTRY_PT(sinpi)(double x);
double G_ENTRY_PT(sqrt)(double x);
double G_ENTRY_PT(tan)(double x);
double G_ENTRY_PT(tanh)(double x);
double G_ENTRY_PT(tanpi)(double x);
double G_ENTRY_PT(trunc)(double x);


/*
 * Single Precision functions
 */
float G_ENTRY_PT(acosf)(float x);
float G_ENTRY_PT(acoshf)(float x);
float G_ENTRY_PT(asinf)(float x);
float G_ENTRY_PT(asinhf)(float x);
float G_ENTRY_PT(atan2f)(float x, float y);
float G_ENTRY_PT(atanf)(float x);
float G_ENTRY_PT(atanhf)(float x);
float G_ENTRY_PT(cbrtf)(float x);
float G_ENTRY_PT(ceilf)(float x);
float G_ENTRY_PT(copysignf)(float x, float y);
float G_ENTRY_PT(cosf)(float x);
float G_ENTRY_PT(coshf)(float fx);
float G_ENTRY_PT(cospif)(float x);
float G_ENTRY_PT(exp10f)(float x);
float G_ENTRY_PT(exp2f)(float x);
float G_ENTRY_PT(expf)(float x);
float G_ENTRY_PT(expm1f)(float x);
float G_ENTRY_PT(fabsf)(float x);
float G_ENTRY_PT(fdimf)(float x, float y);
float G_ENTRY_PT(floorf)(float x);
float G_ENTRY_PT(fmaf)(float x, float y, float z);
float G_ENTRY_PT(fmaxf)(float x, float y);
float G_ENTRY_PT(fminf)(float x, float y);
float G_ENTRY_PT(fmodf)(float x, float y);
float G_ENTRY_PT(frexpf)(float value, int *exp);
float G_ENTRY_PT(hypotf)(float x, float y);
float G_ENTRY_PT(ldexpf)(float x, int exp);
float G_ENTRY_PT(log10f)(float x);
float G_ENTRY_PT(log1pf)(float x);
float G_ENTRY_PT(log2f)(float x);
float G_ENTRY_PT(logbf)(float x);
float G_ENTRY_PT(logf)(float x);
float G_ENTRY_PT(modff)(float x, float *iptr);
float G_ENTRY_PT(nanf)(const char *tagp);
float G_ENTRY_PT(nearbyintf)(float x);
float G_ENTRY_PT(nextafterf)(float x, float y);
float G_ENTRY_PT(nexttowardf)(float x, long double y);
float G_ENTRY_PT(powf)(float x, float y);
float G_ENTRY_PT(remainderf)(float x, float y);
float G_ENTRY_PT(remquof) (float x, float y, int *quo);
float G_ENTRY_PT(rintf)(float x);
float G_ENTRY_PT(roundf)(float f);
float G_ENTRY_PT(scalblnf)(float x, long int n);
float G_ENTRY_PT(scalbnf)(float x, int n);
float G_ENTRY_PT(sinf)(float x);
float G_ENTRY_PT(sinhf)(float x);
float G_ENTRY_PT(sinpif)(float x);
float G_ENTRY_PT(sqrtf)(float x);
float G_ENTRY_PT(tanf)(float x);
float G_ENTRY_PT(tanhf)(float x);
float G_ENTRY_PT(tanpif)(float x);
float G_ENTRY_PT(truncf)(float x);

/*
 * Integer variants
 */
int G_ENTRY_PT(finite)(double x);
int G_ENTRY_PT(finitef)(float x);
int G_ENTRY_PT(ilogb)(double x);
int G_ENTRY_PT(ilogbf)(float x);
long int G_ENTRY_PT(lrint)(double x);
long int G_ENTRY_PT(lrintf)(float x);
long int G_ENTRY_PT(lround)(double d);
long int G_ENTRY_PT(lroundf)(float f);
long long int G_ENTRY_PT(llrint)(double x);
long long int G_ENTRY_PT(llrintf)(float x);
long long int G_ENTRY_PT(llround)(double d);
long long int G_ENTRY_PT(llroundf)(float f);

void G_ENTRY_PT(vrda_cbrt)(int, double*, double*);
void G_ENTRY_PT(vrda_cos)(int, double*, double*);
void G_ENTRY_PT(vrda_exp)(int, double*, double*);
void G_ENTRY_PT(vrda_exp2)(int, double*, double*);
void G_ENTRY_PT(vrda_exp10)(int, double*, double*);
void G_ENTRY_PT(vrda_expm1)(int, double*, double*);
void G_ENTRY_PT(vrda_log)(int, double*, double*);
void G_ENTRY_PT(vrda_log10)(int, double*, double*);
void G_ENTRY_PT(vrda_log1p)(int, double*, double*);
void G_ENTRY_PT(vrda_log2)(int, double*, double*);
void G_ENTRY_PT(vrda_pow)(int, double*, double*);
void G_ENTRY_PT(vrda_sin)(int, double*, double*);


void G_ENTRY_PT(vrsa_cosf)(int, float*, float*);
void G_ENTRY_PT(vrsa_expf)(int, float*, float*);
void G_ENTRY_PT(vrsa_exp2f)(int, float*, float*);
void G_ENTRY_PT(vrsa_exp10f)(int, float*, float*);
void G_ENTRY_PT(vrsa_expm1f)(int, float*, float*);
void G_ENTRY_PT(vrsa_log1pf)(int, float*, float*);
void G_ENTRY_PT(vrsa_sinf)(int, float*, float*);
void G_ENTRY_PT(vrsa_log2f)(int, float*, float*);
void G_ENTRY_PT(vrsa_cbrtf)(int, float*, float*);
void G_ENTRY_PT(vrsa_log10f)(int, float*, float*);
void G_ENTRY_PT(vrsa_logf)(int, float*, float*);

#ifndef _MSC_VER
void  G_ENTRY_PT(sincos)(double,double *,double *);
void  G_ENTRY_PT(sincosf)(float,float *,float *);
#endif

__m128  G_ENTRY_PT(vrs4_cosf)(__m128);
__m128  G_ENTRY_PT(vrs4_cbrtf)(__m128);
__m128  G_ENTRY_PT(vrs4_expf)(__m128);
__m128  G_ENTRY_PT(vrs4_exp2f)(__m128);
__m128  G_ENTRY_PT(vrs4_exp10f)(__m128);
__m128  G_ENTRY_PT(vrs4_expm1f)(__m128);
__m128  G_ENTRY_PT(vrs4_logf)(__m128);
__m128  G_ENTRY_PT(vrs4_log2f)(__m128);
__m128  G_ENTRY_PT(vrs4_log10f)(__m128);
__m128  G_ENTRY_PT(vrs4_log1pf)(__m128);
__m128  G_ENTRY_PT(vrs4_sinf)(__m128);
__m128  G_ENTRY_PT(vrs4_tanf)(__m128);

__m128d G_ENTRY_PT(vrd2_cbrt)(__m128d);
__m128d G_ENTRY_PT(vrd2_cos)(__m128d);
__m128d G_ENTRY_PT(vrd2_exp)(__m128d);
__m128d G_ENTRY_PT(vrd2_exp2)(__m128d);
__m128d G_ENTRY_PT(vrd2_exp10)(__m128d);
__m128d G_ENTRY_PT(vrd2_expm1)(__m128d);
__m128d G_ENTRY_PT(vrd2_log)(__m128d);
__m128d G_ENTRY_PT(vrd2_log2)(__m128d);
__m128d G_ENTRY_PT(vrd2_log10)(__m128d);
__m128d G_ENTRY_PT(vrd2_log1p)(__m128d);
__m128d G_ENTRY_PT(vrd2_pow)(__m128d);
__m128d G_ENTRY_PT(vrd2_sin)(__m128d);
__m128d G_ENTRY_PT(vrd2_tan)(__m128d);

__m256d G_ENTRY_PT(vrd4_cbrt)(__m256d);
__m256d G_ENTRY_PT(vrd4_cos)(__m256d);
__m256d G_ENTRY_PT(vrd4_exp)(__m256d);
__m256d G_ENTRY_PT(vrd4_exp2)(__m256d);
__m256d G_ENTRY_PT(vrd4_exp10)(__m256d);
__m256d G_ENTRY_PT(vrd4_expm1)(__m256d);
__m256d G_ENTRY_PT(vrd4_log)(__m256d);
__m256d G_ENTRY_PT(vrd4_log2)(__m256d);
__m256d G_ENTRY_PT(vrd4_log10)(__m256d);
__m256d G_ENTRY_PT(vrd4_log1p)(__m256d);
__m256d G_ENTRY_PT(vrd4_pow)(__m256d);
__m256d G_ENTRY_PT(vrd4_sin)(__m256d);
__m256d G_ENTRY_PT(vrd4_tan)(__m256d);


#ifdef __cplusplus
}
#endif

static void CONSTRUCTOR
init_map_entry_points(void)
{
    /* First call __init_cpu_featues() via libm_cpu_get_features */

    /* Call interfaces to initialize functions */

    if (libm_cpu_feature_is_avx2_usable()) {

        g_amd_libm_ep_cbrt       =	FN_PROTOTYPE_FMA3(cbrt);
        g_amd_libm_ep_cbrtf      =	FN_PROTOTYPE_FMA3(cbrtf);
        g_amd_libm_ep_sin        =	FN_PROTOTYPE_FMA3(sin);
#ifndef _MSC_VER
        g_amd_libm_ep_sincos     =	FN_PROTOTYPE_FMA3(sincos);
        g_amd_libm_ep_sincosf    =	FN_PROTOTYPE_FMA3(sincosf);
#endif
        g_amd_libm_ep_sinf       =	FN_PROTOTYPE_FMA3(sinf);
        g_amd_libm_ep_cos        =	FN_PROTOTYPE_FMA3(cos) ;
        g_amd_libm_ep_cosf       =	FN_PROTOTYPE_FMA3(cosf);
        g_amd_libm_ep_tan        =	FN_PROTOTYPE_FMA3(tan);
        g_amd_libm_ep_tanf       =	FN_PROTOTYPE_FMA3(tanf);
        g_amd_libm_ep_vrd2_cbrt  =	FN_PROTOTYPE_FMA3(vrd2_cbrt);
        g_amd_libm_ep_vrs4_cbrtf =	FN_PROTOTYPE_FMA3(vrs4_cbrtf);
        g_amd_libm_ep_vrd2_cos   =	FN_PROTOTYPE_FMA3(vrd2_cos);
        g_amd_libm_ep_vrs4_cosf  =	FN_PROTOTYPE_FMA3(vrs4_cosf);
        g_amd_libm_ep_vrd2_sin   =	FN_PROTOTYPE_FMA3(vrd2_sin);
        g_amd_libm_ep_vrs4_sinf  =	FN_PROTOTYPE_FMA3(vrs4_sinf);
        g_amd_libm_ep_vrd2_tan   =	FN_PROTOTYPE_FMA3(vrd2_tan);
        g_amd_libm_ep_vrs4_tanf  =	FN_PROTOTYPE_FMA3(vrs4_tanf);
        g_amd_libm_ep_vrd2_log   =	FN_PROTOTYPE_FMA3(vrd2_log);
        g_amd_libm_ep_vrd2_log2  =	FN_PROTOTYPE_FMA3(vrd2_log2);
        g_amd_libm_ep_vrd2_log10 =	FN_PROTOTYPE_FMA3(vrd2_log10);
        g_amd_libm_ep_vrd2_log1p =	FN_PROTOTYPE_FMA3(vrd2_log1p);
        g_amd_libm_ep_vrs4_logf  =	FN_PROTOTYPE_FMA3(vrs4_logf);
        g_amd_libm_ep_vrs4_log2f =	FN_PROTOTYPE_FMA3(vrs4_log2f);
        g_amd_libm_ep_vrs4_log10f=	FN_PROTOTYPE_FMA3(vrs4_log10f);
        g_amd_libm_ep_vrs4_log1pf=	FN_PROTOTYPE_FMA3(vrs4_log1pf);
        g_amd_libm_ep_logf       =	FN_PROTOTYPE_FMA3(logf);
        g_amd_libm_ep_log2f      =	FN_PROTOTYPE_FMA3(log2f);
        g_amd_libm_ep_log10f     =	FN_PROTOTYPE_FMA3(log10f);
        g_amd_libm_ep_log1pf     =	FN_PROTOTYPE_FMA3(log1pf);
        g_amd_libm_ep_log        =	FN_PROTOTYPE_FMA3(log);
        g_amd_libm_ep_log2       =	FN_PROTOTYPE_FMA3(log2);
        g_amd_libm_ep_log10      =	FN_PROTOTYPE_FMA3(log10);
        g_amd_libm_ep_log1p      =	FN_PROTOTYPE_FMA3(log1p);
        g_amd_libm_ep_exp        =	FN_PROTOTYPE_FMA3(exp);
        g_amd_libm_ep_exp2       =	FN_PROTOTYPE_FMA3(exp2);
        g_amd_libm_ep_exp10      =	FN_PROTOTYPE_FMA3(exp10);
        g_amd_libm_ep_expm1      =	FN_PROTOTYPE_FMA3(expm1);
        g_amd_libm_ep_expf       =	FN_PROTOTYPE_FMA3(expf);
        g_amd_libm_ep_exp2f	 =	FN_PROTOTYPE_FMA3(exp2f);
        g_amd_libm_ep_exp10f	 =	FN_PROTOTYPE_FMA3(exp10f);
        g_amd_libm_ep_expm1f	 =	FN_PROTOTYPE_FMA3(expm1f);
        g_amd_libm_ep_vrs4_expf  =	FN_PROTOTYPE_FMA3(vrs4_expf);
        g_amd_libm_ep_vrs4_exp2f =	FN_PROTOTYPE_FMA3(vrs4_exp2f);
        g_amd_libm_ep_vrs4_exp10f=	FN_PROTOTYPE_FMA3(vrs4_exp10f);
        g_amd_libm_ep_vrs4_expm1f=	FN_PROTOTYPE_FMA3(vrs4_expm1f);
        g_amd_libm_ep_vrd2_exp   =	FN_PROTOTYPE_FMA3(vrd2_exp);
        g_amd_libm_ep_vrd2_exp2	 =	FN_PROTOTYPE_FMA3(vrd2_exp2);
        g_amd_libm_ep_vrd2_exp10 =	FN_PROTOTYPE_FMA3(vrd2_exp10);
        g_amd_libm_ep_vrd2_expm1 =	FN_PROTOTYPE_FMA3(vrd2_expm1);
        g_amd_libm_ep_vrda_cbrt	 =	FN_PROTOTYPE_FMA3(vrda_cbrt);
        g_amd_libm_ep_vrda_log	 =	FN_PROTOTYPE_FMA3(vrda_log);
        g_amd_libm_ep_vrda_log10 =	FN_PROTOTYPE_FMA3(vrda_log10);
        g_amd_libm_ep_vrda_log1p =	FN_PROTOTYPE_FMA3(vrda_log1p);
        g_amd_libm_ep_vrda_log2	 =	FN_PROTOTYPE_FMA3(vrda_log2);
        g_amd_libm_ep_vrda_sin	 =	FN_PROTOTYPE_FMA3(vrda_sin);
        g_amd_libm_ep_vrda_cos	 =	FN_PROTOTYPE_FMA3(vrda_cos);


        g_amd_libm_ep_vrsa_cbrtf =	FN_PROTOTYPE_FMA3(vrsa_cbrtf);
        g_amd_libm_ep_vrsa_logf	 =	FN_PROTOTYPE_FMA3(vrsa_logf);
        g_amd_libm_ep_vrsa_log10f=	FN_PROTOTYPE_FMA3(vrsa_log10f);
        g_amd_libm_ep_vrsa_log1pf=	FN_PROTOTYPE_FMA3(vrsa_log1pf);
        g_amd_libm_ep_vrsa_log2f =	FN_PROTOTYPE_FMA3(vrsa_log2f);
        g_amd_libm_ep_vrsa_sinf	 =	FN_PROTOTYPE_FMA3(vrsa_sinf);
        g_amd_libm_ep_vrsa_cosf	 =	FN_PROTOTYPE_FMA3(vrsa_cosf);

        g_amd_libm_ep_vrda_exp   =	FN_PROTOTYPE_FMA3(vrda_exp);
        g_amd_libm_ep_vrda_exp2	 =	FN_PROTOTYPE_FMA3(vrda_exp2);
        g_amd_libm_ep_vrda_exp10 =	FN_PROTOTYPE_FMA3(vrda_exp10);
        g_amd_libm_ep_vrda_expm1 =	FN_PROTOTYPE_FMA3(vrda_expm1);
        g_amd_libm_ep_vrsa_expf  =	FN_PROTOTYPE_FMA3(vrsa_expf);
        g_amd_libm_ep_vrsa_exp2f =	FN_PROTOTYPE_FMA3(vrsa_exp2f);
        g_amd_libm_ep_vrsa_exp10f=	FN_PROTOTYPE_FMA3(vrsa_exp10f);
        g_amd_libm_ep_vrsa_expm1f=	FN_PROTOTYPE_FMA3(vrsa_expm1f);
        g_amd_libm_ep_fmaf       =	FN_PROTOTYPE_FMA3(fmaf);
        g_amd_libm_ep_fma        =	FN_PROTOTYPE_FMA3(fma);
        g_amd_libm_ep_pow        =	FN_PROTOTYPE_FMA3(pow);
        g_amd_libm_ep_fastpow        =  FN_PROTOTYPE_FMA3(fastpow);
        g_amd_libm_ep_powf       =	FN_PROTOTYPE_FMA3(powf);
    }

    libm_iface_init();
}
