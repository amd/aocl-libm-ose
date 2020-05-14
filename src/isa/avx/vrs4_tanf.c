/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "libm_amd.h"
#include "libm_util_amd.h"


float FN_PROTOTYPE_BAS64(tanf)(float x);

__m128 FN_PROTOTYPE_BAS64(vrs4_tanf)(__m128 x)
{

	union XMMREGTYP_
	{

		__m128 f128;
		float f[4];
	};

	typedef union XMMREGTYP_ XMMREGTYP;


	XMMREGTYP inp128;
	XMMREGTYP res128;

	inp128.f128 = x;

	res128.f[0] = FN_PROTOTYPE_BAS64(tanf)(inp128.f[0]);
	res128.f[1] = FN_PROTOTYPE_BAS64(tanf)(inp128.f[1]);
	res128.f[2] = FN_PROTOTYPE_BAS64(tanf)(inp128.f[2]);
	res128.f[3] = FN_PROTOTYPE_BAS64(tanf)(inp128.f[3]);


	return res128.f128;

}


