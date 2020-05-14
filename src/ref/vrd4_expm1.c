/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "libm_amd.h"
#include "libm_util_amd.h"
#include <libm/amd_funcs_internal.h>

__m256d FN_PROTOTYPE_REF(vrd4_expm1)(__m256d x)
{

	union XMMREGTYP_
	{

		__m256d d256;
		double d[4];
	};

	typedef union XMMREGTYP_ XMMREGTYP;


	XMMREGTYP inp256;
	XMMREGTYP res256;

	inp256.d256 = x;

	FN_PROTOTYPE_FMA3(vrda_expm1)(4, &inp256.d[0], &res256.d[0]);

	return res256.d256;

}


