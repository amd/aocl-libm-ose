/*
 * Copyright (C) 2019-2020, Advanced Micro Devices. All rights reserved.
 */

#ifndef __LIBM_POLY_H__
#define __LIBM_POLY_H__

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
            __typeof(r) t1, t2, t3, r2, q;              \
            t1 = c0 + c1*r;                             \
            t2 = c2 + c3*r;                             \
            r2 = r * r;                                 \
            t3 = c4 + c5*r;                             \
            q = t1 + r2 * t2;                           \
            q = q + r2 * r2 * t3;                       \
            q;                                          \
            })

/*
 * poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4 + C6*r^5 \
 *           + C7*r^6 + C8*r^7 + C9*r^8
 *
 *      = ((C6+C7*x)*x2 + (C4+C5*x))*x4 +
 *                      (C8+C9*x)*x8) +
 *                      ((C2+C3*x)*x2 + (C0+C1*x));
 */
#define POLY_EVAL_9(r, c0, c1, c2, c3, c4, c5, c6, c7, c8) ({   \
                        __typeof(r) a1, a2, a3, a4, b1, b2, q;  \
                        __typeof(r) r2, r4;                     \
                        a1 = c2*r + c1;                         \
                        a2 = c4*r + c3;                         \
                        r2 = r * r;                             \
                        a3 = c6*r + c5;                         \
                        r4 = r2 * r2;                           \
                        a4 = c8*r + c7;                         \
                                                                \
                        b1 = a4*r4 + a3*r2 + a2;                \
                        b2 = a1*r2 + c0*r;                      \
                        q = b1*r4 + b2;                         \
                        q;                                      \
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

#endif /* LIBM_POLY_H */

