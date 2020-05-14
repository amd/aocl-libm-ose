/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_inlines_amd.h"
#include "libm_special.h"

/* sin(x) approximation valid on the interval [-pi/4,pi/4]. */
static inline double sin_piby4(double x, double xx)
{
  /* Taylor series for sin(x) is x - x^3/3! + x^5/5! - x^7/7! ...
                          = x * (1 - x^2/3! + x^4/5! - x^6/7! ...
                          = x * f(w)
     where w = x*x and f(w) = (1 - w/3! + w^2/5! - w^3/7! ...
     We use a minimax approximation of (f(w) - 1) / w
     because this produces an expansion in even powers of x.
     If xx (the tail of x) is non-zero, we add a correction
     term g(x,xx) = (1-x*x/2)*xx to the result, where g(x,xx)
     is an approximation to cos(x)*sin(xx) valid because
     xx is tiny relative to x.
  */
  static const double
    c1 = -0.166666666666666646259241729,
    c2 = 0.833333333333095043065222816e-2,
    c3 = -0.19841269836761125688538679e-3,
    c4 = 0.275573161037288022676895908448e-5,
    c5 = -0.25051132068021699772257377197e-7,
    c6 = 0.159181443044859136852668200e-9;
  double x2, x3, r;
  x2 = x * x;
  x3 = x2 * x;
  r = (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))));
  if (xx == 0.0)
    return x + x3 * (c1 + x2 * r);
  else
    return x - ((x2 * (0.5 * xx - x3 * r) - xx) - x3 * c1);
}

/* cos(x) approximation valid on the interval [-pi/4,pi/4]. */
static inline double cos_piby4(double x, double xx)
{
  /* Taylor series for cos(x) is 1 - x^2/2! + x^4/4! - x^6/6! ...
                          = f(w)
     where w = x*x and f(w) = (1 - w/2! + w^2/4! - w^3/6! ...
     We use a minimax approximation of (f(w) - 1 + w/2) / (w*w)
     because this produces an expansion in even powers of x.
     If xx (the tail of x) is non-zero, we subtract a correction
     term g(x,xx) = x*xx to the result, where g(x,xx)
     is an approximation to sin(x)*sin(xx) valid because
     xx is tiny relative to x.
  */
  double r, x2, t;
  static const double
    c1 = 0.41666666666666665390037e-1,
    c2 = -0.13888888888887398280412e-2,
    c3 = 0.248015872987670414957399e-4,
    c4 = -0.275573172723441909470836e-6,
    c5 = 0.208761463822329611076335e-8,
    c6 = -0.113826398067944859590880e-10;

  x2 = x * x;
  r = 0.5 * x2;
  t = 1.0 - r;
  return t + ((((1.0 - t) - r) - x * xx) + x2 * x2 *
              (c1 + x2 * (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))))));
}


double FN_PROTOTYPE_REF(sinpi)(double x)
{
    double r, dx, xsgn;
    long long ux;
    const double pi = 3.1415926535897932384626433832795;
    GET_BITS_DP64(x, ux);
		ux = ux & ~SIGNBIT_DP64;

    // Handle +-Inf and NaN
    if (ux >= PINFBITPATT_DP64)
    {
		PUT_BITS_DP64(QNANBITPATT_DP64,x);
        return x;
	}

    // When |x| > 2^52, the result is always an integer
    if (ux > 0x432fffffffffffffL)
        return x * 0.0;

    dx = x > 0.0 ? x : -x;
    GET_BITS_DP64(dx, ux);
    // Handle small arguments
    if (dx <= 0.25) {
        if (ux < 0x3f20000000000000) {
            if (ux < 0x3e40000000000000)
                return x * pi;

            dx = x * pi;
            return dx - dx*dx*dx*(1.0/6.0);
        }

        return sin_piby4(x*pi, 0.0);
    }

    // Remaining cases
    ux = (long)dx;
    r = dx - ux;
    xsgn = (x > 0.0 ? 1.0 : -1.0) * (ux & 0x1 ? -1.0 : 1.0);

    if (r == 0.0)
        return xsgn * 0.0;

    if (r <= 0.25)
        return xsgn * sin_piby4(r*pi, 0.0);

    if (r < 0.5)
        return xsgn * cos_piby4((0.5 - r)*pi, 0.0);

    if (r == 0.5)
        return xsgn;

    if (r <= 0.75)
        return xsgn * cos_piby4((r - 0.5)*pi, 0.0);

    // r < 1
    return xsgn * sin_piby4((1.0 - r)*pi, 0.0);
}

