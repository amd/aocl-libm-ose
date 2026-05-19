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
 *   double complex cexp(double complex x)
 *
 *   Let x = a + I*b
 *
 *   exp(x) = exp(a) * exp(I * b)
 *
 *   exp(I * b) = cos(b) + I*sin(b) (in polar form)
 *
 *   exp(x) = exp(a)*cos(b) + I*exp(a)*sin(b)
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

/* Mantissa mask for double precision (52 bits) */
#define ALM_F64_MANTISSA_MASK 0xFFFFFFFFFFFFFULL

/* Underflow threshold: approximately log(DBL_MIN) */
#define CEXP_UNDERFLOW_THRESHOLD -745.0

/* Overflow threshold for scaled computation */
#define CEXP_MAX_ARG 0x1.62e42fefa39efp+9

/* Maximum representable value before exp overflows */
#define CEXP_EXP_MAX_ARG 0x1.fffffffffff2ap+1023

fc64_t
ALM_PROTO_OPT(cexp)(fc64_t z)
{
    double re, im;
    double zy_re, zy_im;
    double cos_im, sin_im;

    re = creal(z);
    im = cimag(z);

    uint64_t a_re = asuint64(re);
    uint64_t a_im = asuint64(im);

    uint64_t re_exp = (a_re >> 52) & 0x7FF;
    uint64_t im_exp = (a_im >> 52) & 0x7FF;

    /* Check if either component is NaN or Inf - exponent field is all 1s */
    int re_special = (re_exp == 0x7FF);
    int im_special = (im_exp == 0x7FF);

    /* Handle special cases */
    if (re_special || im_special) {

        /* Check if imaginary is NaN */
        if (im_special && ((a_im & ALM_F64_MANTISSA_MASK) != 0)) {
            /* im is NaN */
            if ((a_re & ~ALM_F64_SIGN_MASK) == 0) {
                /* (±0, NaN) -> (NaN, NaN) */
                zy_re = im;
                zy_im = im;
            } else if (re_special && ((a_re & ALM_F64_MANTISSA_MASK) == 0) &&
                       (a_re & ALM_F64_SIGN_MASK) != 0) {
                /* (-∞, NaN) -> (±0, ±0) */
                zy_re = POS_ZERO_F64;
                zy_im = POS_ZERO_F64;
            } else {
                /* (x, NaN) or (+∞, NaN) -> (NaN, NaN) */
                zy_re = re_special ? re : im;
                zy_im = im;

                /* If real is finite (not special), raise FE_INVALID */
                if (!re_special) {
                    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                        return alm_cexp_special((fc64_t) { zy_re, zy_im }, ALM_E_IN_Y_INF);
                    #else
                        return alm_cexp_special(CMPLX(zy_re, zy_im), ALM_E_IN_Y_INF);
                    #endif
                }
            }
        }
        /* Check if real is NaN */
        else if (re_special && ((a_re & ALM_F64_MANTISSA_MASK) != 0)) {
            /* re is NaN */
            if ((a_im & ~ALM_F64_SIGN_MASK) == 0) {
                /* (NaN, ±0) -> (NaN, ±0) - preserve sign of zero */
                zy_re = re;
                zy_im = im;
            } else {
                /* (NaN, y) -> (POS_NaN, POS_NaN) */
                zy_re = asdouble(POS_QNAN_F64);
                zy_im = asdouble(POS_QNAN_F64);

                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return alm_cexp_special((fc64_t) { zy_re, zy_im }, ALM_E_IN_Y_INF);
                #else
                    return alm_cexp_special(CMPLX(zy_re, zy_im), ALM_E_IN_Y_INF);
                #endif
            }
        }
        /* Check if imaginary is INF */
        else if (im_special) {
            /* im is INF, re is finite or INF (but not NaN) */
            if (!re_special) {
                /* (finite, ±∞) -> (NaN, NaN) and raise FE_INVALID */
                zy_re = asdouble(POS_QNAN_F64);
                zy_im = asdouble(POS_QNAN_F64);

                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))

                    return alm_cexp_special((fc64_t) { zy_re, zy_im }, ALM_E_IN_Y_INF);

                #else

                    return alm_cexp_special(CMPLX(zy_re, zy_im), ALM_E_IN_Y_INF);

                #endif

            } else if ((a_re & ALM_F64_SIGN_MASK) == 0) {
                /* (+∞, ±∞) -> (±∞, NaN) and raise FE_INVALID */
                zy_re = asdouble(POS_INF_F64);
                zy_im = asdouble(NEG_QNAN_F64);

                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return alm_cexp_special((fc64_t) { zy_re, zy_im }, ALM_E_IN_X_INF | ALM_E_IN_Y_INF);
                #else
                    return alm_cexp_special(CMPLX(zy_re, zy_im), ALM_E_IN_X_INF | ALM_E_IN_Y_INF);
                #endif

            } else {
                /* (-∞, ±∞) -> (±0, ±0) */
                zy_re = POS_ZERO_F64;
                zy_im = POS_ZERO_F64;
            }
        }
        /* Only real is INF (imaginary is finite) */
        else {
            int im_is_zero = ((a_im & ~ALM_F64_SIGN_MASK) == 0);

            if ((a_re & ALM_F64_SIGN_MASK) != 0) {
                /* (-∞, y) for finite y -> +0*cis(y) */
                if (im_is_zero) {
                    zy_re = POS_ZERO_F64;
                    zy_im = im;  /* Preserve sign of zero */
                } else {
                    ALM_PROTO(sincos)(im, &sin_im, &cos_im);
                    zy_re = POS_ZERO_F64 * cos_im;
                    zy_im = POS_ZERO_F64 * sin_im;
                }
            } else {
                /* (+∞, y) for finite y -> +∞*cis(y) */
                if (im_is_zero) {
                    zy_re = asdouble(POS_INF_F64);
                    zy_im = im;  /* Preserve sign of zero */
                } else {
                    ALM_PROTO(sincos)(im, &sin_im, &cos_im);
                    zy_re = asdouble(POS_INF_F64) * cos_im;
                    zy_im = asdouble(POS_INF_F64) * sin_im;
                }
            }
        }
    }
    /* Both components are finite */
    else {
        int re_is_zero = ((a_re & ~ALM_F64_SIGN_MASK) == 0);
        int im_is_zero = ((a_im & ~ALM_F64_SIGN_MASK) == 0);

        if (re_is_zero) {
            if (im_is_zero) {
                /* (±0, ±0) -> (1, ±0) - preserve sign of zero */
                zy_re = 1.0;
                zy_im = im;
            } else {
                /* (±0, y) -> cis(y) */
                ALM_PROTO(sincos)(im, &zy_im, &zy_re);
            }
        } else if (im_is_zero) {
            /* (x, ±0) -> (exp(x), ±0) - preserve sign of zero */
            zy_re = ALM_PROTO(exp)(re);
            zy_im = im;

            /* Check for overflow or underflow */
            uint64_t a_zy_re = asuint64(zy_re);
            uint64_t zy_re_exp = (a_zy_re >> 52) & 0x7FF;

            if (zy_re_exp == 0x7FF && ((a_zy_re & ALM_F64_MANTISSA_MASK) == 0)) {
                /* Overflow */
                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return alm_cexp_special((fc64_t) { zy_re, zy_im }, ALM_E_OVERFLOW);
                #else
                    return alm_cexp_special(CMPLX(zy_re, zy_im), ALM_E_OVERFLOW);
                #endif
            } else if (zy_re == 0.0 && re < CEXP_UNDERFLOW_THRESHOLD) {
                /* Underflow */
                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return alm_cexp_special((fc64_t) { zy_re, zy_im }, ALM_E_UNDERFLOW);
                #else
                    return alm_cexp_special(CMPLX(zy_re, zy_im), ALM_E_UNDERFLOW);
                #endif
            }
        } else {
            /* General case: finite nonzero real and imaginary */

            if (re > CEXP_MAX_ARG) {
                /* Potential overflow case - split computation to avoid premature overflow */
                double t = re - CEXP_MAX_ARG;
                ALM_PROTO(sincos)(im, &sin_im, &cos_im);
                double r = ALM_PROTO(exp)(t);

                zy_re = r * cos_im * CEXP_EXP_MAX_ARG;
                zy_im = r * sin_im * CEXP_EXP_MAX_ARG;

                /* Check if result overflowed */
                uint64_t a_zy_re = asuint64(zy_re);
                uint64_t a_zy_im = asuint64(zy_im);

                if ((((a_zy_re >> 52) & 0x7FF) == 0x7FF && ((a_zy_re & ALM_F64_MANTISSA_MASK) == 0)) ||
                    (((a_zy_im >> 52) & 0x7FF) == 0x7FF && ((a_zy_im & ALM_F64_MANTISSA_MASK) == 0))) {
                    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                        return alm_cexp_special((fc64_t) { zy_re, zy_im }, ALM_E_OVERFLOW);
                    #else
                        return alm_cexp_special(CMPLX(zy_re, zy_im), ALM_E_OVERFLOW);
                    #endif
                }
            } else if (re < CEXP_UNDERFLOW_THRESHOLD) {
                /* Potential underflow case */
                double exp_re = ALM_PROTO(exp)(re);
                ALM_PROTO(sincos)(im, &sin_im, &cos_im);
                zy_re = exp_re * cos_im;
                zy_im = exp_re * sin_im;
                if (exp_re == 0.0) {
                    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                        return alm_cexp_special((fc64_t) { zy_re, zy_im }, ALM_E_UNDERFLOW);
                    #else
                        return alm_cexp_special(CMPLX(zy_re, zy_im), ALM_E_UNDERFLOW);
                    #endif
                }
            } else {
                /* Normal computation */
                double exp_re = ALM_PROTO(exp)(re);
                ALM_PROTO(sincos)(im, &sin_im, &cos_im);

                zy_re = exp_re * cos_im;
                zy_im = exp_re * sin_im;
            }
        }
    }

    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
        return (fc64_t) { zy_re, zy_im };
    #else
        return CMPLX(zy_re, zy_im);
    #endif
}