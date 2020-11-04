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


#ifndef __LIBM_POLY_VEC_H__
#define __LIBM_POLY_VEC_H__

#if defined(AMD_LIBM_FMA_USABLE)

#define mul_add(x, y, z)                                        \
        _Generic((x),                                           \
                 float  : _mm_fmadd_ss,                         \
                 double : _mm_fmadd_sd,                         \
                 __m128 : _mm_fmadd_ps,                         \
                 __m128d: _mm_fmadd_pd,                         \
                 __m256 : _mm256_fmadd_ps,                      \
                 __m256d: _mm256_fmadd_pd,                      \
                 __m512 : _mm512_fmadd_ps,                      \
                 __m512d: _mm512_fmadd_pd)((x), (y), (z))

#else /* ! FMA_USABLE */

#define no_fma_mul(a, b)                                                \
        _Generic((a),                                                   \
                 float  : _mm_mul_ss,                                   \
                 double : _mm_mul_sd,                                   \
                 __m128 : _mm_mul_ps,                                   \
                 __m128d: _mm_mul_pd,                                   \
                 __m256 : _mm256_mul_ps,                                \
                 __m256d: _mm256_mul_pd,                                \
                 __m512 : _mm512_mul_ps,                                \
                 __m512d: _mm512_mul_pd)((a), (b))

#define mul_add(x, y, z)                                                \
        _Generic((x),                                                   \
                 float  : _mm_add_ss,                                   \
                 double : _mm_add_sd,                                   \
                 __m128 : _mm_add_ps,                                   \
                 __m128d: _mm_add_pd,                                   \
                 __m256 : _mm256_add_ps,                                \
                 __m256d: _mm256_add_pd,                                \
                 __m512 : _mm512_add_ps,                                \
                 __m512d: _mm512_add_pd)(no_fma_mul((x), (y)), (z))

#endif  /* FMA_USABLE */

/*
 *  Estrins Scheme Polynomial evaluation
 */

/*
 * p20(x),
 * for special polynomial, assumes c0=0.0 and c1=1.0, rename to POLY_EVAL_20_1
 *								\
 * p1 = c2 + c3 * x;
 * p2 = c4 + c5 * x;
 * p3 = c6 + c7 * x;
 * p4 = c8 + c9 * x;
 * p5 = c10 + c11* x;
 * p6 = c12 + c13 * x;
 * p7 = c14 + c15 * x;
 * p8 = c16 + c17 * x;
 * p9 = c18 + c19 * x;
 *
 */
#define POLY_EVAL_20(x, c0, c1, c2, c3, c4, c5, c6, c7,                 \
                     c8, c9, c10,                                       \
                     c11, c12, c13, c14, c15, c16, c17,                 \
                     c18, c19, c20) ({                                  \
    __typeof(x) x2 = x  * x;                                            \
    __typeof(x) x4 = x2 * x2;                                           \
    __typeof(x) x8 = x4 * x4;                                           \
    __typeof(x) x16= x8 * x8;                                           \
    __typeof(x) q;                                                      \
    __typeof(x) q1, q2, q3, q4, q5;                                     \
    __typeof(x) r1, r2, r3;                                             \
    __typeof(x) p10 = c20 * x4;                                         \
                                                                        \
    /*q1 = x + x2 * p1; */                                              \
    q1 = mul_add(mul_add(c3, x, c2),                                    \
                 x2,                                                    \
                 mul_add(c1, x, c0));                                   \
                                                                        \
    /* q2 = p2 + x2 * p3; */                                            \
    q2 = mul_add(mul_add(c7, x, c6),                                    \
                 x2,                                                    \
                 mul_add(c5, x, c4));                                   \
                                                                        \
    /*q3 = p4 + x2 * p5; */                                             \
    q3 = mul_add(mul_add(c11, x, c10),                                  \
                 x2,                                                    \
                 mul_add(c9, x, c8));                                   \
                                                                        \
    /* q4 = p6 + x2 * p7; */                                            \
    q4 = mul_add(mul_add(c15, x, c14),                                  \
                 x2,                                                    \
                 mul_add(c13, x, c12));                                 \
                                                                        \
    /* q5 = p8 + x2 * p9; */                                            \
    q5 = mul_add(mul_add(c19, x, c18),                                  \
                 x2,                                                    \
                 mul_add(c17, x, c16));                                 \
                                                                        \
    r1 = q1 + x4 * q2;                                                  \
    r2 = x8 * (q3 + x4 * q4);                                           \
    r3 = x16 * (q5 + p10);                                              \
                                                                        \
    q = r1 + r2 + r3;                                                   \
    q;                                                                  \
    })

/*
 * p(x) = C1 + C2*r + C3*r^2 + C4*r^3 + C5*r^4 + C6*r^5 +
 *          C7*r^6 + C8*r^7 + C9*r^8 + C10*r^9 + C11*r^10 + C12*r^11
 *      = (C1 + C2*r) + r^2(C3 + C4*r) + r^4(C5 + C6*r) +
 *           r^6(C7 + C8*r) + r^8(C9 + C10*r) + r^10(C11 + C12*r)
 */
#define POLY_EVAL_11(x, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11) ({ \
        __typeof(x) x2 = x * x;                                              \
        __typeof(x) x4 = x2 * x2;                                            \
        __typeof(x) x8 = x4 * x4;                                            \
        __typeof(x) q = mul_add( mul_add ( mul_add(c11, x, c10), x2,         \
                                           mul_add(c9 ,x ,c8)), x8,          \
                                 mul_add ( mul_add(mul_add(c7, x, c6), x2,   \
                                           mul_add(c5, x, c4)) ,x4,          \
                                           mul_add( mul_add(c3, x, c2),x2,   \
                                           mul_add(c0, x, c1)) ));           \
         q;                                                                  \
         })



/*
 * p(x) = c10*x^10 + c9*x^9 + c8*x^8 + c7*x^7 + c6*x^6 + c5*x^5 + c4*x^4 + \
 *                      c3*x^3 + c2*x^2 + c1*x + c0
 *      = (((c6+c7*x)*x2 + (c4+c5*x))*x4 + (c8+c9*x+c10*x2)*x8) + \
 *                      ((c2+c3*x)*x2 + (c0+c1*x));
 */
#define POLY_EVAL_10(x, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10) ({ \
        __typeof(x) x2 = x  * x;                                        \
        __typeof(x) x4 = x2 * x2;                                       \
        __typeof(x) x8 = x4 * x4;                                       \
        __typeof(x) q =  mul_add(mul_add(x2, c10, mul_add(c9, x, c8)),  \
                                x8,                                     \
                                mul_add(mul_add(mul_add(c7, x, c6),     \
                                                x2,                     \
                                                mul_add(c5, x, c4)),    \
                                        x4,                             \
                                        mul_add(mul_add(c3, x, c2),     \
                                                x2,                     \
                                                mul_add(c1, x, c0))));	\
        q;                                                              \
        })

/*
 * p(x) = c9*x^9 + c8*x^8 + c7*x^7 + c6*x^6 + c5*x^5 +
 *                      c4*x^4 + c3*x^3 + c2*x^2 + c1*x + c0
 *      = ((c6+c7*x)*x2 + (c4+c5*x))*x4 +
 *                      (c8+c9*x)*x8) +
 *                      ((c2+c3*x)*x2 + (c0+c1*x));
 */
#define POLY_EVAL_9(x, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9) ({       \
    __typeof(x) x2 = x  * x;                                            \
    __typeof(x) x4 = x2 * x2;                                           \
    __typeof(x) x8 = x4 * x4;                                           \
    __typeof(x) q = mul_add(mul_add(c9, x, c8),                         \
                            x8,                                         \
                            mul_add(mul_add(mul_add(c7, x, c6),         \
                                            x2,                         \
                                            mul_add(c5, x, c4)),        \
                                    x4,                                 \
                                    mul_add(mul_add(c3, x, c2),         \
                                            x2,                         \
                                            mul_add(c1, x, c0))));      \
        q;                                                              \
    })

/*
 * p(x) = c8*x^8 + c7*x^7 + c6*x^6 + c5*x^5 + c4*x^4 + c3*x^3 + c2*x^2 + c1*x + c0
 *      = ((c6+c7*x)*x2 + (c4+c5*x))*x4 +
 *                 (c8*x8 + (c2+c3*x)*x2 +  (c0+c1*x))
 */
#define POLY_EVAL_8(x, c0, c1, c2, c3, c4, c5, c6, c7, c8) ({           \
        __typeof(x) x2 = x * x;                                         \
        __typeof(x) x4 = x2 * x2;                                       \
        __typeof(x) x8 = x4 * x4;                                       \
        __typeof(x) q = mul_add(mul_add(mul_add(c7, x, c6),             \
                                        x2,                             \
                                        mul_add(c5, x, c4)),            \
                                x4,                                     \
                                mul_add(mul_add(c3, x, c2),             \
                                        x2,                             \
                                        mul_add(c1, x, c0) + c8*x8));   \
        q;                                                              \
        })


/*
 * p(x) = c7*x^7 + c6*x^6 + c5*x^5 + c4*x^4 + c3*x^3 + c2*x^2 + c1*x + c0
 *      = ((c6+c7*x)*x2 + (c4+c5*x))*x4 + ((c2+c3*x)*x2 + (c0+c1*x))
 */

#define POLY_EVAL_7(x, c0, c1, c2, c3, c4, c5, c6, c7) ({               \
            __typeof(x) x2 = x * x;                                     \
            __typeof(x) x4 = x2 * x2;                                   \
            __typeof(x) q = mul_add(mul_add(mul_add(c7, x, c6),         \
                                            x2,                         \
                                            mul_add(c5, x, c4)),        \
                                    x4,                                 \
                                    mul_add(mul_add(c3, x, c2),         \
                                            x2,                         \
                                            mul_add(c1, x, c0)));       \
            q;                                                          \
        })

/*
 * p(x) = 1*x^6 + c5*x^5 + c4*x^4 + c3*x^3 + c2*x^2 + c1*x + c0
 *      = (c4+c5*x+x2)*x4 + ((c2+c3*x)*x2 + (c0+c1*x));
 */
#define POLY_EVAL_6(x, c0, c1, c2, c3, c4, c5, c6) ({                   \
            __typeof(x) x2 = x * x;                                     \
            __typeof(x) x4 = x2 * x2;                                   \
            __typeof(x) q = mul_add(mul_add(c6,                         \
                                            x2,                         \
                                            mul_add(c5, x, c4)),        \
                                    x4,                                 \
                                    mul_add(mul_add(c3, x, c2),         \
                                            x2,                         \
                                            mul_add(c1, x, c0)));       \
            q;                                                          \
        })

/*
 * p(x) = c5*x^5 + c4*x^4 + c3*x^3 + c2*x^2 + c1*x + c0
 *      = (c2+c3*x)*x2 + ((c4+c5*x)*x4 + (c0+c1*x))
 */
#define POLY_EVAL_5(x, c0, c1, c2, c3, c4, c5) ({                       \
            __typeof(x) x2 = x * x;                                     \
            __typeof(x) x4 = x2 * x2;                                   \
            __typeof(x) q = mul_add(mul_add(c3, x, c2),                 \
                                    x2,                                 \
                                    mul_add(mul_add(c5, x, c4),         \
                                            x4,                         \
                                            mul_add(c1, x, c0)));       \
            q;                                                          \
        })

/*
 * p(x) = c1*x^3 + c0*x^2 + x
 *      = (c0+c1*x)*x^2 + x
 */

#define POLY_EVAL_1(x, c0, c1) ({                                       \
            __typeof(x) x2 = x * x;                                     \
            __typeof(x) q = mul_add(mul_add(c1, x, c0),                 \
                                    x2,                                 \
                                    x);                                 \
            q;                                                          \
        })

/*
 * Polynomial of degree 15,
 *      - uses only odd terms and
 *      - C0 = 0
 * p(x) = (c7*x^15 + c6*x^13 + c5*x^11 + c4*x^9 + c3*x^7 + c2*x^5 + c1*x^3) + x
 *      = (c7*x^12 + c6*x^10 + c5*x^8 + c4*x^6 + c3*x^4 + c2*x^2 + c1) * \
 *                      x^2 *x + x
 */
#define POLY_EVAL_ODD_15(x, c1, c2, c3, c4, c5, c6, c7) ({          \
            __typeof(x) x2  = x * x;                                \
            __typeof(x) x4  = x2 * x2;                              \
            __typeof(x) x8  = x4 * x4;                              \
            __typeof(x) q = mul_add(mul_add(c7,                     \
                                            x4,                     \
                                            mul_add(c6, x2, c5)),   \
                                    x8,                             \
                                    mul_add(mul_add(c4, x2, c3),    \
                                            x4,                     \
                                            mul_add(c2, x2, c1)));  \
            x + ((q * x2) * x) ;                                  \
        })

/*
 * Polynomial of degree 15,
 *      - uses only even terms and
 *      - C0 = 0
 * p(x) = (c8*x^14 + c7*x^12 + c6*x^10 + c5*x^8 + c4*x^6 + c3*x^4 + c2*x^2 + c1)*x
 *      = ((c7+c8*x2)*x4 + (c5+c6*x2))*x8 +
 *                 (c3+c4*x2)*x4 + (c1+c2*x2))*x
 */
#define POLY_EVAL_EVEN_15(x, c1, c2, c3, c4, c5, c6, c7, c8) ({         \
            __typeof(x) x2  = x * x;                                    \
            __typeof(x) x4  = x2 * x2;                                  \
            __typeof(x) x8  = x4 * x4;                                  \
            __typeof(x) q = mul_add(mul_add(mul_add(c8, x2, c7),        \
                                            x4,                         \
                                            mul_add(c6, x2, c5)),       \
                                    x8,                                 \
                                    mul_add(mul_add(c4, x2, c3),        \
                                            x4,                         \
                                            mul_add(c2, x2, c1)));      \
            x * q;                                                      \
    })


#endif  /* LIBM_POLY_VEC_H */

