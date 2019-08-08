/*This is quick prototype for the pow vector variant for the
 *Open64 team. */
#include "libm_amd.h"

__m128d FN_PROTOTYPE(vrd2_pow)(__m128d x, __m128d y)
{

	union XMMREGTYP_
	{

		__m128d d128;
		double d[2];
	};

	typedef union XMMREGTYP_ XMMREGTYP;


	XMMREGTYP inp_x_128;
	XMMREGTYP inp_y_128;
	XMMREGTYP res128;

	inp_x_128.d128 = x;
	inp_y_128.d128 = y;

	res128.d[0] = FN_PROTOTYPE(pow)(inp_x_128.d[0], inp_y_128.d[0]);
	res128.d[1] = FN_PROTOTYPE(pow)(inp_x_128.d[1], inp_y_128.d[1]);

	return res128.d128;
}


weak_alias (__vrd2_pow, FN_PROTOTYPE(vrd2_pow))
