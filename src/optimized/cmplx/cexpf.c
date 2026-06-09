/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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
 *
 *
 * Implementation Notes
 * --------------------
 * Signature:
 *   float complex cexpf(float complex x)
 *
 *   Let x = a + I*b
 *
 *   expf(x) = expf(a) * expf(I * b)
 *
 *   expf(I * b) = cosf(b) + I*sinf(b) (in polar form)
 *
 *   expf(x) = expf(a)*cosf(b) + I*expf(a)*sinf(b)
 *
 * Special Values:
 *   Follows ISO/IEC 9899:2011 specifications.
 *
 * Sign of Zero:
 *   This implementation carefully preserves the sign of zero in the imaginary
 *   part. When the input has ±0 imaginary part, the output must preserve that sign.
 *
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>
#include <libm/alm_special.h>
#include <libm_util_amd.h>

/* Mantissa mask for single precision (23 bits) */
#define ALM_F32_MANTISSA_MASK 0x7FFFFF

/* Underflow threshold: approximately log(FLT_MIN) */
#define CEXPF_UNDERFLOW_THRESHOLD -103.0f

/* Overflow threshold for scaled computation */
#define CEXPF_MAX_ARG 0x1.62e42ep6f

/* Maximum representable value before expf overflows */
#define CEXPF_EXP_MAX_ARG 0x1.fffffep127f

fc32_t
ALM_PROTO_OPT(cexpf)(fc32_t z)
{
    float re, im;
    float zy_re, zy_im;
    float cos_im, sin_im;

    re = crealf(z);
    im = cimagf(z);

    uint32_t a_re = asuint32(re);
    uint32_t a_im = asuint32(im);

    uint32_t re_exp = (a_re >> 23) & 0xFF;
    uint32_t im_exp = (a_im >> 23) & 0xFF;

    /* Check if either component is NaN or Inf - exponent field is all 1s */
    int re_special = (re_exp == 0xFF);
    int im_special = (im_exp == 0xFF);

    /* Handle special cases */
    if (re_special || im_special) {

        /* Check if imaginary is NaN */
        if (im_special && ((a_im & ALM_F32_MANTISSA_MASK) != 0)) {
            /* im is NaN */
            if ((a_re & ~ALM_F32_SIGN_MASK) == 0) {
                /* (±0, NaN) -> (NaN, NaN) */
                zy_re = im;
                zy_im = im;
            } else if (re_special && ((a_re & ALM_F32_MANTISSA_MASK) == 0) &&
                       (a_re & ALM_F32_SIGN_MASK) != 0) {
                /* (-∞, NaN) -> (±0, ±0) */
                zy_re = POS_ZERO_F32;
                zy_im = POS_ZERO_F32;
            } else {
                /* (x, NaN) or (+∞, NaN) -> (NaN, NaN) or (±∞, NaN) */
                zy_re = re_special ? re : im;
                zy_im = im;
                /* If real is finite (not special), raise FE_INVALID */
                if (!re_special) {
                    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                      return alm_cexpf_special((fc32_t) { zy_re, zy_im }, ALM_E_IN_Y_INF);
                    #else
                      return alm_cexpf_special(CMPLXF(zy_re, zy_im), ALM_E_IN_Y_INF);
                    #endif
                }
            }
        }
        /* Check if real is NaN */
        else if (re_special && ((a_re & ALM_F32_MANTISSA_MASK) != 0)) {
            /* re is NaN */
            if ((a_im & ~ALM_F32_SIGN_MASK) == 0) {
                /* (NaN, ±0) -> (NaN, ±0) - preserve sign of zero */
                zy_re = re;
                zy_im = im;
            } else {
                /* (NaN, y) -> (POS_NaN, POS_NaN) */
                zy_re = asfloat(POS_QNAN_F32);
                zy_im = asfloat(POS_QNAN_F32);

                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return alm_cexpf_special((fc32_t) { zy_re, zy_im }, ALM_E_IN_Y_INF);
                #else
                    return alm_cexpf_special(CMPLXF(zy_re, zy_im), ALM_E_IN_Y_INF);
                #endif
            }
        }
        /* Check if imaginary is INF */
        else if (im_special) {
            /* im is INF, re is finite or INF (but not NaN) */
            if (!re_special) {
                /* (finite, ±∞) -> (NaN, NaN) and raise FE_INVALID */
                zy_re = asfloat(POS_QNAN_F32);
                zy_im = asfloat(POS_QNAN_F32);

                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return alm_cexpf_special((fc32_t) { zy_re, zy_im }, ALM_E_IN_Y_INF);
                #else
                    return alm_cexpf_special(CMPLXF(zy_re, zy_im), ALM_E_IN_Y_INF);
                #endif

            } else if ((a_re & ALM_F32_SIGN_MASK) == 0) {
                /* (+∞, ±∞) -> (±∞, NaN) and raise FE_INVALID */
                zy_re = asfloat(POS_INF_F32);
                zy_im = asfloat(NEG_QNAN_F32);

                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return alm_cexpf_special((fc32_t) { zy_re, zy_im }, ALM_E_IN_X_INF | ALM_E_IN_Y_INF);
                #else
                    return alm_cexpf_special(CMPLXF(zy_re, zy_im), ALM_E_IN_X_INF | ALM_E_IN_Y_INF);
                #endif
            } else {
                /* (-∞, ±∞) -> (±0, ±0) */
                zy_re = POS_ZERO_F32;
                zy_im = POS_ZERO_F32;
            }
        }
        /* Only real is INF (imaginary is finite) */
        else {
            int im_is_zero = ((a_im & ~ALM_F32_SIGN_MASK) == 0);

            if ((a_re & ALM_F32_SIGN_MASK) != 0) {
                /* (-∞, y) for finite y -> +0*cis(y) */
                if (im_is_zero) {
                    zy_re = POS_ZERO_F32;
                    zy_im = im;  /* Preserve sign of zero */
                } else {
                    ALM_PROTO(sincosf)(im, &sin_im, &cos_im);
                    zy_re = POS_ZERO_F32 * cos_im;
                    zy_im = POS_ZERO_F32 * sin_im;
                }
            } else {
                /* (+∞, y) for finite y -> +∞*cis(y) */
                if (im_is_zero) {
                    zy_re = asfloat(POS_INF_F32);
                    zy_im = im;  /* Preserve sign of zero */
                } else {
                    ALM_PROTO(sincosf)(im, &sin_im, &cos_im);
                    zy_re = asfloat(POS_INF_F32) * cos_im;
                    zy_im = asfloat(POS_INF_F32) * sin_im;
                }
            }
        }
    }
    /* Both components are finite */
    else {
        int re_is_zero = ((a_re & ~ALM_F32_SIGN_MASK) == 0);
        int im_is_zero = ((a_im & ~ALM_F32_SIGN_MASK) == 0);

        if (re_is_zero) {
            if (im_is_zero) {
                /* (±0, ±0) -> (1, ±0) - preserve sign of zero */
                zy_re = 1.0f;
                zy_im = im;
            } else {
                /* (±0, y) -> cis(y) */
                ALM_PROTO(sincosf)(im, &zy_im, &zy_re);
            }
        } else if (im_is_zero) {
            /* (x, ±0) -> (expf(x), ±0) - preserve sign of zero */
            zy_re = ALM_PROTO(expf)(re);
            zy_im = im;

            /* Check for overflow or underflow */
            uint32_t a_zy_re = asuint32(zy_re);
            uint32_t zy_re_exp = (a_zy_re >> 23) & 0xFF;

            if (zy_re_exp == 0xFF && ((a_zy_re & ALM_F32_MANTISSA_MASK) == 0)) {
                /* Overflow */
                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return alm_cexpf_special((fc32_t) { zy_re, zy_im }, ALM_E_OVERFLOW);
                #else
                    return alm_cexpf_special(CMPLXF(zy_re, zy_im), ALM_E_OVERFLOW);
                #endif
            } else if (zy_re == 0.0f && re < CEXPF_UNDERFLOW_THRESHOLD) {
                /* Underflow */
                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return alm_cexpf_special((fc32_t) { zy_re, zy_im }, ALM_E_UNDERFLOW);
                #else
                    return alm_cexpf_special(CMPLXF(zy_re, zy_im), ALM_E_UNDERFLOW);
                #endif
            }
        } else {
            /* General case: finite nonzero real and imaginary */

            if (re > CEXPF_MAX_ARG) {
                /* Potential overflow case - split computation to avoid premature overflow */
                float t = re - CEXPF_MAX_ARG;
                ALM_PROTO(sincosf)(im, &sin_im, &cos_im);
                float r = ALM_PROTO(expf)(t);

                zy_re = r * cos_im * CEXPF_EXP_MAX_ARG;
                zy_im = r * sin_im * CEXPF_EXP_MAX_ARG;

                /* Check if result overflowed */
                uint32_t a_zy_re = asuint32(zy_re);
                uint32_t a_zy_im = asuint32(zy_im);

                if ((((a_zy_re >> 23) & 0xFF) == 0xFF && ((a_zy_re & ALM_F32_MANTISSA_MASK) == 0)) ||
                    (((a_zy_im >> 23) & 0xFF) == 0xFF && ((a_zy_im & ALM_F32_MANTISSA_MASK) == 0))) {
                    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                        return alm_cexpf_special((fc32_t) { zy_re, zy_im }, ALM_E_OVERFLOW);
                    #else
                        return alm_cexpf_special(CMPLXF(zy_re, zy_im), ALM_E_OVERFLOW);
                    #endif
                }
            } else if (re < CEXPF_UNDERFLOW_THRESHOLD) {
                /* Potential underflow case */
                float exp_re = ALM_PROTO(expf)(re);
                if (exp_re == 0.0f) {
                    ALM_PROTO(sincosf)(im, &sin_im, &cos_im);
                    zy_re = exp_re * cos_im;
                    zy_im = exp_re * sin_im;
                    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                        return alm_cexpf_special((fc32_t) { zy_re, zy_im }, ALM_E_UNDERFLOW);
                    #else
                        return alm_cexpf_special(CMPLXF(zy_re, zy_im), ALM_E_UNDERFLOW);
                    #endif
                } else {
                    ALM_PROTO(sincosf)(im, &sin_im, &cos_im);
                    zy_re = exp_re * cos_im;
                    zy_im = exp_re * sin_im;
                }
            } else {
                /* Normal computation */
                float exp_re = ALM_PROTO(expf)(re);
                ALM_PROTO(sincosf)(im, &sin_im, &cos_im);

                zy_re = exp_re * cos_im;
                zy_im = exp_re * sin_im;
            }
        }
    }

    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
        return (fc32_t) { zy_re, zy_im };
    #else
        return CMPLXF(zy_re, zy_im);
    #endif
}