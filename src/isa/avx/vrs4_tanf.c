/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
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


