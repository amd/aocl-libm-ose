/*
 * Copyright (C) 2018-2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
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

#endif  /* LIBM_POLY_VEC_H */

