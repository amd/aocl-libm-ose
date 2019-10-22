#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_inlines_amd.h"
#include "libm_special.h"

/* Computes the exact product of x and y, the result being the
nearly doublelength number (z,zz) */
static inline void dekker_mul12(double x, double y,
								double *z, double *zz)
{
	double hx, tx, hy, ty;
	/* Split x into hx (head) and tx (tail). Do the same for y. */
	unsigned long long u;
	GET_BITS_DP64(x, u);
	u &= 0xfffffffff8000000;
	PUT_BITS_DP64(u, hx);
	tx = x - hx;
	GET_BITS_DP64(y, u);
	u &= 0xfffffffff8000000;
	PUT_BITS_DP64(u, hy);
	ty = y - hy;
	*z = x * y;
	*zz = (((hx * hy - *z) + hx * ty) + tx * hy) + tx * ty;
}

#undef _FUNCNAME
#define _FUNCNAME "remquo"
double FN_PROTOTYPE(remquo)(double x, double y, int *quo)
{
	double dx, dy, scale, w, t, v, c, cc;
	int i, ntimes, xexp, yexp;
	int xsgn, ysgn, qsgn;
	unsigned long long u, ux, uy, ax, ay, todd, temp;

	dx = x;
	dy = y;


	GET_BITS_DP64(dx, ux);
	GET_BITS_DP64(dy, uy);
	ax = ux & ~SIGNBIT_DP64;
	ay = uy & ~SIGNBIT_DP64;
	xsgn = (ax==ux) ? 1 : -1;
	ysgn = (ay==uy) ? 1 : -1;
	qsgn = xsgn * ysgn;
	*quo = 0;
	xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
	yexp = (int)((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);

	if (xexp < 1 || xexp > BIASEDEMAX_DP64 ||
		yexp < 1 || yexp > BIASEDEMAX_DP64)
	{
		/* x or y is zero, denormalized, NaN or infinity */
		if (xexp > BIASEDEMAX_DP64)
		{
			/* x is NaN or infinity */
			if (ux & MANTBITS_DP64)
			{
				/* x is NaN */
#ifdef WINDOWS
				return __amd_handle_error("remquo", __amd_remquo, ux|0x0008000000000000, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
                return x+x;
#endif
            }
			else
			{
				/* x is infinity; result is NaN */
				return __amd_handle_error("remquo", __amd_remquo, INDEFBITPATT_DP64, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
			}
		}
		else if (yexp > BIASEDEMAX_DP64)
		{
			/* y is NaN or infinity */
			if (uy & MANTBITS_DP64)
			{/* y is NaN */
#ifdef WINDOWS
				return __amd_handle_error("remquo", __amd_remquo, uy|0x0008000000000000, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
                return y+y;
#endif
            }
			else
			{
				/* y is infinity; result is indefinite */
				return __amd_handle_error("remquo", __amd_remquo, INDEFBITPATT_DP64, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
			}
		}
		else if (ax == 0x0000000000000000)
		{
			/* x is zero */
			if (ay == 0x0000000000000000)
			{
				/* y is zero */
				return __amd_handle_error("remquo", __amd_remquo, INDEFBITPATT_DP64, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
			}
			else
				return dx;
		}
		else if (ay == 0x0000000000000000)
		{
			/* y is zero */
			return __amd_handle_error("remquo", __amd_remquo, INDEFBITPATT_DP64, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
		}

		/* We've exhausted all other possibilities. One or both of x and
		y must be denormalized */
		if (xexp < 1)
		{
			/* x is denormalized. Figure out its exponent. */
			u = ax;
			while (u < IMPBIT_DP64)
			{
				xexp--;
				u <<= 1;
			}
		}
		if (yexp < 1)
		{
			/* y is denormalized. Figure out its exponent. */
			u = ay;
			while (u < IMPBIT_DP64)
			{
				yexp--;
				u <<= 1;
			}
		}
	}
	else if (ax == ay)
	{
		/* abs(x) == abs(y); return zero with the sign of x */
		*quo = qsgn;
		return 0.0*xsgn;
	}

	/* Set x = abs(x), y = abs(y) */
	PUT_BITS_DP64(ax, dx);
	PUT_BITS_DP64(ay, dy);

	if (ax < ay)
	{
		/* abs(x) < abs(y) */
		if (dx > 0.5*dy)
		{
			dx -= dy;
			*quo = qsgn;
		}
		return x < 0.0? -dx : dx;
	}

	/* Set ntimes to the number of times we need to do a
	partial remainder. If the exponent of x is an exact multiple
	of 52 larger than the exponent of y, and the mantissa of x is
	less than the mantissa of y, ntimes will be one too large
	but it doesn't matter - it just means that we'll go round
	the loop below one extra time. */
	if (xexp <= yexp)
		ntimes = 0;
	else
		ntimes = (xexp - yexp) / 52;

	if (ntimes == 0)
	{
		w = dy;
		scale = 1.0;
	}
	else
	{
		/* Set w = y * 2^(52*ntimes) */
		w = scaleDouble_3(dy, ntimes * 52);

		/* Set scale = 2^(-52) */
		PUT_BITS_DP64((unsigned long long)(-52 + EXPBIAS_DP64) << EXPSHIFTBITS_DP64,
			scale);
	}


	/* Each time round the loop we compute a partial remainder.
	This is done by subtracting a large multiple of w
	from x each time, where w is a scaled up version of y.
	The subtraction must be performed exactly in quad
	precision, though the result at each stage can
	fit exactly in a double precision number. */
	for (i = 0; i < ntimes; i++)
	{
		/* t is the integer multiple of w that we will subtract.
		We use a truncated value for t.

		N.B. w has been chosen so that the integer t will have
		at most 52 significant bits. This is the amount by
		which the exponent of the partial remainder dx gets reduced
		every time around the loop. In theory we could use
		53 bits in t, but the quad precision multiplication
		routine dekker_mul12 does not allow us to do that because
		it loses the last (106th) bit of its quad precision result. */

		/* Set dx = dx - w * t, where t is equal to trunc(dx/w). */
		t = (double)(long long)(dx / w);
		/* At this point, t may be one too large due to
		rounding of dx/w */

		/* Compute w * t in quad precision */
		dekker_mul12(w, t, &c, &cc);

		/* Subtract w * t from dx */
		v = dx - c;
		dx = v + (((dx - v) - c) - cc);

		/* If t was one too large, dx will be negative. Add back
		one w */
		/* It might be possible to speed up this loop by finding
		a way to compute correctly truncated t directly from dx and w.
		We would then avoid the need for this check on negative dx. */
		if (dx < 0.0)
			dx += w;

		/* Scale w down by 2^(-52) for the next iteration */
		w *= scale;
	}

	/* One more time */
	/* Variable todd says whether the integer t is odd or not */
	temp = (long long)(dx / w);
	*quo = (int)(temp & 0x7fffffff);
	t = (double) temp;
	todd = temp & 1;
	dekker_mul12(w, t, &c, &cc);
	v = dx - c;
	dx = v + (((dx - v) - c) - cc);
	if (dx < 0.0)
	{
		todd = !todd;
		dx += w;
		(*quo)--;
	}

	/* At this point, dx lies in the range [0,dy) */
	/* For the remainder function, we need to adjust dx
	so that it lies in the range (-y/2, y/2] by carefully
	subtracting w (== dy == y) if necessary. The rigmarole
	with todd is to get the correct sign of the result
	when x/y lies exactly half way between two integers,
	when we need to choose the even integer. */
	if (ay < 0x7fd0000000000000)
	{
		if (dx + dx > w || (todd && (dx + dx == w)))
		{
			dx -= w;
			(*quo)++;
		}
	}
	else if (dx > 0.5 * w || (todd && (dx == 0.5 * w)))
	{
		dx -= w;
		(*quo)++;
	}

	(*quo) *= qsgn;

	/* Set the result sign according to input argument x */
	return x < 0.0? -dx : dx;

}

