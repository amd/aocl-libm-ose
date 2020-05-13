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
	__typeof(r) a1, a2, a3, a4, b1, b2, q;			\
	__typeof(r) r2, r4;					\
	a1 = c2*r + c1;						\
	a2 = c4*r + c3;						\
	r2 = r * r;						\
	a3 = c6*r + c5;						\
	r4 = r2 * r2;						\
	a4 = c8*r + c7;						\
                                                                \
	b1 = a4*r4 + a3*r2 + a2;				\
	b2 = a1*r2 + c0*r;					\
	q = b1*r4 + b2;						\
	q;							\
	})

#endif	/* LIBM_POLY_H */
