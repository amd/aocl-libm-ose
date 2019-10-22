/*This is quick prototype for the pow vector variant for the
 *Open64 team. */
#include "libm_amd.h"

__m128d FN_PROTOTYPE(vrd2_cosh)(__m128d x)
{

	union XMMREGTYP_
	{

		__m128d d128;
		double d[2];
	};

	typedef union XMMREGTYP_ XMMREGTYP;


	XMMREGTYP inp_x_128;
	XMMREGTYP res128;

	inp_x_128.d128 = x;

	res128.d[0] = FN_PROTOTYPE(cosh)(inp_x_128.d[0]);
	res128.d[1] = FN_PROTOTYPE(cosh)(inp_x_128.d[1]);

	return res128.d128;
}



