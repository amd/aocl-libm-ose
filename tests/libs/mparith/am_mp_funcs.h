#ifndef __ALM_MP_FUNCS_H__
#define __ALM_MP_FUNCS_H__

#ifdef __cplusplus
extern "C" {
#endif

  
float   alm_mp_acos        (float x);
float   alm_mp_acosh       (float x);
float   alm_mp_asinf       (float x);
float   alm_mp_asinhf      (float x);
float   alm_mp_atan2       (float x, float y);
float   alm_mp_atanf       (float x);
float   alm_mp_atanhf      (float x);
float   alm_mp_cbrtf       (float x);
float   alm_mp_ceilf       (float x);
float   alm_mp_copysignf   (float x, float y);
float   alm_mp_cosf        (float x);
float   alm_mp_coshf       (float x);
float   alm_mp_cospif      (float x);
float   alm_mp_exp10       (float x);
float   alm_mp_exp2        (float x);
float   alm_mp_expf        (float x);
float   alm_mp_expm1       (float x);
float   alm_mp_fabsf       (float x);
float   alm_mp_fdimf       (float x, float y);
float   alm_mp_floorf      (float x);
float   alm_mp_fmaf        (float x, float y, float z);
float   alm_mp_fmaxf       (float x, float y);
float   alm_mp_fminf       (float x, float y);
float   alm_mp_fmodf       (float x, float y);
float   alm_mp_frexpf      (float x, int *ptr);
float   alm_mp_hypotf      (float x, float y);
float   alm_mp_ldexpf      (float x, int expn);
float   alm_mp_log10       (float x);
float   alm_mp_log1pf      (float x);
float   alm_mp_log2        (float x);
float   alm_mp_logbf       (float x);
float   alm_mp_logf        (float x);
float   alm_mp_modff       (float x, float *ptr);
float   alm_mp_powf        (float x, float y);
float   alm_mp_remainderf  (float x, float y);
float   alm_mp_remquof     (float x, float y, int* quotient);
float   alm_mp_rintf       (float x);
float   alm_mp_roundf      (float x);
float   alm_mp_sinf        (float x);
float   alm_mp_sinhf       (float x);
float   alm_mp_sinpif      (float x);
float   alm_mp_sqrtf       (float x);
float   alm_mp_tanf        (float x);
float   alm_mp_tanhf       (float x);
float   alm_mp_tanpif      (float x);
float   alm_mp_truncf      (float x);

/* Single precision ULP functions */
float   alm_mp_acoshf_ULP    (float x,float z, double *, double *);
float   alm_mp_acosf_ULP     (float x,float z, double *, double *);
float   alm_mp_asinhf_ULP    (float x,float z, double *, double *);
float   alm_mp_asinf_ULP     (float x,float z, double *, double *);
float   alm_mp_atan2f_ULP    (float x, float y,float z, double *, double *);
float   alm_mp_atanhf_ULP    (float x,float z, double *, double *);
float   alm_mp_atanf_ULP     (float x,float z, double *, double *);
float   alm_mp_cbrtf_ULP     (float x, float z, double *, double *);
float   alm_mp_ceilf_ULP     (float x, float z, double *, double *);
float   alm_mp_copysignf_ULP (float x, float y,float z,double *, double *);
float   alm_mp_coshf_ULP     (float x,float z, double *, double *);
float   alm_mp_cospif_ULP    (float x,float z, double *, double *);
float   alm_mp_cosf_ULP      (float x, float z, double *, double *);
float   alm_mp_exp10f_ULP    (float x,float z, double *, double *);
float   alm_mp_exp2f_ULP     (float x,float z, double *, double *);
float   alm_mp_expm1f_ULP    (float x,float z, double *, double *);
float   alm_mp_expf_ULP      (float x,float z, double *, double *);
float   alm_mp_fabsf_ULP     (float x, float z, double *, double *);
float   alm_mp_fdimf_ULP     (float x, float y,float z, double *, double *);
float   alm_mp_floorf_ULP    (float x,float z, double *, double *);
float   alm_mp_fmaf_ULP      (float x, float y, float z,float re, double *, double *);
float   alm_mp_fmaxf_ULP     (float x, float y,float z, double *, double *);
float   alm_mp_fminf_ULP     (float x, float y,float z, double *, double *);
float   alm_mp_fmodf_ULP     (float x, float y,float z, double *, double *);
float   alm_mp_frexpf_ULP    (float x, int *ptr, float z, double *, double *);
float   alm_mp_hypotf_ULP    (float x, float y,float z, double *, double *);
float   alm_mp_ldexpf_ULP    (float x, int expn, float z, double *, double *);
float   alm_mp_log10f_ULP    (float x,float z, double *, double *);
float   alm_mp_log1pf_ULP    (float x,float z, double *, double *);
float   alm_mp_log2f_ULP     (float x,float z, double *, double *);
float   alm_mp_logbf_ULP     (float x,float z, double *, double *);
float   alm_mp_logf_ULP      (float x,float z, double *, double *);
float   alm_mp_modff_ULP     (float x, float *ptr, float z, double *, double *);
float   alm_mp_powf_ULP      (float x, float y,float z, double *, double *);
float   alm_mp_remainderf_ULP(float x, float y,float z, double *, double *);
float   alm_mp_remquof_ULP   (float x, float y, int* quotient, float z, double * , double *);
float   alm_mp_rintf_ULP     (float x,float z, double *, double *);
float   alm_mp_roundf_ULP    (float x,float z, double *, double *);
float   alm_mp_sinhf_ULP     (float x,float z, double *, double *);
float   alm_mp_sinpif_ULP    (float x,float z, double *, double *);
float   alm_mp_sinf_ULP      (float x,float z, double *, double *);
float   alm_mp_sqrtf_ULP     (float x,float z, double *, double *);
float   alm_mp_tanhf_ULP     (float x,float z, double *, double *);
float   alm_mp_tanpif_ULP    (float x,float z, double *, double *);
float   alm_mp_tanf_ULP      (float x,float z, double *, double *);
float   alm_mp_truncf_ULP    (float x,float z,double *, double *);


double  alm_mp_acos      (double x);
double  alm_mp_acosh     (double x);
double  alm_mp_asin      (double x);
double  alm_mp_asinh     (double x);
double  alm_mp_atan2     (double x, double y);
double  alm_mp_atan      (double x);
double  alm_mp_atanh     (double x);
double  alm_mp_cbrt      (double x);
double  alm_mp_ceil      (double x);
double  alm_mp_copysign  (double x, double y);
double  alm_mp_cos       (double x);
double  alm_mp_cosh      (double x);
double  alm_mp_cospi     (double x);
double  alm_mp_exp10     (double x);
double  alm_mp_exp2      (double x);
double  alm_mp_exp       (double x);
double  alm_mp_expm1     (double x);
double  alm_mp_fabs      (double x);
double  alm_mp_fdim      (double x, double y);
double  alm_mp_floor     (double x);
double  alm_mp_fma       (double x, double y, double z);
double  alm_mp_fmax      (double x, double y);
double  alm_mp_fmin      (double x, double y);
double  alm_mp_fmod      (double x, double y);
double  alm_mp_frexp     (double x, int *ptr);
double  alm_mp_hypot     (double x, double y);
double  alm_mp_ldexp     (double x, int expn);
double  alm_mp_log10     (double x);
double  alm_mp_log1p     (double x);
double  alm_mp_log2      (double x);
double  alm_mp_logb      (double x);
double  alm_mp_log       (double x);
double  alm_mp_modf      (double x, double *ptr);
double  alm_mp_pow       (double x, double y);
double  alm_mp_remainder (double x, double y);
double  alm_mp_remquo    (double x, double y, int* quotient);
double  alm_mp_rint      (double x);
double  alm_mp_round     (double x);
double  alm_mp_sin       (double x);
double  alm_mp_sinh      (double x);
double  alm_mp_sinpi     (double x);
double  alm_mp_sqrt      (double x);
double  alm_mp_tan       (double x);
double  alm_mp_tanh      (double x);
double  alm_mp_tanpi     (double x);
double  alm_mp_trunc     (double x);

  
double  alm_mp_acosh_ULP     (double x,double z, double *, double *);
double  alm_mp_acos_ULP      (double x,double z, double *, double *);
double  alm_mp_asinh_ULP     (double x,double z, double *, double *);
double  alm_mp_asin_ULP      (double x,double z, double *, double *);
double  alm_mp_atan2_ULP     (double x, double y,double z, double *, double *);
double  alm_mp_atanh_ULP     (double x,double z, double *, double *);
double  alm_mp_atan_ULP      (double x,double z, double *, double *);
double  alm_mp_cbrt_ULP      (double x, double z, double *, double *);
double  alm_mp_ceil_ULP      (double x, double z, double *, double *);
double  alm_mp_copysign_ULP  (double x, double y,double z,double *, double *);
double  alm_mp_cosh_ULP      (double x,double z, double *, double *);
double  alm_mp_cospi_ULP     (double x,double z, double *, double *);
double  alm_mp_cos_ULP       (double x, double z, double *, double *);
double  alm_mp_exp10_ULP     (double x,double z, double *, double *);
double  alm_mp_exp2_ULP      (double x,double z, double *, double *);
double  alm_mp_expm1_ULP     (double x,double z, double *, double *);
double  alm_mp_exp_ULP       (double x,double z, double *, double *);
double  alm_mp_fabs_ULP      (double x, double z, double *, double *);
double  alm_mp_fdim_ULP      (double x, double y,double z, double *, double *);
double  alm_mp_floor_ULP     (double x,double z, double *, double *);
double  alm_mp_fma_ULP       (double x, double y, double z,double re, double *, double *);
double  alm_mp_fmax_ULP      (double x, double y,double z, double *, double *);
double  alm_mp_fmin_ULP      (double x, double y,double z, double *, double *);
double  alm_mp_fmod_ULP      (double x, double y,double z, double *, double *);
double  alm_mp_frexp_ULP     (double x, int *ptr, double z, double *, double *);
double  alm_mp_hypot_ULP     (double x, double y,double z, double *, double *);
double  alm_mp_ldexp_ULP     (double x, int expn, double z, double *, double *);
double  alm_mp_log10_ULP     (double x,double z, double *, double *);
double  alm_mp_log1p_ULP     (double x,double z, double *, double *);
double  alm_mp_log2_ULP      (double x,double z, double *, double *);
double  alm_mp_logb_ULP      (double x,double z, double *, double *);
double  alm_mp_log_ULP       (double x,double z, double *, double *);
double  alm_mp_modf_ULP      (double x, double *ptr, double z, double *, double *);
double  alm_mp_pow_ULP       (double x, double y,double z, double *, double *);
double  alm_mp_remainder_ULP (double x, double y,double z, double *, double *);
double  alm_mp_remquo_ULP    (double x, double y, int* quotient, double z, double * , double *);
double  alm_mp_rint_ULP      (double x,double z, double *, double *);
double  alm_mp_round_ULP     (double x,double z, double *, double *);
double  alm_mp_sinh_ULP      (double x,double z, double *, double *);
double  alm_mp_sinpi_ULP     (double x,double z, double *, double *);
double  alm_mp_sin_ULP       (double x,double z, double *, double *);
double  alm_mp_sqrt_ULP      (double x,double z, double *, double *);
double  alm_mp_tanh_ULP      (double x,double z, double *, double *);
double  alm_mp_tanpi_ULP     (double x,double z, double *, double *);
double  alm_mp_tan_ULP       (double x,double z, double *, double *);
double  alm_mp_trunc_ULP     (double x,double z,double *, double *);

  
#ifdef __cplusplus
}
#endif


#endif
