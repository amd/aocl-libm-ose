
#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_inlines_amd.h"
#include "libm_special.h"


/* sin(x) approximation valid on the interval [-pi/4,pi/4]. */
static inline double sinf_piby4(double x)
{
  /* Taylor series for sin(x) is x - x^3/3! + x^5/5! - x^7/7! ...
                          = x * (1 - x^2/3! + x^4/5! - x^6/7! ...
                          = x * f(w)
     where w = x*x and f(w) = (1 - w/3! + w^2/5! - w^3/7! ...
     We use a minimax approximation of (f(w) - 1) / w
     because this produces an expansion in even powers of x.
  */
  double x2;
  static const double
  c1 = -0.166666666638608441788607926e0,
  c2 = 0.833333187633086262120839299e-2,
  c3 = -0.198400874359527693921333720e-3,
  c4 = 0.272500015145584081596826911e-5;

  x2 = x * x;
  return (x + x * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4))));
}

/* cos(x) approximation valid on the interval [-pi/4,pi/4]. */
static inline double cosf_piby4(double x)
{
  /* Taylor series for cos(x) is 1 - x^2/2! + x^4/4! - x^6/6! ...
                          = f(w)
     where w = x*x and f(w) = (1 - w/2! + w^2/4! - w^3/6! ...
     We use a minimax approximation of (f(w) - 1 + w/2) / (w*w)
     because this produces an expansion in even powers of x.
  */
  double x2;
  static const double
    c1 = 0.41666666664325175238031e-1,
    c2 = -0.13888887673175665567647e-2,
    c3 = 0.24800600878112441958053e-4,
    c4 = -0.27301013343179832472841e-6;

  x2 = x * x;
  return (1.0 - 0.5 * x2 + (x2 * x2 *
                            (c1 + x2 * (c2 + x2 * (c3 + x2 * c4)))));
}

float FN_PROTOTYPE_REF(cospif)(float x)
{
	float r, dx, xsgn;
	unsigned int ux;
	const float pi = 3.1415926535897932F;

	/* handle special cases before any multiplication*/
	if (x==0.0f) return 1.0f;

	if (x > 0)
	{
		dx = x;
	}
	else
	{
		dx = -x;
	}

	GET_BITS_SP32(dx, ux);

	if ((ux & EXPBITS_SP32) == EXPBITS_SP32)
	{
		/* x is either NaN or infinity */
		if (ux & MANTBITS_SP32)
		{ /* x is NaN */
			return x; /* Raise invalid if it is a signalling NaN */
		}
		else
		{ /* x is infinity. Return a NaN */
			PUT_BITS_SP32(QNANBITPATT_SP32,x);
			return x;
		}
	}

	if (ux >= 0x4b800000)
	{/* when x >= 2^24, the result is always even integer */
		return 1.0F;
	}
	else if (ux >= 0x4b000000)
	{/* when x >= 2^23, the result is always integer */
		ux   = (unsigned int) dx;
		if (ux&0x1) return -1.0f;
		return 1.0f;
	}

	if (dx <= 0.25f) /* abs(x) <= pi/4 */
	{
		if (dx < 0.0078125f) /* abs(x) < 2.0^(-7) */
		{
			if (dx < 0.000122070313f) /* abs(x) < 2.0^(-13) */
			{
				return 1.0f;
			}

			dx = x * pi;
			return 1.0f - dx*dx*0.5f;
		}
		else
			return (float)cosf_piby4(x*pi);
	}

	ux   = (unsigned int) dx;
	if (ux&0x1) xsgn = -1.0f;
	else xsgn= 1.0f;

	r    = dx - ux;

	if ( r == 0.0f) return xsgn;

	if (r<=0.25f)
	{
		return (float)( xsgn * cosf_piby4(r*pi));
	}
	if (r<0.5f)
	{
		r = 0.5f - r;
		return (float)( xsgn * sinf_piby4(r*pi));
	}
	if (r == 0.5f)
	{
		return 0.0f;
	}
	if (r<=0.75f)
	{
		r = r - 0.5f;
		return (float)( -xsgn * sinf_piby4(r*pi));
	}

	/* r<1 */
	r = 1.0f - r;
	return (float)( -xsgn * cosf_piby4(r*pi));
}

