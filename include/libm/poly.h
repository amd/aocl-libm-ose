/*
 * Copyright (C) 2019-2020, AMD. All rights reserved.
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

#endif	/* LIBM_POLY_H */
