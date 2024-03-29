/*
 * Copyright (C) 2008-2022, Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __LIBM_POLY_H__
#define __LIBM_POLY_H__

/*
* poly = C0*r^0 + C1*r^1 + c2*r^2
*/
#define POLY_EVAL_2(r, C0, C1, C2)  ({          \
            __typeof(r) _t1, __r2, _q;          \
            _t1 = C0 + C1*r;                    \
            __r2 = r*r;                         \
            _q = _t1 + C2*__r2;                 \
            _q;                                 \
})

/*
 * poly = C1 + C2*r + C3*r^2 + C4*r^3
 */
#define POLY_EVAL_3(r, c1, c2, c3, c4) ({       \
            __typeof(r) _t1, _t2, _r2, _q;      \
            _t1 = c1 + c2*r;                    \
            _t2 = c3 + c4*r;                    \
            _r2 = r * r;                        \
            _q = _t1 + _r2 * _t2;               \
            _q;                                 \
        })


/*
 * poly = C1 + C2*r + C3*r^2 + C4*r^3
 *      = (C1 + C2*r) + r^2(C3 + C4*r)
 */
#define POLY_EVAL_4(r, c0, c1, c2, c3)     ({   \
            __typeof(r) t1, t2, r2, q;          \
            t1 = c0 + c1*r;                     \
            t2 = c2 + c3*r;                     \
            r2 = r * r;                         \
            q = t1 + r2 * t2;                   \
            q;                                  \
        })

/*
 * poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4
 *      = (C1 + C2*r) + r^2(C3 + C4*r) + r^4*C5
 */
#define POLY_EVAL_5(r, c0, c1, c2, c3, c4) ({   \
            __typeof(r) t1, t2, r2, q;          \
            t1 = c0 + c1*r;                     \
            t2 = c2 + c3*r;                     \
            r2 = r * r;                         \
            q = t1 + r2 * t2;                   \
            q = q + r2 * r2 * c4;               \
            q;                                  \
        })

/*
 * poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4 + C6*r^5
 *      = (C1 + C2*r) + r^2(C3 + C4*r) + r^4(C5 + C6*r)
 */
#define POLY_EVAL_6(r, c0, c1, c2, c3, c4, c5) ({       \
            __typeof(r) t1, t2, t3, r2, _q;             \
            t1 = c0 + c1*r;                             \
            t2 = c2 + c3*r;                             \
            r2 = r * r;                                 \
            t3 = c4 + c5*r;                             \
            _q = t1 + r2 * t2;                          \
            _q = _q + r2 * r2 * t3;                     \
            _q;                                         \
            })

/*
 * poly = C0 + C1*r + C2*r2 + C3*r3 + C4*r4 + C5*r5 + C6*r6
 *      = (C0 + C1*r) + r2(C2 + C3*r) + r4(C4+ C5*r) + r6*C6
 */
#define POLY_EVAL_7(r, c0, c1, c2, c3, c4, c5, c6) ({     \
            __typeof(r) q, r2, r3, r4, r5, r6;            \
            r2 = r*r;                                     \
            r3 = r2*r;                                    \
            r4 = r2*r2;                                   \
            r5 = r3*r2;                                   \
            r6 = r3*r3;                                   \
            q = c0 + c1*r;                                \
            q += c2*r2 + c3*r3;                           \
            q += c4*r4 + c5*r5;                           \
            q += c6*r6;                                   \
            q;                                            \
        })

/*
    This function returns a polynomial:
    p(r) = c0*r^0 + c1*r^1 + c2*r^2 + c3r^3 + c4*r^4 + c5*r^5 +
            c6*r^6 + c7*r^7
*/
#define POLY_EVAL_8(r, c0, c1, c2, c3, c4, c5, c6, c7) ({   \
            __typeof(r) q, r2, r3, r4, r5, r6, r7;          \
            r2 = r * r;                                     \
            r3 = r2 * r;                                    \
            r4 = r2 * r2;                                   \
            r5 = r2 * r3;                                   \
            r6 = r3 * r3;                                   \
            r7 = r5 * r2;                                   \
                                                            \
            q = c0 + c1*r;                                  \
            q += c2*r2 + c3*r3;                             \
            q += c4*r4 + c5*r5;                             \
            q += c6*r6 + c7*r7;                             \
                                                            \
            q;                                              \
        })

/*
 * C0 is assumed to be added by caller
 * poly = C1*r + C2*r^2 + C3*r^3 + C4*r^4 + C5 *r^5 + C6*r^6 \
 *           + C7*r^7 + C8*r^8 + C9*r^9
 *
 *      = ((C6+C7*x)*x2 + (C4+C5*x))*x4 +
 *                      (C8+C9*x)*x8) +
 *                      ((C2+C3*x)*x2 + (C0+C1*x));
 */
#define POLY_EVAL_9(r, c1, c2, c3, c4, c5, c6, c7, c8, c9) ({   \
                        __typeof(r) a1, a2, a3, a4, b1, b2, q;  \
                        __typeof(r) r2, r4;                     \
                        a1 = c3*r + c2;                         \
                        a2 = c5*r + c4;                         \
                        r2 = r * r;                             \
                        a3 = c7*r + c6;                         \
                        r4 = r2 * r2;                           \
                        a4 = c9*r + c8;                         \
                                                                \
                        b1 = a4*r4 + a3*r2 + a2;                \
                        b2 = a1*r2 + c1*r;                      \
                        q = b1*r4 + b2;                         \
                        q;                                      \
                })

#define POLY_EVAL_HORNER_5(x, c0, c1, c2, c3, c4) ({            \
        __typeof(x) q = (((( c4 * x + c3)* x + c2) * x + c1) *  \
                        x + c0);                                \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_5_0(x, c0, c1, c2, c3, c4) ({          \
        __typeof(x) q = x * (((( c4 * x + c3)* x + c2) * x +    \
                        c1) * x + c0);                          \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_6(x, c0, c1, c2, c3, c4, c5) ({        \
        __typeof(x) q = (((((c5 * x + c4) * x + c3)* x + c2) *  \
                        x + c1) * x + c0);                      \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_6_0(x, c0, c1, c2, c3, c4, c5) ({      \
        __typeof(x) q = x * (((((c5 * x + c4) * x + c3)* x +    \
                        c2) * x + c1) * x + c0);                \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_7_0(x, c0, c1, c2, c3, c4, c5, c6) ({  \
        __typeof(x) q = x * ((((((c6 * x + c5) * x + c4) * x +  \
                        c3)* x + c2) * x + c1) * x + c0);       \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_8(x, c0, c1, c2, c3, c4, c5, c6, c7) ({        \
        __typeof(x) q = (((((((c7 * x + c6) * x + c5) * x + c4) \
                        * x + c3)* x + c2) * x + c1) * x + c0); \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_8_0(x, c0, c1, c2, c3, c4, c5, c6, c7) ({      \
        __typeof(x) q = x * (((((((c7 * x + c6) * x + c5) * x + \
                        c4) * x + c3)* x + c2) * x + c1) * x +  \
                        c0);                                    \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_9(x, c0, c1, c2, c3, c4, c5, c6, c7, c8) ({        \
        __typeof(x) q = ((((((((c8 *x + c7) * x + c6) * x + c5) \
                        * x + c4) * x + c3)* x + c2) *          \
                        x + c1) * x + c0);                      \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_9_0(x, c0, c1, c2, c3, c4, c5, c6, c7, c8) ({        \
        __typeof(x) q = x * ((((((((c8 *x + c7) * x + c6) * x + \
                        c5) * x + c4) * x + c3)* x + c2) *      \
                        x + c1) * x + c0);                      \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_11(x, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11) ({ \
        __typeof(x) q = (((((((((((                             \
                        c11 * x + c10) * x + c9) * x + c8) *    \
                        x + c7) * x + c6) * x + c5) * x + c4) * \
                        x + c3)* x + c2) * x + c1)* x + c0);    \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_12(x, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12) ({ \
        __typeof(x) q = ((((((((((((c12 * x +                   \
                        c11) * x + c10) * x + c9) * x + c8) *    \
                        x + c7) * x + c6) * x + c5) * x + c4) * \
                        x + c3)* x + c2) * x + c1)* x + c0);    \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_16_0(x, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15) ({ \
        __typeof(x) q = ((((((((((((((( c15*  x + c14)* x + \
                        c13)* x + c12) * x +                   \
                        c11) * x + c10) * x + c9) * x + c8) *    \
                        x + c7) * x + c6) * x + c5) * x + c4) * \
                        x + c3)* x + c2) * x + c1)* x + c0) * x;    \
         q;                                                     \
         })

#define POLY_EVAL_HORNER_20_0(x, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19) ({ \
        __typeof(x) q = ((((((((((((((((((( c19 * x + c18)* x + \
                        c17)* x + c16) * x + c15)*  x + c14)* x + \
                        c13)* x + c12) * x +                   \
                        c11) * x + c10) * x + c9) * x + c8) *    \
                        x + c7) * x + c6) * x + c5) * x + c4) * \
                        x + c3)* x + c2) * x + c1)* x + c0) * x;    \
         q;                                                     \
         })


/*
 * poly = C0 + C1*r + C2*r^2 + C3*r^3 + C4 *r^4 + C5*r^5 + C6*r^6 + C7*r^7 + C8*r^8
 *
 *      = (C0 + C1*r) + r2(C2 + C3*r) + r4((C4+ C5*r) + r2(C6 + C7*r)) + C8*r^8
 *
 *      = ((C6+C7*x)*x2 + (C4+C5*x))*x4 +
 *                      (C8+C9*x)*x8) +
 *                      ((C2+C3*x)*x2 + (C0+C1*x));
 */
#define POLY_EVAL_9_0(r, c0, c1, c2, c3, c4, c5, c6, c7, c8) ({ \
                        __typeof(r) a1, a2, a3, a4, b1, q;      \
                         __typeof(r) r2, r4;                    \
                        a1 = c1*r + c0;                         \
                        a2 = c3*r + c2;                         \
                        r2 = r * r;                             \
                        a3 = c5*r + c4;                         \
                        r4 = r2 * r2;                           \
                        a4 = c7*r + c6 +c8*r2;                  \
                                                                \
                        b1 = (a4*r2 + a3)*r4 + a2*r2;           \
                        q = b1 +a1;                             \
                         q;                                     \
                 })

/*
 * Polynomial of degree 27,
 *      - uses only odd terms and
 *      - C0 = 0
 * p(x) = (c13*x^27 + c12*x^25 + c11*x^23 + c10*x^21 + c9*x^19 + c8*x^17 + c7*x^15 +
 *         c6*x^13 + c5*x^11 + c4*x^9 + c3*x^7 + c2*x^5 + c1*x^3) + x
 *      = x(x^12*(x4*(c8 + c9*x2) + (c6 + c7*x2)) + x4*(x4*(c4 + c5*x2) + (c2 + c3*x2))+
 *        (c0 + c1*x2))
 *
 */
#define POLY_EVAL_ODD_27(r, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13) ({  \
        __typeof(r) r2, r4, r8;                                     \
        __typeof(r) a1, a2, a3, a4, a5, a6, b1, b2, b3, b4, q;                  \
        r2 = r * r;                                                 \
        r4 = r2 * r2;                                               \
        r8 = r4 * r4;                                               \
        a1 = c1 + c2 * r2;                                          \
        a2 = c3 + c4 * r2;                                          \
        a3 = c5 + c6 * r2;                                          \
        a4 = c7 + c8 * r2;                                          \
        a5 = c9 + c10 * r2;\
        a6 = c12 + c13 * r2;\
        a6 = c11 + a6 * r2;\
        b4 = a5 + a6 * r4;\
        b1 = a1 + a2 * r4;                                          \
        b2 = a4 + b4 * r4;                                          \
        b3 = a3 + b2 * r4;                                          \
        q  = b1 + b3 * r8;    \
        q = r * c0 + ((q * r2) * r);                                     \
        q;                                                     \
    })

#define POLY_EVAL_ODD_31(r, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15) ({  \
        __typeof(r) r2, r4, r8;                                     \
        __typeof(r) a1, a2, a3, a4, a5, a6, a7, b1, b2, b3, q;                  \
        r2 = r * r;                                                 \
        r4 = r2 * r2;                                               \
        r8 = r4 * r4;                                               \
        a1 = c1 + c2 * r2;                                          \
        a2 = c3 + c4 * r2;                                          \
        a3 = c5 + c6 * r2;                                          \
        a4 = c7 + c8 * r2;                                          \
        a5 = c9 + c10 * r2;\
        a6 = c11 + c12 * r2;\
        a7 = c14 + c15*r2;\
        a7 = c13 + a7 * r2;\
        b1 = a2 + a3*r4;\
        b1 = a1 + b1*r4;\
        b2 = a4 + a5*r4;\
        b3 = a6 + a7*r4;\
        b2 = b2 + b3*r8;                     \
        q  = b1 + b2*r8*r4;    \
        q = r * c0 + ((q * r2) * r);                                     \
        q;                                                     \
    })

/*
 * poly = x * (C1 + C2*x^2 + C3*x^4 + C4*x^6 + C5*x^8 + \
 *              C6*x^10 + C7*x^12 + C8*x^14)
 * 15 degree polynomial with only even terms
 */
#define POLY_EVAL_EVEN_15(r, c1, c2, c3, c4, c5, c6, c7, c8) ({         \
                        __typeof(r) a1, a2, a3, a4, b1, b2 ,q;          \
                        __typeof(r) r2, r4, r8, r12;                    \
                        r2 = r * r;                                     \
                        r4 = r2 * r2;                                   \
                        r8 = r4 * r4;                                   \
                        r12 = r8 * r4;                                  \
                        a1 = c1 + c2*r2;                                \
                        a2 = c3 + c4*r2;                                \
                        a3 = c5 + c6*r2;                                \
                        a4 = c7 + c8*r2;                                \
                                                                        \
                        b1 = a1 + a2*r4;                                \
                        b2 = r8*a3 + r12*a4;                            \
                                                                        \
                        q = r*(b1 + b2);                                \
                        q;                                              \
                })


#define POLY_EVAL_ODD_17(r, c1, c3, c5, c7, c9, c11, c13, c15) ({				\
                        __typeof(r) a1, a2, a3, a4, b1, b2 ,q;          \
                        __typeof(r) r2, r4, r6, r10, r14;               \
                        r2 = r * r;                                     \
                        r4 = r2 * r2;                                   \
                        r6 = r4 * r2;                                   \
                        r10 = r6 * r4;                                  \
                        r14 = r10 * r4;                                 \
                        a1 = c1 + c3*r2;                                \
                        a2 = c5 + c7*r2;                                \
                        a3 = c9 + c11*r2;																\
                        a4 = c13 + c15*r2;															\
                                                                        \
                        b1 = a1*r2 + a2*r6;                             \
                        b2 = r10*a3 + r14*a4;                           \
                                                                        \
                        q = r*(b1 + b2);                                \
                        q;                                              \
                })

/*
 *  poly = x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11 + \
 *          C6*x^13 + C7*x^15 + C8*x^17 + C9*x^19 +											\
 *          C10*x^21 + C11*x^23 + C12*x^25 + C13*x^27 + C14*x^29;
 *
 *       = x + x * G*(C6 + G*(C7 + G*(C8 +
 *                  G*(C9 + G*(C10+ G*(C11 + G*(C12 +
 *                          G*(C13 + C14*G))))
 */

#define POLY_EVAL_ODD_29(r, c1, c3, c5, c7, c9, c11, c13, c15,          \
                         c17, c19, c21, c23, c25, c27)                  \
    ({                                                                  \
        __typeof(r) a1, a2, a3, a4, a5, a6, a7;                         \
        __typeof(r) b1, b2, b3, b4, q;                                  \
        __typeof(r) g, g2, g3, g5, g7, g9, g11, g13;										\
				g  = r * r;																											\
        g2 = g * g;                                                     \
        g3 = g * g2;																										\
        g5 = g3 * g2;																										\
				g7 = g5 * g2;																										\
        g9 = g7 * g2;																										\
				g11 = g9 * g2;																									\
        g13 = g11 * g2;																									\
																																				\
        a1 = c1 + c3*g;																									\
        a2 = c5 + c7*g;																									\
        a3 = c9 + c11*g;																								\
        a4 = c13 + c15*g;																								\
        a5 = c17 + c19*g;																								\
        a6 = c21 + c23*g;																								\
        a7 = c25 + c27*g;																								\
																																				\
        b1 = g*a1  + g3*a2;																							\
        b2 = g5*a3 + g7*a4;																							\
        b3 = g9*a5 + g11*a6;																						\
        b4 = g13*a7;                                                    \
																																				\
        q = b1 + b2 + b3 + b4;                                          \
        q = r + r*q;                                                    \
        q;                                                              \
    })


/*
 * Polynomial of degree 19,
 *      - uses only odd terms and
 *      - C0 = 0
 * p(x) = (c9*x^19 + c8*x^17 + c7*x^15 + c6*x^13 + c5*x^11 + c4*x^9 + c3*x^7 +
 *         c2*x^5 + c1*x^3) + x
 *      = x(x^12*(x4*(c8 + c9*x2) + (c + c7*x2)) + x4*(x4*(c4 + c5*x2) + (c2 + c3*x2))+
 *        (1 + c1*x2))
 *
 */
#define POLY_EVAL_ODD_19(r, c1, c2, c3, c4, c5, c6, c7, c8, c9) ({  \
        __typeof(r) r2, r4, r8;                                     \
        __typeof(r) a1, a2, a3, a4, b1, b2, b3, q;                  \
        r2 = r * r;                                                 \
        r4 = r2 * r2;                                               \
        r8 = r4 * r4;                                               \
        a1 = c1 + c2 * r2;                                          \
        a2 = c3 + c4 * r2;                                          \
        a3 = c5 + c6 * r2;                                          \
        a4 = c7 + c8 * r2;                                          \
        b1 = a1 + a2 * r4;                                          \
        b2 = a4 + c9 * r4;                                          \
        b3 = a3 + b2 * r4;                                          \
        q  = b1 + b3 * r8;                                          \
        q = r + ((q * r2) * r);                                     \
        q;                                                          \
    })

/*
 * poly = x + (C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11 + \
 *              C6*x^13 + C7*x^15)
 *      = x + x3*(C1 + C2*x^2 + C3*x^4 + C4*x^6 + C5*x^8 + \
 *              C6*x^10 + C7*x^12)
 * 15 degree polynomial with only odd terms
 */
#define POLY_EVAL_ODD_15(r, c1, c2, c3, c4, c5, c6, c7) ({              \
            __typeof(r) a1, a2, a3, b1, b2 ,q;                          \
            __typeof(r) r2, r4, r8, r12;                                \
            r2 = r * r;                                                 \
            r4 = r2 * r2;                                               \
            r8 = r4 * r4;                                               \
            r12 = r8 * r4;                                              \
            a1 = c1 + c2*r2;                                            \
            a2 = c3 + c4*r2;                                            \
            a3 = c5 + c6*r2;                                            \
                                                                        \
            b1 = a1 + a2*r4;                                            \
            b2 = r8*a3 + r12*c7;                                        \
                                                                        \
            q = r + r * r2 * (b1 + b2);                                 \
            q;                                                          \
        })


/*
 * poly = C0 + C1*r^3 + C2*r^5 + C3*r^7+ C4 *r^9 \
 *
 *      = C0 + r^2*(C1 + C2*r^2) + r^4*(C3*r^2+C4*r^4)
 *
 */
#define POLY_EVAL_ODD_9(r, c0, c1, c2, c3, c4) ({               \
        __typeof(r) a0, a1, a2, q;                              \
        __typeof(r) r2, r4;                                     \
        r2 = r * r;                                             \
        r4 = r2 * r2;                                           \
                                                                \
        a0 = c2*r2 + c1;                                        \
        a1 = a0*r2 + c0;                                        \
        a2 = (c3*r2 + c4*r4)*r4;                                \
        q = r*(a1 + a2);                                        \
        q;                                                      \
        })

/*
 * Polynomial of degree 7,
 *      - uses only odd terms and
 * p(x) = c2*x^7 + c1*x^5 + c0*x^3 + x
 *      = x(x4*(c1 + c2*x2)+(1 + c0*x2))
 *
 */
#define POLY_EVAL_ODD_7(r, c0, c1, c2) ({                       \
        __typeof(r) r2, r4;                                     \
        __typeof(r) a1, a2, a3, q;                              \
        r2 = r * r;                                             \
        r4 = r2 * r2;                                           \
        a1 = c1 + c2 * r2;                                      \
        a2 = a1 * r4;                                           \
        a3 = 1 + c0 * r2;                                       \
        q = (a2 + a3) * r;                                      \
        q;                                                      \
    })


/*
 * p(x) = C1 + C2*r + C3*r^2 + C4*r^3 + C5*r^4 + C6*r^5 +
 *          C7*r^6 + C8*r^7 + C9*r^8 + C10*r^9 + C11*r^10 + C12*r^11
 *      = (C1 + C2*r) + r^2(C3 + C4*r) + r^4(C5 + C6*r) +
 *           r^6(C7 + C8*r) + r^8(C9 + C10*r) + r^10(C11 + C12*r)
 */
#define POLY_EVAL_12(x, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12) ({\
        __typeof(x) x2 =  x * x;                                             \
        __typeof(x) x4 = x2 * x2;                                            \
        __typeof(x) x8 = x4 * x4;                                            \
        __typeof(x) q, a0, a1, a2, a3, a4, a5, b0, b1, b2;                   \
         a0 =  c1 + c2  * x;                                                 \
         a1 =  c3 + c4  * x;                                                 \
         a2 =  c5 + c6  * x;                                                 \
         a3 =  c7 + c8  * x;                                                 \
         a4 =  c9 + c10 * x;                                                 \
         a5 = c11 + c12 * x;                                                 \
         b0 =  a0 + a1 * x2;                                                 \
         b1 =  a2 + a3 * x2;                                                 \
         b2 =  a4 + a5 * x2;                                                 \
                                                                             \
         q = (b0 + b1 * x4 ) + b2 * x8;                                      \
         q;                                                                  \
         })


/*
 * p(x) = 1+ r*(c1 +(c2 + (c3 + (c4 +(c5 +(c6 + c7*r)*r)*r)*r)*r)*r)
 */
#define POLY_EVAL_SINHF(x, c1, c2, c3, c4, c5, c6, c7) ({\
        __typeof(x) q, a0, a1, a2, a3, a4, a5;           \
         a0 =  c6 + c7  * x;                             \
         a1 =  c5 + a0  * x;                             \
         a2 =  c4 + a1  * x;                             \
         a3 =  c3 + a2  * x;                             \
         a4 =  c2 + a3 * x;                              \
         a5 =  c1 + a4 * x;                              \
         q =   1 + a5 * x ;                                \
         q;                                              \
         })

#define POLY_EVAL_SINH(x, c1, c2, c3, c4, c5, c6, c7) ({\
        __typeof(x) q, a0, a1, a2, a3, a4, a5;           \
         a0 =  c6 + c7  * x;                             \
         a1 =  c5 + a0  * x;                             \
         a2 =  c4 + a1  * x;                             \
         a3 =  c3 + a2  * x;                             \
         a4 =  c2 + a3 * x;                              \
         a5 =  c1 + a4 * x;                              \
         q =   a5 * x ;                                \
         q;                                              \
         })

/*
* p(x) = C0*x3 + C1*x5 + C2*x7 - C3*x9
*/
#define POLY_EVAL_TANH(x, c0, c1, c2, c3) ({                 \
        __typeof(x) q, x2, x3, x5, x7, x9;                      \
        x2 = x * x;                                          \
        x3 = x2 * x;                                         \
        x5 = x3 * x2;                                        \
        x7 = x5 * x2;                                        \
        x9 = x7 * x2;                                        \
        q = c0 * x3 + c1 * x5;                               \
        q += c2 * x7 - c3 * x9;                              \
        q;                                                   \
        })

/*
* p(x) = C0*x^0 + C1*x^2 + C2*x^4 + C3*x^6 +C4*x^8 + C5*x^10 + C6*x^12
*/
#define POLY_EVAL_EVEN_7(x, c0, c1, c2, c3, c4, c5, c6) ({\
         __typeof(x) q, a0, a1, a2, a3;                   \
         __typeof(x) x2, x4, x6, x8, x10, x12;            \
         x2 = x*x;                                        \
         x4 = x2*x2;                                      \
         x6 = x2*x4;                                      \
         x8 = x4*x4;                                      \
         x10 = x4*x6;                                     \
         x12 = x6*x6;                                     \
         a0 = c0 + c1*x2;                                 \
         a1 = c2*x4 + c3*x6;                              \
         a2 = c4*x8 + c5*x10;                             \
         a3 = c6*x12;                                     \
         q = a0 + a1;                                     \
         q += a2 + a3;                                    \
         q;                                               \
         })

/*
 * poly = C0 + C1*r^2 + C2*r^4 + C3*r^6+ C4*r^8 \
 *
 *      = C0 + r^2*(C1 + C2*r^2) + r^4*(C3*r^2+C4*r^4)
 *
 */
#define POLY_EVAL_EVEN_8(r, c0, c1, c2, c3, c4) ({              \
        __typeof(r) a0, a1, a2, q;                              \
        __typeof(r) r2, r4;                                     \
        r2 = r * r;                                             \
        r4 = r2 * r2;                                           \
                                                                \
        a0 = c2*r2 + c1;                                        \
        a1 = a0*r2 + c0;                                        \
        a2 = (c3*r2 + c4*r4)*r4;                                \
        q = (a1 + a2);                                          \
        q;                                                      \
        })

/* 
 * poly = C0 + C1*r^2 + C2*r^4 + C3*r^6 \
 *      = C0 + r^2*(C1 + C2*r^2 + C3*r4)
 */
#define POLY_EVAL_EVEN_6(r, c0, c1, c2, c3) ({      \
        __typeof(r) q, r2, r4, r6;                  \
        r2 = r * r;                                 \
        r4 = r2 * r2;                               \
        r6 = r4 * r2;                               \
        q = c0 + c1*r2;                             \
        q += c2*r4 + c3*r6;                         \
        q;                                          \
        })        

/*
 * poly = C0 + C1*r^2 + C2*r^4 + C3*r^6+ C4*r^8 + c5*r^10 \
 *
 *      = C0 + r^2*(C1 + C2*r^2) + r^4*(C3*r^2+C4*r^4+C5*r^6)
 *
 */
#define POLY_EVAL_EVEN_10(r, c0, c1, c2, c3, c4, c5) ({         \
        __typeof(r) a0, a1, a2, q;                              \
        __typeof(r) r2, r4, r6;                                 \
        r2 = r * r;                                             \
        r4 = r2 * r2;                                           \
        r6 = r4 * r2;                                           \
        a0 = c2*r2 + c1;                                        \
        a1 = a0*r2 + c0;                                        \
        a2 = (c3*r2 + c4*r4 + c5*r6)*r4;                        \
        q = (a1 + a2);                                          \
        q;                                                      \
        })
         
/* 
 * poly = C0 + C1*r^2 + C2*r^4 \
 *
 *      = C0 + r^2*(C1 + C2*r^2)
 *
 */
#define POLY_EVAL_EVEN_4(r, c0, c1, c2) ({              \
        __typeof(r) a0, a1, r2;                         \
        r2 = r * r;                                     \
        a0 = c1 + c2*r2;                                \
        a1 = c0 + r2*a0;                                \
        a1;                                             \
        })        

#endif /* LIBM_POLY_H */

