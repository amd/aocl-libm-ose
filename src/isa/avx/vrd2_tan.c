/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "libm_amd.h"
#include "libm_util_amd.h"


double FN_PROTOTYPE_BAS64(tan)(double x);

__m128d FN_PROTOTYPE_BAS64(vrd2_tan)(__m128d x)
{

	union XMMREGTYP_
	{

		__m128d d128;
		double d[2];
	};

	typedef union XMMREGTYP_ XMMREGTYP;


	XMMREGTYP inp128;
	XMMREGTYP res128;

	inp128.d128 = x;

	res128.d[0] = FN_PROTOTYPE_BAS64(tan)(inp128.d[0]);
	res128.d[1] = FN_PROTOTYPE_BAS64(tan)(inp128.d[1]);

	return res128.d128;

}


