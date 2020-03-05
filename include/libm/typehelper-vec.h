#ifndef __LIBM_TYPEHELPER_VEC_H__
#define __LIBM_TYPEHELPER_VEC_H__

static inline v_f64x4_t
v_to_f32_f64(v_f32x4_t _xf32)
{
    return _mm256_cvtps_pd(_xf32);
}

static inline v_f32x4_t
v_to_f64_f32(v_f64x4_t _xf64)
{
    return _mm256_cvtpd_ps(_xf64);
}

#endif
